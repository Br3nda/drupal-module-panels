<?php
// $Id: vocabulary_terms.inc,v 1.1.2.4 2008/05/27 20:21:11 sdboyer Exp $


/**
 * Callback function to supply a list of content types.
 */
function panels_vocabulary_terms_panels_content_types() {
  if (module_exists('taxonomy')) {
    $items['vocabulary_terms'] = array(
      'title' => t('Term description'),
      'content_types' => 'panels_admin_content_types_vocabulary_terms',
      'single' => TRUE,
      'render callback' => 'panels_content_vocabulary_terms',
      'add callback' => 'panels_admin_edit_vocabulary_terms',
      'edit callback' => 'panels_admin_edit_vocabulary_terms',
      'title callback' => 'panels_admin_title_vocabulary_terms',
    );
    return $items;
  }
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_vocabulary_terms($conf, $panel_args, $context) {
  $vocab = isset($context->data) ? drupal_clone($context->data) : NULL;
  $max_depth = (!empty($conf['max_depth']) ? (int)$conf['max_depth'] : NULL);
  if ($conf['tree'] == FALSE) {
    $terms = taxonomy_get_tree($vocab->vid, 0, -1, $max_depth);
    $items = array();
    foreach ($terms as $term) {
      $items[] = l(check_plain($term->name), 'taxonomy/term/'. $term->tid);
    }
    $output = theme('item_list', $items);
  }
  else {
    $output = theme('item_list', _panels_content_vocabulary_terms(0, $max_depth));
  }

  $block = new stdClass();
  $block->module  = 'node_type';
  $block->subject = $vocab->name;
  $block->content = $output;
  $block->delta   = $vocab->tid;

  return $block;
}

function _panels_content_vocabulary_terms($tid, $max_depth, $depth = -1) {
  $depth++;
  if ($max_depth != NULL && $depth == $max_depth) {
    return array();
  }
  $return = array();
  $query = db_query('SELECT t.name, t.tid FROM {term_data} t INNER JOIN {term_hierarchy} h ON t.tid = h.tid WHERE h.parent = %d', $tid);
  while ($result = db_fetch_object($query)) {
    $return[] = array(
      'data' => l($result->name, 'taxonomy/term/'. $result->tid),
      'children' => _panels_content_vocabulary_terms($result->tid, $max_depth + 1, $depth),
    );
  }
  return $return;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_vocabulary_terms() {
  return array(
    'description' => array(
      'title' => t('Vocabulary terms'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('All the terms in a vocabulary.'),
      'required context' => new panels_required_context(t('Vocabulary'), 'vocabulary'),
      'category' => array(t('Vocabulary context'), -9),
    ),
  );
}

function panels_admin_title_vocabulary_terms($conf, $context) {
  return t('"@s" terms', array('@s' => $context->identifier));
}

function panels_admin_edit_vocabulary_terms($id, $parents, $conf = array()) {
  // Apply defaults
  if (empty($conf)) {
    $conf = array('max_depth' => '', 'tree' => 1);
  }

  $form['list_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Max depth'),
    '#default_value' => $conf['max_depth'],
    '#description' => t('Leave blank for unlimited.'),
  );

  $form['tree'] = array(
    '#type' => 'checkbox',
    '#title' => t('Tree'),
    '#default_value' => $conf['tree'],
    '#description' => t('Whether to display the terms in a tree.'),
  );

  return $form;
}

