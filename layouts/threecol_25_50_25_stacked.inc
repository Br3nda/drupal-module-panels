<?php
// $Id: threecol_25_50_25_stacked.inc,v 1.5.6.4 2007/08/24 02:49:06 merlinofchaos Exp $

/**
 * implementation of hook_panels_layouts
 */
function panels_threecol_25_50_25_stacked_panels_layouts() {
  $items['threecol_25_50_25_stacked'] = array(
    'module' => 'panels',
    'title' => t('Three column 25/50/25 stacked'),
    'icon' => 'layouts/threecol_25_50_25_stacked.png',
    'theme' => 'panels_threecol_25_50_25_stacked',
    'css' => 'layouts/threecol_25_50_25_stacked.css',
    'panels' => array('top' => t('Top'), 'left' => t('Left side'), 'middle' => t('Middle column'), 'right' => t('Right side'), 'bottom' => t('Bottom')),
  );

  return $items;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_threecol_25_50_25_stacked($id, $content) {
  if ($id) {
    $idstr = " id='$id'";
  }

  $output = <<<EOT
<div class="panel-3col-stacked clear-block" $idstr>
  <div class="panel-col-top">
    <div class="inside">$content[top]</div>
  </div>
  <div class="center-wrapper">
    <div class="panel-col-first">
      <div class="inside">$content[left]</div>
    </div>

    <div class="panel-col">
      <div class="inside">$content[middle]</div>
    </div>

    <div class="panel-col-last">
      <div class="inside">$content[right]</div>
    </div>
  </div>
  <div class="panel-col-bottom">
    <div class="inside">$content[bottom]</div>
  </div>
</div>
EOT;
  return $output;
}
