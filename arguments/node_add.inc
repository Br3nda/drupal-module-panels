<?php
// $Id: node_add.inc,v 1.1.2.3 2007/10/11 20:12:33 merlinofchaos Exp $
/** 
 * @file arguments/nid.inc
 * 
 * Plugin to provide an argument handler for a Node add form
 */

function panels_node_add_panels_arguments() {
  $args['node_add'] = array(
    'title' => t("Node add form"),
    'description' => t('Displays the node add form for a content type.'),
    'context' => 'panels_node_add_context',
    'settings form' => 'panels_node_add_settings_form',
    'title function' => 'panels_node_add_title',
    'displays' => 'panels_node_add_displays',
    'choose display' => 'panels_node_add_choose_display',
  );
  return $args;
}

/**
 * Discover if this argument gives us the node we crave.
 */
function panels_node_add_context($arg = NULL, $conf = NULL, $context = NULL, $index = 0) {
  // If unset it wants a generic, unfilled context.
  if (!isset($arg)) {
    return new panels_context(array('form', 'node_add', 'node_form'), NULL);
  }

  if (empty($conf['types'][$arg])) {
    return FALSE;
  }

  $context = panels_node_add_form($arg);
  return $context;
}

/**
 * A version of node_add that creates a context.
 */
function panels_node_add_form($type) {
  global $user;

  $types = node_get_types();
  $type = str_replace('-', '_', $type);
  // If a node type has been specified, validate its existence.
  if (isset($types[$type]) && node_access('create', $type)) {
    // Initialize settings:
    $node = array('uid' => $user->uid, 'name' => $user->name, 'type' => $type);

    $form = drupal_retrieve_form($type . '_node_form', $node);
    drupal_process_form($type . '_node_form', $form);
    $context = new panels_context(array('form', 'node_add', 'node_form'), $form);
    $context->form_id = $type . '_node_form';
    $context->form_title = t('Submit @name', array('@name' => $types[$type]->name));
    $context->node_type = $type;
    return $context;
  }
}

/**
 * Settings form for the argument
 */
function panels_node_add_settings_form($conf) {
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
function panels_node_add_displays($conf, $id) {
  $displays = array();
  if (!empty($conf['own_default'])) {
    $displays['default'] = array(
      'title' => t('Node add form @id Default', array('@id' => $id)),
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
        'title' => t('Node add form @id @type', array('@id' => $id, '@type' => $options[$type])),
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
function panels_node_add_choose_display($conf, $context) {
  if (empty($context->data)) {
    return;
  }

  if (!empty($conf['displays'][$context->data['#node']->type])) {
    return $context->data['#node']->type;
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
function panels_node_add_title($context) {
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
