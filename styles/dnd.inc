<?php
// $Id: dnd.inc,v 1.1.2.1 2007/08/29 22:02:04 merlinofchaos Exp $
/**
 * @file styles/dnd.inc
 * Definition of the drag and drop panel style.
 */

/**
 * Implementation of hook_panels_style_info().
 */
function panels_dnd_panels_styles() {
  return array(
    'dnd' => array(
      'title' => t('dnd'),
      'hidden' => TRUE,
      'panels implementations' => array(),
      'render' => 'panels_dnd_style_render',
      'settings form' => 'panels_dnd_style_settings_form',
      'settings validate' => 'panels_dnd_style_settings_validate',
    ),
  );
}

/**
 * Implementation of hook_panels_panel_style_render().
 */
function panels_dnd_style_render($display, $panel_id, $panes) {
  return theme('panels_dnd_style', $display, $panel_id, $panes);
}

/**
 * Panel style: dnd of panes (panes are items in an unordered dnd).
 */
function theme_panels_dnd_style($display, $panel_id, $panes) {
  $items = array();

  foreach ($panes as $pane_id => $pane) {
    $items[] = theme('panels_pane', $pane, $display);
  }
  return theme('item_dnd', $items);
}
