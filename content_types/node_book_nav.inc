<?php
// $Id: node_book_nav.inc,v 1.1.2.3 2007/10/25 02:23:19 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_node_book_nav_panels_content_types() {
  if (module_exists('book')) {
    $items['node_book_nav'] = array(
      'title' => t('Node book navigation'),
      'single' => TRUE, // only provides a single content type
      'content_types' => 'panels_admin_content_types_node_book_nav',
      'render callback' => 'panels_content_node_book_nav',
      'add callback' => 'panels_admin_edit_node_book_nav',
      'edit callback' => 'panels_admin_edit_node_book_nav',
      'title callback' => 'panels_admin_title_node_book_nav',
    );
    return $items;
  }
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_node_book_nav($conf, $panel_args, $context) {
  $node = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block->module = 'book_nav';

  if ($node) {
    $block->subject = t('Book navigation');
    $block->content = theme('book_navigation', $node);
    $block->delta = $node->nid;
  }
  else {
    $block->subject = t('Book navigation.');
    $block->content = t('Book navigation goes here.');
    $block->delta = 'unknown';
  }

  if (!empty($conf['override_title'])) {
    $block->subject = filter_xss_admin($conf['override_title_text']);
  }

  return $block;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_node_book_nav() {
  return array(
    'node_type' => array(
      'title' => t('Book navigation'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('A list of files attached to the node.'),
      'required context' => new panels_required_context(t('Node'), 'node'),
      'category' => array(t('Node context'), -9),
    ),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_node_book_nav($id, $parents, $conf = array()) {
  $form['aligner_start'] = array(
    '#value' => '<div class="option-text-aligner">',
  );
  $form['override_title'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['override_title'],
    '#title' => t('Override title'),
    '#id' => 'override-title-checkbox',
  );
  $form['override_title_text'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['override_title_text'],
    '#size' => 35,
    '#id' => 'override-title-textfield',
  );

  $form['aligner_stop'] = array(
    '#value' => '</div><div style="clear: both; padding: 0; margin: 0"></div>',
  );

  return $form;
}

function panels_admin_title_node_book_nav($conf, $context) {
  return t('Book navigation');
}
