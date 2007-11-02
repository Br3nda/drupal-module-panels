<?php
// $Id: node.inc,v 1.1.2.2 2007/11/02 22:24:47 merlinofchaos Exp $
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

  // TODO: uid autcomplete

  return $form;
}

/**
 * Validate a node.
 */
function panels_context_node_settings_form_validate($form, $form_values) {
  // Validate the uid autocomplete
}
