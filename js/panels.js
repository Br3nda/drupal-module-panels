// $Id: panels.js,v 1.1.2.2 2008/01/08 18:40:14 merlinofchaos Exp $

Drupal.Panels = {};

Drupal.Panels.autoAttach = function() {
  $("div.panel-pane").hover(
    function() { 
      $('div.panel-hide', this).addClass("hover"); 
      return true;
    }, 
    function(){ 
      $('div.panel-hide', this).removeClass("hover"); 
      return true;
    }
  );
}

$(Drupal.Panels.autoAttach);
