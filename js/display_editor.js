$(document).ready(function () {
  $('.panels-dnd').Panels();
  $('#panels-dnd-save').click(function() {
    serial = $.SortSerialize();
    save = $('#panels-dnd-save').val();
    $.ajax({
      type: "POST",
      url: "/panels/ajax/save-display/" + $('#panel-did').val(),
      data: serial.hash,
      global: true,
      success: function() {
        $('#panel-op').val(save);
        $('#panels-edit-display')[0].submit();
      }
    });
    return false;
  });
  $('.panels-add-block').click(function() {
    // show the empty dialog right away.
    $('#panels-modal').modalContent({
        opacity: '.40', 
        background: '#fff'
      }
    );
    $.ajax({
      type: "POST",
      url: "/panels/ajax/add-content/" + $('#panel-did').val(),
      global: true,
      success: bindResponse,
      dataType: 'json'
    });
    return false;
  });
  $('.panels-js-only').css('display', 'inline');
  $('#panels-hide-all').click(function() {
    $('.panel-portlet .content').hide();
    $('.panel-portlet .toggle').addClass('toggle-collapsed');
    return false;
  });

  $('#panels-show-all').click(function() {
    $('.panel-portlet .content').show();
    $('.panel-portlet .toggle').removeClass('toggle-collapsed');
    return false;
  });
  bindConfigure($(document));
  bindDelete($(document));

});

function bindResponse(data) {
  // On success, append the returned HTML to the panel's element.
  if (data.type == 'display') {
    if (data.debug) {
      alert(data.debug);
    }
    // append the output
    $('#modalContent .modal-title').html(data.title);
    $('#modalContent .modal-content').html(data.output);

    // Bind forms to ajax submit.
    $('.panels-modal-content form').submit(function() {
      $(this).ajaxSubmit({
        url: '/panels/ajax/submit-form/' +  $('#panel-did').val(),
        method: 'post',
        after: bindResponse,
        dataType: 'json'
      });
      return false;
    });
  }
  else if (data.type == 'add') {
    // add the data to our sortables
    $('#panel-pane-' + data.area).append(data.output)
      .SortableAddItem(document.getElementById('panel-pane-' + data.id));
    
    bindConfigure($('#panel-pane-' + data.id));
    bindDelete($('#panel-pane-' + data.id));
    $('#panel-pane-' + data.id).each(bindPortlet);
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  }
  else if (data.type == 'replace') {
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible').html(data.output);
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible').each(bindPortlet);
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  } 
}

function bindConfigure(o) {
  o.find('.pane-configure').click(function() {
    id = $(this)[0].id.replace('edit-button-', '').replace('-configure', '');
    // show the empty dialog right away.
    $('#panels-modal').modalContent({
        opacity: '.40', 
        background: '#fff'
      }
    );
    $.ajax({
      type: "POST",
      url: "/panels/ajax/configure/" + $('#panel-did').val() + '/' + id,
      global: true,
      success: bindResponse,
      dataType: 'json'
    });
    return false;
  });
}
function bindDelete(o) {
  o.find('.pane-delete').click(function() {
    if (confirm('Remove this pane?')) {
      id = $(this)[0].id.replace('edit-button-', '').replace('-delete', '');
      $('#panel-pane-' + id).remove();
      refreshObjects();
    }
    return false;
  });

}

/* this is a hack that forces objects to refresh, because sometimes they
   don't move to where they are supposed to when things snap.
   It's not very reliable, however.
*/
function refreshObjects() {
/*  $('.grabber').hide().show(); */
}

jQuery.Panels = {
  build: function(o) {
    // default settings
    var s = {
      column: '.panels-dnd-col',
      block: '.panel-pane',
      handle: '.grabber',
      helperclass: 'helperclass',
      hoverclass: 'hoverclass'
    }

    // merge options with default settings
    if (o) {
      $.extend(s, o);
    }

    // add .panel-portlet to all draggable blocks
    $(this).find(s.block).addClass('panel-portlet');

    // make columns sortable
    $(this).find(s.column).Sortable({
      accept: 'panel-portlet',
      handle: s.handle,
      helperclass: s.helperclass,
      hgverclass: s.hoverclass,
      tolerance: 'intersect',
      fx: 100,
      revert: true,
      onStop: refreshObjects
    });

    $(this).find('.panel-portlet').each(bindPortlet);
  }
}

function bindPortlet() {
  var handle = $(this).find('.title');
  var content = $(this).find('.content');
  if (content.length) {
    var toggle = $('<span class="toggle toggle-collapsed"></span>');
    handle.before(toggle);
    toggle.click(function() {
      content.slideToggle(20);
      toggle.toggleClass('toggle-collapsed');
    });
    handle.click(function() {
      content.slideToggle(20);
      toggle.toggleClass('toggle-collapsed');
    });
    content.hide();
  }
}

jQuery.fn.Panels = jQuery.Panels.build;
