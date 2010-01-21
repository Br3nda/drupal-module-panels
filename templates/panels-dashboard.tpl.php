<?php
// $Id: panels-dashboard.tpl.php,v 1.2 2010/01/21 07:11:43 sdboyer Exp $
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
  <h3 class="dashboard-title"><?php print t('Create new') . '...'; ?></h3>
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

  <h3 class="dashboard-title"><?php print t('Manage mini panels') . '...'; ?></h3>
  <div class="dashboard-minis">
    <?php print $minis; ?>
    <div class="links">
      <?php print $minilink; ?>
    </div>
  </div>

</div>

<div class="dashboard-right">
  <h3 class="dashboard-title"><?php print t('Manage pages') . '...'; ?></h3>
  <div class="dashboard-pages">
    <?php print $pages; ?>
    <div class="links">
      <?php print $pagelink; ?>
    </div>
  </div>
</div>
