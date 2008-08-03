<?php
// $Id: display_edit.inc,v 1.1.2.63 2008/08/03 04:34:01 sdboyer Exp $


/*
 * @file display_edit.inc
 * Core Panels API include file containing various display-editing functions.
 * This includes all the basic editing forms (content, layout, layout settings)
 * as well as the ajax modal forms associated with them.
 */

/**
 * Handle calling and processing of the form for editing display content.
 *
 * Helper function for panels_edit().
 *
 * @see panels_edit() for details on the various behaviors of this function.
 */
function _panels_edit($display, $destination, $content_types) {
  $did = $display->did;
  if (!$did) {
    $display->did = $did = 'new';
  }

  // Load the display being edited from cache, if possible.
  if (!empty($_POST) && is_object($cache = panels_cache_get($did))) {
    $display = $cache->display;
  }
  else {
    panels_cache_clear($did);
    $cache = new stdClass();
    $cache->display = $display;
    $cache->content_types = $content_types;
    panels_cache_set($did, $cache);
  }

  // Break out the form pieces so we can return the new $display upon
  // successful submit.

  $form_id = 'panels_edit_display';
  $form = drupal_retrieve_form($form_id, $display, $destination);

  if ($result = drupal_process_form($form_id, $form)) {
    // successful submit
    return $result;
  }

  $output = drupal_render_form($form_id, $form);
  $output .= theme('panels_hidden');
  return $output;
}

/**
 * Form definition for the panels display editor
 *
 * No validation function is necessary, as all 'validation' is handled
 * either in the lead-up to form rendering (through the selection of
 * specified content types) or by the validation functions specific to
 * the ajax modals & content types.
 *
 * @ingroup forms
 * @see panels_edit_display_submit()
 */
function panels_edit_display($display, $destination) {
  $form['did'] = array(
    '#type' => 'hidden',
    '#value' => $display->did,
    '#id' => 'panel-did',
  );

  $form['op'] = array(
    '#type' => 'hidden',
    '#id' => 'panel-op',
  );

  $form['panels_display'] = array(
    '#type' => 'value',
    '#value' => $display,
  );

  if (!empty($destination)) {
    $form['destination'] = array(
      '#type' => 'value',
      '#value' => $destination,
    );
  }
  else {
    $form['#redirect'] = FALSE;
  }

  $explanation_text = '<p>';
  $explanation_text .= t('Grab the title bar of any pane to drag-and-drop it into another panel. Click the add pane button (!addicon) in any panel to add more content. Click the configure (!configicon) button on any pane to re-configure that pane. Click the cache (!cacheicon) button to configure caching for that pane specifically. Click the show/hide (!showicon/!hideicon) toggle button to show or hide that pane. Panes hidden in this way will be hidden from <em>everyone</em> until the hidden status is toggled off.',
    array(
      '!addicon' => '<span class="inline-icon-help">' . theme('image', panels_get_path('images/icon-addcontent.png'), t('Add content to this panel'), t('Add content to this panel')) . '</span>',
      '!configicon' => '<span class="inline-icon-help">' . theme('image', panels_get_path('images/icon-configure.png'), t('Configure this pane'), t('Configure this pane')) . '</span>',
      '!cacheicon' => '<span class="inline-icon-help">' . theme('image', panels_get_path('images/icon-cache.png'), t('Control caching'), t('Control caching')) . '</span>',
      '!showicon' => '<span class="inline-icon-help">' . theme('image', panels_get_path('images/icon-showpane.png'), t('Show this pane'), t('Show this pane')) . '</span>',
      '!hideicon' => '<span class="inline-icon-help">' . theme('image', panels_get_path('images/icon-hidepane.png'), t('Hide this pane'), t('Hide this pane')) . '</span>',
    )
  );
  $explanation_text .= '</p>';

  $form['explanation'] = array(
    '#value' => $explanation_text,
  );

  $layout = panels_get_layout($display->layout);
  $layout_panels = panels_get_panels($layout, $display);

  $form['button']['#tree'] = TRUE;
  $caches = panels_get_caches();

  foreach ($display->content as $pid => $pane) {
    // This test ensures we don't put anything for panes that are in panels
    // that don't exist -- as can happen when flexible changes.
    if ($layout_panels[$pane->panel]) {
      $form['button'][$pid]['#tree'] = TRUE;
      if ($caches && user_access('use panels caching features')) {
        $form['button'][$pid]['cache'] = panels_add_button('icon-cache.png', t('Caching'), t('Control caching'), 'pane-cache');
      }
      $form['button'][$pid]['show_hide'] = panels_add_button($pane->shown ? 'icon-hidepane.png' : 'icon-showpane.png', t('Show/Hide Toggle'), $pane->shown ? t('Hide this pane') : t('Show this pane'), 'pane-toggle-shown');
      $form['button'][$pid]['configure'] = panels_add_button('icon-configure.png', t('Configure'), t('Configure this pane'), 'pane-configure');
      $form['button'][$pid]['delete'] = panels_add_button('icon-delete.png', t('Delete'), t('Remove this pane'), 'pane-delete');
    }
  }

  foreach ($layout_panels as $id => $name) {
    $form['panels'][$id]['add'] = panels_add_button('icon-addcontent.png', t('Add content'), t('Add content to this panel'), 'pane-add', "pane-add-$id");
  }

  $form['panel'] = array('#tree' => TRUE);
  $form['panel']['pane'] = array('#tree' => TRUE);

  foreach ($layout_panels as $panel_id => $title) {
    $form['panel']['pane'][$panel_id] = array(
      // Use 'hidden' instead of 'value' so the js can access it.
      '#type' => 'hidden',
      '#default_value' => implode(',', (array) $display->panels[$panel_id]),
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#id' => 'panels-dnd-save',
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );

  $form['hide'] = array(
    '#prefix' => '<span id="panels-js-only">',
    '#suffix' => '</span>',
  );

  $form['hide']['hide-all'] = array(
    '#type' => 'submit',
    '#value' => t('Hide all'),
    '#id' => 'panels-hide-all',
  );

  $form['hide']['show-all'] = array(
    '#type' => 'submit',
    '#value' => t('Show all'),
    '#id' => 'panels-show-all',
  );

  $form['hide']['cache-settings'] = array(
    '#type' => 'submit',
    '#value' => t('Cache settings'),
    '#id' => 'panels-cache-settings',
  );

  return $form;
}

/**
 * Theme the form for editing display content.
 *
 * @ingroup themeable
 *
 * @param array $form
 *  A structured FAPI $form array.
 *
 * @return string $output
 *  HTML ready to be rendered. Note that the html produced here should be printed,
 *  not returned, as it bypasses block rendering. Block rendering must be bypassed
 *  because sidebars created using negative margins break ajax drag-and-drop.
 */
function theme_panels_edit_display($form) {
  _panels_js_files();
  $display = $form['panels_display']['#value'];
  $layout = panels_get_layout($display->layout);
  $layout_panels = panels_get_panels($layout, $display);
  $save_buttons = drupal_render($form['submit']) . drupal_render($form['cancel']);

  foreach ($layout_panels as $panel_id => $title) {
    foreach ((array) $display->panels[$panel_id] as $pid) {
      $pane = $display->content[$pid];
      $left_buttons = NULL;
      $buttons = drupal_render($form['button'][$pid]['configure']);
      if (!empty($form['button'][$pid]['cache'])) {
        $buttons .= drupal_render($form['button'][$pid]['cache']);
      }
      $buttons .= drupal_render($form['button'][$pid]['show_hide']);
      $buttons .= drupal_render($form['button'][$pid]['delete']);
      $content[$pane->panel] .= panels_show_pane($display, $pane, $left_buttons, $buttons);
    }
    $content[$panel_id] = theme('panels_panel_dnd', $content[$panel_id], $panel_id, $title, drupal_render($form['panels'][$panel_id]['add']));
  }
  $output .= drupal_render($form);
  $output .= theme('panels_dnd', panels_render_layout($layout, $content, '', $display->layout_settings));
  $output .= $save_buttons;
  return $output;
}

/**
 * Handle form submission of the display content editor.
 *
 * The behavior of this function is fairly complex and irregular compared to
 * most FAPI submit handlers. See the documentation on panels_edit() for a
 * detailed discussion of this behavior.
 */
function panels_edit_display_submit($form_id, $form_values) {
  $display = $form_values['panels_display'];
  if ($form_values['op'] == t('Save')) {
    $old_content = $display->content;
    $display->content = array();

    foreach ($form_values['panel']['pane'] as $panel_id => $panes) {
      $display->panels[$panel_id] = array();
      if ($panes) {
        $pids = explode(',', $panes);
        // need to filter the array, b/c passing it in a hidden field can generate trash
        foreach (array_filter($pids) as $pid) {
          if ($old_content[$pid]) {
            $display->panels[$panel_id][] = $pid;
            $old_content[$pid]->panel = $panel_id;
            $display->content[$pid] = $old_content[$pid];
          }
        }
      }
    }
    drupal_set_message(t('Panel content has been updated.'));
    panels_save_display($display);
  }

  panels_cache_clear($display->did);
  if (empty($form_values['destination'])) {
    return $display;
  }
}

/**
 * Handle calling and processing of the form for editing display layouts.
 *
 * Helper function for panels_edit_layout().
 *
 * @see panels_edit_layout() for details on the various behaviors of this function.
 */
function _panels_edit_layout($display, $finish, $destination, $allowed_layouts) {
  panels_load_include('common');
  // module_name has been provided; the data was saved by the api_save() method.
  if (is_string($allowed_layouts)) {
    $allowed_layouts = unserialize(variable_get($allowed_layouts . "_allowed_layouts", serialize('')));
  }
  // if no parameter was provided, or the variable_get failed
  if (!$allowed_layouts) {
    // tries to load the common panels allowed layouts
    $allowed_layouts = unserialize(variable_get('panels_common_allowed_layouts', serialize('')));
    if (!$allowed_layouts) {
      // still no dice. simply creates a dummy version where all layouts are allowed.
      $allowed_layouts = new panels_allowed_layouts();
      $allowed_layouts->allow_new = TRUE;
    }
  }
  // sanitize allowed layout listing; this is redundant if the $allowed_layouts param was null, but the data is cached anyway
  $allowed_layouts->sync_with_available();
  // Break out the form pieces so we can return the new $display upon
  // successful submit.
  $form_id = 'panels_choose_layout';
  $form = drupal_retrieve_form($form_id, $display, $finish, $destination, array_filter($allowed_layouts->allowed_layout_settings));

  if ($result = drupal_process_form($form_id, $form)) {
    // successful submit
    return $result;
  }
  $output = drupal_render_form($form_id, $form);
  return $output;
}

/**
 * Form definition for the display layout editor.
 *
 * @ingroup forms
 */
function panels_choose_layout($display, $finish, $destination, $allowed_layouts) {
  $layouts = array();
  $available_layouts = panels_get_layouts();
  foreach ($available_layouts as $layout_key => $layout_settings) {
    if (!empty($allowed_layouts[$layout_key])) {
      $layouts[$layout_key] = $layout_settings;
    }
  }
  foreach ($layouts as $id => $layout) {
    $options[$id] = panels_print_layout_icon($id, $layout, check_plain($layout['title']));
  }

  drupal_add_js(panels_get_path('js/layout.js'));
  $form['layout'] = array(
    '#type' => 'radios',
    '#title' => t('Choose layout'),
    '#options' => $options,
    '#default_value' => in_array($display->layout, array_keys($layouts)) ? $display->layout : NULL,
    '#attributes' => array('class' => 'clear-block'),
  );

  $form['variables'] = array(
    '#type' => 'value',
    '#value' => array($display, $finish, $destination),
  );

  if (empty($destination)) {
    $form['#redirect'] = FALSE;
  }

  if ($_POST['op'] && $_POST['op'] != t('Back') && $display->content) {
    $form['#post'] = $_POST;
    $form = form_builder('panels_choose_layout', $form);
    unset($form['#post']);
    $form['layout']['#type'] = 'hidden';
    panels_change_layout($form, $display, $form['layout']['#value']);
  }

  if (($_POST['op'] && $_POST['op'] != t('Back')) || !$display->content) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => $finish,
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
    );
  }

  // no token please
  $form['#token'] = FALSE;
  return $form;
}

/**
 * Handle form submission of the display layout editor.
 *
 * The behavior of this function is fairly complex and irregular compared to
 * most FAPI submit handlers. See the documentation on panels_edit_layout() for
 * detailed discussion of this behavior.
 */
function panels_choose_layout_submit($form_id, $form_values) {
  list($display, $finish, $destination) = $form_values['variables'];
  $new_layout_id = $form_values['layout'];

  if ($form_values['op'] == $finish) {
    if (!empty($form_values['old'])) {
      foreach ($form_values['old'] as $id => $new_id) {
        $content[$new_id] = array_merge((array) $content[$new_id], $display->panels[$id]);
        foreach ($content[$new_id] as $pid) {
          $display->content[$pid]->panel = $new_id;
        }
      }

      $display->panels = $content;
    }

    $display->layout = $new_layout_id;

    // save it back to our session.
    panels_save_display($display);
    if (empty($destination)) {
      return $display;
    }

    return $destination;
  }

  return FALSE;
}

/**
 * Form definition for the display layout converter.
 *
 * This form is only triggered if the user attempts to change the layout
 * for a display that has already had content assigned to it. It allows
 * the user to select where the panes located in to-be-deleted panels should
 * be relocated to.
 *
 * @ingroup forms
 *
 * @param array $form
 *  A structured FAPI $form array.
 * @param object $display instanceof panels_display \n
 *  The panels_display object that was modified on the preceding display layout
 *  editing form.
 * @param string $new_layout_id
 *  A string containing the name of the layout the display is to be converted to.
 *  These strings correspond exactly to the filenames of the *.inc files in panels/layouts.
 *  So, if the new layout that's been selected is the 'Two Column bricks' layout, then
 *  $new_layout_id will be 'twocol_bricks', corresponding to panels/layouts/twocol_bricks.inc.
 */
function panels_change_layout(&$form, $display, $new_layout_id) {
  $new_layout = panels_get_layout($new_layout_id);
  $new_layout_panels = panels_get_panels($new_layout, $display);

  $options = $new_layout_panels;
  $keys = array_keys($options);
  $default = $options[0];

  $old_layout = panels_get_layout($display->layout);

  $form['container'] = array(
    '#prefix' => '<div class="change-layout-display">',
    '#suffix' => '</div>',
  );

  $form['container']['old_layout'] = array(
    '#value' => panels_print_layout_icon($display->layout, $old_layout, check_plain($old_layout['title'])),
  );

  $form['container']['right_arrow'] = array(
    '#value' => theme('image', drupal_get_path('module', 'panels') . '/images/go-right.png'),
  );
  $form['container']['new_layout'] = array(
    '#value' => panels_print_layout_icon($new_layout_id, $new_layout, check_plain($new_layout['title'])),
  );

  $form['container-clearer'] = array(
    // TODO: FIx this ot use clear-block instead
    '#value' => '<div style="clear: both;"></div>',
  );

  $form['old'] = array(
    '#tree' => true,
    '#prefix' => '<div class="panels-layout-list">',
    '#suffix' => '</div>',
  );

  $old_layout_panels = panels_get_panels($old_layout, $display);
  foreach ($display->panels as $id => $content) {
    $form['old'][$id] = array(
      '#type' => 'select',
      '#title' => t('Move content in @layout to', array('@layout' => $old_layout_panels[$id])),
      '#options' => $options,
      '#default_value' => array_key_exists($id, $options) ? $id : $default,
    );
  }

  $form['back'] = array(
    '#type' => 'submit',
    '#value' => t('Back'),
  );
  return $form;
}

/**
 * Handle calling and processing of the form for editing display layout settings.
 *
 * Helper function for panels_edit_layout_settings().
 *
 * @see panels_edit_layout_settings() for details on the various behaviors of this function.
 */
function _panels_edit_layout_settings($display, $finish, $destination, $title) {
  // Break out the form pieces so we can return the new $display upon
  // successful submit.
  $form_id = 'panels_edit_layout_settings_form';
  $form = drupal_retrieve_form($form_id, $display, $finish, $destination, $title);

  if ($result = drupal_process_form($form_id, $form)) {
    // successful submit
    return $result;
  }
  $output = drupal_render_form($form_id, $form);
  return $output;
}

/**
 * Form definition for the display layout settings editor.
 *
 * @ingroup forms
 * @see panels_edit_layout_settings_form_validate()
 * @see panels_edit_layout_settings_form_submit()
 */
function panels_edit_layout_settings_form($display, $finish, $destination, $title) {
  $layout = panels_get_layout($display->layout);
  $form = array();
  if (!empty($layout['settings form']) && function_exists($layout['settings form'])) { // TODO doc the ability to do this as part of the API
    $form['layout_settings'] = $layout['settings form']($display, $layout, $display->layout_settings);
    $form['layout_settings']['#tree'] = TRUE;
  }

  if ($title) {
    $form['display_title'] = array (
      '#type' => 'fieldset',
      '#title' => t('Panel Title'),
      '#tree' => TRUE,
    );

    $form['display_title']['title'] = array(
      '#type' => 'textfield',
      '#size' => 35,
      '#default_value' => $display->title,
      '#title' => t('Title'),
      '#description' => t('The title of this panel. Your theme will render this text as the main page title when a user views this panel. Note that there are some circumstances in which this title can be overridden elsewhere.'),
    );

    $form['display_title']['hide_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide Title?'),
      '#default_value' => $display->hide_title,
      '#description' => t('Check this box to hide the main page title for this panel.'),
    );

    if (is_string($title)) {
      $form['display_title']['title']['#description'] .= t(" If you leave this field blank, then the default title, '@title', will be used instead.", array('@title' => $title));
    }
  }

  $form += panels_panel_settings($display);

  $form['variables'] = array(
    '#type' => 'value',
    '#value' => array($display, $finish, $destination, $title),
  );

  if (empty($destination)) {
    $form['#redirect'] = FALSE;
  }

  $form['layout'] = array(
    '#type' => 'value',
    '#value' => $layout,
  );

  // Always show a Save button even if they sent in a Next or something similar
  // button.
  if ($finish !== t('Save')) {
    $form['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $finish,
  );


  return $form;
}

function panels_edit_layout_settings_form_validate($form_id, $form_values, $form) {
  list($display, $finish, $destination) = $form_values['variables'];
  panels_panel_settings_validate($form_id, $form_values, $form);
  $layout = $form_values['layout'];
  if (!empty($layout['settings validate']) && function_exists($layout['settings validate'])) {
    $layout['settings validate']($form_values['layout_settings'], $form['layout_settings'], $display, $layout, $display->layout_settings);
  }
}

function panels_edit_layout_settings_form_submit($form_id, $form_values) {
  list($display, $finish, $destination, $title) = $form_values['variables'];
  panels_panel_settings_submit($form_id, $form_values);

  $layout = $form_values['layout'];
  if (!empty($layout['settings submit']) && function_exists($layout['settings submit'])) {
    $layout['settings submit']($form_values['layout_settings'], $display, $layout, $display->layout_settings);
  }

  if (isset($form_values['display_title']['title'])) {
    $display->title = $form_values['display_title']['title'];
    $display->hide_title = $form_values['display_title']['hide_title'];
  }

  if ($form_values['op'] == $finish || $form_values['op'] == t('Save')) {
    $display->layout_settings = $form_values['layout_settings'];
    $display->panel_settings = $form_values['panel_settings'];

    panels_save_display($display);
    drupal_set_message("Your layout settings have been saved.");
    if ($form_values['op'] != $finish) {
      // This forces us to come back here if they hit Save.
      $_REQUEST['destination'] = $_GET['q'];
    }
    if (empty($destination)) {
      return $display;
    }
    return $destination;
  }
}

/**
 * Display the edit form for a pane.
 *
 * @param $pane
 *   The pane to edit.
 * @param $parents
 *   The form api #parents array that this subform will live in.
 */
function panels_get_pane_edit_form($pane, $contexts, $parents) {
  return panels_ct_get_edit_form($pane->type, $pane->subtype, $contexts, $pane->configuration, $parents);
}

/**
 * Render a single pane in the edit environment.
 *
 * @param $pane
 *   The pane to render.
 * @param $left_buttons
 *   Buttons that go on the left side of the pane.
 * @param $buttons
 *   Buttons that go on the right side of the pane.
 * @param $skin
 *   If true, provide the outside div. Used to provide an easy way to get
 *   just the innards for ajax replacement
 */
// TODO check and see if $skin is ever FALSE; pane show/hide setting is dependent on it being TRUE. can't imagine it could be...
function panels_show_pane($display, $pane, $left_buttons, $buttons, $skin = TRUE) {
  $content_type = panels_get_content_type($pane->type);
  $block = new stdClass();
  if (empty($content_type)) {
    $block->title = '<em>' . t('Missing content type') . '</em>';
    $block->content = t('This pane\'s content type is either missing or has been deleted. This pane will not render.');
  }
  elseif (function_exists($content_type['editor render callback'])) {
    $block = $content_type['editor render callback']($display, $pane);
  }
  else {
    $block = _panels_render_preview_pane_disabled($pane, $display->context);
  }

  // This is just used for the title bar of the pane, not the content itself.
  // If we know the content type, use the appropriate title for that type,
  // otherwise, set the title using the content itself.
  $title = !empty($content_type) ? panels_get_pane_title($pane, $display->context) : $block->title;
  $output = theme('panels_pane_dnd', $block, $pane->pid, $title, $left_buttons, $buttons);
  if ($skin) {
    $class = 'panel-pane' . ($pane->shown ? '' : ' hidden-pane');
    $output = '<div class="' . $class . '" id="panel-pane-' . $pane->pid . '">' . $output . '</div>';
  }
  return $output;
}

/**
 * Provide filler content for dynamic pane previews in the editor, as they're just a
 * bad idea to have anyway, and can also cause infinite recursion loops that render the
 * editor inaccessible in some cases.
 *
 */
function _panels_render_preview_pane_disabled($pane, $context) {
  $block = new stdClass();
  $block->title   = panels_get_pane_title($pane, $context);
  $block->content = '<em>' . t("Dynamic content previews have been disabled to improve performance and stability for this editing screen.") . '</em>';
  return $block;
}

/**
 * Entry point into all ajax operations.
 *
 * @param string $op
 *  The name of the edit operation being performed.
 * @param integer $did
 *  The id of the $display object being edited (if any).
 * @param integer $pid
 *  The id of the pane object being edited (if any).
 */
// TODO deprecated. should be able to remove it.
function panels_ajax($op = NULL, $did = NULL, $pid = NULL) {
  switch ($op) {
    case 'submit-form':
      if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
        $output = panels_edit_submit_subform($cache->display);
      }
      break;

    default:
  }

  panels_ajax_render($output);
}

/**
 * Entry point for AJAX: 'Add Content' modal form, from which the user selects the
 * type of pane to add.
 *
 * @ingroup PanelsAjax
 *
 * @param int $did
 *  The display id of the $display object currently being edited.
 * @param string $panel_id
 *  A string with the name of the panel region to which the selected
 *  pane type will be added.
 */
function panels_ajax_add_content($did = NULL, $panel_id = NULL) {
  $output = new stdClass();
  if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
    $display = $cache->display;
    $layout = panels_get_layout($display->layout);
    $layout_panels = panels_get_panels($layout, $display);

    if ($layout && array_key_exists($panel_id, $layout_panels)) {
      $output->output = panels_add_content($cache, $panel_id);
      $output->type   = 'display';
      $output->title  = t('Add content to !s', array('!s' => $layout_panels[$panel_id]));
    }
  }
  panels_ajax_render($output);
}

/**
 * @ingroup PanelsAjax
 */
function panels_add_content($cache, $panel_id) {
  $return = new stdClass();
  $return->type  = 'display';
  $return->title = t('Choose type');
  // TODO get rid of this deprecated method
  panels_set('return', $return);

  if (!isset($cache->content_types)) {
    $cache->content_types = panels_get_available_content_types();
  }

  $weights    = array(t('Contributed modules') => 0);
  $categories = array();
  $titles     = array();

  foreach ($cache->content_types as $type_name => $subtypes) {
    if (is_array($subtypes)) {
      foreach ($subtypes as $subtype_name => $subtype_info) {
        $title = filter_xss_admin($subtype_info['title']);
        $description = isset($subtype_info['description']) ? $subtype_info['description'] : $title;

        if (isset($subtype_info['icon'])) {
          $icon = $subtype_info['icon'];
          $path = isset($subtype_info['path']) ? $subtype_info['path'] : panels_get_path("content_types/$type_name");
        }
        else {
          $icon = 'no-icon.png';
          $path = panels_get_path('images');
        }

        if (isset($subtype_info['category'])) {
          if (is_array($subtype_info['category'])) {
            list($category, $weight) = $subtype_info['category'];
            $weights[$category] = $weight;
          }
          else {
            $category = $subtype_info['category'];
            if (!isset($weights['category'])) {
              $weights[$category] = 0;
            }
          }
        }
        else {
          $category = t('Contrib modules');
        }

        $output = '<div class="content-type-button clear-block">';
        $link_text = theme('image', $path . '/' . $icon, $description, $description);
        $output .= l($link_text, 'javascript: void()', array('class' => 'panels-modal-add-config', 'id' => $type_name . '-' . $panel_id . '-' . $subtype_name), NULL, NULL, NULL, TRUE);
        $output .= "<div>" . l($title, 'javascript: void()', array('class' => 'panels-modal-add-config', 'id' => $type_name . '-' . $panel_id . '-' . $subtype_name), NULL, NULL, NULL, TRUE) . "</div>";
        $output .= '</div>';
        if (!isset($categories[$category])) {
          $categories[$category] = array();
          $titles[$category] = array();
        }

        $categories[$category][] = $output;
        $titles[$category][] = $title;
      }
    }
  }
  if (!$categories) {
    $output = t('There are no content types you may add to this display.');
  }
  else {
    asort($weights);
    $output = '';

    $columns = 3;
    foreach (range(1, $columns) as $column) {
      $col[$column] = '';
      $size[$column] = 0;
    }

    foreach ($weights as $category => $weight) {
      $which = 1; // default;
      $count = count($titles[$category]) + 3;
      // Determine which column to use by seeing which column has the most
      // free space. This algorithm favors left.
      foreach (range($columns, 2) as $column) {
        if ($size[$column - 1] - $size[$column] > $count / 2) {
          $which = $column;
          break;
        }
      }

      $col[$which] .= '<div class="panels-section">';
      $col[$which] .= '<h3 class="panels-section-title">' . $category . '</h3>';
      $col[$which] .= '<div class="panels-section-decorator"></div>';
      $col[$which] .= '<div class="panels-section-content">';
      if (is_array($titles[$category])) {
        natcasesort($titles[$category]);
        foreach ($titles[$category] as $id => $title) {
          $col[$which] .= $categories[$category][$id];
        }
      }
      $col[$which] .= '</div>';
      $col[$which] .= '</div>';
      $size[$which] += $count; // add 3 to account for title.
    }

    foreach ($col as $column) {
      $output .= '<div class="panels-section-column">' . $column . '</div>';
    }
  }
  return $output;
}

/**
 * Entry point for AJAX: toggle pane show/hide status.
 *
 * @ingroup PanelsAjax
 *
 * @param int $did
 *  The display id for the display object currently being edited. Errors out silently if absent.
 * @param int $pid
 *  The pane id for the pane object whose show/hide state we're toggling.
 * @param string $op
 *  The operation - showing or hiding - that this should perform. This could be calculated from
 *  cached values, but given that this is a toggle button and people may click it too fast,
 *  it's better to leave the decision on which $op to use up to the js than to calculate it here.
 */
function panels_ajax_toggle_shown($did = NULL, $pid = NULL, $op = NULL) {
  if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
    $ajax_vars = new stdClass();
    list($ajax_vars->type, $ajax_vars->id, $ajax_vars->old_op) = array('toggle-shown', $pid, drupal_strtolower($op));

    if ($op == 'Show') {
      list($cache->display->content[$pid]->shown, $ajax_vars->output, $ajax_vars->new_op) = array(1, 'Hide', 'hide');
    }
    elseif ($op == 'Hide') {
      list($cache->display->content[$pid]->shown, $ajax_vars->output, $ajax_vars->new_op) = array(0, 'Show', 'show');
    }
    else {
      // bad args, error out.
      panels_ajax_render();
    }

    panels_cache_set($cache->display->did, $cache);
    panels_ajax_render($ajax_vars);
  }
  panels_ajax_render();
}

/**
 * Entry point for AJAX: Add pane configuration.
 *
 * After updating the $cache data and equipping a new $pane object with basic values,
 * the general-purpose ajax form handler, panels_ajax_form(), is called and pushes the
 * configuration form to the screen.
 *
 * Once form submission is completed, data is pushed back into js by panels_ajax_render().
 *
 * @ingroup PanelsAjax
 *
 * @param int $did
 *  The display id for the display object currently being edited. Errors out silently if absent.
 * @param string $pane_info
 *  A string generated by the following code in panels_add_content(): \n
 *  @code
 *  $output .= l($link_text, 'javascript: void()', array('class' => 'panels-modal-add-config', 'id' => $type_name . '-' . $panel_id . '-' . $subtype_name), NULL, NULL, NULL, TRUE);
 *  @endcode \n
 *  $type_name contains the name of the content type, $panel_id the name of the panel region
 *  to which the pane will be added, and $subtype_name is the name of the content subtype.
 *
 * @see panels_content_config_form()
 */
function panels_ajax_add_config($did = NULL, $pane_info = NULL) {
  if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
    if (!isset($_POST['form_id'])) {
      $pid = $cache->display->next_new_pid();
      $pane = new stdClass();
      $pane->pid = "new-$pid";
      // Ensure there's no data relics.
      unset($cache->content_config[$pane->pid], $cache->pane);
      $cc = &$cache->content_config[$pane->pid];

      list($cc['content_type'], $cc['panel_id'], $cc['content_subtype']) = explode('-', $pane_info, 3);
      list($pane->type, $pane->subtype, $pane->configuration, $pane->access) = array($cc['content_type'], $cc['content_subtype'], array(), array());
      _panels_ajax_ct_preconfigure($cache, $pane);
      $cache->pane = drupal_clone($pane);
      panels_cache_set($did, $cache);
    }
    else {
      // Not the render passthrough, so the above data has been cached; we retrieve $pane from $cache.
      $pane = $cache->pane;
      unset($cache->pane);
    }

    $ajax_vars = panels_ajax_form('panels_content_config_form',
      t('Configure !subtype_title', array('!subtype_title' => $cache->content_config[$pane->pid]['ct_data']['subtype']['title'])),
      url($_GET['q'], NULL, NULL, TRUE), $cache, $pane, TRUE
    );
    panels_ajax_render($ajax_vars);
  }
  panels_ajax_render();
}

/**
 * Entry point for AJAX: Edit pane configuration.
 *
 * @ingroup PanelsAjax
 *
 * Prepares the display $cache and a pre-existing $pane so that the pane configuration
 * form for the $pane can be rendered.
 *
 * Once form submission is completed, data is pushed back into js by panels_ajax_render().
 *
 * @param int $did
 *  The display id of the display object currently being edited. Errors out silently if absent.
 * @param
 *  The pane id of the pane object whose configuration form we're calling up to edit.
 *
 * @see panels_content_config_form()
 */
function panels_ajax_configure($did = NULL, $pid = NULL) {
  if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
    if (isset($cache->display->content[$pid]) && $pane = $cache->display->content[$pid]) {
      // Only perform on a form rendering passthrough and if the data hasn't already been built.
      if (!isset($_POST['form_id']) && empty($cc['built'])) {
        $cc = &$cache->content_config[$pane->pid];
        if (!isset($cc)) {
          list($cc['content_type'], $cc['content_subtype']) = array($pane->type, $pane->subtype);
          _panels_ajax_ct_preconfigure($cache, $pane);
          panels_cache_set($did, $cache);
        }
      }

      $ajax_vars = panels_ajax_form('panels_content_config_form',
        t('Configure !subtype_title', array('!subtype_title' => $cache->content_config[$pane->pid]['ct_data']['subtype']['title'])),
        url($_GET['q'], NULL, NULL, TRUE), $cache, $pane
      );
      panels_ajax_render($ajax_vars);
    }
  }
  panels_ajax_render();
}

/**
 * Helper function for ajax pane type editing callbacks. When needed, preps
 * the cached $display object with relevant data from the content type.
 *
 * @ingroup PanelsAjax
 *
 * @param array $cache
 *  The loaded $cache object containing all the current $display editing data.
 * @param object $pane
 *  The $pane object this ajax callback intends to operate on.
 *
 * @see panels_ajax_configure()
 * @see panels_ajax_add_config()
 */
function _panels_ajax_ct_preconfigure(&$cache, &$pane) {
  $cc = &$cache->content_config[$pane->pid];
  // indicates that the data for this pane has been built for this round of edits and need not be rebuilt
  $cc['built'] = TRUE;
  $subtype = $cc['ct_data']['subtype'] = $cache->content_types[$pane->type][$pane->subtype];
  $type = $cc['ct_data']['type'] = panels_get_content_type($pane->type);

  $cc['ignore_roles'] = !$type['role-based access'];

  if (panels_plugin_get_function('content_types', $type, 'visibility control')) {
    $cc['visibility_controller'] = $type['visibility control'];
    // defaults to NOT using the built-in pane access system if the ct defines a visibility callback
    $cc['ignore_roles'] = TRUE;
    if (isset($type['roles and visibility']) && $type['roles and visibility'] === TRUE) {
      $cc['ignore_roles'] = FALSE;
    }
  }

  if (panels_plugin_get_function('content_types', $type, 'form control')) {
    $cc['form_controller'] = $type['form control'];
  }
}

/**
 * Master FAPI definition for all pane add/edit configuration forms.
 *
 * @ingroup PanelsAjax
 *
 * @param object $cache
 *  The $cache object for the panels $display currently being edited.
 * @param object $pane
 *  The $pane object currently being added/edited.
 * @param bool $add
 *  A boolean indicating whether we are adding a new pane ($add === TRUE)
 *  operation in this operation, or editing an existing pane ($add === FALSE).
 *
 * @return array $form
 *  A structured FAPI form definition, having been passed through all the appropriate
 *  content-type specific callbacks.
 */
function panels_content_config_form($cache, $pane, $add = FALSE) {
  $op = $add ? 'add' : 'edit';
  // for brevity and readability.
  $cc = $cache->content_config[$pane->pid];

  $form['start_form'] = array('#value' => '<div class="modal-form">');

  $form['configuration'] = panels_ct_conf_form($cc['ct_data'], $cache->display->context, $pane->configuration);
  if (is_array($form_additions = panels_ct_pane_config_form($pane, $cache->display->context, array('configuration'), $op))) {
    $form['configuration'] += $form_additions;
  }
  $form['configuration']['#tree'] = TRUE;

  if (!$cc['ignore_roles']) {
    if (user_access('administer pane access')) {
      $rids = array();
      $result = db_query("SELECT r.rid, r.name FROM {role} r ORDER BY r.name");
      while ($obj = db_fetch_object($result)) {
        $rids[$obj->rid] = $obj->name;
      }

      $form['access'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Access'),
        '#default_value' => $add ? array() : $pane->access,
        '#options' => $rids,
        '#description' => t('Only the checked roles will be able to see this pane; if no roles are checked, access will not be restricted.'),
      );
    }
    else {
      $form['access'] = array(
        '#type' => 'value',
        '#value' => isset($pane->access) ? $pane->access : array(),
      );
    }
  }

  if (isset($cc['visibility_controller'])) {
    $form['visibility'] = $cc['visibility_controller']($cache->display->context, $pane->subtype, $pane->configuration, $add);
  }

  $form['end_form'] = array('#value' => '</div>');

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => $add ? t('Add pane') : t('Save'),
  );

  $form['vars'] = array('#type' => 'value', '#value' => array($cache, $pane, $add, $op));

  // Allows content types that define this callback to have full control over the pane config form.
  if (isset($cc['form_controller'])) {
    call_user_func_array($cc['form_controller'], array(&$form, &$cache->content_config[$pane->pid], &$cache->display, $add));
    // Only set the cache with any new vars on the render passthrough; setting
    // it again could, depending on the changes in the plugin function, cause
    // serious data consistency problems.
    if (!isset($_POST['form_id'])) {
      panels_cache_set($cache->display->did, $cache);
    }
  }
  return $form;
}

/**
 * FAPI validator for panels_content_config_form().
 *
 * Call any validation functions defined by the content type.
 *
 */
function panels_content_config_form_validate($form_id, $form_values, $form) {
  list($cache, $pane, $add, $op) = $form_values['vars'];
  panels_ct_pane_validate_form($pane->type, $form['configuration'], $form_values['configuration'], $op);
}

/**
 * FAPI submit handler for panels_content_config_form().
 *
 * @ingroup PanelsAjax
 *
 * Call any submit handlers defined by the content type, update
 * the $cache object with the relevant data, save $cache to the
 * object_cache table, and return data for ajax updates.
 *
 * @return object $ajax_vars
 *  Variables to be passed to the ajax handler so that the overall edit form can
 *  be updated as needed.
 */
function panels_content_config_form_submit($form_id, $form_values) {
  list($cache, $pane, $add, $op) = $form_values['vars'];
  // Start by saving all initial $pane values into the array of pane data in $display->content.
  $cache->display->content[$pane->pid] = $pane;
  // Call the submit handler provided by the pane type, if any.
  panels_ct_pane_submit_form($pane->type, $form_values['configuration'], $op);

  if (isset($form_values['visibility'])) {
    if ($visibility_submit = panels_plugin_get_function('content_types', $cache->content_config[$pane->pid]['ct_data']['type'], 'visibility submit')) {
      // Use call_user_func_array() in order to ensure that all these values can only be passed by value.
      $cache->display->content[$pane->pid]->visibility = call_user_func_array($visibility_submit,
        array($form_values['visibility'], $add, $cache->display->content[$pane->pid], $cache->display));
    }
    else {
      // If no visibility submit callback is defined, fall back to the default storage behavior. Should be
      // adequate for the vast majority of use cases, so most client modules won't need to define callbacks.
      $cache->display->content[$pane->pid]->visibility = is_array($form_values['visibility']) ? array_keys(array_filter($form_values['visibility'])) : $form_values['visibility'];
    }
  }
  else {
    $cache->display->content[$pane->pid]->visibility = '';
  }

  if (isset($form_values['access'])) {
    $cache->display->content[$pane->pid]->access = array_keys(array_filter($form_values['access']));
  }
  else {
    $cache->display->content[$pane->pid]->access = array();
  }

  $cache->display->content[$pane->pid]->configuration = $form_values['configuration'];

  // Call submit handlers specific to the $op ('add' or 'edit').
  $ajax_vars = call_user_func_array("panels_content_config_" . $op . "_form_submit", array($form_values, &$cache, $pane));
  $ajax_vars->id = $pane->pid;
  panels_cache_set($cache->display->did, $cache);
  return $ajax_vars;
}

function panels_content_config_add_form_submit($form_values, $cache, $pane) {
  $ajax_vars = new stdClass();
  $ajax_vars->type   = 'add';
  $ajax_vars->region = $cache->content_config[$pane->pid]['panel_id'];

  $cache->display->panels[$cache->content_config[$pane->pid]['panel_id']][] = $pane->pid;
  $cache->display->content[$pane->pid]->shown = TRUE;

  // we need to fake the buttons a little.
  // new panes are shown by default
  $buttons['configure'] = panels_add_button('icon-configure.png', t('Configure'), t('Configure this pane'), 'pane-configure');
  $buttons['configure']['#parents'] = array('button', $pane->pid, 'configure');

  if (panels_get_caches() && user_access('use panels caching features')) {
    $buttons['cache'] = panels_add_button('icon-cache.png', t('Caching'), t('Control caching'), 'pane-cache');
    $buttons['cache']['#parents'] = array('button', $pane->pid, 'cache');
  }

  $buttons['show_hide'] = panels_add_button('icon-hidepane.png', t('Show/Hide Toggle'), t('Hide this pane'), 'pane-toggle-shown');
  $buttons['show_hide']['#parents'] = array('button', $pane->pid, 'show_hide');

  $buttons['delete'] = panels_add_button('icon-delete.png', t('Delete'), t('Remove this pane'), 'pane-delete');
  $buttons['delete']['#parents'] = array('button', $pane->pid, 'delete');

  $buttons = form_builder('dummy', $buttons);

  // Render the new pane for the javascript.
  $ajax_vars->output = panels_show_pane($cache->display, $cache->display->content[$pane->pid], NULL, drupal_render($buttons));
  return $ajax_vars;
}

/**
 * @ingroup PanelsAjax
 */
function panels_content_config_edit_form_submit($form_values, $cache, $pane) {
  // If this content type defines its own 'editor render callback', we need to
  // invoke that again, since we're not doing a full panels_show_pane().
  $content_type = $cache->content_config[$pane->pid]['ct_data']['type'];
  if (function_exists($content_type['editor render callback'])) {
    $block = $content_type['editor render callback']($cache->display, $pane);
  }
  else {
    $block = _panels_render_preview_pane_disabled($pane, $cache->display->context);
  }
  $ajax_vars = new stdClass();
  $ajax_vars->type   = 'replace';
  $ajax_vars->output = theme('panels_pane_collapsible', $block, $cache->display);
  return $ajax_vars;
}

/**
 * Entry point for AJAX modal: configure pane
 * @ingroup PanelsAjax
 */
function panels_ajax_cache($did = NULL, $pid = NULL) {
  if (!((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did))) {
    panels_ajax_render();
  }

  // First, check to see if this is a form being submitted and, if it is, which
  // one, because we can have two forms here.
  if (!empty($_POST) && !empty($_POST['form_id']) && $_POST['form_id'] == 'panels_edit_cache_settings_form') {
    return panels_ajax_cache_settings($cache, $pid);
  }

  $method = panels_ajax_form('panels_edit_cache_method_form',
    t('Select cache method'), url($_GET['q'], NULL, NULL, TRUE),
    $cache->display, $pid
  );
  return panels_ajax_cache_settings($cache, $pid, $method);
}

/**
 * Choose cache method form
 * @ingroup PanelsAjax
 */
function panels_edit_cache_method_form($display, $pid) {
  $conf = $pid ? $display->content[$pid]->cache : $display->cache;

  // Set to 0 to ensure we get a selected radio.
  if (!isset($conf['method'])) {
    $conf['method'] = 0;
  }

  $caches = panels_get_caches();
  if (empty($caches)) {
    $form['markup'] = array('#value' => t('No caching options are available at this time. Please enable a panels caching module in order to use caching options.'));
    return $form;
  }

  $options[0] = t('No caching');
  foreach ($caches as $cache => $info) {
    $options[$cache] = check_plain($info['title']);
  }

  $form['method'] = array(
    '#prefix' => '<div class="no-float">',
    '#suffix' => '</div>',
    '#type' => 'radios',
    '#title' => t('Method'),
    '#options' => $options,
    '#default_value' => $conf['method'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );
  return $form;
}

/**
 * Submit callback for panels_edit_cache_method_form.
 *
 * All this needs to do is return the method.
 * @ingroup PanelsAjax
 */
function panels_edit_cache_method_form_submit($form_id, $form_values) {
  return $form_values['method'];
}

/**
 * Handle the cache settings form
 * @ingroup PanelsAjax
 */
function panels_ajax_cache_settings($cache, $pid, $method = NULL) {
  if (empty($method) && isset($_POST['method'])) {
    // Retrieve the method from the form.
    $method = $_POST['method'];
  }

  if (empty($method) || !($function = panels_plugin_get_function('cache', $method, 'settings form'))) {
    panels_ajax_set_cache_data($cache->display, $pid, 0);
  }
  else {
    $cache->display = panels_ajax_form('panels_edit_cache_settings_form',
      t('Configure cache settings'), url($_GET['q'], NULL, NULL, TRUE),
      $cache->display, $pid, $method, $function
    );
  }

  panels_cache_set($cache->display->did, $cache);

  if ($pid) {
    $ajax_vars = new stdClass();
    list($ajax_vars->id, $ajax_vars->output, $ajax_vars->type) = array($pid, 'Changed', 'dismiss-changed');
    panels_ajax_render($ajax_vars);
  }
  else {
    panels_ajax_render('dismiss');
  }
}

/**
 * Set the cache method and associated settings on the display.
 * @ingroup PanelsAjax
 */
function panels_ajax_set_cache_data(&$display, $pid, $method, $settings = array()) {
  if ($pid) {
    $conf = &$display->content[$pid]->cache;
  }
  else {
    $conf = &$display->cache;
  }

  $conf['method'] = $method;
  $conf['settings'] = $settings;
}

/**
 * Cache settings form
 * @ingroup PanelsAjax
 */
function panels_edit_cache_settings_form($display, $pid, $method, $settings_function) {
  $conf = $pid ? $display->content[$pid]->cache : $display->cache;
  $info = panels_get_cache($method);

  $form['description'] = array(
    '#prefix' => '<div class="description">',
    '#suffix' => '</div>',
    '#value' => check_plain($info['description']),
  );
  $form['settings'] = $settings_function($conf['settings'], $display, $pid);
  $form['settings']['#tree'] = TRUE;

  $form['method'] = array(
    '#type' => 'hidden',
    '#value' => $method,
  );

  $form['display'] = array(
    '#type' => 'value',
    '#value' => $display,
  );

  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $pid,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Validate cache settings.
 */
function panels_edit_cache_settings_form_validate($form_id, $form_values, $form) {
  if ($function = panels_plugin_get_function('cache', $form_values['method'], 'settings form validate')) {
    $function($form, $form_values['settings']);
  }
}

/**
 * Allows panel styles to validate their style settings.
 * @ingroup PanelsAjax
 */
function panels_edit_cache_settings_form_submit($form_id, $form_values) {
  if ($function = panels_plugin_get_function('cache', $form_values['method'], 'settings form submit')) {
    $function($form_values['settings']);
  }

  // Identify which configuration we're setting
  $pid = $form_values['pid'];
  $display = $form_values['display'];

  panels_ajax_set_cache_data($display, $pid, $form_values['method'], $form_values['settings']);
  return $display;
}

/**
 * @ingroup PanelsAjax
 */
function panels_edit_submit_subform($display) {
  // Check forms to make sure only valid forms can be rendered this way.
  $valid_forms = array('panels_add_content_config_form', 'panels_edit_pane_config_form');
  $form_id = $_POST['form_id'];
  if (!in_array($form_id, $valid_forms)) {
    return panels_ajax_render();
  }

  $output = drupal_get_form($form_id, $display);

  $next = panels_get('next');

  if ($next) {
    $output = drupal_get_form($next['form'], $display, $next['data']);
    $return = panels_get('return');
    if (!$return->output) {
      $return->output = $output;
    }
  }
  else {
    if (!($return = panels_get('return'))) {
      $return->type = 'display';
      $return->output = $output;
    }
    else if ($return->type == 'display' && !$return->output) {
      $return->output = $output;
    }
  }

  if ($return->type == 'display') {
    $return->output = theme('status_messages') . $return->output;
  }
  return $return;
}

// ------------------------------------------------------------------
// Panels settings + ajax for modal popup

/**
 * Form to edit panel style settings.
 * @ingroup PanelsAjax
 */
function panels_panel_settings($display) {
  $panel_settings = $display->panel_settings;
  $style = panels_get_style((!empty($panel_settings['style'])) ? $panel_settings['style'] : 'default');

  // Let the user choose between panel styles that are available for any
  // panels implementation or specifically to this one.
  $options = array();
  foreach (panels_get_styles() as $name => $properties) {
    if (empty($properties['hidden']) && (!empty($properties['render panel']))) {
      $options[$name] = $properties['title'];
    }
  }

  $form = array();

  $form['display'] = array('#type' => 'value', '#value' => $display);

  $form['panel_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Panel settings'),
    '#tree' => TRUE,
  );

  $form['panel_settings']['start_box'] = array(
    '#value' => '<div class="form-item clear-block"><label>' . t('Default panel style') . ':</label>',
  );

  $modals = array();
  $form['panel_settings']['style'] = array(
    '#prefix' => '<div class="panels-style-settings-box">',
    '#suffix' => '</div>',
    '#type' => 'select',
    '#options' => $options,
    '#id' => 'panel-settings-style',
    '#default_value' => $style['name'],
  );

  // Is this form being posted? If so, check cache.
  if (!empty($_POST)) {
    $style_settings = panels_common_cache_get('style_settings', $display->did);
  }
  if (!isset($style_settings)) {
    $style_settings = !empty($panel_settings['style_settings']) ? $panel_settings['style_settings'] : array();
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

  // Set up the AJAX settings for the modal.
  $modals['#panels-style-settings'] = array(url('panels/ajax/panel_settings/' . $display->did . '/default', NULL, NULL, TRUE), '#panel-settings-style');

  $form['panel_settings']['end_box'] = array(
    '#value' => '</div>',
  );

  $form['panel_settings']['individual'] = array(
    '#type' => 'checkbox',
    '#title' => t('Per panel settings'),
    '#id' => 'panel-settings-individual',
    '#description' => t('If this is checked, each region in the display can have its own style.'),
    '#default_value' => $panel_settings['individual'],
  );

  $layout_options = array_merge(array('-1' => t('Use the default panel style')), $options);
  $layout         = panels_get_layout($display->layout);
  $layout_panels  = panels_get_panels($layout, $display);

  $checkboxes = array();
  foreach ($layout_panels as $id => $name) {
    $form['panel_settings']['panel'][$id]['start_box'] = array(
      '#value' => '<div class="form-item clear-block"><label>' . $name . ':</label>',
    );
    $form['panel_settings']['panel'][$id]['style'] = array(
      '#prefix' => '<div class="panels-style-settings-box">',
      '#suffix' => '</div>',
      '#type' => 'select',
      '#options' => $layout_options,
      '#id' => 'panel-settings-style-' . $id,
      '#default_value' => $display->panel_settings['panel'][$id]['style'],
    );
    $checkboxes[] = '#panel-settings-style-' . $id;

    $form['panel_settings']['panel'][$id]['edit_style'] = array(
      '#type' => 'submit',
      '#id' => 'panels-style-settings-' . $id,
      '#attributes' => array('class' => 'panels-style-settings'),
      '#value' => t('Edit style settings'),
    );
    $checkboxes[] = '#panels-style-settings-' . $id;

    // Set up the AJAX settings for the modal.
    $modals['#panels-style-settings-' . $id ] = array(url('panels/ajax/panel_settings/' . $display->did . '/' . $id, NULL, NULL, TRUE), '#panel-settings-style-' . $id);

    $form['panel_settings']['panel'][$id]['end_box'] = array(
      '#value' => '</div>',
    );
  }

  // while we don't use this directly some of our forms do.
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/autocomplete.js');

  $ajax = array('panels' => array(
    'closeText' => t('Close Window'),
    'closeImage' => theme('image', panels_get_path('images/icon-delete.png'), t('Close window'), t('Close window')),
    'throbber' => theme('image', panels_get_path('images/throbber.gif'), t('Loading...'), t('Loading')),
    'checkboxes' => array('#panel-settings-individual' => $checkboxes),
    'modals' => $modals,
  ));

  $form['panel_settings']['did'] = array(
    '#type' => 'value',
    '#value' => $display->did,
  );

  drupal_add_js(panels_get_path('js/lib/dimensions.js'));
  drupal_add_js(panels_get_path('js/lib/mc.js'));
  drupal_add_js(panels_get_path('js/lib/form.js'));
  drupal_add_js($ajax, 'setting');
  drupal_add_js(panels_get_path('js/checkboxes.js'));
  drupal_add_js(panels_get_path('js/modal_forms.js'));
  drupal_add_css(panels_get_path('css/panels_dnd.css'));

  return $form;
}

/**
 * @} ends PanelsAjax group
 */

function panels_panel_settings_validate($form_id, $form_values, $form) {
  $settings = panels_common_cache_get('style_settings', $form_values['panel_settings']['did']);
  form_set_value($form['panel_settings']['style_settings'], $settings);
}

function panels_panel_settings_submit($form_id, $form_values) {
  panels_common_cache_clear('style_settings', $form_values['panel_settings']['did']);
}

/**
 * AJAX incoming to deal with the style settings modal
 *
 * @ingroup PanelsAjax
 */
function panels_panel_settings_ajax($did, $panel, $name) {
  if ($name == '0') {
    panels_ajax_render(t('There are no style settings to edit.'), t('Edit default style settings'));
  }

  $style = panels_get_style($name);
  $style_settings = panels_common_cache_get('style_settings', $did);
  if (!isset($style_settings)) {
    panels_ajax_render();
  }

  // Render the panels ajax form. This only returns to us on successful
  // submit; otherwise the form is rendered for us and nothing else happens.
  $style_settings[$panel] = panels_ajax_form('panels_common_style_settings_form',
    t('Edit style settings for @style', array('@style' => $style['title'])),
    url($_GET['q'], NULL, NULL, TRUE),
    $did, $style, $style_settings[$panel]
  );

  panels_common_cache_set('style_settings', $did, $style_settings);
  panels_ajax_render('dismiss');
}

/**
 * Form for the style settings modal.
 *
 * @ingroup PanelsAjax
 */
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

  return $form_values['style_settings'];
}

/**
 * Includes required JavaScript libraries.
 */
function _panels_js_files() {
  // while we don't use this directly some of our forms do.
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/autocomplete.js');

  drupal_add_js(panels_get_path('js/lib/dimensions.js'));
  drupal_add_js(panels_get_path('js/lib/mc.js'));
  drupal_add_js(panels_get_path('js/lib/form.js'));
  drupal_add_js(array('panelsAjaxURL' => url('panels/ajax', NULL, NULL, TRUE)), 'setting');
  drupal_add_js(panels_get_path('js/display_editor.js'));
  drupal_add_js(panels_get_path('js/checkboxes.js'));
  drupal_add_js(panels_get_path('js/modal_forms.js'));
  drupal_add_css(panels_get_path('css/panels_dnd.css'));
  drupal_add_css(panels_get_path('css/panels_admin.css'));
}

// ---------------------------------------------------------------------------
// Panels theming functions

// @DND
function theme_panels_dnd($content) {
  $output = "<div class=\"panels-dnd\" id=\"panels-dnd-main\">$content</div>";
  return $output;
}

// @DND
function theme_panels_panel_dnd($content, $region, $label, $footer) {
  return "<div class=\"panels-display\" id=\"panel-pane-$region\">$footer<h2 class=\"label\">$label</h2>$content</div>";
}

// @DND
function theme_panels_pane_dnd($block, $id, $label, $left_buttons = NULL, $buttons = NULL) {
  if (!$block->title) {
    $block->title = t('No title');
  }
  static $count = 0;
  $output .= "<div class=\"grabber\">";
  if ($buttons) {
    $output .= "<span class='buttons'>$buttons</span>";
  }
  if ($left_buttons) {
    $output .= "<span class='left_buttons'>$left_buttons</span>";
  }
  $output .= "<span class=\"text\">$label</span></div>";
  $output .= '<div class="panel-pane-collapsible">';
  $output .= theme('panels_pane_collapsible', $block);
  $output .= '</div>';
  return $output;
}

// @DND
function theme_panels_pane_collapsible($block) {
  $output .= '<h2 class="title">' . $block->title . '</h2>';
  $output .= '<div class="content">' . filter_xss_admin($block->content) . '</div>';
  return $output;
}

/**
 * This is separate because it must be outside the <form> to work, and
 * everything in the form theme is inside the form.
 */
// @DND
function theme_panels_hidden() {
  $close_text     = t('Close Window');
  $close_image    = theme('image', panels_get_path('images/icon-delete.png'), t('Close window'), t('Close window'));
  $throbber_image = theme('image', panels_get_path('images/throbber.gif'), t('Loading...'), t('Loading'));
  $output         = <<<EOF
<div class="panels-hidden">\n
  <div id="panels-modal">\n
    <div class="panels-modal-content">\n
       <div class="modal-header">\n
         <a class="close" href="#">$close_text $close_image</a>\n
         <span class="modal-title">&nbsp;</span>\n
       </div>\n
       <div class="modal-content">\n
       </div>\n
    </div>\n
  </div>\n
  <div id="panels-throbber">\n
    <div class="panels-throbber-wrapper">
      $throbber_image\n
    </div>
  </div>\n
</div>\n
EOF;

  return $output;
}

