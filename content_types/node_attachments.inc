<?php
// $Id: node_attachments.inc,v 1.1.2.1 2007/09/27 19:25:52 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_node_attachments_panels_content_types() {
  if (module_exists('comment')) {
    $items['node_attachments'] = array(
      'title' => t('Node type description'),
      'content_types' => 'panels_admin_content_types_node_attachments',
      'render callback' => 'panels_content_node_attachments',
      'add callback' => 'panels_admin_edit_node_attachments',
      'edit callback' => 'panels_admin_edit_node_attachments',
      'title callback' => 'panels_admin_title_node_attachments',
    );
    return $items;
  }
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_node_attachments($conf, $panel_args, $context) {
  $node = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block->module = 'attachments';

  if ($node) {
    $block->subject = t('Attached files');
    $block->content = theme('upload_attachments', $node->files);
    $block->delta = $node->nid;
  }
  else {
    $block->subject = t('Attached files.');
    $block->content = t('Attached files go here.');
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
function panels_admin_content_types_node_attachments() {
  return array(
    'node_type' => array(
      'title' => t('Attached files'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('A list of files attached to the node.'),
      'required context' => 'node',
      'category' => array(t('Node context'), -9),
    ),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_node_attachments($id, $parents, $conf = array()) {
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

function panels_admin_title_node_attachments($conf, $context) {
  return t('Attached files');
}
