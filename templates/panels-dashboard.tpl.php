<?php
// $Id: panels-dashboard.tpl.php,v 1.1.2.2 2009/07/10 03:03:36 merlinofchaos Exp $
/**
 * @file views-view.tpl.php
 * Main view template
 *
 * Variables available:
 *  -
 *
 */
?>
<div class="dashboard-left">
  <h3 class="dashboard-title">Create new...</h3>
  <div class="dashboard-entry clear-block">
    <div class="dashboard-text">
      <div class="dashboard-link">
        <?php print $new_panel_page; ?>
      </div>
      <div class="description">
        <?php print $panel_page_description; ?>
      </div>
    </div>
  </div>

  <div class="dashboard-entry clear-block">
    <div class="dashboard-text">
      <div class="dashboard-link">
        <?php print $new_panel_node; ?>
      </div>
      <div class="description">
        <?php print $panel_node_description; ?>
      </div>
    </div>
  </div>

  <div class="dashboard-entry clear-block">
    <div class="dashboard-text">
      <div class="dashboard-link">
        <?php print $new_panel_mini; ?>
      </div>
      <div class="description">
        <?php print $panel_mini_description; ?>
      </div>
    </div>
  </div>

  <h3 class="dashboard-title">Manage mini panels...</h3>
  <div class="dashboard-minis">
    <?php print $minis; ?>
    <div class="links">
      <?php print $minilink; ?>
    </div>
  </div>

</div>

<div class="dashboard-right">
  <h3 class="dashboard-title">Manage pages...</h3>
  <div class="dashboard-pages">
    <?php print $pages; ?>
    <div class="links">
      <?php print $pagelink; ?>
    </div>
  </div>
</div>
