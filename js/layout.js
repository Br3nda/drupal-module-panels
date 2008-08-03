// $Id: layout.js,v 1.1.2.4 2008/08/03 04:34:00 sdboyer Exp $
/**
 * @file layout.js 
 *
 * Contains javascript to make layout modification a little nicer.
 */

Drupal.Panels.Layout = {};
Drupal.Panels.Layout.autoAttach = function() {
  $('div.form-item .layout-icon').click(function() {
    $(this).prev().attr('checked', true);
  });
};

$(Drupal.Panels.Layout.autoAttach);
