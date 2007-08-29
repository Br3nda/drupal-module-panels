<?php
// $Id: default.inc,v 1.1.2.1 2007/08/29 22:02:04 merlinofchaos Exp $
/**
 * Implementation of hook_panels_style_info().
 */
function panels_default_panels_styles() {
  return array(
    'default' => array(
      'title' => t('Default'),
      'panels implementations' => array(),
      'render panel' => 'panels_default_style_render_panel',
    ),
  );
}

/**
 * Implementation of hook_panels_panel_style_render().
 */
function panels_default_style_render_panel($display, $panel_id, $panes) {
  return theme('panels_default_style', $display, $panel_id, $panes);
}

/**
 * Panel style: default (panes separated by whitespace).
 */
function theme_panels_default_style($display, $panel_id, $panes) {
  $output = '';

  foreach ($panes as $pane) {
    if (isset($output)) {
      $output .= '<div class="panel-separator"></div>';
    }
    $output .= theme('panels_pane', $pane, $display); 
  }
  return $output;
}

