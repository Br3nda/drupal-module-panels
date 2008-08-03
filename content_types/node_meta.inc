<?php
// $Id: node_meta.inc,v 1.1.2.1 2008/08/03 03:06:48 sdboyer Exp $


/**
 * Callback function to supply a list of content types.
 */
function panels_node_meta_panels_content_types() {
  $items['node_meta'] = array(
    'single' => TRUE,
    'content_types' => 'panels_admin_content_types_node_meta',
    'render callback' => 'panels_content_node_meta',
    'add callback' => 'panels_admin_edit_node_meta',
    'edit callback' => 'panels_admin_edit_node_meta',
    'title callback' => 'panels_admin_title_node_meta',
  );
  return $items;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_node_meta() {
  return array(
    'content' => array(
      'title' => t('Node meta data'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Meta data of the referenced node.'),
      'required context' => new panels_required_context(t('Node'), 'node'),
      'category' => array(t('Node context'), -9),
    ),
  );
}

function panels_content_node_meta($conf, $panel_args, $context) {
  if (!empty($context) && empty($context->data)) {
    return;
  }

  $node = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block = new stdClass();
  $block->module = 'node';
  $block->delta  = $node->nid;

  if (empty($node)) {
    $block->delta   = 'placeholder';
    $block->subject = t('Node title.');
    $block->content = t('Node meta content goes here.');
  }
  else {
    $block->content = theme('panels_node_meta', $node, $conf);
  }
  if (empty($block->content)) {
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

  return $block;
}

/**
 * Render the provided node's meta data. Can be overridden using the normal
 * theming techniques.
 */
function theme_panels_node_meta($node, $conf) {
  $output = '';
  if ($conf['title']) {
    $output .= '<div class="node-title">';
    $output .= check_plain($node->title);
    $output .= '</div>';
  }
  if ($conf['author']) {
    $output .= '<div class="node-author">';
    // 2nd parameter for User Display API support.
    $output .= theme('username', $node, 'panels_node_meta_'. $node->type);
    $output .= '</div>';
  }
  if ($conf['created']) {
    $output .= '<div class="node-created">';
    $output .= format_date($node->created);
    $output .= '</div>';
  }
  if ($conf['changed']) {
    $output .= '<div class="node-modified">';
    $output .= format_date($node->changed);
    $output .= '</div>';
  }

  return $output;
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_node_meta($id, $parents, $conf = array()) {
  $conf += array(
    'override_title' => FALSE,
    'override_title_text' => '',
    'title' => TRUE,
    'author' => FALSE,
    'created' => FALSE,
    'modified' => FALSE,
  );

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
  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'checkbox',
    '#default_value' => $conf['title'],
    '#description' => t('Check here to display the title of the node.'),
  );
  $form['author'] = array(
    '#title' => t('Author'),
    '#type' => 'checkbox',
    '#default_value' => $conf['author'],
    '#description' => t('Check here to display the author of the node.'),
  );
  $form['created'] = array(
    '#title' => t('Created date'),
    '#type' => 'checkbox',
    '#default_value' => $conf['created'],
    '#description' => t('Check here to display the creation date of the node.'),
  );
  $form['changed'] = array(
    '#title' => t('Modified date'),
    '#type' => 'checkbox',
    '#default_value' => $conf['modified'],
    '#description' => t('Check here to display the modification date of the node.'),
  );
  return $form;
}

function panels_admin_title_node_meta($conf, $context) {
  return t('"@s" meta content', array('@s' => $context->identifier));
}

