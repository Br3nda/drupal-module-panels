<?php
// $Id: threecol_33_34_33_stacked.inc,v 1.5.6.5 2007/11/27 17:43:33 merlinofchaos Exp $

/**
 * implementation of hook_panels_layouts
 */
function panels_threecol_33_34_33_stacked_panels_layouts() {
  $items['threecol_33_34_33_stacked'] = array(
    'module' => 'panels',
    'title' => t('Three column 33/34/33 stacked'),
    'icon' => 'layouts/threecol_33_34_33_stacked.png',
    'theme' => 'panels_threecol_33_34_33_stacked',
    'css' => 'layouts/threecol_33_34_33_stacked.css',
    'panels' => array('top' => t('Top'), 'left' => t('Left side'), 'middle' => t('Middle column'), 'right' => t('Right side'), 'bottom' => t('Bottom')),
  );

  return $items;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_threecol_33_34_33_stacked($id, $content) {
  if ($id) {
    $idstr = " id='$id'";
  }

  $output = <<<EOT
<div class="panel-3col-33-stacked  clear-block" $idstr>
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
