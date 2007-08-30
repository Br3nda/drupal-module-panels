<?php
// $Id: common.inc,v 1.1.2.2 2007/08/30 02:45:22 merlinofchaos Exp $

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
 * The layout information fieldset displayed at admin/edit/panel-%implementation%/add/%layout%.
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
 * admin/panels/panel-%implementation%/%panel_id%/edit, if the panels
 * implementation wants to support panel settings.
 */
function panels_common_panel_settings($display, $panels_implementation = '', $label = '') {
  $panel_settings = $display->panel_settings;
  $style = panels_get_style((!empty($panel_settings['style'])) ? $panel_settings['style'] : 'default');

  // Let the user choose between panel styles that are available for any
  // panels implementation or specifically to this one.
  $options = array();
  foreach (panels_get_styles() as $name => $properties) {
    $implementations = $properties['panels implementations'];
    if (!empty($properties['hidden'])) {
      continue;
    }

    if (!is_array($implementations) || count($implementations) == 0 || in_array($panels_implementation, $implementations)) {
      // TODO: 'label' is deprecated, will go away.
      $options[$name] = empty($properties['title']) ? $properties['label'] : $properties['title'];
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
    '#type' => 'select',
    '#title' => t('Panel style'),
    '#options' => $options,
    '#description' => t(
      'The style in which the panes of each panel in this %panels-label will be displayed.',
      array('%panels-label' => $label)
    ),
    '#id' => 'panel-settings-style',
    '#default_value' => $style['name'],
  );

  // Is this form being posted? If so, check cache.
  if (!empty($_POST)) {
    $style_settings = panels_common_cache_get('style_settings', $display->did);
  }
  if (!isset($style_settings)) {
    $style_settings = $panel_settings['style_settings'];
    panels_common_cache_set('style_settings', $display->did, $style_settings);
  }
  $form['panel_settings']['style_settings'] = array(
    '#type' => 'value',
    '#value' => $style_settings,
  );
  $form['panel_settings']['edit_style'] = array(
    '#type' => 'submit',
    '#id' => 'panels-style-settings',
    '#value' => t('Edit style settings'),
  );
  
  // while we don't use this directly some of our forms do.
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/autocomplete.js');

  $ajax = array('panels' => array(
    'ajaxURL' => url('panels/common/ajax/panel_settings/' . $display->did, NULL, NULL, TRUE),
    'closeText' => t('Close Window'),
    'closeImage' => theme('image', panels_get_path('images/close.gif'), t('Close window'), t('Close window')),
    'throbber' => theme('image', panels_get_path('images/throbber.gif'), t('Loading...'), t('Loading')),
  ));

  $form['panel_settings']['did'] = array(
    '#type' => 'value',
    '#value' => $display->did,
  );

  drupal_add_js(panels_get_path('js/lib/dimensions.js'));
  drupal_add_js(panels_get_path('js/lib/mc.js'));
  drupal_add_js(panels_get_path('js/lib/form.js'));
  drupal_add_js($ajax, 'setting');
  drupal_add_js(panels_get_path('js/edit_settings.js'));
  drupal_add_js(panels_get_path('js/modal_forms.js'));
  drupal_add_css(panels_get_path('css/panels_dnd.css'));

  return $form;
}

function panels_common_panel_settings_validate($form_id, $form_values, $form) {
  $settings = panels_common_cache_get('style_settings', $form_values['panel_settings']['did']);
  form_set_value($form['panel_settings']['style_settings'], $settings);
}

function panels_common_panel_settings_submit($form_id, $form_values) {
  panels_common_cache_clear('style_settings', $form_values['panel_settings']['did']);
}

/**
 * Incoming menu function for ajax calls.
 */
function panels_common_ajax($module = NULL, $data = NULL, $info = NULL) {
  if ($module == 'panel_settings') {
    return panels_common_panel_settings_ajax($data, $info);
  }
  panels_ajax_render(t('An error occurred'), t('Error'));
}

function panels_common_panel_settings_ajax($did, $name) {
  $style = panels_get_style($name);
  $style_settings = panels_common_cache_get('style_settings', $did);
  
  $args = 
  $form = drupal_retrieve_form('panels_common_style_settings_form', $did, $style, $style_settings);
  if ($_POST) {
    $form['#programmed'] = TRUE;
    $form['#post'] = $_POST;
  }

//  drupal_prepare_form('panels_common_style_settings_form', $form);

  $result = drupal_process_form('panels_common_style_settings_form', $form);
  if ($result) { 
    // successful submit
    $output = new stdClass();
    $output->type = $output->output = 'dismiss';
    panels_ajax_render($output);
  }
  else {
    $output = drupal_render_form('panels_common_style_settings_form', $form);
    panels_ajax_render($output, t('Edit style settings for @style', array('@style' => $style['title'])), url($_GET['q'], NULL, NULL, TRUE));
  }
}

function panels_common_style_settings_form($did, $style, $style_settings) {
  $form['start_form'] = array('#value' => '<div class="modal-form">');
  $form['description'] = array(
    '#prefix' => '<div class="description">',
    '#suffix' => '</div>',
    '#value' => check_plain($style['description']),
  );

  if (isset($style['settings form']) && function_exists($style['settings form'])) {
    $form['style_settings'] = $style['settings form']($style_settings);
    $form['style_settings']['#tree'] = TRUE;
  }
  else {
    $form['markup'] = array(
      '#value' => t('This style does not have any settings.'),
    );
  }
  $form['end_form'] = array('#value' => '</div>');

  if (!isset($form['markup'])) {
    $form['style'] = array(
      '#type' => 'value',
      '#value' => $style,
    );

    $form['did'] = array(
      '#type' => 'value',
      '#value' => $did,
    );

    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }

  return $form;
}

/**
 * Allows panel styles to validate their style settings.
 */
function panels_common_style_settings_form_validate($form_id, $form_values, $form) {
  $style = $form_values['style'];

  if (isset($style['settings form validate']) && function_exists($style['settings form validate'])) {
    $style['settings form validate']($form, $form_values['style_settings']);
  }
}

/**
 * Allows panel styles to validate their style settings.
 */
function panels_common_style_settings_form_submit($form_id, $form_values) {
  $style = $form_values['style'];

  if (isset($style['settings form submit']) && function_exists($style['settings form submit'])) {
    $style['settings form submit']($form_values['style_settings']);
  }

  panels_common_cache_set('style_settings', $form_values['did'], $form_values['style_settings']);
  return 'dismiss';
}

// ---------------------------------------------------------------------------
// cache handling stuff for display editing

/**
 * Get an object from cache.
 */
function panels_common_cache_get($obj, $did) {
  static $cache = array();
  if (!array_key_exists($did, $cache)) {
    $data = cache_get(session_id() . ":$obj:$did", 'cache');
    $cache[$did] = unserialize($data->data);
  }
  return $cache[$did];
}

/**
 * Save the edited display into the cache.
 */
function panels_common_cache_set($obj, $did, $cache) {
  cache_set(session_id() . ":$obj:$did", 'cache', serialize($cache), time() + 86400);
}

/**
 * Clear a display from the cache; used if the editing is aborted.
 */
function panels_common_cache_clear($did) {
  cache_clear_all(session_id() . ":$obj:$did", 'cache');
}
