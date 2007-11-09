<?php
// $Id: user.inc,v 1.1.2.4 2007/11/09 22:35:38 merlinofchaos Exp $
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
    'no ui' => TRUE,
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

  return $form;
}
