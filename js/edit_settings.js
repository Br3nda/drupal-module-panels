// $Id: edit_settings.js,v 1.1.2.3 2007/12/06 02:35:45 merlinofchaos Exp $
/**
 * @file edit_settings.js 
 *
 * Contains the javascript for the Panels display editor.
 */

/** edit settings button **/
Drupal.Panels.clickEdit = function() {
  // show the empty dialog right away.
  $('#panels-modal').modalContent({
      opacity: '.40', 
      background: '#fff'
    }
  );
  $('#modalContent .modal-content').html($('div#panels-throbber').html());
  $.ajax({
    type: "POST",
    url: Drupal.settings.panels.ajaxURL + '/' + $('#panel-settings-style').val(),
    data: '',
    global: true,
    success: Drupal.Panels.Subform.bindAjaxResponse,
    error: function() { alert("An error occurred."); $('#panels-modal').unmodalContent(); },
    dataType: 'json'
  });
  return false;
}

Drupal.Panels.autoAttach = function() {
  // Bind buttons.
  Drupal.Panels.Subform.createModal();
  $('input#panels-style-settings').click(function() { return false; } );
  $('input#panels-style-settings').click(Drupal.Panels.clickEdit);
}

$(Drupal.Panels.autoAttach);
