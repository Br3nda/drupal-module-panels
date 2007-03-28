<?php
/**
 * Callback function to supply a list of content types.
 */
function panels_custom_panels_content_types() {
  $items['custom'] = array(
    'name' => t('Custom'),
    'callback' => 'panels_content_custom',
    'admin' => 'panels_admin_custom',
    // this is so simple we don't use the add form
    'no_add' => true,
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
    $id == 'id="' . $id . '"';
  }

  $output = '<div class="panels-custom ' . $class . '" ' . $id . '>';
  $output .= $content;
  $output .= '</div>';
  return $output;
}

/** 
 * Callback to perform administrative functions on the content block
 */
function panels_admin_custom($op, &$arg, $arg2 = NULL) {
  switch ($op) {
    case 'list':
      $conf = $arg;
      return '<strong>Custom</strong>';
    case 'add form':
      return NULL;
    case 'add':
      return $arg;
    case 'edit':
      $conf = &$arg;
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
      $arg2[] = 'format';
      $form['format'] = filter_form($conf['format'], 1, $arg2);

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
    case 'validate':
      // This one has nothing to validate.
      $form = &$arg;
      return;
    case 'save':
      // For this one, the form values go directly into the config.
      $form = &$arg;
      return $form;
  }
}
