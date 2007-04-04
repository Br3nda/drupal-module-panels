<?php
/**
 * Callback function to supply a list of content types.
 */
function panels_views_panels_content_types() {
  if (module_exists('views')) {
    // Only valid if views module loaded.
    $items['views'] = array(
      'title' => t('View'),
      'content_types' => 'panels_admin_content_types_views',
      'render callback' => 'panels_content_views',
      'add callback' => 'panels_admin_add_views',
      'edit callback' => 'panels_admin_edit_views',
      'title callback' => 'panels_admin_title_views',
      'validate callback' => 'panels_admin_validate_views',
    );
    return $items;
  }
}

/**
 * Output function for the 'views' content type. Outputs a views
 * based on the module and delta supplied in the configuration.
 */
function panels_content_views($conf, $panel_args) {
  $view = views_get_view($conf['view']);
  if ($view) {
    if (function_exists('views_access') && !views_access($view)) {
      return NULL;
    }
    $arguments = explode('/', $_GET['q']);
    foreach ($arguments as $id => $arg) {
      $args = str_replace("%$id", $arg, $args);
    }

    $args = preg_replace('/\\/%\d/', '', $args);
    $args = $args ? explode('/', $args) : array();

    if ($conf['panel_args'] && is_array($panel_args)) {
      $args = array_merge($panel_args, $args);
    }
    
    if ($conf['url']) {
      $view->url = $conf['url'];
    }

    $block->module = 'views';
    $block->delta = $view->name;
    $view_type = $conf['type'] == 'embed' ? 'page' : $conf['type'];
    $title = $conf['override_title'] ? $conf['override_title_text'] : views_get_title($view, $view_type);
    $block->subject = filter_xss_admin($title);
    $pager_id = empty($conf['use_pager']) ? 0 : intval($conf['pager_id']);
    $block->content = views_build_view($conf['type'], $view, $args, $pager_id, intval($conf['nodes_per_page']));
  }
  return $block;
}
/**
 * Return all content types available.
 */
function panels_admin_content_types_views() {
  $types = array();

  $result = db_query("SELECT * FROM {view_view}");
  while ($view = db_fetch_object($result)) {
    $title = views_get_title($view, 'admin');
    $types[$view->name] = array(
      'title' => strip_tags($title ? $title : $view->name),
      'icon' => 'views.png',
      'description' => filter_xss_admin($view->description),
    );
  }

  views_load_cache();
  $default_views = _views_get_default_views();
  $views_status = variable_get('views_defaults', array());
  foreach ($default_views as $view) {
    if (!$views[$view->name] && 
      ($views_status[$view->name] == 'enabled' || (!$views_status[$view->name] && !$view->disabled))) {
      $title = views_get_title($view, 'admin');
      $types[$view->name] = array(
        'title' => strip_tags($title ? $title : $view->name),
        'icon' => 'views.png',
        'description' => filter_xss_admin($view->description),
      );
    }
  }
  uasort($types, 'panels_title_sort');
  return $types;
}

/**
 * Returns the form for a new view.
 */
function panels_admin_add_views($id) {
  $conf = array();
  $view = views_get_view($id);
  if (!$view) { 
    return;
  }
  $conf['view'] = $id;
  if ($view->page) {
    $conf['type'] = 'page';
    $conf['nodes_per_page'] = $view->nodes_per_page;
  }
  else {
    $conf['type'] = 'block';
    $conf['nodes_per_page'] = $view->nodes_per_block;
  }
  $conf['type'] = 'embed';
  $conf['pager_id'] = 1;
  $conf['use_pager'] = 0;
  return panels_admin_edit_views($conf);
}

/**
 * Returns an edit form for a block.
 */
function panels_admin_edit_views($conf) {
  $form['view'] = array(
    '#type' => 'value',
    '#default_value' => $conf['view'],
  );
  $form['type'] = array(
    '#type' => 'select',
    '#default_value' => $conf['type'],
    '#title' => t('View type'),
    '#description' => t('Select which type of the view to display.'),
    '#options' => array('page' => t('Page'), 'block' => t('Block'), 'embed' => t('Embedded')),
  );

  $form['aligner_start'] = array(
    '#value' => '<div class="option-text-aligner">',
  );
  $form['override_title'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['override_title'],
    '#title' => t('Override title'),
    '#id' => 'override-title-checkbox',
  );
  $form['override_title_text'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['override_title_text'],
    '#size' => 35,
    '#id' => 'override-title-textfield',
  );
  $form['aligner_stop'] = array(
    '#value' => '</div><div style="clear: both; padding: 0; margin: 0"></div>',
  );
  
  $form['pager_aligner_start'] = array(
    '#value' => '<div class="option-text-aligner">',
  );
  $form['use_pager'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use pager'),
    '#default_value' => $conf['use_pager'],
    '#id' => 'use-pager-checkbox',
  );
  $form['pager_id'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['pager_id'],
    '#title' => t('Pager ID'),
    '#size' => 4,
    '#id' => 'use-pager-textfield',
  );
  $form['pager_aligner_stop'] = array(
    '#value' => '</div><div style="clear: both; padding: 0; margin: 0"></div>',
  );

  $form['nodes_per_page'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['nodes_per_page'],
    '#title' => t('Num posts'),
    '#size' => 4,
    '#description' => t('Select the number of posts to display, or 0 to display all results.'),
  );

  $form['panel_args'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send arguments'),
    '#default_value' => $conf['panel_args'],
    '#description' => t('Select this to send URL arguments from the the panel to the view.'),
  );

  $form['args'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['args'],
    '#title' => t('Arguments'),
    '#size' => 30,
    '#description' => t('Additional arguments to send to the view as if they were part of the URL in the form of arg1/arg2/arg3.'),
  );

  $form['url'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['url'],
    '#title' => t('Override URL'),
    '#size' => 30,
    '#description' => t('If this is set, override the View URL; this can sometimes be useful to set to the panel URL'),
  );

  return $form;
}

/**
 * Returns the administrative title for a type.
 */
function panels_admin_title_views($conf) {
  $view = views_get_view($conf['view']);
  $title = views_get_title($view, 'admin');
  return $title ? $title : $view->name;
}
