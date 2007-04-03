<?php
/**
 * Callback function to supply a list of content types.
 */
function panels_block_panels_content_types() {
  $items['block'] = array(
    'title' => t('Block'),
    'content_types' => 'panels_admin_content_types_block',
    'render callback' => 'panels_content_block',
    'add callback' => 'panels_admin_add_block',
    'edit callback' => 'panels_admin_edit_block',
    'title callback' => 'panels_admin_title_block',
    //'validate callback' => 'panels_admin_validate_block',
  );
  return $items;
}

/**
 * Output function for the 'block' content type. Outputs a block
 * based on the module and delta supplied in the configuration.
 */
function panels_content_block($conf) {
  $block = (object) module_invoke($conf['module'], 'block', 'view', $conf['delta']);
  $block->module = $conf['module'];
  $block->delta = $conf['delta'];
  if ($conf['override_title']) {
    $block->subject = check_plain($conf['override_title_text']);
  }
  // Test for block visibility
  $result = db_query("SELECT pages, visibility FROM {blocks} WHERE module = '%s' AND delta = %d", $block->module, $block->delta);
  $block_visibility = db_fetch_object($result);

  if ($block_visibility && $block_visibility->pages) {
   if ($block_visibility->visibility < 2) {
     $path = drupal_get_path_alias($_GET['q']);
     $regexp = '/^('. preg_replace(array('/(\r\n?|\n)/', '/\\\\\*/', '/(^|\|)\\\\<front\\\\>($|\|)/'), array('|', '.*', '\1'. preg_quote(variable_get('site_frontpage', 'node'), '/') .'\2'), preg_quote($block->pages, '/')) .')$/';
     $page_match = !($block->visibility xor preg_match($regexp, $path));
   } else {
     $page_match = drupal_eval($block->pages);
   }
  } else {
   $page_match = TRUE;
  }

  if ($page_match) {
    return $block;
  }
}

/**
 * Return all content types available.
 */
function panels_admin_content_types_block() {
  $types = array();
  foreach (module_list() as $module) {
    $module_blocks = module_invoke($module, 'block', 'list');
    if ($module_blocks) {
      foreach ($module_blocks as $delta => $block) {
        // strip_tags used because it goes through check_plain and that
        // just looks bad.
        $types["$module-$delta"] = array(
          'title' => strip_tags($block['info']),
          'icon' => 'block.png',
          // Do we want to create some default icons here?
        );
      }
    }
  }
  uasort($types, 'panels_title_sort');
  return $types;
}

/**
 * Returns the form for a new block.
 */
function panels_admin_add_block($id) {
  $conf = array();
  list($conf['module'], $conf['delta']) = explode('-', $id, 2);
  return panels_admin_edit_block($conf);
}

/**
 * Returns an edit form for a block.
 */
function panels_admin_edit_block($conf) {
  $form['module'] = array(
    '#type' => 'value',
    '#value' => $conf['module'],
  );
  $form['delta'] = array(
    '#type' => 'value',
    '#value' => $conf['delta'],
  );
  $form['override_title'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['override_title'],
    '#title' => t('Override title'),
    '#description' => t('If checked, the block title will be overridden with the override title text.')
  );
  $form['override_title_text'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['override_title_text'],
    '#title' => t('Override title text'),
    '#size' => 15,
  );

  return $form;
}

/**
 * Returns the administrative title for a type.
 */
function panels_admin_title_block($conf) {
  $block = module_invoke($conf['module'], 'block', 'list');
  $title = filter_xss_admin($block[$conf['delta']]['info']);
  return $title;
}
