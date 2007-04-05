
/** add content button **/
Drupal.Panels = {};

Drupal.Panels.clickAdd = function() {
  id = $(this)[0].id.replace('pane-add-', '');
  // show the empty dialog right away.
  $('#panels-modal').modalContent({
      opacity: '.40', 
      background: '#fff'
    }
  );
  $('#modalContent .modal-content').html($('div#panels-throbber').html());
  $.ajax({
    type: "POST",
    url: "/panels/ajax/add-content/" + $('#panel-did').val() + '/' + id,
    global: true,
    success: Drupal.Panels.bindAjaxResponse,
    dataType: 'json'
  });
  return false;
}

/** hide all button **/
Drupal.Panels.clickHideAll = function() {
  $('.panel-portlet .content').hide();
  $('.panel-portlet .toggle').addClass('toggle-collapsed');
  return false;
}

/** show all button **/
Drupal.Panels.clickShowAll = function() {
  $('.panel-portlet .content').show();
  $('.panel-portlet .toggle').removeClass('toggle-collapsed');
  return false;
}

/** Configure pane button */
Drupal.Panels.bindClickConfigure = function (o) {
  $('input.pane-configure').click();
  $('input.pane-configure').click(function() {
    id = $(this)[0].id.replace('edit-button-', '').replace('-configure', '');
    // show the empty dialog right away.
    $('#panels-modal').modalContent({
        opacity: '.40', 
        background: '#fff'
      }
    );
    $('#modalContent .modal-content').html($('div#panels-throbber').html());
    $.ajax({
      type: "POST",
      url: "/panels/ajax/configure/" + $('#panel-did').val() + '/' + id,
      global: true,
      success: Drupal.Panels.bindAjaxResponse,
      dataType: 'json'
    });
    return false;
  });
}

/** Delete pane button **/
Drupal.Panels.bindClickDelete = function(o) {
  o.find('input.pane-delete').click(function() {
    if (confirm('Remove this pane?')) {
      id = $(this)[0].id.replace('edit-button-', '').replace('-delete', '');
      $('#panel-pane-' + id).remove();
    }
    return false;
  });
}

/** Bind an item to a checkbox to auto disable it when unchecked **/
Drupal.Panels.bindCheckBox = function(checkbox, gadget) {
  var clickCheckBox = function() {
    if ($(checkbox).attr('checked')) {
      $(gadget).attr('disabled', false);
    }
    else {
      $(gadget).attr('disabled', true);
    }    
  }

  $(checkbox).change(); // unset any existing
  $(checkbox).change(clickCheckBox);
  clickCheckBox();
}

Drupal.Panels.Subform = {};

/** Basic submit on a subform **/
Drupal.Panels.Subform.bindClickSubmit = function() {
  $(this).ajaxSubmit({
    url: '/panels/ajax/submit-form/' +  $('#panel-did').val(),
    method: 'post',
    after: Drupal.Panels.bindAjaxResponse,
    dataType: 'json'
  });
  return false;
}

Drupal.Panels.Subform.bindClickAddLink = function() {
  id = $(this)[0].id;
  // show the empty dialog right away.
  $.ajax({
    type: "POST",
    url: "/panels/ajax/add-config/" + $('#panel-did').val() + '/' + id,
    global: true,
    success: Drupal.Panels.bindAjaxResponse,
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

Drupal.Panels.bindAjaxResponse = function(data) {
  // On success, append the returned HTML to the panel's element.
  if (data.type == 'display') {
    if (data.debug) {
      alert(data.debug);
    }
    // append the output
    $('#modalContent span.modal-title').html(data.title);
    $('#modalContent div.modal-content').html(data.output);

    // Bind forms to ajax submit.
    $('div.panels-modal-content form').unsubmit(); // be safe here.
    $('div.panels-modal-content form').submit(Drupal.Panels.Subform.bindClickSubmit);

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
    // add the data to our sortables
    $('#panel-pane-' + data.area).append(data.output)
      .SortableAddItem(document.getElementById('panel-pane-' + data.id));
    
    Drupal.Panels.bindClickConfigure($('#panel-pane-' + data.id));
    Drupal.Panels.bindClickDelete($('#panel-pane-' + data.id));
    $('#panel-pane-' + data.id).each(Drupal.Panels.bindPortlet);
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  }
  else if (data.type == 'replace') {
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible').html(data.output);
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible').each(Drupal.Panels.bindPortlet);
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  } 
}

Drupal.Panels.bindPortlet = function() {
  var handle = $(this).find('.title');
  var content = $(this).find('.content');
  if (content.length) {
    var toggle = $('<span class="toggle toggle-collapsed"></span>');
    handle.before(toggle);
    toggle.click(function() {
      content.slideToggle(20);
      toggle.toggleClass('toggle-collapsed');
    });
    handle.click(function() {
      content.slideToggle(20);
      toggle.toggleClass('toggle-collapsed');
    });
    content.hide();
  }
}

Drupal.Panels.autoAttach = function() {
  // Show javascript only items.
  $('span.panels-js-only').css('display', 'inline');

  // add .panel-portlet to all draggable blocks
  $('div.panel-pane').addClass('panel-portlet');

  // make columns sortable
  $('div.panels-display').Sortable({
    accept: 'panel-portlet',
    handle: 'div.grabber',
    helperclass: 'helperclass',
    hgverclass: 'hoverclass',
    tolerance: 'intersect',
    fx: 100,
    revert: true,
    floats: true
  });

  $('div.panel-portlet').each(Drupal.Panels.bindPortlet);

  $('#panels-dnd-save').click(function() {
    serial = $.SortSerialize();
    save = $('#panels-dnd-save').val();
    $.ajax({
      type: "POST",
      url: "/panels/ajax/save-display/" + $('#panel-did').val(),
      data: serial.hash,
      global: true,
      success: function() {
        $('#panel-op').val(save);
        $('#panels-edit-display')[0].submit();
      }
    });
    return false;
  });

  // Bind buttons.
  $('.pane-add').click(Drupal.Panels.clickAdd);
  $('#panels-hide-all').click(Drupal.Panels.clickHideAll);
  $('#panels-show-all').click(Drupal.Panels.clickShowAll);
  Drupal.Panels.bindClickConfigure($(document));
  Drupal.Panels.bindClickDelete($(document));
}

$(document).ready(Drupal.Panels.autoAttach);
