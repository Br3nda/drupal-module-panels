<?php
// $Id: twocol.inc,v 1.2 2010/01/21 07:11:42 sdboyer Exp $

// Plugin definition
$plugin = array(
  'title' => t('Two column'),
  'icon' => 'twocol.png',
  'theme' => 'panels_twocol',
  'css' => 'twocol.css',
  'panels' => array(
    'left' => t('Left side'),
    'right' => t('Right side')
  ),
);
