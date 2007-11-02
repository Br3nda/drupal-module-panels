<?php
// $Id: user.inc,v 1.1.2.2 2007/11/02 22:24:47 merlinofchaos Exp $
/**
 * @file contexts/user.inc
 *
 * Plugin to provide a user context
 */

function panels_user_panels_contexts() {
  $args['user'] = array(
    'title' => t("User"),
    'description' => t('A single user object.'),
    'context' => 'panels_context_create_user',
    'settings form' => 'panels_context_user_settings_form',
    'settings form validate' => 'panels_context_user_settings_form_validate',
  );
  return $args;
}

/**
 * It's important to remember that $conf is optional here, because contexts
 * are not always created from the UI.
 */
function panels_context_create_user($empty, $data = NULL, $conf = FALSE) {
  $context = new panels_context('user');
  $context->plugin = 'user';

  if ($empty) {
    return $context;
  }

  if ($conf) {
    $data = user_load(array('uid' => $data['uid']));
  }

  if (!empty($data)) {
    $context->data = $data;
    $context->title = $data->name;
    $context->argument = $data->vid;
    return $context;
  }
}

function panels_context_user_settings_form($conf, $external = FALSE) {
  $options = array();
  if ($external) {
    $options[0] = t('External source');
  }

  foreach (taxonomy_get_vocabularies() as $vid => $user) {
    $options[$vid] = $user->name;
  }

  $form['vid'] = array(
    '#title' => t('user'),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $conf['vids'],
    '#prefix' => '<div class="clear-block">',
    '#suffix' => '</div>',
  );
  if ($external) {
    $form['vid']['#description'] .= ' ' . t('Select external to require this from an external source such as a containing panel page.');
  }

  return $form;
}
