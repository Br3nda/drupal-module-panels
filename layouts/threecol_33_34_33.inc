<?php
// $Id: threecol_33_34_33.inc,v 1.5.6.5 2007/12/17 22:09:19 merlinofchaos Exp $
/**
 * implementation of hook_panels_layouts
 */
function panels_threecol_33_34_33_panels_layouts() {
  $items['threecol_33_34_33'] = array(
    'module' => 'panels',
    'title' => t('Three column 33/34/33'),
    'icon' => 'layouts/threecol_33_34_33.png',
    'theme' => 'panels_threecol_33_34_33',
    'css' => 'layouts/threecol_33_34_33.css',
    'panels' => array('left' => t('Left side'), 'middle' => t('Middle column'), 'right' => t('Right side')),
  );

  return $items;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_threecol_33_34_33($id, $content) {
  if ($id) {
    $idstr = " id='$id'";
  }

  $output = <<<EOT
<div class="panel-display panel-3col-33 clear-block" $idstr>
  <div class="panel-panel panel-col-first">
    <div class="inside">$content[left]</div>
  </div>

  <div class="panel-panel panel-col">
    <div class="inside">$content[middle]</div>
  </div>

  <div class="panel-panel panel-col-last">
    <div class="inside">$content[right]</div>
  </div>
</div>
EOT;
  return $output;
}
