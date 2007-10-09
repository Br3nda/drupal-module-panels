<?php
// $Id: panels_page.admin.inc,v 1.1.2.9 2007/10/09 23:22:54 merlinofchaos Exp $
/**
 * Provide a list of panels, with links to edit or delete them.
 */
function panels_page_list_page() {
  $layouts = panels_get_layouts();
  foreach (panels_page_load_all() as $panel_page) {
    $ops = array(
      l(t('Edit'), "admin/panels/panel-page/$panel_page->name/edit/general"),
      l(t('Export'), "admin/panels/panel-page/$panel_page->name/export"),
    );
    if ($panel_page->type != t('Default')) {
      $ops[] = l(t('Delete'), "admin/panels/panel-page/$panel_page->name/delete");
    }
    else {
      if (empty($panel_page->disabled)) {
        $ops[] = l(t('Disable'), "admin/panels/panel-page/disable/$panel_page->name", NULL, drupal_get_destination());
      }
      else {
        $ops[] = l(t('Enable'), "admin/panels/panel-page/enable/$panel_page->name", NULL, drupal_get_destination());
      }
    }

    $path = empty($panel_page->disabled) && strpos($panel_page->path, '%') === FALSE ? l($panel_page->path, $panel_page->path) : check_plain($panel_page->path);
    $item = array();
    $item[] = check_plain(panels_page_get_title($panel_page));
    $item[] = $panel_page->type; // this is safe as it's always programmatic
    $item[] = check_plain($layouts[$panel_page->display->layout]['title']);
    $item[] = $path;
    $item[] = implode(' | ', $ops);
    $items[] = $item;
  }
  $header = array(
    t('Page title'),
    t('Type'),
    t('Layout'),
    t('URL'),
    t('Operations'),
  );
  $output = theme('table', $header, $items);
  return $output;
}

/**
 * Enable a default panel.
 */
function panels_page_enable_page($name = NULL) {
  $defaults = panels_page_default_panels();
  if (isset($defaults[$name])) {
    $status = variable_get('panel_page_defaults', array());
    $status[$name] = FALSE;
    variable_set('panel_page_defaults', $status);
    menu_rebuild();
    drupal_set_message(t('Panel page enabled'));
  }
  drupal_goto();
}

/**
 * Disable a default panel.
 */
function panels_page_disable_page($name = NULL) {
  $defaults = panels_page_default_panels();
  if (isset($defaults[$name])) {
    $status = variable_get('panel_page_defaults', array());
    $status[$name] = TRUE;
    variable_set('panel_page_defaults', $status);
    drupal_set_message(t('Panel page disabled'));
    menu_rebuild();
  }
  drupal_goto();
}

/*
 * Provide a form to confirm deletion of a panel page.
 */
function panels_page_delete_confirm($panel_page) {
  if (!is_object($panel_page)) {
    $panel_page = panels_page_load($panel_page);
  }
  $form['pid'] = array('#type' => 'value', '#value' => $panel_page->pid);
  $form['did'] = array('#type' => 'value', '#value' => $panel_page->did);
  return confirm_form( $form,
    t('Are you sure you want to delete "@title"?', array('@title' => panels_page_get_title($panel_page))),
    $_GET['destination'] ? $_GET['destination'] : 'admin/panels/panel-page',
    t('This action cannot be undone.'),
    t('Delete'), t('Cancel')
  );
}

/*
 * Handle the submit button to delete a panel page.
 */
function panels_page_delete_confirm_submit($formid, $form) {
  if ($form['confirm']) {
    panels_page_delete((object) $form);
    // TODO: Is this necessary or did we feed it the location already?
    return 'admin/panels/panel-page';
  }
}

/**
 * Handle the add panel-page page
 */
function panels_page_add_page($layout = NULL) {
  $layouts = panels_get_layouts();

  if ($layout === NULL) {
    foreach ($layouts as $id => $layout) {
      $output .= panels_print_layout_link($id, $layout, $_GET['q'] . '/' . $id);
    }
    return $output;
  }

  if (!$layouts[$layout]) {
    return drupal_not_found();
  }

  $panel_page->display = panels_new_display();
  $panel_page->display->layout = $layout;
  $panel_page->pid = 'new';
  $panel_page->did = 'new';

  drupal_set_title(t('Add panel page'));
  return drupal_get_form('panels_page_edit_form', $panel_page);
}


/**
 * Edit a panel page. Called from both the add and edit points to provide
 * for common flow.
 */
function panels_page_edit($panel_page) {
  if (!is_object($panel_page)) {
    $panel_page = panels_page_load($panel_page);
  }

  $panel_page->display = panels_load_display($panel_page->did);

  drupal_set_title(check_plain(panels_page_get_title($panel_page)));
  return drupal_get_form('panels_page_edit_form', $panel_page);
}

/**
 * The form to edit the page portion of a panel.
 */
function panels_page_edit_form($panel_page) {
  drupal_add_css(panels_get_path('css/panels_admin.css'));
  $layout = panels_get_layout($panel_page->display->layout);

  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $panel_page->pid,
  );

  $form['panel_page'] = array(
    '#type' => 'value',
    '#value' => $panel_page
  );

  $form['right'] = array(
    '#prefix' => '<div class="layout-container">',
    '#suffix' => '</div>',
  );

  $form['left'] = array(
    '#prefix' => '<div class="panel-page-info-container">',
    '#suffix' => '</div>',
  );

  $form['left']['info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page settings'),
  );

  $form['right']['layout'] = array(
    '#type' => 'fieldset',
    '#title' => t('Layout'),
  );

  $form['right']['layout']['layout-icon'] = array(
    '#value' => panels_print_layout_icon($panel_page->display->layout, $layout),
  );

  $form['right']['layout']['layout-display'] = array(
    '#value' => check_plain($layout['title']),
  );
  $content = '<dl class="content-list">';

  foreach (panels_get_panels($layout, $panel_page->display) as $panel_id => $title) {
    $content .= "<dt>$title</dt><dd>";
    if ($panel_page->display->panels[$panel_id]) {
      $content .= '<ol>';
      foreach ($panel_page->display->panels[$panel_id] as $pid) {
        $content .= '<li>' . panels_get_pane_title($panel_page->display->content[$pid]) . '</li>';
      }
      $content .= '</ol>';
    }
    else {
      $content .= t('Empty');
    }
    $content .= '</dd>';
  }
  $content .= '</dl>';

  $form['right']['layout']['layout-content'] = array(
    '#value' => $content,
  );

  $form['left']['info']['name'] = array(
    '#type' => 'textfield',
    '#size' => 35,
    '#default_value' => $panel_page->name,
    '#title' => t('Panel name'),
    '#description' => t('A unique name used to identify this panel page internally. It must be only be alpha characters and underscores. No spaces, numbers or uppercase characters.'),
  );

  $form['left']['info']['title'] = array(
    '#type' => 'textfield',
    '#size' => 35,
    '#default_value' => $panel_page->title,
    '#title' => t('Page title'),
    '#description' => t('The page title for this panels layout'),
  );

  $form['left']['info']['css_id'] = array(
    '#type' => 'textfield',
    '#size' => 35,
    '#default_value' => $panel_page->css_id,
    '#title' => t('CSS ID'),
    '#description' => t('The CSS ID to apply to this page'),
  );

  $form['left']['info']['path'] = array(
    '#type' => 'textfield',
    '#size' => 35,
    '#default_value' => $panel_page->path,
    '#title' => t('Path'),
    '#description' => t('The URL path to give this page, i.e, path/to/page. You may use "%" as an argument placeholder: i.e, node/%/panel'),
    '#required' => TRUE,
  );

  $label = ($panel_page->pid == 'new') ? t('Next') : t('Save');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $label,
  );

  return $form;
}

/**
 * Validate a panel page edit form
 */
function panels_page_edit_form_validate($form_id, $form_values, $form) {
  // Test uniqueness of name:
  $query = "SELECT pid FROM {panels_page} WHERE name = '%s'";
  if (!empty($form_values['pid']) && is_numeric($form_values['pid'])) {
    $query .= " AND pid != $form_values[pid]";
  }
  if (db_result(db_query($query, $form_values['name']))) {
    form_error($form['left']['info']['name'], t('Panel name must be unique.'));
  }
  if (!$form_values['path']) {
    form_error($form['left']['info']['path'], t('Path is required.'));
  }
  else {
    $result = db_result(db_query("SELECT pid FROM {panels_page} WHERE path = '%s' AND pid <> %d", $form_values['path'], $form_values['pid']));
    if ($result) {
      form_error($form['left']['info']['path'], t('Path may not be the same as another panel page path.'));
    }
  }
}

/**
 * Process submission of the panel page edit form
 */
function panels_page_edit_form_submit($form_id, $form_values) {
  $panel_page = $form_values['panel_page'];

  $panel_page->display->panel_settings = $form_values['panel_settings'];
  $panel_page->title  = $form_values['title'];
  $panel_page->name  = preg_replace("/[\W]+/", '', $form_values['name']);
  $panel_page->css_id = $form_values['css_id'];
  $panel_page->path  = $form_values['path'];

  if ($panel_page->pid == 'new') {
    unset($_SESSION['pp_import']);
    $pid = panels_page_save($panel_page);
    $GLOBALS['form_values']['pid'] = $pid;
    $layout = panels_get_layout($panel_page->display->layout);

    if ($layout['settings form']) {
      return "admin/panels/panel-page/$panel_page->name/edit/settings/next";
    }
    return "admin/panels/panel-page/$panel_page->name/edit/content";
  }
  else {
    drupal_set_message(t('Your changes have been saved.'));
    panels_page_save($panel_page);
  }
}

function panels_page_edit_advanced($panel_page) {
  if (!is_object($panel_page)) {
    $panel_page = panels_page_load($panel_page);
  }
  drupal_set_title(check_plain(panels_page_get_title($panel_page)));
  return drupal_get_form('panels_page_advanced_form', $panel_page);
}

/**
 * The form to edit the advanced settings of a panel page
 */
function panels_page_advanced_form($panel_page) {
  drupal_add_css(panels_get_path('css/panels_admin.css'));
  $layout = panels_get_layout($panel_page->display->layout);

  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $panel_page->pid,
  );

  $form['panel_page'] = array(
    '#type' => 'value',
    '#value' => $panel_page
  );

  $form['right'] = array(
    '#prefix' => '<div class="layout-container">',
    '#suffix' => '</div>',
  );

  $form['left'] = array(
    '#prefix' => '<div class="panel-page-info-container">',
    '#suffix' => '</div>',
  );

  $form['right']['advanced'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Advanced settings'),
  );

  $form['right']['advanced'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#title' => t('Advanced settings'),
  );

  $form['right']['advanced']['no_blocks'] = array(
    '#type' => 'checkbox',
    '#default_value' => $panel_page->no_blocks,
    '#title' => t('Disable Drupal blocks/regions'),
    '#description' => t('Check this to have the panel page disable all regions displayed in the theme.'),
  );

  $rids = array();
  $result = db_query("SELECT r.rid, r.name FROM {role} r ORDER BY r.name");
  while ($obj = db_fetch_object($result)) {
    $rids[$obj->rid] = $obj->name;
  }

  $form['right']['advanced']['access'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Access'),
    '#default_value' => $panel_page->access,
    '#options' => $rids,
    '#description' => t('Only the checked roles will be able to see this panel in any form; if no roles are checked, access will not be restricted.'),
  );

  $form['right']['advanced']['css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS code'),
    '#description' => t('Enter well-formed CSS code here; this code will be embedded into the page, and should only be used for minor adjustments; it is usually better to try to put CSS for the page into the theme if possible.'),
    '#default_value' => $panel_page->css,
  );

  $form['left']['arg-info'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Arguments'),
  );

  $form['left']['menu-info'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t('Menu'),
  );

  $form['left']['menu-info']['menu'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide Menu'),
    '#return_value' => 1,
    '#default_value' => $panel_page->menu,
    '#description' => t('If checked this panel be given a menu entry in the Drupal menu system. If not checked the data in this group will be ignored.'),
  );

  $form['left']['menu-info']['menu_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide Menu as Tab'),
    '#return_value' => 1,
    '#default_value' => $panel_page->menu_tab,
    '#description' => t("If checked this panel's menu entry will be provided as a tab rather than in the main menu system."),
  );

  $form['left']['menu-info']['menu_tab_weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Tab Weight'),
    '#default_value' => $panel_page->menu_tab_weight,
    '#size' => 5,
    '#description' => t('If this is a menu tab, select the weight; lower numbers will be further to the left.'),
  );

  $form['left']['menu-info']['menu_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Menu Title'),
    '#default_value' => $panel_page->menu_title,
    '#size' => 35,
    '#maxlength' => 255,
    '#description' => t('Enter the title to use for the menu entry or tab. If blank, the page title will be used.'),
  );

  $form['left']['menu-info']['default-tab'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Default Menu Tab'),
  );

  $form['left']['menu-info']['default-tab']['menu_tab_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Make Default Menu Tab'),
    '#return_value' => 1,
    '#default_value' => $panel_page->menu_tab_default,
    '#description' => t("If checked this panel's menu entry will be provided as a tab, and will be the default tab for that URL path. For example, if the URL is 'tracker/all' and it is set as the default menu tab, it will be put into the menu as 'tracker' and 'tracker/all' will be the default tab. The following settings allow you to customize the parent item, for example 'tracker'. For tabs to work properly, one tab in the group must be set as the default."),
  );

  $form['left']['menu-info']['default-tab']['menu_tab_default_parent_type'] = array(
    '#type' => 'select',
    '#title' => t('Parent Menu Item Type'),
    '#default_value' => $panel_page->menu_tab_default_parent_type,
    '#options' => array(
      'tab' => t("Tab"),
      'normal' => t("Normal menu item"),
      'existing' => t("Already exists (don't create)"),
    ),
    '#description' => t("Select type of parent item to use for this default menu tab. You can either specify the parent should be a tab (the default), a normal menu item, or to use the menu item that already exists at the specified URL. For example, if the URL for the default tab is 'tracker/all', then 'tracker' would already have to be a valid menu item to use this final choice."),
  );

  $form['left']['menu-info']['default-tab']['menu_parent_tab_weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Tab Weight'),
    '#default_value' => $panel_page->menu_parent_tab_weight,
    '#size' => 5,
    '#description' => t('If the parent menu item is a tab, select the weight; lower numbers will be further to the left.'),
  );

  $form['left']['menu-info']['default-tab']['menu_parent_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Parent Menu Item Title'),
    '#default_value' => $panel_page->menu_parent_title,
    '#size' => 35,
    '#maxlength' => 255,
    '#description' => t('If the Parent Menu Item is being defined by this panel (if you set the %type_field to either %tab or %menu), you can specify its title here.  If blank, the menu title will be used if that is defined, or the page title if not.', array('%type_field' => t('Parent Menu Item Type'), '%tab' => t('Tab'), '%menu' => t('Normal menu item'))),
  );

  $form['left']['arg-info']['arguments_table'] = array(
    '#theme' => 'panels_page_arguments_form',
    '#panel_name' => $panel_page->name,
  );

  $form['left']['arg-info']['arguments_table']['argument'] = array(
    '#tree' => TRUE,
  );

  // Is this form being posted? If so, check cache.
  if (!empty($_POST)) {
    // TODO: Combine these two caches?
    $panel_page->arguments = $arguments = panels_common_cache_get('arguments', $panel_page->name);
    $panel_page->contexts = $relationships = panels_common_cache_get('relationships', $panel_page->name);
  }
  if (!isset($arguments)) {
    $arguments = $panel_page->arguments;
    panels_common_cache_set('arguments', $panel_page->name, $arguments);
    $relationships = $panel_page->contexts;
    panels_common_cache_set('relationships', $panel_page->name, $relationships);
  }

  if (isset($arguments) && is_array($arguments)) {
    foreach ($arguments as $position => $argument) {
      panels_page_add_argument_to_form( $form['left']['arg-info']['arguments_table']['argument'][$position], $position, $argument);
    }
  }

  $form['arguments'] = array(
    '#type' => 'value',
    '#value' => $arguments,
  );

  // Store the order the choices are in so javascript can manipulate it.
  $form['order'] = array(
    '#type' => 'hidden',
    '#id' => 'argument-order',
    '#default_value' => $arguments ? implode(',', array_keys($arguments)) : '',
  );

  $form['left']['arg-info']['arguments_table']['buttons'] = array(
    '#tree' => TRUE,
  );

  $args = panels_get_arguments();
  $choices = array();
  foreach ($args as $name => $arg) {
    $choices[$name] = $arg['title'];
  }

  asort($choices);

  $form['left']['arg-info']['arguments_table']['buttons']['add_argument'] = array(
    '#type' => 'select',
    '#options' => $choices,
  );

  $form['left']['arg-info']['arguments_table']['buttons']['add_submit'] = array(
    '#type' => 'submit',
    '#attributes' => array('class' => 'argument-add'),
    '#value' => t('Add argument'),
  );

  // ---------------------------------------------------
  // Relationships, which are closely tied to arguments.
  
  $form['relationships'] = array(
    '#type' => 'value',
    '#value' => $relationships,
  );

  $form['left']['arg-info']['relationships_table'] = array(
    '#prefix' => '<div id="panels-relationships-table">',
    '#suffix' => '</div>',
    '#theme' => 'panels_page_relationships_form',
    '#panel_name' => $panel_page->name
  );

  // Store the order the choices are in so javascript can manipulate it.
  $form['left']['arg-info']['relationships_table']['markup'] = array(
    '#value' => '&nbsp;'
  );
  $form['relationship_order'] = array(
    '#type' => 'hidden',
    '#id' => 'relationship-order',
    '#default_value' => $arguments ? implode(',', array_keys($relationships)) : '',
  );

  $available_relationships = panels_get_relevant_relationships(panels_page_get_contexts($panel_page));

  if (!empty($available_relationships) || !empty($relationships)) {
    panels_page_add_relationship_table($form['left']['arg-info']['relationships_table'], $available_relationships, $relationships);
  }

  $label = ($panel_page->pid == 'new') ? t('Next') : t('Save');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $label,
  );

  return $form;
}
function panels_page_add_relationship_table(&$form, $available_relationships, $relationships) {
  $form['relationship'] = array(
    '#tree' => TRUE,
  );

  if (isset($relationships) && is_array($relationships)) {
    foreach ($relationships as $position => $relationship) {
      panels_page_add_relationship_to_form( $form['relationship'][$position], $position, $relationship);
    }
  }

  $form['buttons'] = array(
    '#tree' => TRUE,
  );

  $form['buttons']['add_relationship'] = array(
    '#type' => 'select',
    '#options' => $available_relationships,
  );

  $form['buttons']['add_relationship_submit'] = array(
    '#type' => 'submit',
    '#attributes' => array('class' => 'relationship-add'),
    '#value' => t('Add relationship'),
  );
}

/**
 * Process submission of the panel page edit form
 */
function panels_page_advanced_form_submit($form_id, $form_values) {
  $panel_page = $form_values['panel_page'];

  $panel_page->css = $form_values['css'];

  // Organize arguments
  $panel_page->arguments = array();
  if (isset($form_values['order']) && $form_values['order'] !== '') {
    foreach (explode(',', $form_values['order']) as $position) {
      // We retain the original position here because we need argument IDs.
      $panel_page->arguments[$position] = $form_values['arguments'][$position];
    }
  }

  // Organize relationships
  $panel_page->contexts = array();
  if (isset($form_values['relationship_order']) && $form_values['relationship_order'] !== '') {
    foreach (explode(',', $form_values['relationship_order']) as $position) {
      // We retain the original position here because we need relationship IDs.
      $panel_page->contexts[$position] = $form_values['relationships'][$position];
    }
  }

  // Match up our displays, carry them forward and add new ones.
  $old_displays = $panel_page->displays;
  $panel_page->displays = array();

  foreach ($panel_page->arguments as $id => $argument) {
    $def = panels_get_argument($argument['name']);
    if (function_exists($def['displays'])) {
      // Figure out which instance of this particular argument type we're using.

      $displays = $def['displays']($argument['argument_settings'], $argument['id']);
      foreach ($displays as $did => $info) {
        $pdid = "argument_$id" . '-' . $did;
        if (isset($old_displays[$pdid])) {
          $panel_page->displays[$pdid] = $old_displays[$pdid];
          unset($old_displays[$pdid]);
          // ensure titles get updated if necessary
          $panel_page->displays[$pdid]['title'] = $info['title'];
        }
        else {
          $panel_page->displays[$pdid] = array(
            'did' => 'new',
            'title' => $info['title'],
            'default' => "argument_$id" . '-' . $info['default'],
            'argument_id' => $id,
            'context' => $info['context'],
          );
        }
      }
    }
  }

  // Remove remaining old displays.
  foreach ($old_displays as $id => $info) {
    if (is_numeric($info['did'])) {
      panels_delete_display($info['did']);
      drupal_set_message(t('Removed unused display @title', $info['title']));
    }
  }

  $panel_page->no_blocks = $form_values['no_blocks'];
  $panel_page->menu = $form_values['menu'];
  $panel_page->menu_tab = $form_values['menu_tab'];
  $panel_page->menu_tab_weight = $form_values['menu_tab_weight'];
  $panel_page->menu_title = $form_values['menu_title'];
  $panel_page->menu_tab_default = $form_values['menu_tab_default'];
  $panel_page->menu_tab_default_parent_type = $form_values['menu_tab_default_parent_type'];
  $panel_page->menu_parent_title = $form_values['menu_parent_title'];
  $panel_page->menu_parent_tab_weight = $form_values['menu_parent_tab_weight'];

  $panel_page->access = array_keys(array_filter($form_values['access']));

  drupal_set_message(t('Your changes have been saved.'));
  panels_page_save($panel_page);
}


/**
 * Add an argument row to the form. Used both in the main form and by
 * the ajax to add an item.
 */
function panels_page_add_argument_to_form(&$form, $position, $argument) {
  $arg = panels_get_argument($argument['name']);
  $form['title'] = array(
    '#value' => check_plain($argument['identifier']),
  );

  $form['top'] = panels_add_button('go-top.png', t('Top'),
    t('Move this item to the top of the list'),
    'argument-top',
    'argument-top-' . $position
  );

  $form['up'] = panels_add_button('go-up.png', t('Up'),
    t('Move this item up'),
    'argument-up',
    'argument-up-' . $position
  );

  $form['down'] = panels_add_button('go-down.png', t('Down'),
    t('Move this item down'),
    'argument-down',
    'argument-down-' . $position
  );

  $form['bottom'] = panels_add_button('go-bottom.png', t('Bottom'),
    t('Move this item to the bottom of the list'),
    'argument-bottom',
    'argument-bottom-' . $position
  );

  $form['remove'] = panels_add_button('delete.png', t('Remove'),
    t('Remove this item'),
    'argument-remove',
    'argument-remove-' . $position
  );

  $form['settings'] = panels_add_button('configure.gif', t('Configure'),
    t('Configure this item'),
    'argument-settings',
    'argument-settings-' . $position
  );
}

/**
 * Theme the form item for the argument entry.
 */
function theme_argument_row($form, $position, $count) {
  $output = '<tr id="argument-row-' . $position . '" class="argument-row ' . ($count % 2 ? 'even' : 'odd') . '">';
  $output .= '<td class="argument-title">&nbsp;' . drupal_render($form['title']) . '</td>';
  $output .= '<td class="argument-operation">' . drupal_render($form['settings']) .
    drupal_render($form['up']) .
    drupal_render($form['down']) .
    drupal_render($form['top']) .
    drupal_render($form['bottom']) .
    drupal_render($form['remove']) .
  '</td>';
  $output .= '</tr>';
  return $output;
}

/**
 * Add the arguments form to panel page settings
 */
function theme_panels_page_arguments_form($form) {
  $output = '';

  $output .= '<table id="argument-table">';
  $output .= '<thead>';
  $output .= '<tr>';
  $output .= '<th class="argument-title">' . t('Argument') . '</th>';
  $output .= '<th class="argument-operation">' . t('Operation') . '</th>';
  $output .= '</thead>';
  $output .= '<tbody>';

  $count = 0;
  foreach (array_keys($form['argument']) as $id) {
    if (!is_numeric($id)) {
      continue;
    }
    $output .= theme('argument_row', $form['argument'][$id], $id, $count++);
  }

  $output .= '</tbody>';
  $output .= '</table>';

  // Display the add argument item.
  $row = array();
  $row[] = array('data' => drupal_render($form['buttons']['add_argument']), 'class' => 'argument-title');
  $row[] = array('data' => drupal_render($form['buttons']['add_submit']), 'class' => 'argument-add', 'width' => "60%");

  $output .= theme('table', array(), array($row), array('id' => 'argument-add-table'));

  $settings = array(
    'list' => array('argument-table' => array(
      // The gadget that stores our the order of items.
      'order' => 'input#argument-order',
      // The buttons that do stuff.
      'up' => 'input.argument-up',
      'down' => 'input.argument-down',
      'top' => 'input.argument-top',
      'bottom' => 'input.argument-bottom',
      'remove' => 'input.argument-remove',
      'configure' => 'input.argument-settings',
      'configure_path' => url('panels/argument/ajax/edit/' . $form['#panel_name'], NULL, NULL, TRUE),

      // The button that adds an item
      'add' => 'input#edit-buttons-add-submit',
      // Path for ajax on adding an item
      'path' => url('panels/argument/ajax/add/' . $form['#panel_name'], NULL, NULL, TRUE),
      // Which items to post when adding
      'post' => array('#edit-buttons-add-argument', '#edit-buttons-add-submit'),
      // Where to get the id of an item
      'tr' => 'argument-row-',
      'row_class' => 'tr.argument-row',
      // Additional HTML to replace on add.
      'replace' => array('div#panels-relationships-table' => 'relationships_table'),
    )),
    'panels' => array(
      'closeText' => t('Close Window'),
      'closeImage' => theme('image', panels_get_path('images/close.gif'), t('Close window'), t('Close window')),
      'throbber' => theme('image', panels_get_path('images/throbber.gif'), t('Loading...'), t('Loading')),
    ),
  );

  drupal_add_js($settings, 'setting');
  drupal_add_js(panels_get_path('js/list.js'));
  drupal_add_js(panels_get_path('js/lib/dimensions.js'));
  drupal_add_js(panels_get_path('js/lib/mc.js'));
  drupal_add_js(panels_get_path('js/lib/form.js'));
  drupal_add_js(panels_get_path('js/modal_forms.js'));
  drupal_add_css(panels_get_path('css/panels_dnd.css'));

  return $output;
}

/**
 * Ajax entry point to add an argument
 */
function panels_page_ajax_argument_add($panel_name) {
  // Figure out which argument we're adding
  if (isset($_POST['buttons']['add_argument'])) {
    $name = $_POST['buttons']['add_argument'];
  }
  else if (isset($_POST['argument']['name'])) {
    $name = $_POST['argument']['name'];
  }

  if (empty($name)) {
    panels_ajax_render();
  }

  // load the argument
  $argument = panels_get_argument($name);

  if (empty($argument)) {
    panels_ajax_render();
  }

  // Retrieve our stored list of arguments from cache
  $arguments = panels_common_cache_get('arguments', $panel_name);

  // Figure out which instance of this argument we're creating
  $id = 1;
  foreach ($arguments as $arg) {
    if ($arg['name'] == $name) {
      $id++;
    }
  }

  if (!isset($arguments)) {
    panels_ajax_render();
  }

  // Figure out the position for our new argument.
  $position = 0;
  if (!empty($arguments)) {
    $position = max(array_keys($arguments)) + 1;
  }

  $arguments[$position]['id'] = $id;
  if (empty($arguments[$position]['identifier'])) {
    $arguments[$position]['identifier'] = $argument['title'] . ' ' . $id;
  }

  $form = drupal_retrieve_form('panels_page_edit_argument_form', $panel_name, $argument, $arguments, $position);
  if ($_POST && $_POST['form_id'] == 'panels_page_edit_argument_form') {
    $form['#programmed'] = TRUE;
    $form['#post'] = $_POST;
  }

  $arguments = drupal_process_form('panels_page_edit_argument_form', $form);
  if ($arguments) {
    // successful submit

    // Build a chunk of the form.
    $arg_form['argument'] = array(
      '#tree' => TRUE,
    );

    panels_page_add_argument_to_form($arg_form['argument'], $position, $arguments[$position]);
    $arg_form = form_builder('dummy_form_id', $arg_form);

    // Build the relationships table so we can ajax it in.
    $rel_form = array(
      '#theme' => 'panels_page_relationships_form',
      '#panel_name' => $panel_name
    );

    $rel_form['relationship'] = array(
      '#tree' => TRUE,
    );

    $page = panels_page_load($panel_name);

    $page->arguments = $arguments;
    $page->contexts = panels_common_cache_get('relationships', $panel_name);

    $contexts = panels_page_get_contexts($page);

    $available_relationships = panels_get_relevant_relationships($contexts);

    $output = new stdClass();
    if (!empty($available_relationships)) {
      panels_page_add_relationship_table($rel_form, $available_relationships, $page->contexts);
      $rel_form = form_builder('dummy_form_id', $rel_form);
      $output->relationships_table = drupal_render($rel_form);
    }

    $output->type = $output->output = 'add';
    $output->output = theme('argument_row', $arg_form['argument'], $position, $position);
    $output->position = $position;
    panels_ajax_render($output);
  }
  else {
    $title = t('Add argument @argument', array('@argument' => $argument['title']));
    $output = drupal_render_form('panels_page_edit_argument_form', $form);
    panels_ajax_render($output, $title, url($_GET['q'], NULL, NULL, TRUE));
  }
}

/**
 * Ajax entry point to add an argument
 */
function panels_page_ajax_argument_edit($panel_name) {
  // Figure out which argument we're adding
  if (isset($_POST['position'])) {
    $position = $_POST['position'];
  }

  // Retrieve our stored list of arguments from cache
  $arguments = panels_common_cache_get('arguments', $panel_name);

  if (!isset($arguments)) {
    panels_ajax_render();
  }

  $name = $arguments[$position]['name'];
  if (empty($name)) {
    panels_ajax_render();
  }

  // load the argument
  $argument = panels_get_argument($name);

  if (empty($argument)) {
    panels_ajax_render();
  }

  $title = t('Edit argument @argument', array('@argument' => $argument['title']));

  $form = drupal_retrieve_form('panels_page_edit_argument_form', $panel_name, $argument, $arguments, $position);
  if ($_POST && $_POST['form_id'] == 'panels_page_edit_argument_form') {
    $form['#programmed'] = TRUE;
    $form['#post'] = $_POST;
  }

  $result = drupal_process_form('panels_page_edit_argument_form', $form);
  if ($result) {
    // successful submit
    $output->type = $output->output = 'dismiss';
    panels_ajax_render($output);
  }
  else {
    $output = drupal_render_form('panels_page_edit_argument_form', $form);
    panels_ajax_render($output, $title, url($_GET['q'], NULL, NULL, TRUE));
  }
}

/**
 * Form (for ajax use) to add an argument
 */
function panels_page_edit_argument_form($panel_name, $argument, $arguments, $position) {
  // Basic values required to orient ourselves
  $form['panel_name'] = array(
    '#type' => 'value',
    '#value' => $panel_name,
  );

  $form['position'] = array(
    '#type' => 'hidden',
    '#value' => $position,
  );

  $form['start_form'] = array('#value' => '<div class="modal-form">');

  $form['description'] = array(
    '#prefix' => '<div class="description">',
    '#suffix' => '</div>',
    '#value' => check_plain($argument['description']),
  );

  // Basic argument values
  $form['argument']['#tree'] = TRUE;
  $form['argument']['name'] = array(
    '#type' => 'hidden',
    '#value' => $argument['name'],
  );

  $form['argument']['default'] = array(
    '#type' => 'select',
    '#title' => t('Default'),
    '#options' => array(
      '404' => t('Display page not found.'),
      'ignore' => t('Ignore it; content that requires this context will not be available.'),
    ),
    '#default_value' => $arguments[$position]['default'],
    '#description' => t('If the argument is missing or is not valid, select how this should behave.'),
  );

  $form['argument']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $arguments[$position]['title'],
    '#description' => t('Enter a title to use when this argument is present. You may use %# substitution to get the title for the given argument, where # is the argument position. i.e, use %1 for the first argument, %2 for the next argument, and so on.'),
  );

  $form['argument']['id'] = array(
    '#type' => 'value',
    '#value' => $arguments[$position]['id'],
  );

  $form['argument']['identifier'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier'),
    '#description' => t('Enter a title to identify this relationship on administrative screens.'),
    '#default_value' => $arguments[$position]['identifier'],
  );

  // Settings particular to this argument
  $argument_settings = array();
  if (isset($arguments[$position]['argument_settings'])) {
    $argument_settings = $arguments[$position]['argument_settings'];
  }

  if (isset($argument['settings form']) && function_exists($argument['settings form'])) {
    $form['argument']['argument_settings'] = $argument['settings form']($argument_settings);
    $form['argument']['argument_settings']['#tree'] = TRUE;
  }

  $form['end_form'] = array('#value' => '</div>');

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * validate an argument edited/added via ajax
 */
function panels_page_edit_argument_form_validate($form_id, $form_values, $form) {
  $argument = $form_values['argument'];

  if (isset($argument['settings form validate']) && function_exists($argument['settings form validate'])) {
    $argument['settings form validate']($form, $form_values['argument']['argument_settings']);
  }
}

/**
 * Updates an argument edited/added via ajax
 */
function panels_page_edit_argument_form_submit($form_id, $form_values) {
  $argument = $form_values['argument'];
  $position = $form_values['position'];

  if (isset($argument['settings form submit']) && function_exists($argument['settings form submit'])) {
    $argument['settings form submit']($form_values['argument_settings']);
  }

  $arguments = panels_common_cache_get('arguments', $form_values['panel_name']);
  $arguments[$position] = $argument;

  panels_common_cache_set('arguments', $form_values['panel_name'], $arguments);
  return $arguments;
}

// ----
// relationships

/**
 * Add an argument row to the form. Used both in the main form and by
 * the ajax to add an item.
 */
function panels_page_add_relationship_to_form(&$form, $position, $relationship) {
  $rdata = panels_get_relationship($relationship['name']);
  $form['title'] = array(
    '#value' => check_plain($relationship['identifier']),
  );

  $form['remove'] = panels_add_button('delete.png', t('Remove'),
    t('Remove this item'),
    'relationship-remove',
    'relationship-remove-' . $position
  );

  $form['settings'] = panels_add_button('configure.gif', t('Configure'),
    t('Configure this item'),
    'relationship-settings',
    'relationship-settings-' . $position
  );
}

/**
 * Theme the form item for the relationship entry.
 */
function theme_relationship_row($form, $position, $count) {
  $output = '<tr id="relationship-row-' . $position . '" class="relationship-row ' . ($count % 2 ? 'even' : 'odd') . '">';
  $output .= '<td class="relationship-title">&nbsp;' . drupal_render($form['title']) . '</td>';
  $output .= '<td class="relationship-operation">' . drupal_render($form['settings']) .
    drupal_render($form['remove']) .
  '</td>';
  $output .= '</tr>';
  return $output;
}

/**
 * Add the relationships form to panel page settings
 */
function theme_panels_page_relationships_form($form) {
  $output = '';

  if (!empty($form['relationship'])) {
    $output .= '<table id="relationship-table">';
    $output .= '<thead>';
    $output .= '<tr>';
    $output .= '<th class="relationship-title">' . t('Relationship') . '</th>';
    $output .= '<th class="relationship-operation">' . t('Operation') . '</th>';
    $output .= '</thead>';
    $output .= '<tbody>';

    $count = 0;
    foreach (array_keys($form['relationship']) as $id) {
      if (!is_numeric($id)) {
        continue;
      }
      $output .= theme('relationship_row', $form['relationship'][$id], $id, $count++);
    }

    $output .= '</tbody>';
    $output .= '</table>';
  }

  if (!empty($form['buttons'])) {
    // Display the add relationship item.
    $row = array();
    $row[] = array('data' => drupal_render($form['buttons']['add_relationship']), 'class' => 'relationship-title');
    $row[] = array('data' => drupal_render($form['buttons']['add_relationship_submit']), 'class' => 'relationship-add', 'width' => "60%");
    $output .= theme('table', array(), array($row), array('id' => 'relationship-add-table'));
  }

  $settings = array(
    'list' => array('relationship-table' => array(
      // The buttons that do stuff.
      'remove' => 'input.relationship-remove',
      // The gadget that stores our the order of items.
      'order' => 'input#relationship-order',
      'configure' => 'input.relationship-settings',
      'configure_path' => url('panels/relationship/ajax/edit/' . $form['#panel_name'], NULL, NULL, TRUE),

      // The button that adds an item
      'add' => 'input#edit-buttons-add-relationship-submit',
      // Path for ajax on adding an item
      'path' => url('panels/relationship/ajax/add/' . $form['#panel_name'], NULL, NULL, TRUE),
      // Which items to post when adding
      'post' => array('#edit-buttons-add-relationship', '#edit-buttons-add-relationship-submit'),
      // Where to get the id of an item
      'tr' => 'relationship-row-',
      'row_class' => 'tr.relationship-row',
    )),
  );

  drupal_add_js($settings, 'setting');
  drupal_add_js(panels_get_path('js/list.js'));
  drupal_add_js(panels_get_path('js/lib/dimensions.js'));
  drupal_add_js(panels_get_path('js/lib/mc.js'));
  drupal_add_js(panels_get_path('js/lib/form.js'));
  drupal_add_js(panels_get_path('js/modal_forms.js'));
  drupal_add_css(panels_get_path('css/panels_dnd.css'));

  return $output;
}

/**
 * Ajax entry point to add an relationship
 */
function panels_page_ajax_relationship_add($panel_name) {
  // Figure out which relationship we're adding
  if (isset($_POST['buttons']['add_relationship'])) {
    $name = $_POST['buttons']['add_relationship'];
  }
  else if (isset($_POST['relationship']['name'])) {
    $name = $_POST['relationship']['name'];
  }

  if (empty($name)) {
    panels_ajax_render();
  }

  // load the relationship
  $relationship = panels_get_relationship($name);

  if (empty($relationship)) {
    panels_ajax_render();
  }

  // Retrieve our stored list of relationships from cache
  $arguments = panels_common_cache_get('arguments', $panel_name);
  $relationships = panels_common_cache_get('relationships', $panel_name);

  if (!isset($relationships)) {
    panels_ajax_render();
  }

  // Figure out which instance of this relationship we're creating
  $id = 1;
  foreach ($relationships as $arg) {
    if ($arg['name'] == $name) {
      $id++;
    }
  }

  // Figure out the position for our new relationship.
  $position = 0;
  if (!empty($relationships)) {
    $position = max(array_keys($relationships)) + 1;
  }

  $relationships[$position]['id'] = $id;
  if (empty($relationships[$position]['identifier'])) { 
    $relationships[$position]['identifier'] = $relationship['title'] . ' ' . $id;
  }

  // fake $page
  $page = new stdClass();
  $page->arguments = $arguments;
  $page->contexts = $relationships;
  $contexts = panels_page_get_contexts($page);

  $form = drupal_retrieve_form('panels_page_edit_relationship_form', $panel_name, $relationship, $relationships, $position, $contexts);
  if ($_POST && $_POST['form_id'] == 'panels_page_edit_relationship_form') {
    $form['#programmed'] = TRUE;
    $form['#post'] = $_POST;
  }

  $relationships = drupal_process_form('panels_page_edit_relationship_form', $form);
  if ($relationships) {
    // successful submit

    // Build a chunk of the form.
    $arg_form['relationship'] = array(
      '#tree' => TRUE,
    );

    panels_page_add_relationship_to_form($arg_form['relationship'], $position, $relationships[$position]);
    $arg_form = form_builder('dummy_form_id', $arg_form);

    $output = new stdClass();
    $output->type = $output->output = 'add';
    $output->output = theme('relationship_row', $arg_form['relationship'], $position, $position);
    $output->position = $position;
    panels_ajax_render($output);
  }
  else {
    $title = t('Add relationship @relationship', array('@relationship' => $relationship['title']));
    $output = drupal_render_form('panels_page_edit_relationship_form', $form);
    panels_ajax_render($output, $title, url($_GET['q'], NULL, NULL, TRUE));
  }
}

/**
 * Ajax entry point to add an relationship
 */
function panels_page_ajax_relationship_edit($panel_name) {
  // Figure out which relationship we're adding
  if (isset($_POST['position'])) {
    $position = $_POST['position'];
  }

  // Retrieve our stored list of relationships from cache
  $arguments = panels_common_cache_get('arguments', $panel_name);
  $relationships = panels_common_cache_get('relationships', $panel_name);

  if (!isset($relationships)) {
    panels_ajax_render();
  }

  $name = $relationships[$position]['name'];
  if (empty($name)) {
    panels_ajax_render();
  }

  // load the relationship
  $relationship = panels_get_relationship($name);

  if (empty($relationship)) {
    panels_ajax_render();
  }

  $title = t('Edit relationship @relationship', array('@relationship' => $relationship['title']));

  // fake $page
  $page = new stdClass();
  $page->arguments = $arguments;
  $page->contexts = $relationships;
  $contexts = panels_page_get_contexts($page);

  // Remove this context, because we can't really allow circular contexts.
  unset($contexts[panels_relationship_context_id($relationships[$position])]);
  
  $form = drupal_retrieve_form('panels_page_edit_relationship_form', $panel_name, $relationship, $relationships, $position, $contexts);
  if ($_POST && $_POST['form_id'] == 'panels_page_edit_relationship_form') {
    $form['#programmed'] = TRUE;
    $form['#post'] = $_POST;
  }

  $result = drupal_process_form('panels_page_edit_relationship_form', $form);
  if ($result) {
    // successful submit
    $output->type = $output->output = 'dismiss';
    panels_ajax_render($output);
  }
  else {
    $output = drupal_render_form('panels_page_edit_relationship_form', $form);
    panels_ajax_render($output, $title, url($_GET['q'], NULL, NULL, TRUE));
  }
}

/**
 * Form (for ajax use) to add an argument
 */
function panels_page_edit_relationship_form($panel_name, $relationship, $relationships, $position, $contexts) {
  // Basic values required to orient ourselves
  $form['panel_name'] = array(
    '#type' => 'value',
    '#value' => $panel_name,
  );

  $form['position'] = array(
    '#type' => 'hidden',
    '#value' => $position,
  );

  $form['start_form'] = array('#value' => '<div class="modal-form clear-block">');

  $form['description'] = array(
    '#prefix' => '<div class="description">',
    '#suffix' => '</div>',
    '#value' => check_plain($relationship['description']),
  );

  // Basic relationship values
  $form['relationship']['#tree'] = TRUE;

  $form['relationship']['context'] = panels_context_selector($contexts, $relationship['required context'], $relationships[$position]['context']);

  $form['relationship']['name'] = array(
    '#type' => 'hidden',
    '#value' => $relationship['name'],
  );

  $form['relationship']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $relationships[$position]['title'],
    '#description' => t('Enter a title to use when this relationship is present. You may use %# substitution to get the title for the given relationship, where # is the relationship position. i.e, use %1 for the first relationship, %2 for the next relationship, and so on.'),
  );

  $form['relationship']['id'] = array(
    '#type' => 'value',
    '#value' => $relationships[$position]['id'],
  );

  $form['relationship']['identifier'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier'),
    '#description' => t('Enter a title to identify this relationship on administrative screens.'),
    '#default_value' => $relationships[$position]['identifier'],
  );

  // Settings particular to this relationship
  $relationship_settings = array();
  if (isset($relationships[$position]['relationship_settings'])) {
    $relationship_settings = $relationships[$position]['relationship_settings'];
  }

  if (isset($relationship['settings form']) && function_exists($relationship['settings form'])) {
    $form['relationship']['relationship_settings'] = $relationship['settings form']($relationship_settings);
    $form['relationship']['relationship_settings']['#tree'] = TRUE;
  }

  $form['end_form'] = array('#value' => '</div>');

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * validate an relationship edited/added via ajax
 */
function panels_page_edit_relationship_form_validate($form_id, $form_values, $form) {
  $relationship = $form_values['relationship'];

  if (isset($relationship['settings form validate']) && function_exists($relationship['settings form validate'])) {
    $relationship['settings form validate']($form, $form_values['relationship']['relationship_settings']);
  }
}

/**
 * Updates an relationship edited/added via ajax
 */
function panels_page_edit_relationship_form_submit($form_id, $form_values) {
  $relationship = $form_values['relationship'];
  $position = $form_values['position'];

  if (isset($relationship['settings form submit']) && function_exists($relationship['settings form submit'])) {
    $relationship['settings form submit']($form_values['relationship_settings']);
  }

  $relationships = panels_common_cache_get('relationships', $form_values['panel_name']);
  $relationships[$position] = $relationship;

  panels_common_cache_set('relationships', $form_values['panel_name'], $relationships);
  return $relationships;
}

/**
 * TODO: document this
 */
function panel_page_fetch_panel_page($panel_page, $did) {
  if (!is_object($panel_page)) {
    $panel_page = panels_page_load($panel_page);
  }

  // If we visit this page on a default panel, we have to make it real.
  if ($panel_page->pid == 'new') {
    $pid = panels_page_save($panel_page);
    // reload because not everything gets properly reset on save and we need
    // ids that were created.
    $panel_page = panels_page_load($pid);
  }

  if (empty($did)) {
    $panel_page->display = panels_load_display($panel_page->did);
  }
  else {
    $panel_page->display = panels_page_fetch_display($panel_page, $did);

    // Some displays might have an alternate context string which could be
    // important.
    if (isset($panel_page->displays[$did]['context'])) {
      $panel_page->display->display_context = $panel_page->displays[$did]['context'];
    }
  }

  return $panel_page;
}

/**
 * Pass through to the panels layout editor.
 */
function panels_page_edit_layout($panel_page, $did = NULL) {
  $panel_page = panel_page_fetch_panel_page($panel_page, $did);

  drupal_set_title(check_plain(panels_page_get_title($panel_page)));

  $dest = "admin/panels/panel-page/$panel_page->name/edit/layout";
  if ($did) {
    $dest .= "/$did";
  }
  return panels_edit_layout($panel_page->display, t('Save'), $dest);
}

/**
 * Pass through to the panels layout settings editor.
 */
function panels_page_edit_layout_settings($panel_page, $did = NULL, $next = NULL) {
  $panel_page = panel_page_fetch_panel_page($panel_page, $did);

  drupal_set_title(check_plain(panels_page_get_title($panel_page)));
  if (empty($next)) {
    $button = t('Save');
    $dest = "admin/panels/panel-page/$panel_page->name/edit/settings";
    if ($did) {
      $dest .= "/$did";
    }
  }
  else {
    $button = t('Next');
    $dest = "admin/panels/panel-page/$panel_page->name/edit/content";
  }

  return panels_edit_layout_settings($panel_page->display, $button, $dest);
}


/**
 * TODO: MOVE THIS. But where? Hm.
 * Get a bunch of empty contexts for this page
 */
function panels_page_get_contexts($page) {
  // Get contexts from arguments
  $contexts = panels_argument_get_contexts($page->arguments, $page->display->display_context);

  // add contexts from relationships
  if (!empty($page->contexts)) {
    panels_relationship_get_contexts($page->contexts, $contexts);
  }

  return $contexts;
}

/**
 * Pass through to the panels content editor.
 */
function panels_page_edit_content($panel_page, $did = NULL) {
  $panel_page = panel_page_fetch_panel_page($panel_page, $did);

  // Collect a list of contexts required by the arguments on this page.
  $contexts = panels_page_get_contexts($panel_page);

  $panel_page->display->context = $contexts;

  // Get a list of available content.
  require_once drupal_get_path('module', 'panels') . '/includes/common.inc';
  $content_types = panels_common_get_allowed_types('panels_page', $contexts);

  // Print this with theme('page') so that blocks are disabled while editing a display.
  // This is important because negative margins in common block layouts (i.e, Garland)
  // messes up the drag & drop.
  drupal_set_title(check_plain(panels_page_get_title($panel_page)));

  $dest = "admin/panels/panel-page/$panel_page->name/edit/content";
  if ($did) {
    $dest .= "/$did";
  }
  $output = panels_edit($panel_page->display, $dest, $content_types);

  print theme('page', $output, FALSE);
}

/**
 * Page callback to export a panel page to PHP code.
 */
function panels_page_edit_export($panel_page) {
  if (!is_object($panel_page)) {
    $panel_page = panels_page_load($panel_page);
  }
  drupal_set_title(check_plain($panel_page->title));
  $code = panels_page_export($panel_page);

  $lines = substr_count($code, "\n");
  $form['code'] = array(
    '#type' => 'textarea',
    '#title' => $panel_page->title,
    '#default_value' => $code,
    '#rows' => $lines);
  return $form;
}

/*
 * Page callback to import a panel page from PHP code.
 */
function panels_page_import_page() {
  if ($_POST['form_id'] == 'panels_page_edit_form') {
    $panel_page = $_SESSION['pp_import'];
    drupal_set_title(t('Import panel page "@s"', array('@s' => $panel_page->title)));
    return drupal_get_form('panels_page_edit_form', $panel_page);
  }

  return drupal_get_form('panels_page_import_form');
}

/*
 * Form to for the panel page import
 */
function panels_page_import_form() {
  $form['panel_page'] = array(
    '#type' => 'textarea',
    '#title' => t('Panel page code'),
    '#cols' => 60,
    '#rows' => 6,
    '#description' => t('Cut and paste the results of an Export Panel Page here.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Submit"),
  );

  $form['#redirect'] = NULL;
  return $form;
}

/*
 * Handle the submit button on importing a panel page.
 */
function panels_page_import_form_submit($formid, $form) {
  ob_start();
  eval($form['panel_page']);
  ob_end_clean();

  if (isset($page)) {
    drupal_set_title(t('Import panel page "@s"', array('@s' => $page->title)));
    $_SESSION['pp_import'] = $page;
    $output = drupal_get_form('panels_page_edit_form', $page);
    print theme('page', $output);
    exit;
  }
  else {
    drupal_set_message(t('Unable to get a panel page out of that.'));
  }
}
