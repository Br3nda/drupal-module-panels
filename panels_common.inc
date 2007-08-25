<?php
// $Id: panels_common.inc,v 1.1.2.5 2007/08/25 16:14:31 merlinofchaos Exp $

/**
 * @file
 * Functions used by more than one panel implementation module.
*/

/**
 * A common settings page for Panels modules, because this code is relevant to
 * any modules that don't already ahve special requirements.
 */
function panels_common_settings($module_name = 'panels_common') {
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
    $available_content_types = panels_get_available_content_types();
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
function panels_common_get_allowed_types($module, $default_defaults = array(), $default_allowed_types = array()) {
  $content_types = panels_get_available_content_types();
  $default_types = variable_get($module . '_defaults', $default_defaults);
  $allowed_types = variable_get($module .'_allowed_types', $default_allowed_types);

  // By default, if they haven't gone and done the initial setup here,
  // let all 'other' types (which will be all types) be available.
  if (!isset($default_types['other'])) {
    $default_types['other'] = TRUE;
  }
  foreach ($content_types as $id => $types) {
    foreach ($types as $type => $info) {
      $key = $id . '-' . $type;
      if (!isset($allowed_types[$key])) {
        $allowed_types[$key] = isset($default_types[$id]) ? $default_types[$id] : $default_types['other'];
      }
      if (!$allowed_types[$key]) {
        unset($content_types[$id][$type]);
      }
    }
  }
  return $content_types;
}

/**
 * The layout information fieldset displayed at admin/build/panel-%implementation%/add/%layout%.
 */
function panels_common_get_layout_information($panel_implementation) {
  $form = array();
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

/**
 * The panel settings fieldset that can optionally be added to
 * admin/build/panel-%implementation%/%panel_id%/edit, if the panels
 * implementation wants to support panel settings.
 */
function panels_common_panel_settings($display, $panels_implementation = '', $panels_implementation_human = '') {
  $panel_settings = $display->panel_settings;
  $style_name = ($panel_settings['style']) ? $panel_settings['style'] : 'default';
  $style = panels_get_panel_style($style_name);

  // Let the user choose between panel styles that are available for any
  // panels implementation or specifically to this one.
  $options = array();
  foreach (panels_get_panel_styles() as $name => $properties) {
    $implementations = $properties['panels implementations'];
    if (!is_array($implementations) || count($implementations) == 0 || in_array($panels_implementation, $implementations)) {
      $options[$name] = $properties['label'];
    }
  }

  $form = array();
  $form['display'] = array('#type' => 'value', '#value' => $display);
  $form['panel_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Panel settings'),
    '#tree' => TRUE,
  );
  $form['panel_settings']['style'] = array(
    '#type' => 'radios',
    '#title' => t('Panel style'),
    '#options' => $options,
    '#description' => t(
      'The style in which the panes of each panel in this %panels-implementation-human will be displayed.',
      array('%panels-implementation-human' => $panels_implementation_human)
    ),
    '#default_value' => $style['name'],
  );
  $form['panel_settings']['style_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Panel style settings'),
  );
  if (in_array($style['module'], module_list()) && function_exists($style['module'] .'_panels_panel_style_settings_form')) {
    $style_settings = module_invoke($style['module'], 'panels_panel_style_settings_form', $style['name'], $display->panel_settings);
    if (is_array($style_settings)) {
      $form['panel_settings']['style_settings'] += $style_settings;
    }
  }
  else {
    $form['panel_settings']['style_settings']['#description'] = t('The currently active style does not have any settings.');
  }
  $form['#validate']['panels_common_panel_settings_validate'] = array($style['module'], $style['name']);

  return $form;
}

/**
 * Allows panel styles to validate their style settings.
 */
function panels_common_panel_settings_validate($form, $module, $style) {
  global $form_values;

  $panel_settings = $form_values['display']->panel_settings;
  module_invoke($module, 'panels_panel_style_settings_validate', $style, $panel_settings, $form['panel_settings']['style_settings'], $form_values['panel_settings']['style_settings']);
}
