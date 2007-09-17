<?php
// $Id: nid.inc,v 1.1.2.4 2007/09/17 19:11:49 merlinofchaos Exp $
/** 
 * @file arguments/nid.inc
 * 
 * Plugin to provide an argument handler for a node id
 */

function panels_nid_panels_arguments() {
  $args['nid'] = array(
    'title' => t("Node ID"),
    'description' => t('Restricts the argument to a node id.'),
    'context' => 'panels_nid_context',
    'settings form' => 'panels_nid_settings_form',
    'title function' => 'panels_nid_title',
    'displays' => 'panels_nid_displays',
    'choose display' => 'panels_nid_choose_display',
  );
  return $args;
}

/**
 * Discover if this argument gives us the node we crave.
 */
function panels_nid_context($arg = NULL, $conf = NULL, $context = NULL, $index = 0) {
  // If unset it wants a generic, unfilled context.
  if (!isset($arg)) {
    return new panels_context('node', NULL);
  }

  if (!is_numeric($arg)) {
    return FALSE;
  }

  $node = node_load($arg);
  if (!$node) {
    return FALSE;
  }

  if (empty($conf['types'][$node->type])) {
    return FALSE;
  }

  return new panels_context('node', $node);
}

/**
 * Settings form for the argument
 */
function panels_nid_settings_form($conf) {
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
function panels_nid_displays($conf, $id) {
  $displays = array();
  if (!empty($conf['own_default'])) {
    $displays['default'] = array(
      'title' => t('Node ID @id Default', array('@id' => $id)),
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
        'title' => t('Node ID @id @type', array('@id' => $id, '@type' => $options[$type])),
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
function panels_nid_choose_display($conf, $context) {
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
function panels_nid_title($context) {
  if (isset($context->data->title)) {
    return $context->data->title;
  }

  if (isset($context->page_title)) {
    return $context->page_title;
  }
}
