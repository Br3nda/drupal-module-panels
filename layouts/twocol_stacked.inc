<?php
// $Id: twocol_stacked.inc,v 1.5.6.2 2007/05/22 22:08:01 merlinofchaos Exp $

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
<div class="panel-2col-stacked" $idstr>
  <div class="panel-col-top">
    <div class="inside">$content[top]</div>
  </div>
  <div class="panel-col-first">
    <div class="inside">$content[left]</div>
  </div>

  <div class="panel-col-last">
    <div class="inside">$content[right]</div>
  </div>
  <div class="panel-col-bottom">
    <div class="inside">$content[bottom]</div>
  </div>
</div>
<br class="panel-clearer" />
EOT;
  return $output;
}
