// $Id: checkboxes.js,v 1.1.2.2 2007/11/30 19:55:31 merlinofchaos Exp $

if (!Drupal.Panels) {
  Drupal.Panels = {};
}

Drupal.Panels.Checkboxes = {};

/** Bind an item to a checkbox to auto disable it when unchecked **/
Drupal.Panels.Checkboxes.bindCheckbox = function(checkbox, gadget) {
  var clickCheckBox = function() {
    var status = !$(checkbox).attr('checked');

    for (var i in gadget) {
      $(gadget[i]).attr('disabled', status);
    }
  }

  $(checkbox).change(); // unset any existing
  $(checkbox).change(clickCheckBox);
  clickCheckBox();
}


Drupal.Panels.Checkboxes.bindCheckboxes = function() {
  if (Drupal.settings && Drupal.settings.panels && Drupal.settings.panels.checkboxes) {
    for (var checkbox in Drupal.settings.panels.checkboxes) {
      if (!$(checkbox + '.checkbox-processed').size()) {
        Drupal.Panels.Checkboxes.bindCheckbox(checkbox, Drupal.settings.panels.checkboxes[checkbox]);
        $(checkbox).addClass('checkbox-processed');
      }
    }
  }

}

$(Drupal.Panels.Checkboxes.bindCheckboxes);
