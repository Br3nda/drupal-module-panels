<?php
// $Id: onecol.inc,v 1.1.2.7 2008/07/29 10:54:33 sdboyer Exp $


/**
 * implementation of hook_panels_layouts
 */
function panels_onecol_panels_layouts() {
  $items['onecol'] = array(
    'title' => t('Single column'),
    'icon' => 'layouts/onecol.png',
    'theme' => 'panels_onecol',
    'css' => 'layouts/onecol.css',
    'panels' => array('middle' => t('Middle column')),
    'settings form' => 'panels_onecol_settings',
  );

  return $items;
}

function panels_onecol_settings($display, $layout, $settings) {
  $form['plain'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use minimalistic (plain) output'),
    '#default_value' => isset($settings['plain']) ? $settings['plain'] : 0,
    '#description' => t('Enable this option to output minimal panels display HTML markup.'),
  );
  return $form;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_onecol($id, $content, $settings) {
  if ($id) {
    $idstr = " id='$id'";
  }

  if (!empty($settings['plain'])) {
    $output = <<<EOT
<div class="panel-display"$idstr>$content[middle]</div>
EOT;
  }
  else {
    $output = <<<EOT
<div class="panel-display panel-1col clear-block"$idstr>
  <div class="panel-panel panel-col">
    <div>$content[middle]</div>
  </div>
</div>
EOT;
  }
  return $output;
}

