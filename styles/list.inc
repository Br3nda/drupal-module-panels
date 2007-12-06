<?php
// $Id: list.inc,v 1.1.2.5 2007/12/06 02:35:46 merlinofchaos Exp $
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
 * Panel style: list of panes (panes are items in an unordered list).
 */
function theme_panels_list_style_render_panel($display, $panel_id, $panes, $settings) {
  $items = array();

  foreach ($panes as $pane_id => $content) {
    $items[] = theme('panels_pane', $content, $display->content[$pid], $display);
  }
  return theme('item_list', $items, NULL, $settings['list_type']);
}

function panels_list_style_settings_form($style_settings) {
  $form['list_type'] = array(
    '#type' => 'select',
    '#title' => t('List type'),
    '#options' => array('ul' => t('Unordered'), 'ol' => t('Ordered')),
    '#default_value' => isset($style_settings['list_type']) ? $style_settings['list_type'] : 'ul',
  );

  return $form;
}
