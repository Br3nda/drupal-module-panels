<?php
// $Id: twocol_stacked.inc,v 1.5.6.6 2007/12/17 22:09:19 merlinofchaos Exp $

/**
 * implementation of hook_panels_layouts
 */
function panels_twocol_stacked_panels_layouts() {
  $items['twocol_stacked'] = array(
    'module' => 'panels',
    'title' => t('Two column stacked'),
    'icon' => 'layouts/twocol_stacked.png',
    'theme' => 'panels_twocol_stacked',
    'css' => 'layouts/twocol_stacked.css',
    'panels' => array('top' => t('Top'), 'left' => t('Left side'), 'right' => t('Right side'), 'bottom' => t('Bottom')),
  );

  return $items;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_twocol_stacked($id, $content) {
  if ($id) {
    $idstr = " id='$id'";
  }

  $output = <<<EOT
<div class="panel-2col-stacked clear-block panel-display" $idstr>
  <div class="panel-col-top panel-panel">
    <div class="inside">$content[top]</div>
  </div>
  <div class="center-wrapper">
    <div class="panel-col-first panel-panel">
      <div class="inside">$content[left]</div>
    </div>

    <div class="panel-col-last panel-panel">
      <div class="inside">$content[right]</div>
    </div>
  </div>
  <div class="panel-col-bottom panel-panel">
    <div class="inside">$content[bottom]</div>
  </div>
</div>
EOT;
  return $output;
}
