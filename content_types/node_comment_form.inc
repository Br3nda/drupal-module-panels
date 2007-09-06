<?php
// $Id: node_comment_form.inc,v 1.1.2.1 2007/09/06 00:48:49 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_node_comment_form_panels_content_types() {
  if (module_exists('comment')) {
    $items['node_comment_form'] = array(
      'title' => t('Node comment form'),
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
  $block->module = 'comments';
  $block->delta = $node->nid;

  if (empty($conf['override_title'])) {
    $block->subject = t('Add comment');
  }
  else {
    $block->subject = filter_xss_admin($conf['override_title_text']);
  }
  if (empty($node)) {
    $block->content = t('Node comments go here.');
  }
  else {
    if (user_access('post comments') && node_comment_mode($node->nid) == COMMENT_NODE_READ_WRITE) {
      $block->content = drupal_get_form('panels_comment_form', array('nid'  => $node->nid));
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
      'required context' => 'node',
      'category' => array(t('Node context'), -9),
    ),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_node_comment_form($id, $parents, $conf = array()) {
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

function panels_admin_title_node_comment_form($conf, $context) {
  return t('Comment form');
}

function panels_comment_form($edit) {
  $form = comment_form($edit);
  // force the form to come back to here.
  $path = parse_url($_GET['q'], PHP_URL_PATH);
  $form['#action'] = url($path, NULL, 'new');
  $form['#redirect'] = array($path, NULL, 'new');
  $form['#validate']['comment_form_validate'] = array();
  $form['#submit']['comment_form_submit'] = array();
  return $form;
}
