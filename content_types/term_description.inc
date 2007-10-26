<?php
// $Id: term_description.inc,v 1.1.2.5 2007/10/26 06:07:20 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_term_description_panels_content_types() {
  if (module_exists('comment')) {
    $items['term_description'] = array(
      'title' => t('Term description'),
      'content_types' => 'panels_admin_content_types_term_description',
      'single' => TRUE, // only provides a single content type
      'render callback' => 'panels_content_term_description',
      'add callback' => 'panels_admin_edit_term_description',
      'edit callback' => 'panels_admin_edit_term_description',
      'title callback' => 'panels_admin_title_term_description',
    );
    return $items;
  }
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_term_description($conf, $panel_args, $context) {
  $term = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block->module = 'node_type';

  $block->subject = $term->name;
  if ($term) {
    $block->content = filter_xss_admin($term->description);
    $block->delta = $term->tid;
  }
  else {
    $block->content = t('Term description goes here.');
    $block->delta = 'unknown';
  }

  return $block;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_term_description() {
  return array(
    'description' => array(
      'title' => t('Term description'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Term description.'),
      'required context' => new panels_required_context(t('Term'), 'term'),
      'category' => array(t('Term context'), -9),
    ),
  );
}

function panels_admin_title_term_description($conf, $context) {
  return t('"@s" term description', array('@s' => $context->identifier));
}
