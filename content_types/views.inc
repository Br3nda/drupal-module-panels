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
function panels_content_views($conf) {
  $view = views_get_view($conf['view']);
  if ($view) {
    if (function_exists('views_access') && !views_access($view)) {
      return NULL;
    }
    $arguments = explode('/', $_GET['q']);
    $args = $conf['args'];
    
    foreach ($arguments as $id => $arg) {
      $args = str_replace("%$id", $arg, $args);
    }

    $args = preg_replace('/\\/%\d/', '', $args);
    $args = $args ? explode('/', $args) : array();

    if ($conf['url']) {
      $view->url = $conf['url'];
    }

    $block->module = 'views';
    $block->delta = $view->name;
    $view_type = $conf['type'] == 'embed' ? 'page' : $conf['type'];
    $block->subject = $conf['show_title'] ? filter_xss_admin(views_get_title($view, $view_type)) : NULL;
    $block->content = views_build_view($conf['type'], $view, $args, intval($conf['pager_id']), intval($conf['nodes_per_page']));
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
  $conf['pager_id'] = 0;
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
  $form['pager_id'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['pager_id'],
    '#title' => t('Pager ID'),
    '#size' => 4,
    '#description' => t('Select the numeric pager ID to use, or 0 to not have use paging. Select "1" if you aren\'t sure what this means'),
  );

  $form['nodes_per_page'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['nodes_per_page'],
    '#title' => t('Posts to Display'),
    '#size' => 4,
    '#description' => t('Select the number of posts to display, or 0 to display all results.'),
  );

  $form['args'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['args'],
    '#title' => t('View arguments'),
    '#size' => 12,
    '#description' => t('Arguments to send to the view as if they were part of the URL in the form of arg1/arg2/arg3. You can use %0, %1, %2, etc, to use arguments from the actual URL. For example, if your panel URL is foo/bar, and someone hits foo/bar/5 use %2 to get the 5.'),
  );

  $form['url'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['url'],
    '#title' => t('Override URL'),
    '#size' => 12,
    '#description' => t('If this is set, override the View URL; this can sometimes be useful to set to the panel URL'),
  );

  $form['show_title'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['show_title'],
    '#title' => t('Display view title'),
    '#description' => t('If checked, the title of the view will be displayed.'),
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
