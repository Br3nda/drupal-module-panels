
Drupal.Panels = {};

/** add content button **/
Drupal.Panels.clickAdd = function() {
  var id = $(this)[0].id.replace('pane-add-', '');
  // show the empty dialog right away.
  $('#panels-modal').modalContent({
      opacity: '.40', 
      background: '#fff'
    }
  );
  $('#modalContent .modal-content').html($('div#panels-throbber').html());
  $.ajax({
    type: "POST",
    url: "/panels/ajax/add-content/" + $('#panel-did').val() + '/' + id,
    global: true,
    success: Drupal.Panels.bindAjaxResponse,
    dataType: 'json'
  });
  return false;
}

/** hide all button **/
Drupal.Panels.clickHideAll = function() {
  $('.panel-portlet .content').hide();
  $('.panel-portlet .toggle').addClass('toggle-collapsed');
  return false;
}

/** show all button **/
Drupal.Panels.clickShowAll = function() {
  $('.panel-portlet .content').show();
  $('.panel-portlet .toggle').removeClass('toggle-collapsed');
  return false;
}

/** Configure pane button */
Drupal.Panels.bindClickConfigure = function (o) {
  $('input.pane-configure').unclick();
  $('input.pane-configure').click(function() {
    var id = $(this)[0].id.replace('edit-button-', '').replace('-configure', '');
    // show the empty dialog right away.
    $('#panels-modal').modalContent({
        opacity: '.40', 
        background: '#fff'
      }
    );
    $('#modalContent .modal-content').html($('div#panels-throbber').html());
    $.ajax({
      type: "POST",
      url: "/panels/ajax/configure/" + $('#panel-did').val() + '/' + id,
      global: true,
      success: Drupal.Panels.bindAjaxResponse,
      dataType: 'json'
    });
    return false;
  });
}

/** Delete pane button **/
Drupal.Panels.bindClickDelete = function(o) {
  $('input.pane-delete').unclick();
  $('input.pane-delete').click(function() {
    if (confirm('Remove this pane?')) {
      var id = $(this)[0].id.replace('edit-button-', '').replace('-delete', '');
      $('#panel-pane-' + id).remove();
    }
    return false;
  });
}

/** Bind an item to a checkbox to auto disable it when unchecked **/
Drupal.Panels.bindCheckBox = function(checkbox, gadget) {
  var clickCheckBox = function() {
    if ($(checkbox).attr('checked')) {
      $(gadget).attr('disabled', false);
    }
    else {
      $(gadget).attr('disabled', true);
    }    
  }

  $(checkbox).change(); // unset any existing
  $(checkbox).change(clickCheckBox);
  clickCheckBox();
}

Drupal.Panels.Subform = {};

/** Basic submit on a subform **/
Drupal.Panels.Subform.bindClickSubmit = function() {
  $(this).ajaxSubmit({
    url: '/panels/ajax/submit-form/' +  $('#panel-did').val(),
    method: 'post',
    after: Drupal.Panels.bindAjaxResponse,
    dataType: 'json'
  });
  return false;
}

Drupal.Panels.Subform.bindClickAddLink = function() {
  var id = $(this)[0].id;
  // show the empty dialog right away.
  $.ajax({
    type: "POST",
    url: "/panels/ajax/add-config/" + $('#panel-did').val() + '/' + id,
    global: true,
    success: Drupal.Panels.bindAjaxResponse,
    dataType: 'json'
  });
  return false;
}

/** ripped from Drupal cause it's not easy to re-autoattach cleanly */
Drupal.Panels.Subform.bindCollapsible = function() {
  $('div.panels-modal-content fieldset.collapsible > legend').each(function() {
    var fieldset = $(this.parentNode);
    // Expand if there are errors inside
    if ($('input.error, textarea.error, select.error', fieldset).size() > 0) {
      fieldset.removeClass('collapsed');
    }

    // Turn the legend into a clickable link and wrap the contents of the fieldset
    // in a div for easier animation
    var text = this.innerHTML;
    $(this).empty().append($('<a href="#">'+ text +'</a>').click(function() {
      var fieldset = $(this).parents('fieldset:first')[0];
      // Don't animate multiple times
      if (!fieldset.animating) {
        fieldset.animating = true;
        Drupal.toggleFieldset(fieldset);
      }
      return false;
    })).after($('<div class="fieldset-wrapper"></div>').append(fieldset.children(':not(legend)')));
  });
}

/** ripped from Drupal cause it's not easy to re-autoattach cleanly */
Drupal.Panels.Subform.bindAutocomplete = function() {
  var acdb = [];
  $('.panels-modal-content input.autocomplete').each(function () {
    var uri = this.value;
    if (!acdb[uri]) {
      acdb[uri] = new Drupal.ACDB(uri);
    }
    var input = $('#' + this.id.substr(0, this.id.length - 13))
      .attr('autocomplete', 'OFF')[0];
    $(input.form).submit(Drupal.autocompleteSubmit);
    new Drupal.jsAC(input, acdb[uri]);
  });
}

Drupal.Panels.bindAjaxResponse = function(data) {
  // On success, append the returned HTML to the panel's element.
  if (data.type == 'display') {
    if (data.debug) {
      alert(data.debug);
    }
    // append the output
    $('#modalContent span.modal-title').html(data.title);
    $('#modalContent div.modal-content').html(data.output);

    // Bind forms to ajax submit.
    $('div.panels-modal-content form').unsubmit(); // be safe here.
    $('div.panels-modal-content form').submit(Drupal.Panels.Subform.bindClickSubmit);

    // Bind links to ajax links.
    $('a.panels-modal-add-config').click(Drupal.Panels.Subform.bindClickAddLink);

    if ($('#override-title-checkbox').size()) {
      Drupal.Panels.bindCheckBox('#override-title-checkbox', '#override-title-textfield');
    }

    if ($('#use-pager-checkbox').size()) {
      Drupal.Panels.bindCheckBox('#use-pager-checkbox', '#use-pager-textfield');
    }

    // hack: allow collapsible textareas to work
    Drupal.Panels.Subform.bindCollapsible();

    Drupal.Panels.Subform.bindAutocomplete();
  }
  else if (data.type == 'add') {
    // Give it all the goodies that our existing panes have.   
    $('#panel-pane-' + data.area).append(data.output);
    
    Drupal.Panels.attachPane('#panel-pane-' + data.id);
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  }
  else if (data.type == 'replace') {
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible').html(data.output);
    $('#panel-pane-' + data.id + ' .panel-pane-collapsible').each(Drupal.Panels.bindPortlet);
    // dismiss the dialog
    $('#panels-modal').unmodalContent();
  } 
}

Drupal.Panels.bindPortlet = function() {
  var handle = $(this).find('h2.title');
  var content = $(this).find('div.content');
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

Drupal.Panels.Draggable = {
  object: null,

  dropzones: [],
  current_dropzone: null,

  landing_pads: [],
  current_pad: null,

  mouseOffset: { x: 0, y: 0 },
  windowOffset: { x: 0, y: 0 },
  // original settings to be restored
  original: {},

  hoverclass: 'hoverclass',
  helperclass: 'helperclass',
  accept: 'div.panels-display',
  handle: 'div.grabber',
  draggable: 'div.panel-portlet',
  main: 'div#panels-dnd-main',

  // part of the id to remove to get just the number
  draggableId: 'panel-pane-',
  // What to add to the front of a the id to get the form id for a panel
  formId: 'input#edit-',

  maxWidth: 250,

  unsetDropZone: function() {
    $(this.current_dropzone.obj).removeClass(this.hoverclass);
    this.current_dropzone = null;
    for (var i in this.landing_pads) {
      $(this.landing_pads[i].obj).remove();
    }
    this.landing_pads = [];
    this.current_pad = null;
  },

  createLandingPad: function(where, append) {
    var obj = $('<div class="' + this.helperclass +'" id="' + 
      $(where).attr('id') + '-dropzone">&nbsp;</div>');
    if (append) {
      $(where).append(obj);
    }
    else {
      $(where).before(obj);
    }
    var offset = $(obj).offset();

    $(obj).css({
      display: 'none'
    });
    this.landing_pads.push({ 
      centerX: offset.left + ($(obj).innerWidth() / 2),
      centerY: offset.top + ($(obj).innerHeight() / 2),
      obj: obj
    });
    return obj;
  },

  calculateDropZones: function() {
    var dropzones = [];
    $(this.accept).each(function() {
      var offset = $(this).offset();
      offset.obj = this;
      dropzones.push(offset);
    });
    this.dropzones = dropzones;
  },

  reCalculateDropZones: function() {
    for (var i in this.dropzones) {
      offset = $(this.dropzones[i].obj).offset();
      jQuery.extend(this.dropzones[i], offset);
    }
  },

  changeDropZone: function(new_dropzone) {
    // Unset our old dropzone.
    if (this.current_dropzone) {
      this.unsetDropZone();
    }

    // Set up our new dropzone.
    this.current_dropzone = new_dropzone;
    $(this.current_dropzone.obj).addClass(this.hoverclass);
    // add a landing pad
    this.createLandingPad(this.current_dropzone.obj, true);

    var that = this;
    // Create a landing pad before each existing portlet.
    $(this.current_dropzone.obj).find(this.draggable).each(function() {
      if (that.object.id != this.id) {
        that.createLandingPad(this, false);
      }
    });
  },

  findLandingPad: function(x, y) {
    var shortest_distance = null;
    var nearest_pad = null;
    // find the nearest pad.
    for (var i in this.landing_pads) {
      // This isn't the real distance, this is the square of the
      // distance -- no point in spending processing time on
      // sqrt.
      var dstx = Math.abs(x - this.landing_pads[i].centerX);
      var dsty = Math.abs(y - this.landing_pads[i].centerY);
      var distance =  (dstx * dstx) + (dsty * dsty);
      if (shortest_distance == null || distance < shortest_distance) {
        shortest_distance = distance;
        nearest_pad = this.landing_pads[i];
      }
    }
    if (nearest_pad != this.current_pad) {
      if (this.current_pad) {
        $(this.current_pad.obj).hide();
      }
      this.current_pad = nearest_pad;
      $(nearest_pad.obj).show();
    }
  },

  findDropZone: function(x, y) {
    // Go through our dropzones and see if we're over one.
    var new_dropzone = null;
    for (var i in this.dropzones) {
      if (this.dropzones[i].left < x &&
        x < this.dropzones[i].left + this.dropzones[i].width &&
        this.dropzones[i].top < y && 
        y < this.dropzones[i].top + this.dropzones[i].height) {
          new_dropzone = this.dropzones[i];
          break;
      }
    }
    // If we're over one, see if it's different.
    if (new_dropzone) {
      var changed = false;
      if (!this.current_dropzone || new_dropzone.obj.id != this.current_dropzone.obj.id) {
        this.changeDropZone(new_dropzone);
        changed = true;
      }
      this.findLandingPad(x, y);
      if (changed)  {
        // recalculate the size of our drop zones due to the fact that we're drawing landing pads.
        this.reCalculateDropZones();
      }
    }
    // If we're not over one, be sure to unhilite one if we were just
    // over it.
    else if (this.current_dropzone) {
      this.unsetDropZone();
    }
  },

  /** save button clicked **/
  savePositions: function() {
    var draggable = Drupal.Panels.Draggable;
    $(draggable.accept).each(function() {
      var val = '';
      $(this).find(draggable.draggable).each(function() {
        if (val) {
          val += ',';
        }

        val += this.id.replace(draggable.draggableId, '');
      });
      $(draggable.formId + this.id).val(val);
    });
    return false;
  }
}


Drupal.Panels.DraggableHandler = function() {
  var draggable = Drupal.Panels.Draggable;

  getMouseOffset = function(target, docPos, mousePos) {
    return { x: mousePos.x - docPos.x, y: mousePos.y - docPos.y };
  }  
  
  getMousePos = function(ev) {
    ev = ev || window.event;

    if (ev.pageX || ev.pageY) {
      return { x:ev.pageX, y:ev.pageY };
    }
    return {
      x:ev.clientX + document.body.scrollLeft - document.body.clientLeft,
      y:ev.clientY + document.body.scrollTop  - document.body.clientTop
    };
  }

  getPosition = function(e) {   
    /*
    if (document.defaultView && document.defaultView.getComputedStyle) {
      var css = document.defaultView.getComputedStyle(e, null);
      return { 
        x: parseInt(css.getPropertyValue('left')),
        y: parseInt(css.getPropertyValue('top'))
      };
    }
    */
    var left = 0;
    var top  = 0;

    while (e.offsetParent) {
      left += e.offsetLeft;
      top  += e.offsetTop;
      e     = e.offsetParent;
    }

    left += e.offsetLeft;
    top  += e.offsetTop;

    return { x:left, y:top };
  }

  /**
  * Retrieve the coordinates of the given event relative to the center
  * of the widget.
  *
  * @param event
  *  A mouse-related DOM event.
  * @param reference
  *  A DOM element whose position we want to transform the mouse coordinates to.
  * @return
  *  A hash containing keys 'x' and 'y'.
  */
  getRelativePos = function(event, reference) {
    var x, y;
    event = event || window.event;
    var el = event.target || event.srcElement;
    if (!window.opera && typeof event.offsetX != 'undefined') {
      // Use offset coordinates and find common offsetParent
      var pos = { x: event.offsetX, y: event.offsetY };
      // Send the coordinates upwards through the offsetParent chain.
      var e = el;
      while (e) {
        e.mouseX = pos.x;
        e.mouseY = pos.y;
        pos.x += e.offsetLeft;
        pos.y += e.offsetTop;
        e = e.offsetParent;
      }
      // Look for the coordinates starting from the reference element.
      var e = reference;
      var offset = { x: 0, y: 0 }
      while (e) {
        if (typeof e.mouseX != 'undefined') {
          x = e.mouseX - offset.x;
          y = e.mouseY - offset.y;
          break;
        }
        offset.x += e.offsetLeft;
        offset.y += e.offsetTop;
        e = e.offsetParent;
      }
      // Reset stored coordinates
      e = el;
      while (e) {
        e.mouseX = undefined;
        e.mouseY = undefined;
        e = e.offsetParent;
      }
    }
    else {
      // Use absolute coordinates
      var pos = this.getPosition(reference);
      x = event.pageX  - pos.x;
      y = event.pageY - pos.y;
    }
    // Subtract distance to middle
    return { x: x, y: y };
  }

  mouseUp = function(e) {
    draggable.dropzones = [];
    if (draggable.current_pad) {
      $(draggable.object).insertAfter($(draggable.current_pad.obj));
    }
    $(draggable.object).css(draggable.original);
    if (draggable.current_dropzone) {
      draggable.unsetDropZone();
    }

    $('body').unmouseup().unmousemove();
    draggable.savePositions();
  }

  mouseMove = function(e) {
    var mousePos = getMousePos(e);

    draggable.object.style.top = mousePos.y - draggable.mouseOffset.y + 'px';
    draggable.object.style.left = mousePos.x - draggable.mouseOffset.x  + 'px';
  
    draggable.findDropZone(mousePos.x, mousePos.y);
  }

  mouseDown = function(e) {
    if (e.target.nodeName == 'A' || e.target.nodeName == 'INPUT') {
      return;
    }

    draggable.calculateDropZones();
    draggable.object = $(this).parent(draggable.draggable).get(0);

    draggable.original = {
      position: $(draggable.object).css('position'),
      width: 'auto',
      left: $(draggable.object).css('left'),
      top: $(draggable.object).css('top'),
      'z-index': $(draggable.object).css('z-index'),
      'margin-bottom': $(draggable.object).css('margin-bottom')
    };

    var width = Math.min($(draggable.object).innerWidth(), draggable.maxWidth);
    $(draggable.object).css({
      position: 'relative',
      'z-index': 100,
      width: width + 'px',
      'margin-bottom': (-1 * parseInt($(draggable.object).innerHeight())) + 'px'
    })
//      .css('margin-bottom', (-1 * parseInt($(draggable.object).innerHeight())) + 'px')
      .prependTo($(draggable.main));

//    var position = getPosition(draggable.object);
    var mousePos = getMousePos(e);
    var reference = $(draggable.main).get(0);
    var windowOffset = getRelativePos(e, reference);

    // This is stored so we can move with it.
    draggable.mouseOffset = getMouseOffset(draggable.object, windowOffset, mousePos);

/*
    if (draggable.mouseOffset.x > width) {
      position.x += draggable.mouseOffset.x - (width / 2);
      draggable.mouseOffset.x = (width / 2);
    }
*/


    draggable.object.style.top = windowOffset.y + 'px';
    draggable.object.style.left = windowOffset.x + 'px';
    $('body').unmouseup().unmousemove().mouseup(mouseUp).mousemove(mouseMove);

    draggable.findDropZone(mousePos.x, mousePos.y);
    return false;
  }

  $(this).mousedown(mouseDown);
}

jQuery.fn.extend({
  panelsDraggable: Drupal.Panels.DraggableHandler
});

Drupal.Panels.attachPane = function(parent) {
  // add .panel-portlet to all draggable blocks
  if ($(parent).attr('class') == 'panel-pane') {
    $(parent).addClass('panel-portlet').each(Drupal.Panels.bindPortlet);
  }
  else {
    $('div.panel-pane').addClass('panel-portlet')
      .each(Drupal.Panels.bindPortlet);
  }

  $(parent).find('div.grabber').panelsDraggable();

  Drupal.Panels.bindClickConfigure();
  Drupal.Panels.bindClickDelete();
  Drupal.Panels.Draggable.savePositions();
}

Drupal.Panels.autoAttach = function() {
  // Show javascript only items.
  $('span#panels-js-only').css('display', 'inline');

/*
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
*/

  // Bind buttons.
  $('input.pane-add').click(Drupal.Panels.clickAdd);
  $('input#panels-hide-all').click(Drupal.Panels.clickHideAll);
  $('input#panels-show-all').click(Drupal.Panels.clickShowAll);

  Drupal.Panels.attachPane(document);
}

$(Drupal.Panels.autoAttach);