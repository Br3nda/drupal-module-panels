<?php
// $Id: default.inc,v 1.1.2.3 2007/12/06 02:35:46 merlinofchaos Exp $
/**
 * Implementation of hook_panels_style_info().
 */
function panels_default_panels_styles() {
  return array(
    'default' => array(
      'title' => t('Default'),
      'description' => t('The default panel rendering style; displays each pane with a separator.'),
      // This can restrict a style to specific implementations
      'panels implementations' => array(),
      // This is a theme function
      'render panel' => 'panels_default_style_render_panel',
    ),
  );
}

/**
 * Panel style: default (panes separated by whitespace).
 */
function theme_panels_default_style_render_panel($display, $panel_id, $panes, $settings) {
  $output = '';

  $print_separator = FALSE;
  foreach ($panes as $pid => $content) {
    // Add the separator if we've already displayed a pane.
    if ($print_separator) {
      $output .= '<div class="panel-separator"></div>';
    }
    $output .= $text = theme('panels_pane', $content, $display->content[$pid], $display); 

    // If we displayed a pane, this will become true; if not, it will become false.
    $print_separator = (bool) $text;
  }
  return $output;
}

