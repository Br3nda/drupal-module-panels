<?php
// $Id: list.inc,v 1.1.2.1 2007/08/29 22:02:04 merlinofchaos Exp $
/**
 * @file styles/list.inc
 * Definition of the 'list' panel style.
 */

/**
 * Implementation of hook_panels_style_info().
 */
function panels_list_panels_styles() {
  return array(
    'list' => array(
      'title' => t('List'),
      'description' => t('Presents the panes in the form of an HTML list.'),
      'panels implementations' => array(),
      'render panel' => 'panels_list_style_render_panel',
      'settings form' => 'panels_list_style_settings_form',
      'settings validate' => 'panels_list_style_settings_validate',
    ),
  );
}

/**
 * Implementation of hook_panels_panel_style_render().
 */
function panels_list_style_render_panel($display, $panel_id, $panes) {
  return theme('panels_list_style', $display, $panel_id, $panes);
}

/**
 * Panel style: list of panes (panes are items in an unordered list).
 */
function theme_panels_list_style($display, $panel_id, $panes) {
  $items = array();

  foreach ($panes as $pane_id => $pane) {
    $items[] = theme('panels_pane', $pane, $display);
  }
  return theme('item_list', $items);
}

// TODO: Present a settings form to choose between ordered/unordered.