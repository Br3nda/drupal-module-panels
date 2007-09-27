<?php
// $Id: node_edit.inc,v 1.1.2.1 2007/09/27 19:25:51 merlinofchaos Exp $
/** 
 * @file arguments/nid.inc
 * 
 * Plugin to provide an argument handler for a Node edit form
 */

function panels_node_edit_panels_arguments() {
  $args['node_edit'] = array(
    'title' => t("Node edit form"),
    'description' => t('Displays the node edit form for a node.'),
    'context' => 'panels_node_edit_context',
    'settings form' => 'panels_node_edit_settings_form',
    'title function' => 'panels_node_edit_title',
    'displays' => 'panels_node_edit_displays',
    'choose display' => 'panels_node_edit_choose_display',
  );
  return $args;
}

/**
 * Discover if this argument gives us the node we crave.
 */
function panels_node_edit_context($arg = NULL, $conf = NULL, $context = NULL, $index = 0) {
  // If unset it wants a generic, unfilled context.
  if (!isset($arg)) {
    return new panels_context(array('form', 'node_edit', 'node_form', 'node'), NULL);
  }

  if (empty($arg) || !is_numeric($arg)) {
    return FALSE;
  }

  $node = node_load($arg);
  if (empty($node) || !node_access('update', $node)) {
    return FALSE;
  }

  if (empty($conf['types'][$node->type])) {
    return FALSE;
  }

  $context = panels_node_edit_form($node);
  return $context;
}

/**
 * A version of node_edit that creates a context.
 */
function panels_node_edit_form($node) {
  if ($_POST['op'] == t('Delete')) {
    // Note: we redirect from node/nid/edit to node/nid/delete to make the tabs disappear.
    if ($_REQUEST['destination']) {
      $destination = drupal_get_destination();
      unset($_REQUEST['destination']);
    }
    drupal_goto('node/'. $node->nid .'/delete', $destination);
  }

  $form = drupal_retrieve_form($node->type . '_node_form', $node);
  drupal_process_form($node->type . '_node_form', $form);
  $context = new panels_context(array('form', 'node_edit', 'node_form', 'node'), $form);
  $context->form_id = $node->type . '_node_form';
  $context->form_title = check_plain($node->title);
  $context->node_type = $node->type;

  return $context;
}

/**
 * Settings form for the argument
 */
function panels_node_edit_settings_form($conf) {
  $options = array();
  foreach (node_get_types() as $type => $info) {
    $options[$type] = $info->name;
  }
  $form['types'] = array(
    '#title' => t('Node types'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $conf['types'],
    '#prefix' => '<div class="clear-block">',
    '#suffix' => '</div>',
  );

  $form['own_default'] = array(
    '#title' => t('Use different default display'),
    '#type' => 'checkbox',
    '#description' => t('If checked, when this argument is present it will use its own display rather than the default. Displays not selected in the "Own display" field will use this one.'),
    '#default_value' => $conf['own_default'],
  );

  $form['displays'] = array(
    '#title' => t('Own display'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $conf['displays'],
    '#description' => t('Each checked type will get its own special display to layout its content. Only types set in Node types, above, should be set here. Types not set here will use the default display.'),
    '#prefix' => '<div class="clear-block">',
    '#suffix' => '</div>',
  );

  return $form;
}

/**
 * What additional displays does this argument provide?
 */
function panels_node_edit_displays($conf, $id) {
  $displays = array();
  if (!empty($conf['own_default'])) {
    $displays['default'] = array(
      'title' => t('Node edit form @id Default', array('@id' => $id)),
      'context' => 'node',
    );
  }

  if (is_array($conf['displays'])) {
    $options = array();
    foreach (node_get_types() as $type => $info) {
      $options[$type] = $info->name;
    }
    foreach (array_keys(array_filter($conf['displays'])) as $type) {
      $displays[$type] = array(
        'title' => t('Node edit form @id @type', array('@id' => $id, '@type' => $options[$type])),
        // Tell it to base the template for this display off of the default.
        'default' => 'default',
        'context' => 'node',
      );
    }
  }

  return $displays;
}

/**
 * Based upon the settings and the context, choose which display to use.
 */
function panels_node_edit_choose_display($conf, $context) {
  if (empty($context->data)) {
    return;
  }

  if (!empty($conf['displays'][$context->data->type])) {
    return $context->data->type;
  }

  // Please note that 'default' is a special display.
  if (!empty($conf['own_default'])) {
    return 'default';
  }

  // Empty return says to use the default display.
  return;
}

/**
 * Determine the title for substitution with this context
 */
function panels_node_edit_title($context) {
  if (isset($context->data->title)) {
    return $context->data->title;
  }

  if (isset($context->page_title)) {
    return $context->page_title;
  }

  if (isset($context->form_title)) {
    return $context->form_title;
  }

}
