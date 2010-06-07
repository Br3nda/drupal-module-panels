// $Id: panels_ipe.js,v 1.1.2.2 2010/06/07 20:04:53 sdboyer Exp $


$(function() {
  var draggable_options = {
    helper: 'clone', // required for "flawless" interoperation with sortables
    connectToSortable: ($.ui.version === '1.6') ? ['div.panels-ipe-region'] : 'div.panels-ipe-region',
  }

  var sortable_options = {
    revert: true,
    opacity: 0.75, // opacity of sortable while sorting
    placeholder: 'draggable-placeholder',
    forcePlaceholderSize: true,
    connectWith: 'panels-ipe-region',
  }

  $('div.panels-ipe-region').sortable(sortable_options);
  $('div.panels-ipe-pane').parent().draggable(draggable_options);
})