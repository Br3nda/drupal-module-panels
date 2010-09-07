<?php
// $Id: onecol.inc,v 1.3 2010/09/07 09:42:12 sdboyer Exp $

/**
 * implementation of hook_panels_layouts()
 */
// Plugin definition
$plugin = array(
  'title' => t('Single column'),
  'category' => t('Columns: 1'),
  'icon' => 'onecol.png',
  'theme' => 'panels_onecol',
  'css' => 'onecol.css',
  'panels' => array('middle' => t('Middle column')),
);
