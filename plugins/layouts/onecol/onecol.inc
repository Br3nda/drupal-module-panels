<?php
// $Id: onecol.inc,v 1.2 2010/01/21 07:11:42 sdboyer Exp $

/**
 * implementation of hook_panels_layouts()
 */
// Plugin definition
$plugin = array(
  'title' => t('Single column'),
  'icon' => 'onecol.png',
  'theme' => 'panels_onecol',
  'css' => 'onecol.css',
  'panels' => array('middle' => t('Middle column')),
);
