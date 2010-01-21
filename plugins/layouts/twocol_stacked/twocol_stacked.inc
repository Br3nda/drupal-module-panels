<?php
// $Id: twocol_stacked.inc,v 1.2 2010/01/21 07:11:42 sdboyer Exp $

// Plugin definition
$plugin = array(
  'title' => t('Two column stacked'),
  'icon' => 'twocol_stacked.png',
  'theme' => 'panels_twocol_stacked',
  'css' => 'twocol_stacked.css',
  'panels' => array(
    'top' => t('Top'),
    'left' => t('Left side'),
    'right' => t('Right side'),
    'bottom' => t('Bottom')
  ),
);
