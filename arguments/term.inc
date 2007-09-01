<?php
// $Id: term.inc,v 1.1.2.1 2007/09/01 00:23:50 merlinofchaos Exp $
/** 
 * @file arguments/term.inc
 * 
 * Plugin to provide an argument handler for a node id
 */

function panels_term_panels_arguments() {
  $args['term'] = array(
    'title' => t("Taxonomy term ID"),
    'description' => t('Restricts the argument to a node id.'),
    'context' => 'panels_term_context',
    'settings form' => 'panels_term_settings_form',
  );
  return $args;
}
