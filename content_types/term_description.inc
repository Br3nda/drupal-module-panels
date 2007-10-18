<?php
// $Id: term_description.inc,v 1.1.2.2 2007/10/18 22:27:18 merlinofchaos Exp $
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

  if ($term) {
    $block->subject = check_plain($term->name);
    $block->content = check_plain($term->description);
    $block->delta = $term->tid;
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
function panels_admin_content_types_term_description() {
  return array(
    'description' => array(
      'title' => t('Term description'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Term description.'),
      'required context' => new panels_required_context('term'),
      'category' => array(t('Term context'), -9),
    ),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_term_description($id, $parents, $conf = array()) {
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

function panels_admin_title_term_description($conf, $context) {
  return t('Term description');
}
