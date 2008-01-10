// $Id: panels.js,v 1.1.2.5 2008/01/10 17:38:34 merlinofchaos Exp $

Drupal.Panels = {};

Drupal.Panels.autoAttach = function() {
  // Using .hover seems to mess with the href in statusbar when hovering over links in FF.
  $("div.panel-pane").mouseover(
    function() {
      $('div.panel-hide', this).addClass("panel-hide-hover"); return true;
    }
   );
  $("div.panel-pane").mouseout(
   function(){
      $('div.panel-hide', this).removeClass("panel-hide-hover"); return true;
    }
  );
}

$(Drupal.Panels.autoAttach);
