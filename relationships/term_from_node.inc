<?php
// $Id: term_from_node.inc,v 1.1.2.1 2007/10/20 23:15:08 merlinofchaos Exp $
/**
 * @file relationships/term_from_node.inc
 *
 * Plugin to provide an relationship handler for term from node
 */

function panels_term_from_node_panels_relationships() {
  $args['term_from_node'] = array(
    'title' => t("Term from node"),
    'description' => t('Adds a taxonomy term from a node context; if multiple terms are selected, this will get the "first" term only.'),
    'required context' => new panels_required_context('node'),
    'context' => 'panels_term_from_node_context',
    'settings form' => 'panels_term_from_node_settings_form',
    'settings form validate' => 'panels_term_from_node_settings_form_validate',
  );
  return $args;
}

/**
 * Return a new context based on an existing context
 */
function panels_term_from_node_context($context = NULL, $conf) {
  // If unset it wants a generic, unfilled context, which is just NULL
  if (empty($context->data)) {
    return new panels_context(array('term'), NULL);
  }

  if (isset($context->data->taxonomy)) {
    foreach ($context->data->taxonomy as $term) {
      if ($term->vid == $conf['vid']) {
        return new panels_context(array('term'), $term);
      }
    }
  }
}

/**
 * Settings form for the relationship
 */
function panels_term_from_node_settings_form($conf) {
  $options = array();
  foreach (taxonomy_get_vocabularies() as $vid => $vocabulary) {
    $options[$vid] = $vocabulary->name;
  }
  $form['vid'] = array(
    '#title' => t('Vocabularies'),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $conf['vid'],
    '#prefix' => '<div class="clear-block">',
    '#suffix' => '</div>',
  );

  return $form;
}
