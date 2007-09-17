<?php
// $Id: display_edit.inc,v 1.1.2.6 2007/09/12 17:59:55 merlinofchaos Exp $


/**
 * Main API entry point to edit a panel display.
 *
 * TODO: Doc this. Important.
 */
function _panels_edit($display, $destination, $content_types = NULL) {
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
    $cache->highest = 0;
    $cache->content_types = $content_types;
    panels_cache_set($did, $cache);
  }

  $output = drupal_get_form('panels_edit_display', $display, $destination);
  $hidden = theme('panels_hidden');
  return $output . $hidden;
}

/**
 * Form definition for the panels display editor
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
    '#value' => $display
  );

  $form['destination'] = array(
    '#type' => 'value',
    '#value' => $destination
  );

  $form['explanation'] = array(
    '#value' => '<p>' . t('Grab the title bar of any pane to drag it to another panel. Click the add pane button in any panel to add more content. Click the configure button on any pane to re-configure that pane.') . '</p>',
  );

  $form['button']['#tree'] = TRUE;
  foreach ($display->content as $pid => $panel) {
    $form['button'][$pid]['#tree'] = TRUE;
    $form['button'][$pid]['configure'] = panels_add_button('configure.gif', t('Configure'), t('Configure this pane'), 'pane-configure');
    $form['button'][$pid]['delete'] = panels_add_button('close.gif', t('Delete'), t('Remove this pane'), 'pane-delete');
  }

  $layout = panels_get_layout($display->layout);
  $layout_panels = panels_get_panels($layout, $display);

  foreach ($layout_panels as $id => $name) {
    $form['panels'][$id]['add'] = panels_add_button('add.gif', t('Add content'), t('Add content to this panel'), 'pane-add', "pane-add-$id");
  }

  $form['panel'] = array('#tree' => TRUE);
  $form['panel']['pane'] = array('#tree' => TRUE);

  foreach ($layout_panels as $panel_id => $title) {
    $form['panel']['pane'][$panel_id] = array(
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

  if ($submit) {
    // The caller wants their own submit.
    $form['#submit'] = array($submit => array());
  }
  return $form;
}

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
      $buttons .= drupal_render($form['button'][$pid]['delete']);
      $content[$pane->panel] .= panels_show_pane($pane, $left_buttons, $buttons);
    }
    $content[$panel_id] = theme('panels_panel_dnd', $content[$panel_id], $panel_id, $title, drupal_render($form['panels'][$panel_id]['add']));
  }
  $output .= drupal_render($form);
  $output .= theme('panels_dnd', panels_render_layout($layout, $content, '', $display->layout_settings));
  $output .= $save_buttons;
  return $output;
}

function panels_edit_display_submit($form_id, $form_values) {
  $display = $form_values['panels_display'];
  if ($form_values['op'] == t('Save')) {
//    drupal_set_message('<pre>' . var_export($display, true) . '</pre>');
//    drupal_set_message('<pre>' . var_export($form_values, true) . '</pre>');
    $old_content = $display->content;
    $display->content = array();

    foreach ($form_values['panel']['pane'] as $panel_id => $panes) {
      $display->panels[$panel_id] = array();
      if ($panes) {
        $pids = explode(',', $panes);
        foreach ($pids as $pid) {
          if ($old_content[$pid]) {
            $display->panels[$panel_id][] = $pid;
            $old_content[$pid]->panel = $panel_id;
            $display->content[$pid] = $old_content[$pid];
          }
        }
      }
    }
//    drupal_set_message('<pre>' . var_export($display, true) . '</pre>');
    drupal_set_message(t('Panel content has been updated.'));
    panels_save_display($display);
  }

  panels_cache_clear($display->did);
  return $form_values['destination'];
}

/**
 * Form to change the layout of a display.
 */
function panels_choose_layout($display, $finish, $destination) {
  $layouts = panels_get_layouts();

  foreach ($layouts as $id => $layout) {
    $options[$id] = panels_print_layout_icon($id, $layout, check_plain($layout['title']));
  }

  drupal_add_js(panels_get_path('js/layout.js'));
  $form['layout'] = array(
    '#type' => 'radios',
    '#title' => t('Choose layout'),
    '#options' => $options,
    '#default_value' => $display->layout,
  );

  $form['clearer'] = array(
    // TODO: FIx this to use clear-block instead
    '#value' => '<div style="clear: both;"></div>',
  );

  $form['variables'] = array(
    '#type' => 'value',
    '#value' => array($display, $finish, $destination),
  );

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

  $form['#token'] = FALSE; // no token please
  return $form;
}

function panels_choose_layout_submit($form_id, $form_values) {
  list($display, $finish, $destination) = $form_values['variables'];
  $new_layout_id = $form_values['layout'];

  if ($form_values['op'] == $finish) {
    if (!empty($form_values['old'])) {
      foreach ($form_values['old'] as $id => $new_id) {
        $content[$new_id] = array_merge((array) $content[$new_id], $display->panels[$id]);
        foreach($content[$new_id] as $pid) {
          $display->content[$pid]->panel = $new_id;
        }
      }

      $display->panels = $content;
    }

    $display->layout = $new_layout_id;

    // save it back to our session.
    panels_save_display($display);
    return $destination;
  }

  return FALSE;
}

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
 * Form to change the layout of a display.
 */
function panels_edit_layout_settings_form($display, $finish, $destination) {
  $layout = panels_get_layout($display->layout);
  if (!empty($layout['settings form']) && function_exists($layout['settings form'])) {
    $form['layout_settings'] = $layout['settings form']($display, $layout, $display->layout_settings);
  }

  $form['layout_settings']['#tree'] = TRUE;

  $form += panels_common_panel_settings($display);
  
  $form['variables'] = array(
    '#type' => 'value',
    '#value' => array($display, $finish, $destination),
  );

  $form['layout'] = array(
    '#type' => 'value',
    '#value' => $layout,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $finish,
  );


  return $form;
}

function panels_edit_layout_settings_form_validate($form_id, $form_values, $form) {
  list($display, $finish, $destination) = $form_values['variables'];
  panels_common_panel_settings_validate($form_id, $form_values, $form);
  $layout = $form_values['layout'];
  if (!empty($layout['settings validate']) && function_exists($layout['settings validate'])) {
    $layout['settings validate']($form_values['layout_settings'], $form['layout_settings'], $display, $layout, $display->layout_settings);
  }
}

function panels_edit_layout_settings_form_submit($form_id, $form_values) {
  list($display, $finish, $destination) = $form_values['variables'];
  panels_common_panel_settings_submit($form_id, $form_values);

  $layout = $form_values['layout'];
  if (!empty($layout['settings submit']) && function_exists($layout['settings submit'])) {
    $layout['settings submit']($form_values['layout_settings'], $display, $layout, $display->layout_settings);
  }

  if ($form_values['op'] == $finish) {
    $display->layout_settings = $form_values['layout_settings'];
    $display->panel_settings = $form_values['panel_settings'];

    panels_save_display($display);
    drupal_set_message("Your layout settings have been saved.");
    return $destination;
  }
}

/**
 * Display the edit form for a pane.
 *
 * @param $pane
 *   The pane to edit.
 * @param $parents
 *   The form api #parents array for that this subform will live in.
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
 *   Buttons that go on the right side of
 * @param $skin
 *   If true, provide the outside div. Used to provide an easy way to get
 *   just the innards for ajax replacement
 */
function panels_show_pane($pane, $left_buttons, $buttons, $skin = TRUE) {
  $block = panels_get_pane_content($pane);
  $title = panels_get_pane_title($pane);

  $output = theme('panels_pane_dnd', $block, $pane->pid, $title, $left_buttons, $buttons);
  if ($skin) {
    $output = '<div class="panel-pane" id="panel-pane-' . $pane->pid . '">' . $output . '</div>';
  }
  return $output;
}


/**
 * @defgroup panels_ajax Functions for panels ajax features
 * @{
 */

/**
 * entry point into all the ajax stuff
 */
function panels_ajax($op = NULL, $did = NULL, $pid = NULL) {
  switch ($op) {
    case 'submit-form':
      if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
        $output = panels_edit_submit_subform($cache->display);
      }
      break;
  }

  panels_ajax_render($output);
}

function panels_ajax_add_content($did = NULL, $panel_id = NULL) {
  if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
    $display = $cache->display;
    $layout = panels_get_layout($display->layout);
    $layout_panels = panels_get_panels($layout, $display);

    if ($layout && array_key_exists($panel_id, $layout_panels)) {
      $output->output = panels_add_content($cache, $panel_id);
      $output->type = 'display';
      $output->title = t('Add content to !s', array('!s' => $layout_panels[$panel_id]));
    }
  }
  panels_ajax_render($output);
}

function panels_add_content($cache, $panel_id) {
  $return->type = 'display';
  $return->title = t('Choose type');
  panels_set('return', $return);

  if (!isset($cache->content_types)) {
    $cache->content_types = panels_get_available_content_types();
  }

  $weights = array(t('Contributed modules') => 0);
  $categories = array();
  $titles = array();

  foreach ($cache->content_types as $id => $types) {
    if (is_array($types)) {
      foreach ($types as $content_id => $info) {
        if (isset($info['icon'])) {
          $icon = $info['icon'];
          if (isset($info['path'])) {
            $path = $info['path'];
          }
          else {
            $path = panels_get_path("content_types/$id");
          }
        }
        else {
          $icon = 'no-icon.png';
          $path = panels_get_path('images');
        }
        $title = filter_xss_admin($info['title']);
        if (isset($info['description'])) {
          $description = $info['description'];
        }
        else {
          $description = $title;
        }

        if (isset($info['category'])) {
          if (is_array($info['category'])) {
            list($category, $weight) = $info['category'];
            $weights[$category] = $weight;
          }
          else {
            $category = $info['category'];
            if (!isset($weights['category'])) {
              $weights[$category] = 0;
            }
          }
        }
        else {
          $category = t('Contrib modules');
        }
        $output = '<div class="content-type-button">';
        $link_text = theme('image', $path . '/' . $icon, $description, $description);
        $output .= l($link_text, 'javascript: void()', array('class' => 'panels-modal-add-config', 'id' => $id . '-' . $panel_id . '-' . $content_id), NULL, NULL, NULL, TRUE);
        $output .= "<div>$title</div>";
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
    foreach ($weights as $category => $weight) {
      $output .= '<h3 class="panels-section-title">' . $category . '</h3>';
      $output .= '<div class="panels-section-decorator"></div>';
      if (is_array($titles[$category])) {
        natcasesort($titles[$category]);
        foreach ($titles[$category] as $id => $title) {
          $output .= $categories[$category][$id];
        }
      }
    }
  }
  return $output;
}


function panels_ajax_add_config($did = NULL, $pid = NULL) {
  if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
    $display = $cache->display;
    list($content_type_id, $panel_id, $type) = explode('-', $pid, 3);
    $conf = panels_ct_get_add_form($content_type_id, $type, $display->context, array('configuration'));
    $types = panels_ct_get_types($content_type_id);
    $title = $types[$type]['title'];

    $cache->add_config = array($content_type_id, $conf, $panel_id, $title);
    panels_cache_set($display->did, $cache);

    $text = drupal_get_form('panels_add_content_config_form', $cache, $content_type_id, $type, $conf, $panel_id, $title);
    $output = panels_get('return');
    $output->output = $text;
  }
  panels_ajax_render($output);
}

function panels_add_content_config_form($display, $content_type_id = NULL, $subtype = NULL, $conf = NULL, $panel_id = NULL, $title = NULL) {
  if (empty($content_type_id)) {
    $cache = panels_cache_get($display->did);
    if (isset($cache->add_config)) {
      list($content_type_id, $conf, $panel_id, $title) = $cache->add_config;
    }
  }

  $form['start_form'] = array('#value' => '<div class="modal-form">');

  $form['did'] = array(
    '#type' => 'value',
    '#value' => $display->did,
  );

  $form['type'] = array(
    '#type' => 'value',
    '#value' => $content_type_id,
  );

  $form['subtype'] = array(
    '#type' => 'hidden',
    '#default_value' => $subtype,
  );

  $form['panel'] = array(
    '#type' => 'value',
    '#value' => $panel_id,
  );

  $form['configuration'] = $conf;
  $form['configuration']['#tree'] = TRUE;

  if (user_access('administer pane visibility')) {
    $rids = array();
    $result = db_query("SELECT r.rid, r.name FROM {role} r ORDER BY r.name");
    while ($obj = db_fetch_object($result)) {
      $rids[$obj->rid] = $obj->name;
    }

    $form['access'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Access'),
      '#default_value' => array(),
      '#options' => $rids,
      '#description' => t('Only the checked roles will be able to see this pane; if no roles are checked, access will not be restricted.'),
    );
  }
  else {
    $form['access'] = array(
      '#type' => 'value',
      '#value' => array(),
    );
  }
  $form['end_form'] = array('#value' => '</div>');

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Add pane'),
  );

  $return->type = 'display';
  $return->title = t('Configure !s', array('!s' => $title));
  panels_set('return', $return);
  return $form;
}

function panels_add_content_config_form_validate($form_id, $form_values, $form) {
  panels_ct_validate_add_form($form_values['type'], $form['configuration'], $form_values['configuration']);
}

function panels_add_content_config_form_submit($form_id, $form) {
  $cache = panels_cache_get($form['did']);

  // keep an incrementing counter, add the data to the display.
  // This counter is so that we can add this pane to the cache but not
  // make it real until the final 'Save' is clicked.
  $pid = ++$cache->highest;
  $form['pid'] = "new-$pid";
  $form['access'] = array_keys(array_filter($form['access']));

  panels_ct_submit_add_form($form['type'], $form['configuration']);

  $cache->display->content[$form['pid']] = (object) $form;
  $cache->display->panels[$form['panel']][] = $form['pid'];

  panels_cache_set($form['did'], $cache);
  // Set info for the javascript so it knows where to put the new pane.
  // FIXME: Note that ->area appears to be a holdover -- it should be ->panel now.
  $return->type = 'add';
  $return->area = $form['panel'];
  $return->id = $form['pid'];

  // we need to fake the buttons a little.
  $buttons['configure'] = panels_add_button('configure.gif', t('Configure'), t('Configure this pane'), 'pane-configure');
  $buttons['configure']['#parents'] = array('button', $form['pid'], 'configure');

  $buttons['delete'] = panels_add_button('close.gif', t('Delete'), t('Remove this pane'), 'pane-delete');
  $buttons['delete']['#parents'] = array('button', $form['pid'], 'delete');

  $buttons = form_builder('dummy', $buttons);

  // Render the new pane for the javascript.
  $return->output = panels_show_pane($cache->display->content[$form['pid']], NULL, drupal_render($buttons));

  panels_set('return', $return);
  return FALSE;
}

function panels_ajax_configure($did = NULL, $pid = NULL) {
  if ((is_numeric($did) || $did == 'new') && $cache = panels_cache_get($did)) {
    $text = drupal_get_form('panels_edit_pane_config_form', $cache->display, $pid);
    $output = panels_get('return');
    $output->output = $text;
  }
  panels_ajax_render($output);
}

function panels_edit_pane_config_form($display, $pid = NULL) {
  $cache = panels_cache_get($display->did);
  if ($pid === NULL) {
    if (isset($cache->pid)) {
      $pid = $cache->pid;
    }
    else {
      return array();
    }
  }
  else {
    $cache->pid = $pid;
    panels_cache_set($display->did, $cache);
  }

  $pane = $display->content[$pid];

  $form['start_form'] = array('#value' => '<div class="modal-form">');

  $form['did'] = array(
    '#type' => 'value',
    '#value' => $display->did,
  );

  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $pid,
  );

  $form['type'] = array(
    '#type' => 'value',
    '#value' => $content_type_id,
  );

  $return->title = t('Configure !s', array('!s' => panels_get_pane_title($pane)));
  $form['configuration'] = panels_get_pane_edit_form($pane, $display->context, array('configuration'));
  $form['configuration']['#tree'] = true;

  if (user_access('administer pane visibility')) {
    $rids = array();
    $result = db_query("SELECT r.rid, r.name FROM {role} r ORDER BY r.name");
    while ($obj = db_fetch_object($result)) {
      $rids[$obj->rid] = $obj->name;
    }

    $form['access'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Access'),
      '#default_value' => $pane->access,
      '#options' => $rids,
      '#description' => t('Only the checked roles will be able to see this pane; if no roles are checked, access will not be restricted.'),
    );
  }
  else {
    $form['access'] = array(
      '#type' => 'value',
      '#value' => $pane->access,
    );
  }

  $return->type = 'display';
  panels_set('return', $return);

  $form['end_form'] = array('#value' => '</div>');

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function panels_edit_pane_config_form_validate($form_id, $form_values, $form) {
  panels_ct_validate_edit_form($form_values['type'], $form['configuration'], $form_values['configuration']);
}

function panels_edit_pane_config_form_submit($form_id, $form) {
  // save the new configuration.
  $cache = panels_cache_get($form['did']);

  panels_ct_submit_edit_form($form['type'], $form['configuration']);

  $cache->display->content[$form['pid']]->access = array_keys(array_filter($form['access']));
  $cache->display->content[$form['pid']]->configuration = $form['configuration'];
  panels_cache_set($form['did'], $cache);

  $return->type = 'replace';
  $return->id = $form['pid'];

  $block = panels_get_pane_content($cache->display->content[$form['pid']]);
  if (!$block->subject) {
    $block->subject = t('No title');
  }

  $return->output = theme('panels_pane_collapsible', $block, $cache->display);

  panels_set('return', $return);
  return FALSE;
}

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
    'ajaxURL' => url('panels/ajax/panel_settings/' . $display->did, NULL, NULL, TRUE),
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
 * AJAX incoming to deal with the style settings modal
 */
function panels_common_panel_settings_ajax($did, $name) {
  $style = panels_get_style($name);
  $style_settings = panels_common_cache_get('style_settings', $did);
  
  $form = drupal_retrieve_form('panels_common_style_settings_form', $did, $style, $style_settings);
  if ($_POST) {
    $form['#programmed'] = TRUE;
    $form['#post'] = $_POST;
  }

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

/**
 * Form for the style settings modal.
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

  panels_common_cache_set('style_settings', $form_values['did'], $form_values['style_settings']);
  return 'dismiss';
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
function theme_panels_panel_dnd($content, $area, $label, $footer) {
  return "<div class=\"panels-display\" id=\"panel-pane-$area\">$footer<h2 class=\"label\">$label</h2>$content</div>";
}

// @DND
function theme_panels_pane_dnd($block, $id, $label, $left_buttons = NULL, $buttons = NULL) {
  if (!$block->subject) {
    $block->subject = t('No title');
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
  $output .= '<h2 class="title">' . $block->subject . '</h2>';
  $output .= '<div class="content">' . filter_xss_admin($block->content) . '</div>';
  return $output;
}

/**
 * This is separate because it must be outside the <form> to work, and
 * everything in the form theme is inside the form.
 */
// @DND
function theme_panels_hidden() {
  $close_text = t('Close Window');
  $close_image = theme('image', panels_get_path('images/close.gif'), t('Close window'), t('Close window'));
  $throbber_image = theme('image', panels_get_path('images/throbber.gif'), t('Loading...'), t('Loading'));
  $output = <<<EOF
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