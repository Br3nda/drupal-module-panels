<?php
// $Id: node_comment_form.inc,v 1.1.2.9 2008/08/03 03:12:07 sdboyer Exp $


/**
 * Callback function to supply a list of content types.
 */
function panels_node_comment_form_panels_content_types() {
  if (module_exists('comment')) {
    $items['node_comment_form'] = array(
      'title' => t('Node comment form'),
      // only provides a single content type
      'single' => TRUE,
      'content_types' => 'panels_admin_content_types_node_comment_form',
      'render callback' => 'panels_content_node_comment_form',
      'add callback' => 'panels_admin_edit_node_comment_form',
      'edit callback' => 'panels_admin_edit_node_comment_form',
      'title callback' => 'panels_admin_title_node_comment_form',
    );
    return $items;
  }
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_node_comment_form($conf, $panel_args, $context) {
  $node = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block = new stdClass();
  $block->module = 'comments';
  $block->delta  = $node->nid;

  $block->subject = t('Add comment');

  if (empty($node)) {
    $block->content = t('Comment form here.');
  }
  else {
    if (user_access('post comments') && node_comment_mode($node->nid) == COMMENT_NODE_READ_WRITE) {
      $form = drupal_retrieve_form('comment_form', array('nid' => $node->nid));
      $form['#action'] = url($_GET['q'], NULL, 'new');
      $form['#redirect'] = array($_GET['q'], NULL, 'new');
      drupal_process_form('comment_form', $form);
      drupal_prepare_form('comment_form', $form);
      $block->content = drupal_render_form('comment_form', $form);
    }
  }

  return $block;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_node_comment_form() {
  return array(
    'comment_form' => array(
      'title' => t('Comment form'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('A form to add a new comment.'),
      'required context' => new panels_required_context(t('Node'), 'node'),
      'category' => array(t('Node context'), -9),
    ),
  );
}

function panels_admin_title_node_comment_form($conf, $context) {
  return t('"@s" comment form', array('@s' => $context->identifier));
}

