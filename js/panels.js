// $Id: panels.js,v 1.1.2.1 2007/12/06 16:13:30 merlinofchaos Exp $

Drupal.Panels = {};

Drupal.Panels.autoAttach = function() {
  $("div.panel-pane").hover(
    function() { 
      $('div.panel-hide', this).addClass("hover"); 
    }, 
    function(){ 
      $('div.panel-hide', this).removeClass("hover"); 
    }
  );
}

$(Drupal.Panels.autoAttach);
