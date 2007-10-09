<?php
// $Id: term_list.inc,v 1.1.2.1 2007/10/09 23:22:53 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_term_list_panels_content_types() {
  if (module_exists('comment')) {
    $items['term_list'] = array(
      'title' => t('List of related terms'),
      'content_types' => 'panels_admin_content_types_term_list',
      'single' => TRUE, // only provides a single content type
      'render callback' => 'panels_content_term_list',
      'add callback' => 'panels_admin_edit_term_list',
      'edit callback' => 'panels_admin_edit_term_list',
      'title callback' => 'panels_admin_title_term_list',
    );
    return $items;
  }
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_term_list($conf, $panel_args, $context) {
  $term = isset($context->data) ? drupal_clone($context->data) : NULL;
  $block->module = 'term-list';

  if ($term) {
    $options = panels_admin_term_list_options();
    $block->subject = $options[$conf['type']];
    $block->delta = $conf['type'];
    switch ($conf['type']) {
      case 'related':
        $terms = taxonomy_get_related($term->tid);
        break;
      case 'child':
      default:
        $terms = taxonomy_get_children($term->tid);
        break;
      case 'top':
        $terms = taxonomy_get_children(0, $term->vid);
        break;
      case 'sibling':
        $parent = db_result(db_query("SELECT parent FROM {term_hierarchy} WHERE tid = %d", $term->tid));
        $terms = taxonomy_get_children($parent, $term->vid);
        break;
      case 'synonyms':
        $terms = taxonomy_get_synonyms($term->tid);
        break;
    }
    if ($terms) {
      foreach ($terms as $related) {
        $items[$related->tid] = l($related->name, taxonomy_term_path($related), array('rel' => 'tag', 'title' => strip_tags($related->description)));
      }

      $block->content = theme('item_list', $items, NULL, $conf['list_type']);
    }
  }
  else {
    $block->subject = t('Term description');
    $block->content = t('Term description goes here.');
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
function panels_admin_content_types_term_list() {
  return array(
    'description' => array(
      'title' => t('List of related terms'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Terms related to an existing term; may be child, siblings or top level.'),
      'required context' => 'term',
      'category' => array(t('Term context'), -9),
    ),
  );
}

function panels_admin_term_list_options() {
  return array(
    'child' => t('Child terms'),
    'related' => t('Related terms'),
    'sibling' => t('Sibling terms'),
    'top' => t('Top level terms'),
    'synonyms' => t('Term synonyms'),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_term_list($id, $parents, $conf = array()) {
  // Apply defaults
  if (empty($conf)) {
    $conf = array('title' => '', 'type' => 'child', 'list_type' => 'ul');
  }

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

  $form['type'] = array(
    '#type' => 'radios',
    '#title' => t('Which terms'),
    '#options' => panels_admin_term_list_options(),
    '#default_value' => $conf['type'],
    '#prefix' => '<div class="clear-block no-float">',
    '#suffix' => '</div>',
  );

  $form['list_type'] = array(
    '#type' => 'select',
    '#title' => t('List type'),
    '#options' => array('ul' => t('Unordered'), 'ol' => t('Ordered')),
    '#default_value' => $conf['list_type'],
  );

  return $form;
}

function panels_admin_title_term_list($conf, $context) {
  $options = panels_admin_term_list_options();
  return $options[$conf['type']];
}
