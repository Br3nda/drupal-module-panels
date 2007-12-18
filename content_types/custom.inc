<?php
// $Id: custom.inc,v 1.2.6.16 2007/12/18 01:29:27 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_custom_panels_content_types() {
  $items['custom'] = array(
    'title' => t('Custom'),
    'weight' => -10,
    'single' => TRUE, // only provides a single content type
    'content_types' => 'panels_admin_content_types_custom',
    'render callback' => 'panels_content_custom',
    'add callback' => 'panels_admin_edit_custom',
    'edit callback' => 'panels_admin_edit_custom',
    'title callback' => 'panels_admin_title_custom',
    'no override title' => TRUE,
  );
  return $items;
}

/**
 * Output function for the 'custom' content type. Outputs a custom
 * based on the module and delta supplied in the configuration.
 */
function panels_content_custom($conf) {
  static $delta = 0;
  $block->module = 'custom';
  $block->delta = ++$delta;
  $block->subject = filter_xss_admin($conf['title']);
  $block->content = check_markup($conf['body'], $conf['format'], FALSE);

  return $block;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_custom() {
  return array(
    'custom' => array(
      'title' => t('New custom content'),
      'icon' => 'icon_block_custom.png',
      'description' => t('Create a completely custom piece of HTML content.'),
      'category' => array(t('Custom'), -10),
    ),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_custom($id, $parents, $conf = NULL) {
  if (!is_array($conf)) {
    $conf = array('title' => '', 'body' => '');
  }
  $form['title'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['title'],
    '#title' => t('Title'),
  );
  $form['body'] = array(
    '#title' => t('Body'),
    '#type' => 'textarea',
    '#default_value' => $conf['body'],
  );
  $parents[] = 'format';
  $form['format'] = filter_form($conf['format'], 1, $parents);

  return $form;
}

/**
 * Returns the administrative title for a type.
 */
function panels_admin_title_custom($conf) {
  $output = t('Custom');
  if ($conf['title']) {
    $output .= " (" . filter_xss_admin($conf['title']) . ")";
  }
  return $output;
}

