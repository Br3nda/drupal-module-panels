<?php
/**
 * Callback function to supply a list of content types.
 */
function panels_node_panels_content_types() {
  $items['node'] = array(
    'name' => t('Node'),
    'single' => TRUE, // only provides a single content type
    'content_types' => 'panels_admin_content_types_node',
    'render callback' => 'panels_content_node',
    'add callback' => 'panels_admin_add_node',
    'edit callback' => 'panels_admin_edit_node',
    'title callback' => 'panels_admin_title_node',
    'validate callback' => 'panels_admin_validate_node',
  );
  return $items;
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_node($conf) {
  $node = node_load($conf['nid']);
  if (!node_access('view', $node)) {
    return;
  }

  $block->module = 'node';
  $block->delta = $node->nid;

  if (!empty($conf['suppress_title'])) {
    $block->subject = check_plain($node->title);
  }
  unset($node->title);
  $block->content = node_view($node, $conf['teaser'], FALSE, $conf['links']);
  return $block;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_node() {
  return array(
    'node' => array(
      'title' => t('Node content'),
      'icon' => 'node.png',
      'description' => t('Add a node from your site as content.'),
    ),
  );
}

/**
 * Returns the form for a new node.
 */
function panels_admin_add_node($id) {
  $conf = array();
  $form = panels_admin_edit_node($conf);
  $form['nid'] = array(
    '#title' => t('Enter the title or NID of a post'),
    '#type' => 'textfield',
    '#maxlength' => 512,
    '#autocomplete_path' => 'panels/node/autocomplete',
    '#weight'  => -10,
  );
  $form['validate_me'] = array('#type' => 'value', '#value' => TRUE);
  return $form;
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_node($conf) {
  $form['nid'] = array(
    '#type' => 'value',
    '#default_value' => $conf['nid'],
  );
  $form['teaser'] = array(
    '#title' => t('Teaser'),
    '#type' => 'checkbox',
    '#default_value' => $conf['teaser'],
    '#description' => t('Check here to show only the node teaser'),
  );
  $form['links'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['links'],
    '#title' => t('Display links'),
    '#description' => t('Check here to display the links with the post.')
  );
  $form['suppress_title'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['suppress_title'],
    '#title' => t('Suppress node title'),
    '#description' => t('Check here to suppress the node title.')
  );

  return $form;
}

/**
 * Validate a node.
 */
function panels_admin_validate_node($form, $form_values) {
  if (!$form_values['validate_me']) {
    return;
  }

  $nid = $form_values['nid'];
  if (is_numeric($nid)) {
    $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE nid = %d"), $nid));
  }
  else {
    $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE LOWER(title) = LOWER('%s')"), $nid));
    if ($node) {
      form_set_value($form['nid'], $node->nid);
    }
  }
  if (!$node) {
    form_error($form['nid'], t('Invalid node'));
  }
}
/**
 * Returns the administrative title for a node.
 */
function panels_admin_title_node($conf) {
  $node = node_load($conf['nid']);
  if ($node) {
    return check_plain($node->title);
  }
}

/** 
 * Callback to perform administrative functions on the content block
 */
function panels_admin_node($op, &$arg, $arg2 = NULL) {
  switch ($op) {
    case 'list':
      $conf = $arg;
      $node = node_load($conf['nid']);
      return '<strong>Node</strong>: ' . check_plain($node->title);
    case 'add form':
      $form['nid'] = array(
        '#title' => t('Enter the title or NID of a post'),
        '#type' => 'textfield',
        '#maxlength' => 512,
        '#autocomplete_path' => 'panels/node/autocomplete',
        '#weight'  => -10,
      );
      return $form;
    case 'add validate':
      $form_values = $arg;
      $form = $arg2;
      $nid = $form_values['nid'];
      if (is_numeric($nid)) {
        error_log($nid);
        $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE nid = %d"), $nid));
      }
      else {
        $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE LOWER(title) = LOWER('%s')"), $nid));
      }

      if (!$node->nid) {
        form_error($form['nid'], t('Unable to find "%s"', array('%s' => check_plain($nid))));
      }
      return;
    case 'add':
      $form = &$arg;
      if (is_numeric($form['nid'])) {
        $conf = array();
        $conf['nid'] = $form['nid'];
      }
      // default to just teaser
      $conf['teaser'] = TRUE;
      return $conf;
    case 'edit':
      $conf = &$arg;
      $form['nid'] = array(
        '#type' => 'hidden',
        '#default_value' => $conf['nid'],
      );
      $form['teaser'] = array(
        '#title' => t('Teaser'),
        '#type' => 'checkbox',
        '#default_value' => $conf['teaser'],
        '#description' => t('Check here to show only the node teaser'),
      );
      $form['links'] = array(
        '#type' => 'checkbox',
        '#default_value' => $conf['links'],
        '#title' => t('Display links'),
        '#description' => t('Check here to display the links with the post.')
      );
      $form['suppress_title'] = array(
        '#type' => 'checkbox',
        '#default_value' => $conf['suppress_title'],
        '#title' => t('Suppress node title'),
        '#description' => t('Check here to suppress the node title.')
      );

      return $form;
    case 'validate':
      // This one has nothing to validate.
      $form = &$arg;
      return;
    case 'save':
      // For this one, the form values go directly into the config.
      $form = &$arg;
      return $form;
  }
}
