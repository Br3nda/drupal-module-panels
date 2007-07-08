<?php
// $Id: onecol.inc,v 1.1.2.2 2007/07/08 03:31:36 merlinofchaos Exp $
/**
 * implementation of hook_panels_layouts
 */
function panels_onecol_panels_layouts() {
  $items['onecol'] = array(
    'module' => 'panels',
    'title' => t('Single column'),
    'icon' => 'layouts/onecol.png',
    'theme' => 'panels_onecol',
    'css' => 'layouts/onecol.css',
    'panels' => array('middle' => t('Middle column')),
  );

  return $items;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_onecol($id, $content) {
  if ($id) {
    $idstr = " id='$id'";
  }

  $output = <<<EOT
<div class="panel-1col" $idstr>
  <div class="panel-col">
    <div>$content[middle]</div>
  </div>
</div>
<br class="panel-clearer" />
EOT;
  return $output;
}
