<?php
// $Id: form.inc,v 1.1.2.3 2007/10/18 22:27:18 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_form_panels_content_types() {
  if (module_exists('comment')) {
    $items['form'] = array(
      'title' => t('Generic form'),
      'content_types' => 'panels_admin_content_types_form',
      'single' => TRUE, // only provides a single content type
      'render callback' => 'panels_content_form',
      'add callback' => 'panels_admin_edit_form',
      'edit callback' => 'panels_admin_edit_form',
      'title callback' => 'panels_admin_title_form',
      'render last' => TRUE,
    );
    return $items;
  }
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_form() {
  return array(
    'node_type' => array(
      'title' => t('General form'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('The general parts of a form.'),
      'required context' => new panels_required_context('form'),
      'category' => array(t('Form'), -9),
    ),
  );
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_form($conf, $panel_args, &$context) {
  $block->module = 'form';

  if (isset($context->data)) {
    $block->subject = $context->form_title;
    if (!empty($context->form_id)) {
      // If this is a form, drupal_render it.
      $block->content = drupal_render($context->data);
    }
    else {
      // Otherwise just spit back what we were given. This is probably an
      // error message or something.
      $block->content = $context->data;
    }
    $block->delta = $context->form_id;
  }
  else {
    $block->subject = t('Form');
    $block->content = t('Form goes here.');
    $block->delta = 'unknown';
  }

  if (!empty($conf['override_title'])) {
    $block->subject = filter_xss_admin($conf['override_title_text']);
  }

  return $block;
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_form($id, $parents, $conf = array()) {
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

function panels_admin_title_form($conf, $context) {
  return t('Form');
}
