// $Id: panels.js,v 1.1.2.3 2008/01/08 19:20:48 merlinofchaos Exp $

Drupal.Panels = {};

Drupal.Panels.autoAttach = function() {
  // Using .hover seems to mess with the href in statusbar when hovering over links in FF.
  $("div.panel-pane").mouseover(
    function() {
      $('div.panel-hide', this).addClass("hover"); return true;
    }
   );
   $("div.panel-pane").mouseout(
    function(){
      $('div.panel-hide', this).removeClass("hover"); return true;
    }
  );
}

$(Drupal.Panels.autoAttach);
