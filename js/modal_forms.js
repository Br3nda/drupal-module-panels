// $Id: modal_forms.js,v 1.1.2.17 2008/07/09 22:24:24 merlinofchaos Exp $

Drupal.Panels.Subform = {};

/** Basic submit on a subform **/
Drupal.Panels.Subform.bindClickSubmit = function() {
  $(this).ajaxSubmit({
    url: Drupal.settings.panelsAjaxURL + '/submit-form/' +  $('#panel-did').val(),
    data: '',
    method: 'post',
    after: Drupal.Panels.Subform.bindAjaxResponse,
    dataType: 'json'
  });
  return false;
};

Drupal.Panels.Subform.bindClickAddLink = function() {
  var id = $(this)[0].id;
  // show the empty dialog right away.
  $.ajax({
    type: "POST",
    url: Drupal.settings.panelsAjaxURL + "/add-config/" + $('#panel-did').val() + '/' + id,
    data: '',
    global: true,
    success: Drupal.Panels.Subform.bindAjaxResponse,
    error: function() { alert("An error occurred while attempting to process the pane configuration modal form.."); $('#panels-modal').unmodalContent(); },
    dataType: 'json'
  });
  return false;
};

/** ripped from Drupal cause it's not easy to re-autoattach cleanly */
Drupal.Panels.Subform.bindCollapsible = function() {
  $('div.panels-modal-content fieldset.collapsible > legend').each(function() {
    var fieldset = $(this.parentNode);
    // Expand if there are errors inside
    if ($('input.error, textarea.error, select.error', fieldset).size() > 0) {
      fieldset.removeClass('collapsed');
    }

    // Turn the legend into a clickable link and wrap the contents of the fieldset
    // in a div for easier animation
    var text = this.innerHTML;
    $(this).empty().append($('<a href="#">'+ text +'</a>').click(function() {
      var fieldset = $(this).parents('fieldset:first')[0];
      // Don't animate multiple times
      if (!fieldset.animating) {
        fieldset.animating = true;
        Drupal.toggleFieldset(fieldset);
      }
      return false;
    })).after($('<div class="fieldset-wrapper"></div>').append(fieldset.children(':not(legend)')));
  });
};

/** ripped from Drupal cause it's not easy to re-autoattach cleanly */
Drupal.Panels.Subform.bindAutocomplete = function() {
  var acdb = [];
  $('div.panels-modal-content input.autocomplete').each(function () {
    var uri = this.value;
    if (!acdb[uri]) {
      acdb[uri] = new Drupal.ACDB(uri);
    }
    var input = $('#' + this.id.substr(0, this.id.length - 13))
      .attr('autocomplete', 'OFF')[0];
    $(input.form).submit(Drupal.autocompleteSubmit);
    new Drupal.jsAC(input, acdb[uri]);
  });
};

Drupal.Panels.Subform.bindAjaxResponse = function(data) {
  // On success, append the returned HTML to the panel's element.
  if (data.type == 'display') {
    // append the output
    $('#modalContent span.modal-title').html(data.title);
    $('#modalContent div.modal-content').html(data.output);
    
    // TODO this is the code that we want to see go away
    var url = data.url;
    if (!url) {
      url = Drupal.settings.panelsAjaxURL + '/submit-form/' +  $('#panel-did').val();
    }

    // Bind forms to ajax submit.
    $('div.panels-modal-content form').unbind('submit'); // be safe here.
    $('div.panels-modal-content form').submit(function() {
      $(this).ajaxSubmit({
        url: url,
        data: '',
        method: 'post',
        after: Drupal.Panels.Subform.bindAjaxResponse,
        dataType: 'json'
      });
      return false;
      
    });

    // Bind links to ajax links.
    $('a.panels-modal-add-config').click(Drupal.Panels.Subform.bindClickAddLink);

    if ($('#override-title-checkbox').size()) {
      Drupal.Panels.Checkboxes.bindCheckbox('#override-title-checkbox', ['#override-title-textfield']);
    }

    if ($('#use-pager-checkbox').size()) {
      Drupal.Panels.Checkboxes.bindCheckbox('#use-pager-checkbox', ['#use-pager-textfield']);
    }

    // hack: allow collapsible textareas to work
    Drupal.Panels.Subform.bindCollapsible();

    Drupal.Panels.Subform.bindAutocomplete();
  }
  else if (data.type == 'add') {
    // Give it all the goodies that our existing panes have.   
    $('#panel-pane-' + data.region).append(data.output);
    
    Drupal.Panels.attachPane('#panel-pane-' + data.id);
    Drupal.Panels.changed($('#panel-pane-' + data.id));
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  }
  else if (data.type == 'replace') {
    // Replace the HTML in the pane
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible')
      .html(data.output);

    // Re-attach the collapse/expand behavior to the pane.
    $('#panel-pane-' + data.id).addClass('panel-portlet').each(Drupal.Panels.bindPortlet);

    // Mark that this pane has been changed.
    Drupal.Panels.changed($('#panel-pane-' + data.id));

    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  }
  else if (data.type == 'toggle-shown') {
    var src_url = $('#panel-pane-' + data.id + ' input.pane-toggle-shown').attr('src');
    $('#panel-pane-' + data.id + ' input.pane-toggle-shown').attr({
      title: data.output + " this pane",
      alt: data.output + " this pane",
      src: src_url.replace(data.old_op + 'pane.png', data.new_op + 'pane.png')
    });
    $('#panel-pane-' + data.id).toggleClass('hidden-pane');
    Drupal.Panels.changed($('#panel-pane-' + data.id));
  }
  else if (data.type == 'dismiss-changed') {
    // Just mark the pane as changed.   
    Drupal.Panels.changed($('#panel-pane-' + data.id));
    // Dismiss the dialog.
    $('#panels-modal').unmodalContent();
	  }
  else {
    // just dismiss the dialog.
    $('#panels-modal').unmodalContent();
  }
};

/**
 * Display the modal
 */
Drupal.Panels.Subform.show = function() {
  $('#panels-modal').modalContent({
    opacity: '.40', 
    background: '#fff'
  });
  $('#modalContent .modal-content').html($('div#panels-throbber').html());
};

/**
 * Hide the modal
 */
Drupal.Panels.Subform.dismiss = function() {
  $('#panels-modal').unmodalContent();
};

Drupal.Panels.Subform.createModal = function() {
  var html = ''
  html += '<div class="panels-hidden">';
  html += '  <div id="panels-modal">'
  html += '    <div class="panels-modal-content">'
  html += '      <div class="modal-header">';
  html += '        <a class="close" href="#">';
  html +=            Drupal.settings.panels.closeText + Drupal.settings.panels.closeImage;
  html += '        </a>';
  html += '        <span class="modal-title">&nbsp;</span>';
  html += '      </div>';
  html += '      <div class="modal-content">';
  html += '      </div>';
  html += '    </div>';
  html += '  </div>';
  html += '  <div id="panels-throbber">';
  html += '    <div class="panels-throbber-wrapper">';
  html +=        Drupal.settings.panels.throbber;
  html += '    </div>';
  html += '  </div>';
  html += '</div>';

  $('body').append(html);
};

/**
 * Bind a modal form to a button and a URL to go to.
 */
Drupal.Panels.Subform.bindModal = function(id, info) {
  $(id).click(function() {
    var url = info[0];
    if (info[1]) {
      url += '/' + $(info[1]).val();
    }
    // show the empty dialog right away.
    Drupal.Panels.Subform.show();
    $.ajax({
      type: "POST",
      url: url,
      data: '',
      global: true,
      success: Drupal.Panels.Subform.bindAjaxResponse,
      error: function() { alert("Invalid response from server."); Drupal.Panels.Subform.dismiss(); },
      dataType: 'json'
    });
    return false;
  });
};

/**
 * Bind all modals to their buttons. They'll be in the settings like so:
 * Drupal.settings.panels.modals.button-id = url
 */
Drupal.Panels.Subform.autoAttach = function() {
  if (Drupal.settings && Drupal.settings.panels && Drupal.settings.panels.modals) {
    Drupal.Panels.Subform.createModal();
    for (var modal in Drupal.settings.panels.modals) {
      if (!$(modal + '.modal-processed').size()) {
        Drupal.Panels.Subform.bindModal(modal, Drupal.settings.panels.modals[modal]);
        $(modal).addClass('modal-processed');
      }
    }
  }
};

$(Drupal.Panels.Subform.autoAttach);
