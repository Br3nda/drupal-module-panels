<?php
// $Id: panels_layouts.inc,v 1.2 2010/09/07 09:42:12 sdboyer Exp $

$plugin = array(
  'schema' => 'panels_layout',
  'access' => 'administer panels layouts',

  'menu' => array(
    'menu prefix' => 'admin/build/panels',
    'menu item' => 'layouts',
    'menu title' => 'Layouts',
    'menu description' => 'Add, edit or delete custom content layouts.',
  ),

  'title singular' => t('layout'),
  'title singular proper' => t('Layout'),
  'title plural' => t('layouts'),
  'title plural proper' => t('Layouts'),

  'handler' => array(
    'class' => 'panels_layouts_ui',
    'parent' => 'ctools_export_ui',
  ),
);

