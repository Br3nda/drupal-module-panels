<?php
/**
 * implementation of hook_panels_layouts
 */
function panels_threecol_25_50_25_panels_layouts() {
  $items['threecol_25_50_25'] = array(
    'title' => t('Three column 25/50/25'),
    'icon' => 'layouts/threecol_25_50_25.png',
    'theme' => 'panels_threecol_25_50_25',
    'css' => 'layouts/threecol_25_50_25.css',
    'panels' => array(
      'left' => t('Left side'),
      'middle' => t('Middle column'),
      'right' => t('Right side')
    ),
  );

  return $items;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_threecol_25_50_25($id, $content) {
  if ($id) {
    $idstr = " id='$id'";
  }

  $output = <<<EOT
<div class="panel-display panel-3col clear-block" $idstr>
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
