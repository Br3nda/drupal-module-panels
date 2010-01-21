<?php
// $Id: threecol_25_50_25_stacked.inc,v 1.2 2010/01/21 07:11:42 sdboyer Exp $

// Plugin definition
$plugin = array(
  'title' => t('Three column 25/50/25 stacked'),
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
