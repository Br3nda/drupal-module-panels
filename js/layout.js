// $Id: layout.js,v 1.1.2.3 2008/05/27 19:25:57 sdboyer Exp $
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
};

$(Drupal.Panels.Layout.autoAttach);
