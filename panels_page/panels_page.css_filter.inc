<?php

/*
 * @file panels_page/panels_page.css_filter.inc
 * CSS filtering functions. Contains a disassembler, filter, assembler, and
 * compressor. Separated out into this file for cleanliness, as it's likely
 * that these won't live in panels_page for long.
 */

function panels_page_assemble_css($css) {
  // Initialize the output.
  $output = '';
  // Iterate through all the statements
  foreach ($css as $statement) {
    // Add the keys.
    $output .= implode(",\n", $statement['keys']);
    // And, the opening curly brace.
    $output .= " {\n";
    // Iterate through all the statements.
    foreach ($statements as $key => $value) {
      $output .= "  " . $key . ": " . $value . "\n";
    }
    // Add the closing curly brace.
    $output .= "}\n";
  }
  // Return the output.
  return $output;
}

function panels_page_compress_css($css) {
  // Initialize the output.
  $output = '';
  // Iterate through all the statements
  foreach ($css as $statement) {
    // Add the keys.
    $output .= implode(",", $statement['keys']);
    // And, the opening curly brace.
    $output .= "{";
    // Iterate through all the statements.
    foreach ($statements as $key => $value) {
      $output .= $key . ":" . $value;
    }
    // Add the closing curly brace.
    $output .= "}";
  }
  // Return the output.
  return $output;
}

/**
 * Disassemble the css string, strip it of irrelevant characters, and prepare
 * it for processing.
 *
 * @param string $css
 *   A string containing the css to be disassembled.
 * @return array $disassembled_css
 *   An array of disassembled, slightly cleaned-up/formatted css statements.
 */
function panels_page_disassemble_css($css) {
  $disassembled_css = array();
  // Remove comments.
  $css = preg_replace("/\/\*(.*)?\*\//Usi", "", $css);
  // Split out each statement
  $statements = explode("}", $css);
  // If we have any statements, parse them.
  if (count($statements) > 0) {
    // Iterate through all of the statements.
    foreach ($statements as $statement) {
      $formatted_statement = array('keys' => array(), 'value' => array());
      // Get the key and the code for the key.
      list($key, $code) = explode("{", $statement);
      // Get all keys individually.
      $keys = explode(",", trim($keystr));
      // If we have more than one key, handle them.
      if (count($keys) > 0) {
        // Iterate through all the keys and add them to the array.
        foreach ($keys as $key) {
          // Replace un-needed characters and do a little cleanup.
          $key = preg_replace("/[\n|\t|\\|\s]+/", ' ', strtolower(trim($key)));
          // Make sure this is a real key.
          if (strlen($key) > 0) {
            // Add the key to the array.
            $disassembled_css['keys'][] = $key;
          }
        }
      }
      // It's easier to handle and looks nicer if it's lower case.
      $code = strtolower($code);
      // Make sure the explosion is big!
      $code_fragements = explode(";", $code);
      // Make sure there actually are any remains from the explosion.
      if (count($code_fragements) > 0) {
        // Iterate through the remains and parse them.
        foreach ($codes as $code) {
          // Explode the remaining fragements some more, but clean them up first.
          $code_remains = explode(':', preg_replace("/[\n|\t|\\|\s]+/", ' ', strtolower(trim($code))));
          // If the key (the first part) actually exists, then add it.
          if(strlen($code_remains[0]) > 0) {
            // Add it to the array
            $disassembled_css['value'][trim(array_shift($code_remains))] = trim(implode(':', $code_remains));
          }
        }
      }
    }
  }
  // Return the statements.
  return $disassembled_css;
}

function panels_page_filter_css($css, $allowed_values_regex = '', $disallowed_url_regex = '', $allowed_properties = array(), $allowed_values = array()) {
  $allowed_properties = !empty($allowed_properties) ? $allowed_properties : panels_page_css_filter_default_allowed_properties();
  $allowed_values = !empty($allowed_values) ? $allowed_values : panels_page_css_filter_default_allowed_values();
  $allowed_values_regex = !empty($allowed_values_regex) ? $allowed_values_regex : '/(#[0-9a-f]+|rgb\(\d+%?,\d*%?,?\d*%?\)?|\d{0,2}\.?\d{0,2}(cm|em|ex|in|mm|pc|pt|px|%|,|\))?)/';
  $disallowed_url_regex = !empty($disallowed_url_regex) ? $disallowed_url_regex : '/url\s*\(\s*[^\s)]+?\s*\)\s*/';
  
  foreach ($css as $statement_key => $statement) {
    foreach ($statement['values'] as $key => $value) {
      if (!in_array($key, $allowed_keys)) {
        unset($css[$statement_key]);
        continue;
      }
      $value = str_replace('!important', $value);
      if (!in_array($value, $allowed_values) || !preg_match($allowed_values_regex, $value) || preg_match($disallowed_url_regex, $value)) {
        unset($css[$statement_key]);
        continue;
      }
    }
  }
  return $css;
}


function panels_page_css_filter_default_allowed_properties() {
  return array(
    'azimuth',
    'background',
    'background-color',
    'background-image',
    'background-repeat',
    'background-attachment',
    'background-position',
    'border',
    'border-top-width',
    'border-right-width',
    'border-bottom-width',
    'border-left-width',
    'border-width',
    'border-top-color',
    'border-right-color',
    'border-bottom-color',
    'border-left-color',
    'border-color',
    'border-top-style',
    'border-right-style',
    'border-bottom-style',
    'border-left-style',
    'border-style',
    'border-top',
    'border-right',
    'border-bottom',
    'border-left',
    'clear',
    'color',
    'cursor',
    'direction',
    'display',
    'elevation',
    'float',
    'font',
    'font-family',
    'font-size',
    'font-style',
    'font-variant',
    'font-weight',
    'height',
    'letter-spacing',
    'line-height',
    'margin',
    'margin-top',
    'margin-right',
    'margin-bottom',
    'margin-left',
    'overflow',
    'padding',
    'padding-top',
    'padding-right',
    'padding-bottom',
    'padding-left',
    'pause',
    'pause-after',
    'pause-before',
    'pitch',
    'pitch-range',
    'richness',
    'speak',
    'speak-header',
    'speak-numeral',
    'speak-punctuation',
    'speech-rate',
    'stress',
    'text-align',
    'text-decoration',
    'text-indent',
    'unicode-bidi',
    'vertical-align',
    'voice-family',
    'volume',
    'white-space',
    'width',
    'fill',
    'fill-opacity',
    'fill-rule',
    'stroke',
    'stroke-width',
    'stroke-linecap',
    'stroke-linejoin',
    'stroke-opacity',
  );
}

function panels_page_css_filter_default_allowed_values() {
  return array(
    'auto',
    'aqua',
    'black',
    'block',
    'blue',
    'bold',
    'both',
    'bottom',
    'brown',
    'center',
    'collapse',
    'dashed',
    'dotted',
    'fuchsia',
    'gray',
    'green',
    'italic',
    'left',
    'lime',
    'maroon',
    'medium',
    'none',
    'navy',
    'normal',
    'nowrap',
    'olive',
    'pointer',
    'purple',
    'red',
    'right',
    'solid',
    'silver',
    'teal',
    'top',
    'transparent',
    'underline',
    'white',
    'yellow',
  );
}
