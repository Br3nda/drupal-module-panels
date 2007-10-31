<?php
// $Id: common.inc,v 1.1.2.9 2007/10/31 23:58:10 merlinofchaos Exp $

/**
 * @file
 * Functions used by more than one panel implementation module.
*/

/**
 * A common settings page for Panels modules, because this code is relevant to
 * any modules that don't already ahve special requirements.
 */
function panels_common_settings($module_name = 'panels_common') {
  panels_load_include('plugins');
  $content_types = panels_get_content_types();
  $default_types = variable_get($module_name . '_default', NULL);
  if (!isset($default_types)) {
    $default_types = array('block' => TRUE, 'views' => TRUE, 'other' => TRUE);
    $skip = TRUE;
  }

  foreach ($content_types as $id => $info) {
    if (empty($info['single'])) {
      $default_options[$id] = t('New @s', array('@s' => $info['title']));
    }
  }

  $default_options['other'] = t('New content of other types');
  $form['panels_common_default'] = array(
    '#type' => 'checkboxes',
    '#title' => t('New content behavior'),
    '#description' => t('Select the default behavior of new content added to the system. If checked, new content will automatically be immediately available to be added to Panels pages. If not checked, new content will not be available until specifically allowed here.'),
    '#options' => $default_options,
    '#default_value' => array_keys(array_filter($default_types)),
  );

  if ($skip) {
    $form['markup'] = array('#value' => t('<p>Click Submit to be presented with a complete list of available content types set to the defaults you selected.</p>'));
    $form['skip'] = array('#type' => 'value', '#value' => TRUE);
  }
  else {
    // Rebuild the entire list, setting appropriately from defaults. Give
    // each type its own checkboxes set unless it's 'single' in which
    // case it can go into our fake other set.
    $available_content_types = panels_get_all_content_types();
    $allowed_content_types = variable_get($module_name . '_allowed_types', array());

    foreach ($available_content_types as $id => $types) {
      foreach ($types as $type => $info) {
        $key = $id . '-' . $type;
        $checkboxes = empty($content_types[$id]['single']) ? $id : 'other';
        $options[$checkboxes][$key] = $info['title'];
        if (!isset($allowed_content_types[$key])) {
          $allowed[$checkboxes][$key] = isset($default_types[$id]) ? $default_types[$id] : $default_types['other'];
        }
        else {
          $allowed[$checkboxes][$key] = $allowed_content_types[$key];
        }
      }
    }

    $form['content_types'] = array('#tree' => TRUE);
    // cheat a bit
    $content_types['other'] = array('title' => t('Other'), 'weight' => 10);
    foreach ($content_types as $id => $info) {
      if (isset($allowed[$id])) {
        $form['content_types'][$id] = array(
          '#prefix' => '<div class="panels-page-type-container">',
          '#suffix' => '</div>',
          '#type' => 'checkboxes',
          '#title' => t('Allowed @s content', array('@s' => $info['title'])),
          '#options' => $options[$id],
          '#default_value' => array_keys(array_filter($allowed[$id])),
        );
      }
    }
  }

  $form['module_name'] = array(
    '#type' => 'value',
    '#value' => $module_name,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  drupal_add_css(drupal_get_path('module', 'panels_page') . '/css/panels_page.css');
  return $form;
}

/**
 * Submit hook for panels_common_settings
 */
function panels_common_settings_submit($form_id, $form_values) {
  $module_name = $form_values['module_name'];
  variable_set($module_name . '_default', $form_values['panels_common_default']);
  if (!$form_values['skip']) {
    // merge the broken apart array neatly back together
    variable_set($module_name . '_allowed_types', call_user_func_array('array_merge', $form_values['content_types']));
  }
  drupal_set_message(t('Your changes have been saved.'));
}

/**
 * Based upon the settings, get the allowed types for this node.
 */
function panels_common_get_allowed_types($module, $contexts = NULL, $has_content = FALSE, $default_defaults = array(), $default_allowed_types = array()) {

  // Get a list of all types that are available

  $default_types = variable_get($module . '_defaults', $default_defaults);
  $allowed_types = variable_get($module .'_allowed_types', $default_allowed_types);

  // By default, if they haven't gone and done the initial setup here,
  // let all 'other' types (which will be all types) be available.
  if (!isset($default_types['other'])) {
    $default_types['other'] = TRUE;
  }

  panels_load_include('plugins');
  $content_types = panels_get_available_content_types($contexts, $has_content, $allowed_types, $default_types);

  return $content_types;
}

/**
 * The layout information fieldset displayed at admin/edit/panel-%implementation%/add/%layout%.
 */
function panels_common_get_layout_information($panel_implementation) {
  $form = array();
  panels_load_include('plugins');
  $layout = panels_get_layout($panel_implementation->display->layout);

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Layout'),
  );

  $form['layout-icon'] = array(
    '#value' => panels_print_layout_icon($panel_implementation->display->layout, $layout),
  );

  $form['layout-display'] = array(
    '#value' => check_plain($layout['title']),
  );
  $content = '<dl class="content-list">';

  foreach (panels_get_panels($layout, $panel_implementation->display) as $panel_id => $title) {
    $content .= "<dt>$title</dt><dd>";
    if ($panel_implementation->display->panels[$panel_id]) {
      $content .= '<ol>';
      foreach ($panel_implementation->display->panels[$panel_id] as $pid) {
        $content .= '<li>'. panels_get_pane_title($panel_implementation->display->content[$pid]) .'</li>';
      }
      $content .= '</ol>';
    }
    else {
      $content .= t('Empty');
    }
    $content .= '</dd>';
  }
  $content .= '</dl>';

  $form['layout-content'] = array(
    '#value' => $content,
  );

  return $form;
}

// ---------------------------------------------------------------------------
// Ajax tools

/**
 * Incoming menu function for ajax calls. This routes to the proper 'module'
 * -- we really only need this because common.inc relies on panels.module for
 * its menu hook, and this way the code stays together.
 */
function panels_common_ajax($module = NULL, $data = NULL, $info = NULL) {
  if ($module == 'panel_settings') {
    return panels_common_panel_settings_ajax($data, $info);
  }
  panels_ajax_render(t('An error occurred'), t('Error'));
}

