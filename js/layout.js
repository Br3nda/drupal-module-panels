// $Id: layout.js,v 1.1.2.1 2007/07/09 23:45:59 merlinofchaos Exp $
/**
 * @file layout.js 
 *
 * Contains javascript to make layout modification a little nicer.
 */

Drupal.PanelsLayout = {};
Drupal.PanelsLayout.autoAttach = function() {
  $('div.form-item div.layout-icon').click(function() {
    $(this).prev().find('input').attr('checked', true);
  });
}

$(Drupal.PanelsLayout.autoAttach);
