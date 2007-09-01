<?php
// $Id: nid.inc,v 1.1.2.1 2007/09/01 00:23:50 merlinofchaos Exp $
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
  );
  return $args;
}

/**
 * Discover if this argument gives us the node we crave.
 */
function panels_nid_context($arg, $settings) {
  if (!is_numeric($arg)) {
    return FALSE;
  }

  $node = node_load($arg);
  if (!$node) {
    return FALSE;
  }

  if (empty($settings['type'][$node->type])) {
    return FALSE;
  }

  return new panels_context('node', $node);
}

function panels_nid_settings_form($settings) {
  $options = array();
  foreach (node_get_types() as $type => $info) {
    $options[$type] = $info->name;
  }

  $form['types'] = array(
    '#title' => t('Node types'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $settings['types'],
  );

  return $form;
}
