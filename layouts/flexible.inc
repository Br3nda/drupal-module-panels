<?php
// $Id: flexible.inc,v 1.1.2.5 2007/10/11 20:23:24 merlinofchaos Exp $
/**
 * implementation of hook_panels_layouts
 */
function panels_flexible_panels_layouts() {
  $items['flexible'] = array(
    'title' => t('Flexible'),
    'icon' => 'layouts/flexible.png',
    'theme' => 'panels_flexible',
    'css' => 'layouts/flexible.css',
    'settings form' => 'panels_flexible_settings_form',
    'settings validate' => 'panels_flexible_settings_validate',
    'settings submit' => 'panels_flexible_settings_submit',
    'panels function' => 'panels_flexible_panels',
  );

  return $items;
}

function panels_flexible_default_panels() {
  return array(
    'rows' => 3, 
    'row_1' => array(
      'columns' => 1, 
      'width_1' => 100,
      'names' => array(t('Top')),
    ),
    'row_2' => array(
      'columns' => 3, 
      'width_1' => 25,
      'width_2' => 50,
      'width_3' => 25,
      'names' => array(t('Left'), t('Middle'), t('Right')),
    ),
    // row 3
    'row_3' => array(
      'columns' => 1, 
      'width_1' => 100,
      'names' => array(t('Bottom')),
    ),
  );
}

function panels_flexible_settings_form($display, $layout, &$settings) {
  if (empty($settings)) {
    // default for a new flexible layout
    $settings = panels_flexible_default_panels();
  }

  $form['instructions'] = array(
    '#value' => t('<p>Here you may determine the number of rows and columns your layout may have. Each row can have its own number of columns, and each column can have its width set independently. The widths are set in percentages, and for each row the widths must add up to 100%. When changing the number of rows or columns, click Save to update the form so you can set the widths for new cells properly.</p><p><strong>Note: Removing cells which contain panes will cause those panes to be disappear. Please move any content you wish to keep.</strong></p>'),
  );
  $form['rows'] = array(
    '#type' => 'textfield',
    '#size' => 2,
    '#width' => 10,
    '#title' => t('Rows'),
    '#description' => t('The number of rows this layout can have.'),
    '#default_value' => $settings['rows'],
  );

  for ($row = 1; $row <= intval($settings['rows']); $row++) {
    $form["row_$row"] = array(
      '#type' => 'fieldset',
      '#title' => t('Row @d', array('@d' => $row)),
    );
    $form["row_$row"]["columns"] = array(
      '#prefix' => '<div style="float: left; padding-right: 2em">',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#size' => 2,
      '#width' => 10,
      '#title' => t('Columns'),
//      '#description' => t('The number of columns in row @d.', array('@d' => $row)),
      '#default_value' => $settings["row_$row"]["columns"],
    );
    for ($col = 1; $col <= intval($settings["row_$row"]["columns"]); $col++) {
      $form["row_$row"]["width_$row_$col"] = array(
        '#prefix' => '<div style="float: left; padding-right: 2em">',
        '#suffix' => '</div>',
        '#type' => 'textfield',
        '#size' => 2,
        '#width' => 10,
        '#title' => t('Width @d', array('@d' => $col)),
        '#default_value' => $settings["row_$row"]["width_$col"],
      );
    }
    if (is_array($settings["row_$row"]["names"])) {
      $names = implode(', ', $settings["row_$row"]["names"]);
    }
    else {
      $names = '';
    }
    $form["row_$row"]['names'] = array(
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#title' => t('Column titles, separated by commas'),
      '#default_value' => $names,
    );
  }

  return $form;
}

function panels_flexible_settings_validate($values, $form, $display, $layout, $settings) {
  if ($values['rows'] < 1) {
    form_error($form['rows'], t('Rows must be a positive integer.'));
    return;
  }

  for ($row = 1; $row <= intval($values['rows']); $row++) {
    // This takes into account whether or even had a previous setting here.
    if ($settings['rows'] >= $row) {
      if ($values["row_$row"]['columns'] < 1) {
        form_error($form["row_$row"]['columns'], t('Columns must be a positive integer.'));
        return;
      }
      $total = 0;
      for ($col = 1; $col <= intval($values["row_$row"]["columns"]); $col++) {
        $total += $values["row_$row"]["width_$col"];
      }
      if ($total != 100) {
        form_error($form["row_$row"]['columns'], t('Column widths must add up to 100.'));
      }
    }
  }
}

function panels_flexible_settings_submit(&$values, $display, $layout, $settings) {
  for ($row = 1; $row <= $values['rows']; $row++) {
    if ($row > $settings['rows'] && empty($values["row_$row"]['columns'])) {
      $values["row_$row"]['columns'] = 1;
      $values["row_$row"]['width_1'] = 100;
    }
    if (!empty($values["row_$row"]['names'])) {
      $names = explode(',', $values["row_$row"]['names']);
      foreach ($names as $nid => $name) {
        $names[$nid] = trim($name);
      }
      $values["row_$row"]['names'] = $names;
    }
  }
}

/**
 * Define the actual list of columns and rows for this flexible panel.
 */
function panels_flexible_panels($display, $settings) {
  $panels = array();
  if (empty($settings)) {
    $settings = panels_flexible_default_panels();
  }

  for ($row = 1; $row <= intval($settings['rows']); $row++) {
    for ($col = 1; $col <= intval($settings["row_$row"]['columns']); $col++) {
      if (!empty($settings["row_$row"]['names'][$col - 1])) {
        $panels["row_${row}_$col"] = $settings["row_$row"]['names'][$col - 1];
      }
      else {
        $panels["row_${row}_$col"] = t("Row @row, Column @col", array('@row' => $row, '@col' => $col));
      }
    }
  }
  return $panels;
}

/**
 * This function uses heredoc notation to make it easier to convert
 * to a template.
 */
function theme_panels_flexible($id, $content, $settings) {
  if (empty($settings)) {
    $settings = panels_flexible_default_panels();
  }

  if ($id) {
    $idstr = " id='$id'";
    $idcss = "#$id";
  }
  else {
    $idcss = "div.panel-flexible";
  }

  $css = '';
  $output = "<div class=\"panel-flexible\" $idstr>\n";
  
  for ($row = 1; $row <= intval($settings['rows']); $row++) {
    $output .= "<div class=\"panel-row panel-row-$row clear-block\">\n";
    for ($col = 1; $col <= intval($settings["row_$row"]["columns"]); $col++) {
      if ($col == 1) {
        if (intval($settings["row_$row"]["columns"]) == 1) {
          $class = 'panel-col-only';
        }
        else {
          $class = 'panel-col-first';
        }
      }
      elseif ($col == intval($settings["row_$row"]["columns"])) {
        $class = 'panel-col-last';
      }
      else {
        $class = 'panel-col-inside';
      }
      $output .= "<div class=\"panel-col panel-col-$col $class\">\n";
      $output .= "<div class=\"inside\">" . $content["row_${row}_$col"] . "</div>\n";
      $output .= "</div>\n"; // panel-col-$col
      $css .= "$idcss div.panel-row-$row div.panel-col-$col { width: " . (intval($settings["row_$row"]["width_$col"]) - .02) . "%; }\n";
    }
    $output .= "</div>\n"; // panel-row-$row
  }
  $output .= "</div>\n"; // panel-flexible
  drupal_set_html_head("<style type=\"text/css\" media=\"all\">\n$css</style>\n");
  return $output;
}
