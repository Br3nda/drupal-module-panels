<?php
// $Id: node_content.inc,v 1.1.2.1 2007/09/06 00:48:49 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_node_content_panels_content_types() {
  $items['node_content'] = array(
    'title' => t('Node content'),
    'weight' => -10,
    'content_types' => 'panels_admin_content_types_node_content',
    'render callback' => 'panels_content_node_content',
    'add callback' => 'panels_admin_add_node_content',
    'edit callback' => 'panels_admin_edit_node_content',
    'title callback' => 'panels_admin_title_node_content',
  );
  return $items;
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_node_content($conf, $panel_args, $context) {
  $node = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block->module = 'node';
  $block->delta = $node->nid;

  if (empty($conf['override_title'])) {
    if (empty($node)) {
      $block->subject = t('Node title placeholder.');
    }
    else {
      $block->subject = check_plain($node->title);
    }
  }
  else {
    $block->subject = filter_xss_admin($conf['override_title_text']);
  }

  if (!empty($conf['link']) && $node) {
    $block->subject = l($block->subject, "node/$node->nid", NULL, NULL, NULL, NULL, TRUE);
  }

  unset($node->title);

  if (empty($node)) {
    $block->content = t('Node content goes here.');
  }
  else {
    $block->content = node_view($node, $conf['teaser'], FALSE, $conf['links']);
  }

  return $block;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_node_content() {
  return array(
    'content' => array(
      'title' => t('Node content'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('The content of the referenced node.'),
      'required context' => 'node',
      'category' => array(t('Node context'), -9),
    ),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_node_content($id, $parents, $conf = array()) {
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
  $form['link'] = array(
    '#title' => t('Link title to node'),
    '#type' => 'checkbox',
    '#default_value' => $conf['link'],
    '#description' => t('Check here to make the title link to the node.'),
  );
  $form['teaser'] = array(
    '#title' => t('Teaser'),
    '#type' => 'checkbox',
    '#default_value' => $conf['teaser'],
    '#description' => t('Check here to show only the node teaser.'),
  );
  $form['links'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['links'],
    '#title' => t('Display links'),
    '#description' => t('Check here to display the links with the post.')
  );

  return $form;
}

function panels_admin_title_node_content($conf, $context) {
  return t('Node content');
}
