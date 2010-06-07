<?php

/**
 * Standard render pipeline for a panels display.
 *
 */
class panels_renderer_standard {
  var $display;
  var $plugins = array();

  function panels_renderer_standard($display) {
    $this->display = &$display;
  }

  /**
   * Render a panel display.
   *
   * Alterations to output, or to the display controller object itself, can be
   * made by implementing the appropriate pre- and post-render hooks.
   *
   * This is the main entry point for the Panels rendering pipeline.
   * @return string
   *  HTML output that has made its way through the theme layer.
   */
  function render() {
    $this->plugins['layout'] = panels_get_layout($this->display->layout);
    if (!$this->plugins['layout']) {
      return NULL;
    }

    $output = '';

    // Let modules act just prior to render.
    foreach (module_implements('panels_pre_render') as $module) {
      $function = $module . '_panels_pre_render';
      $output .= $function($this->display);
    }

    $output .= $this->render_layout();

    // Let modules act just after render.
    foreach (module_implements('panels_post_render') as $module) {
      $function = $module . '_panels_post_render';
      $output .= $function($this->display);
    }
    return $output;
  }

  /**
   * Builds inner content, then hands off to layout-specified theme for final
   * render step.
   *
   * This is the outermost method in the Panels render pipeline. It calls the
   * inner methods, which return a content array, which is in turn passed to the
   * theme function specified in the layout plugin.
   *
   * @return string
   *  Themed & rendered HTML output.
   */
  function render_layout() {
    // TODO can probably make this early-add CSS business go away by making
    // mini panels use a slightly different plugin
    if (!empty($this->layout_plugin['css'])) {
      if (file_exists(path_to_theme() . '/' . $this->layout_plugin['css'])) {
        drupal_add_css(path_to_theme() . '/' . $this->layout_plugin['css']);
      }
      else {
        drupal_add_css($this->layout_plugin['path'] . '/' . $this->layout_plugin['css']);
      }
    }
    // This now comes after the CSS is added, because panels-within-panels must
    // have their CSS added in the right order; inner content before outer content.

    if (empty($this->display->cache['method'])) {
      $content = $this->render_regions();
    }
    else {
      // TODO This whole approach can & probably should be refactored now. Maybe
      // invert it, and have the caching agent act from the outside?
      $cache = panels_get_cached_content($this->display, $this->display->args, $this->display->context);
      if ($cache === FALSE) {
        $cache = new panels_cache_object();
        $cache->set_content($this->render_regions());
        panels_set_cached_content($cache, $this->display, $this->display->args, $this->display->context);
      }
      $content = $cache->content;
    }

    $output = theme($this->layout_plugin['theme'], check_plain($this->display->css_id), $content, $this->display->layout_settings, $this->display);

    return $output;
  }

  /**
   *
   * @return unknown_type
   */
  function render_regions() {
    ctools_include('content');

    // First, render all the panes into little boxes. We do this here because
    // some panes request to be rendered after other panes (primarily so they
    // can do the leftovers of forms).
    $panes = array();
    $later = array();

    foreach ($this->display->content as $pid => $pane) {
      // TODO remove in 7.x and ensure the upgrade path weeds out any stragglers; it's been long enough
      $pane->shown = !empty($pane->shown); // guarantee this field exists.
      // If the user can't see this pane, do not render it.
      if (!$pane->shown || !panels_pane_access($pane, $display)) {
        continue;
      }

      // If this pane wants to render last, add it to the $later array.
      // TODO store this on the display to minimize calls (optimization)
      $content_type = ctools_get_content_type($pane->type);

      if (!empty($content_type['render last'])) {
        $later[$pid] = $pane;
        continue;
      }

      $panes[$pid] = $this->render_pane_content($pane);
    }

    foreach ($later as $pid => $pane) {
      $panes[$pid] = $this->render_pane_content($pane);
    }

    // Loop through all panels, put all panes that belong to the current panel
    // in an array, then render the panel. Primarily this ensures that the
    // panes are in the proper order.
    $content = array();
    foreach ($this->display->panels as $panel_name => $pids) {
      $panel = array();
      foreach ($pids as $pid) {
        if (!empty($panes[$pid])) {
          $panel[$pid] = $panes[$pid];
        }
      }
      $content[$panel_name] = $this->render_region($panel_name, $panes);
    }

    // Prevent notices by making sure that all panels at least have an entry:
    // TODO refactor to make this unnecessary (optimization)
    $panels = panels_get_panels($this->layout_data, $this->display);
    foreach ($panels as $id => $panel) {
      if (!isset($content[$id])) {
        $content[$id] = NULL;
      }
    }

    return $content;
  }

  // TODO remove this method by collapsing it into $this->render_panes()
  function render_pane_content($pane) {
    $content = $this->get_pane_content($pane);

    // Pass long the css_id that is usually available.
    if (!empty($pane->css['css_id'])) {
      $content->css_id = $pane->css['css_id'];
    }

    // Pass long the css_class that is usually available.
    if (!empty($pane->css['css_class'])) {
      $content->css_class = $pane->css['css_class'];
    }

    return $content;
  }

  function get_pane_content($pane) {
    ctools_include('context');
    // TODO finally safe to remove this check?
    if (!is_array($this->display->context)) {
      $this->display->context = array();
    }

    $content = FALSE;
    $caching = !empty($pane->cache['method']) ? TRUE : FALSE;
    if ($caching && ($cache = panels_get_cached_content($this->display, $this->display->args, $this->display->context, $pane))) {
      $content = $cache->content;
    }
    else {
      $content = ctools_content_render($pane->type, $pane->subtype, $pane->configuration, array(), $this->display->args, $this->display->context);
      foreach (module_implements('panels_pane_content_alter') as $module) {
        $function = $module . '_panels_pane_content_alter';
        $function($content, $pane, $this->display->args, $this->display->context);
      }
      if ($caching) {
        $cache = new panels_cache_object();
        $cache->set_content($content);
        panels_set_cached_content($cache, $this->display, $this->display->args, $this->display->context, $pane);
        $content = $cache->content;
      }
    }

    return $content;
  }

  /**
   * Render a single panel region.
   */
  function render_region($panel_name, $panes) {
    list($style, $style_settings) = panels_get_panel_style_and_settings($this->display->panel_settings, $panel_name);

    // Retrieve the pid (can be a panel page id, a mini panel id, etc.), this
    // might be used (or even necessary) for some panel display styles.
    // TODO: Got to fix this to use panel page name instead of pid, since pid is
    // no longer guaranteed. This needs an API to be able to set the final id.
    $owner_id = 0;
    if (isset($this->display->owner) && is_object($this->display->owner) && isset($this->display->owner->id)) {
      $owner_id = $this->display->owner->id;
    }

    return theme($style['render panel'], $this->display, $owner_id, $panes, $style_settings, $panel);
  }
}