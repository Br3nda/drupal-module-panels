// $Id: layout.js,v 1.1.2.2 2007/12/06 02:35:45 merlinofchaos Exp $
/**
 * @file layout.js 
 *
 * Contains javascript to make layout modification a little nicer.
 */

Drupal.Panels.Layout = {};
Drupal.Panels.Layout.autoAttach = function() {
  $('div.form-item div.layout-icon').click(function() {
    $(this).prev().find('input').attr('checked', true);
  });
}

$(Drupal.Panels.Layout.autoAttach);
