<?php
// $Id: node.inc,v 1.1.2.5 2007/12/31 06:30:16 merlinofchaos Exp $
/**
 * @file contexts/node.inc
 *
 * Plugin to provide a node context
 */

function panels_node_panels_contexts() {
  $args['node'] = array(
    'title' => t("Node"),
    'description' => t('A node object.'),
    'context' => 'panels_context_create_node',
    'settings form' => 'panels_context_node_settings_form',
    'settings form validate' => 'panels_context_node_settings_form_validate',
    'keyword' => 'node',
    'context name' => 'node',
  );
  return $args;
}

/**
 * It's important to remember that $conf is optional here, because contexts
 * are not always created from the UI.
 */
function panels_context_create_node($empty, $data = NULL, $conf = FALSE) {
  $context = new panels_context('node');
  $context->plugin = 'node';
  if ($empty) {
    return $context;
  }

  if ($empty) {
    return $context;
  }

  if ($conf) {
    $data = node_load($data['nid']);
  }
  if (!empty($data)) {
    $context->data = $data;
    $context->title = $data->title;
    $context->argument = $data->nid;
    return $context;
  }
}

function panels_context_node_settings_form($conf, $external = FALSE) {
  if ($external) {
    $form['external'] = array(
      '#type' => 'checkbox',
      '#default_value' => $conf['external'],
      '#title' => t('Require this context from an external source (such as containing panel page)'),
      '#description' => t('If selected, node selection (below) will be ignored'),
    );
  }


  $form['node'] = array(
    '#prefix' => '<div class="no-float">',
    '#suffix' => '</div>',
    '#title' => t('Enter the title or NID of a post'),
    '#type' => 'textfield',
    '#maxlength' => 512,
    '#autocomplete_path' => 'panels/node/autocomplete',
    '#weight'  => -10,
  );

  if (!empty($conf['nid'])) {
    $info = db_fetch_object(db_query("SELECT * FROM {node} WHERE nid = %d", $conf['nid']));
    if ($info) {
      $form['node']['#description'] = t('Currently set to "%title"', array('%title' => $info->title));
    }
  }

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $conf['nid'],
  );

  return $form;
}

/**
 * Validate a node.
 */
function panels_context_node_settings_form_validate($form, $form_values) {
  // Validate the autocomplete
  if (empty($form_values['external']) && empty($form_values['nid']) && empty($form_values['node'])) {   
    form_error($form['node'], t('You must select a node.'));
    return;
  }

  if (empty($form_values['node'])) {
    return;
  }

  $nid = $form_values['node'];
  $preg_matches = array();
  $match = preg_match('/\[nid: (\d+)\]/', $nid, $preg_matches);
  if (!$match) {
    $match = preg_match('/^nid: (\d+)/', $nid, $preg_matches);
  }

  if ($match) {
    $nid = $preg_matches[1];
  }
  if (is_numeric($nid)) {
    $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE nid = %d"), $nid));
  }
  else {
    $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE LOWER(title) = LOWER('%s')"), $nid));
  }

  if (!$node) {
    form_error($form['node'], t('Invalid post selected'));
  }
  else {
    form_set_value($form['nid'], $node->nid);
  }

}
