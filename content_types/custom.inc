<?php
/**
 * Callback function to supply a list of content types.
 */
function panels_custom_panels_content_types() {
  $items['custom'] = array(
    'title' => t('Custom'),
    'weight' => -10,
    'single' => TRUE,
    'content_types' => 'panels_admin_content_types_custom',
    'render callback' => 'panels_content_custom',
    'add callback' => 'panels_admin_edit_custom',
    'edit callback' => 'panels_admin_edit_custom',
    'title callback' => 'panels_admin_title_custom',
  );
  return $items;
}

/**
 * Output function for the 'custom' content type. Outputs a custom
 * based on the module and delta supplied in the configuration.
 */
function panels_content_custom($conf) {
  $block->module = 'panels';
  $block->delta = strip_tags($conf['title']);
  $block->subject = filter_xss_admin($conf['title']);
  $block->content = theme('panels_custom_content', check_markup($conf['body'], $conf['format'], FALSE), check_plain($conf['css_id']), check_plain($conf['css_class']));
  return $block;
}

function theme_panels_custom_content($content, $id, $class) {
  if ($id) {
    $id = 'id="' . $id . '"';
  }

  $output = '<div class="panels-custom ' . $class . '" ' . $id . '>';
  $output .= $content;
  $output .= '</div>';
  return $output;
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
function panels_admin_edit_custom($conf, $parents) {
  if (!is_array($conf)) {
    $conf = array('title' => '', 'body' => '', 'css_id' => '', 'css_class' => '');
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

  $form['css_id'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['css_id'],
    '#title' => t('CSS ID'),
    '#description' => t('CSS ID of this custom content.'),
    '#weight' => 2,
    '#size' => 15,
  );
  $form['css_class'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['css_class'],
    '#title' => t('CSS class'),
    '#description' => t('CSS class of this custom content.'),
    '#weight' => 2,
    '#size' => 15,
  );

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

