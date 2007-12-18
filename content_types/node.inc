<?php
// $Id: node.inc,v 1.2.6.21 2007/12/18 01:29:27 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_node_panels_content_types() {
  $items['node'] = array(
    'title' => t('Node'),
    'weight' => -10,
    'single' => TRUE, // only provides a single content type
    'content_types' => 'panels_admin_content_types_node',
    'render callback' => 'panels_content_node',
    'add callback' => 'panels_admin_add_node',
    'edit callback' => 'panels_admin_edit_node',
    'title callback' => 'panels_admin_title_node',
    'add validate callback' => 'panels_admin_validate_node',
    'edit validate callback' => 'panels_admin_validate_node',
  );
  return $items;
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_node($conf, $panel_args) {
  $nid = $conf['nid'];
  foreach (explode('/', $_GET['q']) as $id => $arg) {
    $nid = str_replace("%$id", $arg, $nid);
  }

  foreach ($panel_args as $id => $arg) {
    $nid = str_replace("@$id", $arg, $nid);
  }

  // Support node translation
  if (module_exists('translation')) {
    if ($translation = module_invoke('translation', 'node_nid', $nid, $GLOBALS['locale'])) {
      $nid = $translation;
    }
  }

  if (!is_numeric($nid)) {
    return;
  }

  $node = node_load($nid);
  if (!node_access('view', $node)) {
    return;
  }

  if (node_access('update', $node)) {
    $block->admin_links['update'] = array(
      'title' => t('Edit node'),
      'alt' => t("Edit this node"),
      'href' => "node/$node->nid/edit",
      'query' => drupal_get_destination(),
    );
  }

  $block->module = 'node';
  $block->delta = $node->nid;

  $block->subject = $node->title;

  if (empty($conf['leave_node_title'])) {
    unset($node->title);
  }

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
      'icon' => 'icon_node.png',
      'description' => t('Add a node from your site as content.'),
      'category' => array(t('Custom'), -10),
    ),
  );
}

/**
 * Returns the form for a new node.
 */
function panels_admin_add_node($id, $parents, $conf = array()) {
  $form = panels_admin_edit_node($id, $parents, $conf);
  $form['nid'] = array(
    '#prefix' => '<div class="no-float">',
    '#suffix' => '</div>',
    '#title' => t('Enter the title or NID of a post'),
    '#description' => t('To use a NID from the URL, you may use %0, %1, ..., %N to get URL arguments. Or use @0, @1, @2, ..., @N to use arguments passed into the panel.'),
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
function panels_admin_edit_node($id, $parents, $conf) {
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

  $form['leave_node_title'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['leave_node_title'],
    '#title' => t('Leave node title'),
    '#description' => t('Advanced: if checked, do not touch the node title; this can cause the node title to appear twice unless your theme is aware of this.')
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
    $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE n.nid = %d"), $nid));
  }
  else {
    $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE LOWER(title) = LOWER('%s')"), $nid));
    if ($node) {
      form_set_value($form['nid'], $node->nid);
    }
  }

  if (!($node || preg_match('/^[@%]\d+$/', $nid))) {
    form_error($form['nid'], t('Invalid node'));
  }
}
/**
 * Returns the administrative title for a node.
 */
function panels_admin_title_node($conf) {
  if (!is_numeric($conf['nid'])) {
    return t('Node loaded from @var', array('@var' => $conf['nid']));
  }

  $node = node_load($conf['nid']);
  if ($node) {
    return check_plain($node->title);
  }
}
