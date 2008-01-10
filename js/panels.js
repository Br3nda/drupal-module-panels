// $Id: panels.js,v 1.1.2.6 2008/01/10 17:44:10 merlinofchaos Exp $

Drupal.Panels = {};

Drupal.Panels.autoAttach = function() {
  $("div.panel-pane").hover(
    function() {
      $('div.panel-hide', this).addClass("panel-hide-hover"); return true;
    },
    function(){
      $('div.panel-hide', this).removeClass("panel-hide-hover"); return true;
    }
  );
}

$(Drupal.Panels.autoAttach);
