// $Id: modal_forms.js,v 1.1.2.2 2007/08/30 02:45:22 merlinofchaos Exp $

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
}

Drupal.Panels.Subform.bindClickAddLink = function() {
  var id = $(this)[0].id;
  // show the empty dialog right away.
  $.ajax({
    type: "POST",
    url: Drupal.settings.panelsAjaxURL + "/add-config/" + $('#panel-did').val() + '/' + id,
    data: '',
    global: true,
    success: Drupal.Panels.Subform.bindAjaxResponse,
  	error: function() { alert("An error occurred."); $('#panels-modal').unmodalContent(); },
    dataType: 'json'
  });
  return false;
}

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
}

/** ripped from Drupal cause it's not easy to re-autoattach cleanly */
Drupal.Panels.Subform.bindAutocomplete = function() {
  var acdb = [];
  $('.panels-modal-content input.autocomplete').each(function () {
    var uri = this.value;
    if (!acdb[uri]) {
      acdb[uri] = new Drupal.ACDB(uri);
    }
    var input = $('#' + this.id.substr(0, this.id.length - 13))
      .attr('autocomplete', 'OFF')[0];
    $(input.form).submit(Drupal.autocompleteSubmit);
    new Drupal.jsAC(input, acdb[uri]);
  });
}

Drupal.Panels.Subform.bindAjaxResponse = function(data) {
  // On success, append the returned HTML to the panel's element.
  if (data.type == 'display') {
    if (data.debug) {
      alert(data.debug);
    }
    // append the output
    $('#modalContent span.modal-title').html(data.title);
    $('#modalContent div.modal-content').html(data.output);

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
      Drupal.Panels.bindCheckBox('#override-title-checkbox', '#override-title-textfield');
    }

    if ($('#use-pager-checkbox').size()) {
      Drupal.Panels.bindCheckBox('#use-pager-checkbox', '#use-pager-textfield');
    }

    // hack: allow collapsible textareas to work
    Drupal.Panels.Subform.bindCollapsible();

    Drupal.Panels.Subform.bindAutocomplete();
  }
  else if (data.type == 'add') {
    // Give it all the goodies that our existing panes have.   
    $('#panel-pane-' + data.area).append(data.output);
    
    Drupal.Panels.attachPane('#panel-pane-' + data.id);
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  }
  else if (data.type == 'replace') {
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible').html(data.output);
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible').each(Drupal.Panels.bindPortlet);
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  } 
  else {
    // just dismiss the dialog.
    $('#panels-modal').unmodalContent();
  } 
}

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

}
