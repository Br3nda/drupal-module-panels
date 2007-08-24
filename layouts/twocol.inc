<?php
// $Id: twocol.inc,v 1.6.6.3 2007/08/24 02:49:06 merlinofchaos Exp $
/**
 * implementation of hook_panels_layouts
 */
function panels_twocol_panels_layouts() {
  $items['twocol'] = array(
    'module' => 'panels',
    'title' => t('Two column'),
    'icon' => 'layouts/twocol.png',
    'theme' => 'panels_twocol',
    'css' => 'layouts/twocol.css',
    'panels' => array('left' => t('Left side'), 'right' => t('Right side')),
  );

  return $items;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_twocol($id, $content) {
  if ($id) {
    $idstr = " id='$id'";
  }

  $output = <<<EOT
<div class="panel-2col clear-block" $idstr>
  <div class="panel-col-first">
    <div class="inside">$content[left]</div>
  </div>

  <div class="panel-col-last">
    <div class="inside">$content[right]</div>
  </div>
</div>
EOT;
  return $output;
}
