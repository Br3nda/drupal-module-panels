<?php
// $Id: node_form.inc,v 1.1.2.1 2007/09/27 19:25:52 merlinofchaos Exp $
/**
 * Callback function to supply a list of content types.
 */
function panels_node_form_panels_content_types() {
  $items['node_form'] = array(
    'title' => t('Node form'),
    'content_types' => 'panels_admin_content_types_node_form',
    'render callback' => 'panels_content_node_form',
    'add callback' => 'panels_admin_edit_node_form',
    'edit callback' => 'panels_admin_edit_node_form',
    'title callback' => 'panels_admin_title_node_form',
  );
  return $items;
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_node_form() {
  $content = array(
    'publishing' => array(
      'title' => t('Node form publishing options'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Publishing options on the Node form.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    ),
    'author' => array(
      'title' => t('Node form author information'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Author information on the Node form.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    ),
    'input_format' => array(
      'title' => t('Node form input format'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Input format for the body field on a node.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    ),
    'log' => array(
      'title' => t('Node form log message'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Log message for the node.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    ),
  );
  if (module_exists('comment')) {
    $content['comment'] = array(
      'title' => t('Node form comment settings'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Comment settings on the Node form.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    );
  }
  if (module_exists('menu')) {
    $content['menu'] = array(
      'title' => t('Node form menu settings'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('menu settings on the Node form.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    );
  }
  if (module_exists('path')) {
    $content['url_path'] = array(
      'title' => t('URL Path settings'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Publishing options on the Node form.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    );
  }
  if (module_exists('upload')) {
    $content['attachments'] = array(
      'title' => t('Node form file attachments'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('File attachments on the Node form.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    );
  }
  if (module_exists('taxonomy')) {
    $content['taxonomy'] = array(
      'title' => t('Node form categories'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Taxonomy categories for the node.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    );
  }
  if (module_exists('book')) {
    $content['book'] = array(
      'title' => t('Node form book options'),
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Book options for the node.'),
      'required context' => 'node_form',
      'category' => array(t('Form'), -9),
    );
  }
  return $content;
}

/**
 * Output function for the 'node' content type. Outputs a node
 * based on the module and delta supplied in the configuration.
 */
function panels_content_node_form($conf, $panel_args, &$context) {
  $block->module = 'node-form';

  switch ($conf['id']) {
    case 'publishing':
      $block->subject = t('Publishing options');
      $block->delta = 'publishing-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id) && $context->data['options']['#type'] == 'fieldset') {
          // remove the fieldset
          unset($context->data['options']['#type']);
          $block->content = drupal_render($context->data['options']);
        }
      }
      else {
        $block->content = t('Publishing options.');
      }
      break;
    case 'comment':
      $block->subject = t('Comment options');
      $block->delta = 'comment-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id) && !empty($context->data['comment_settings']['#access'])) {
          // remove the fieldset
          unset($context->data['comment_settings']['#type']);
          $block->content = drupal_render($context->data['comment_settings']);
        }
      }
      else {
        $block->content = t('Comment options.');
      }
      break;
    case 'author':
      $block->subject = $block->content = t('Authoring information');
      $block->delta = 'author-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id) && !empty($context->data['author']['#access'])) {
          // remove the fieldset
          unset($context->data['author']['#type']);
          $context->data['author']['name']['#size'] /= 2;
          $context->data['author']['date']['#size'] /= 2;
          $block->content = drupal_render($context->data['author']);
        }
      }
      else {
        $block->content = t('Authoring information.');
      }
      break;
    case 'menu':
      $block->subject = t('Menu options');
      $block->delta = 'menu-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id) && !empty($context->data['menu']['#access'])) {
          // remove the fieldset
          unset($context->data['menu']['#type']);
          $context->data['menu']['title']['#size'] /= 2;
          $context->data['menu']['description']['#size'] /= 2;
          $block->content = drupal_render($context->data['menu']);
        }
      }
      else {
        $block->content = t('Menu options.');
      }
      break;
    case 'url_path':
      $block->subject = t('URL path options');
      $block->delta = 'url-path-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id) && !empty($context->data['path']['#access'])) {
          // remove the fieldset
          unset($context->data['path']['#type']);
          $context->data['path']['path']['#size'] /= 2;
          $block->content = drupal_render($context->data['path']);
        }
      }
      else {
        $block->content = t('URL Path options.');
      }
      break;
    case 'attachments':
      $block->subject = t('Attach files');
      $block->delta = 'url-path-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id) && !empty($context->data['attachments']['#access'])) {
          // remove the fieldset
          unset($context->data['attachments']['#type']);
          $block->content = drupal_render($context->data['attachments']);
        }
      }
      else {
        $block->content = t('Attach files.');
      }
      break;
    case 'taxonomy':
      $block->subject = t('Categories');
      $block->delta = 'url-path-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id) && !empty($context->data['taxonomy'])) {
          // remove the fieldset
          unset($context->data['taxonomy']['#type']);
          $block->content = drupal_render($context->data['taxonomy']);
        }
      }
      else {
        $block->content = t('Categories.');
      }
      break;
    case 'book':
      $block->subject = t('Book options');
      $block->delta = 'book-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id)) {
          $block->content = '';
          if ($context->data['parent']['#type'] != 'value') {
            $block->content .= drupal_render($context->data['parent']);
          }
          if ($context->data['weight']['#type'] != 'value') {
            $block->content .= drupal_render($context->data['weight']);
          }
        }
      }
      else {
        $block->content = t('Book options.');
      }
      break;
    case 'input_format':
      $block->subject = t('Input format');
      $block->delta = 'format-options';

      if (isset($context->data)) {
        if (!empty($context->data->form_id) && !empty($context->data['body_filter']['format'])) {
          // remove the fieldset
          unset($context->data['body_filter']['format']['#type']);
          $block->content = drupal_render($context->data['body_filter']['format']);
        }
      }
      else {
        $block->content = t('Input body_filter.');
      }
      break;
  }

  if (!empty($conf['override_title'])) {
    $block->subject = filter_xss_admin($conf['override_title_text']);
  }

  return $block;
}

/**
 * Returns an edit form for the custom type.
 */
function panels_admin_edit_node_form($id, $parents, $conf = array()) {
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

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id
  );

  return $form;
}

function panels_admin_title_node_form($conf, $context) {
  return t('Node form');
}
