<?php
// $Id: threecol_25_50_25_stacked.inc,v 1.3 2010/09/07 09:42:12 sdboyer Exp $

// Plugin definition
$plugin = array(
  'title' => t('Three column 25/50/25 stacked'),
  'category' => t('Columns: 3'),
  'icon' => 'threecol_25_50_25_stacked.png',
  'theme' => 'panels_threecol_25_50_25_stacked',
  'css' => 'threecol_25_50_25_stacked.css',
  'panels' => array(
    'top' => t('Top'),
    'left' => t('Left side'),
    'middle' => t('Middle column'),
    'right' => t('Right side'),
    'bottom' => t('Bottom'),
  ),
);
