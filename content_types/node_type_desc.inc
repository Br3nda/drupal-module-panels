<?php
// $Id: node_type_desc.inc,v 1.1.2.5 2007/10/25 02:23:19 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_node_type_desc_panels_content_types() {
  if (module_exists('comment')) {
    $items['node_type_desc'] = array(
      'title' => t('Node type description'),
      'content_types' => 'panels_admin_content_types_node_type_desc',
      'single' => TRUE, // only provides a single content type
      'render callback' => 'panels_content_node_type_desc',
      'add callback' => 'panels_admin_edit_node_type_desc',
      'edit callback' => 'panels_admin_edit_node_type_desc',
      'title callback' => 'panels_admin_title_node_type_desc',
    );
    return $items;
  }
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_node_type_desc($conf, $panel_args, $context) {
  $node = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block->module = 'node_type';

  if ($node) {
    $type = node_get_types('type', $node);
    $block->subject = check_plain($type->name);
    $block->content = check_plain($type->description);
    $block->delta = $node->type;
  }
  else {
    $block->subject = t('Node type description');
    $block->content = t('Node type description goes here.');
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
function panels_admin_content_types_node_type_desc() {
  return array(
    'node_type' => array(
      'title' => t('Node type description'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Node type description.'),
      'required context' => new panels_required_context(t('Node'), 'node'),
      'category' => array(t('Node context'), -9),
    ),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_node_type_desc($id, $parents, $conf = array()) {
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

function panels_admin_title_node_type_desc($conf, $context) {
  return t('Node type description');
}
