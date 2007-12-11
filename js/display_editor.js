// $Id: display_editor.js,v 1.1.2.29 2007/12/11 21:02:35 merlinofchaos Exp $
/**
 * @file display_editor.js 
 *
 * Contains the javascript for the Panels display editor.
 */

/** add content button **/
Drupal.Panels.clickAdd = function() {
  var id = $(this)[0].id.replace('pane-add-', '');
  // show the empty dialog right away.
  Drupal.Panels.Subform.show();
  $.ajax({
    type: "POST",
    url: Drupal.settings.panelsAjaxURL + "/add-content/" + $('#panel-did').val() + '/' + id,
    data: '',
    global: true,
    success: Drupal.Panels.Subform.bindAjaxResponse,
    error: function() { alert("An error occurred."); Drupal.Panels.Subform.dismiss(); },
    dataType: 'json'
  });
  return false;
}

/** hide all button **/
Drupal.Panels.clickHideAll = function() {
  $('div.panel-portlet div.content').hide(200);
  $('div.panel-portlet div.toggle').addClass('toggle-collapsed');
  return false;
}

/** show all button **/
Drupal.Panels.clickShowAll = function() {
  $('div.panel-portlet div.content').show(200);
  $('div.panel-portlet div.toggle').removeClass('toggle-collapsed');
  return false;
}

/** Configure cache button */
Drupal.Panels.clickCacheSettings = function () {
  // show the empty dialog right away.
  Drupal.Panels.Subform.show();

  $.ajax({
    type: "POST",
    url: Drupal.settings.panelsAjaxURL + "/cache/" + $('#panel-did').val(),
    data: '',
    global: true,
    success: Drupal.Panels.Subform.bindAjaxResponse,
    error: function() { alert("An error occurred."); Drupal.Panels.Subform.dismiss(); },
    dataType: 'json'
  });
  return false;
}

/** Configure pane button */
Drupal.Panels.bindClickConfigure = function (o) {
  $('input.pane-configure').unbind('click');
  $('input.pane-configure').click(function() {
    var id = $(this)[0].id.replace('edit-button-', '').replace('-configure', '');
    // show the empty dialog right away.
    Drupal.Panels.Subform.show();
  
    $.ajax({
      type: "POST",
      url: Drupal.settings.panelsAjaxURL + "/configure/" + $('#panel-did').val() + '/' + id,
      data: '',
      global: true,
      success: Drupal.Panels.Subform.bindAjaxResponse,
      error: function() { alert("An error occurred."); Drupal.Panels.Subform.dismiss(); },
      dataType: 'json'
    });
    return false;
  });
}

/** Configure cache button */
Drupal.Panels.bindClickCache = function (o) {
  $('input.pane-cache').unbind('click');
  $('input.pane-cache').click(function() {
    var id = $(this)[0].id.replace('edit-button-', '').replace('-cache', '');
    // show the empty dialog right away.
    Drupal.Panels.Subform.show();
    
    $.ajax({
      type: "POST",
      url: Drupal.settings.panelsAjaxURL + "/cache/" + $('#panel-did').val() + '/' + id,
      data: '',
      global: true,
      success: Drupal.Panels.Subform.bindAjaxResponse,
      error: function() { alert("An error occurred."); Drupal.Panels.Subform.dismiss(); },
      dataType: 'json'
    });
    return false;
  });
}

/** Delete pane button **/
Drupal.Panels.bindClickDelete = function(o) {
  $('input.pane-delete').unbind('click');
  $('input.pane-delete').click(function() {
    if (confirm('Remove this pane?')) {
      var id = $(this)[0].id.replace('edit-button-', '').replace('-delete', '');
      $('#panel-pane-' + id).remove();
      Drupal.Panels.Draggable.savePositions();
    }
    return false;
  });
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

Drupal.Panels.changed = function(item) {
  if (!item.is('.changed')) {
    item.addClass('changed');
    item.children('div.grabber span.text').append(' <span class="star">*</span> ');
  }
};

Drupal.Panels.Draggable = {
  // The draggable object
  object: null,

  // Where objects can be dropped
  dropzones: [],
  current_dropzone: null,

  // positions within dropzones where an object can be plazed
  landing_pads: [],
  current_pad: null,

  // Where the object is
  mouseOffset: { x: 0, y: 0 },
  windowOffset: { x: 0, y: 0 },
  offsetDivHeight: 0,

  // original settings to be restored
  original: {},
  // a placeholder so that if the object is let go but not over a drop zone,
  // it can be put back where it belongs
  placeholder: {},

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

  calculateDropZones: function(event, dropzone) {
    var dropzones = [];
    $(this.accept).each(function() {
      var offset = $(this).offset({padding:true});
      offset.obj = this;
      offset.width = $(this).outerWidth();
      offset.height = $(this).outerHeight();
      dropzones.push(offset);
    });
    this.dropzones = dropzones;
  },

  reCalculateDropZones: function() {
    for (var i in this.dropzones) {
      offset = $(this.dropzones[i].obj).offset({padding:true});
      offset.width = $(this.dropzones[i].obj).outerWidth();
      offset.height = $(this.dropzones[i].obj).outerHeight();
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
//      console.log('x:' + x + ' left:' + this.dropzones[i].left + ' right: ' + this.dropzones[i].left + this.dropzones[i].width);
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
      // Note: _ is replaced with because Drupal automatically does this
      // with form ids.
      $(draggable.formId + this.id.replace(/_/g, '-')).val(val);
    });
    return false;
  }
}


Drupal.Panels.DraggableHandler = function() {
  var draggable = Drupal.Panels.Draggable;

  getMouseOffset = function(docPos, mousePos, windowPos) {
    return { x: mousePos.x - docPos.x + windowPos.x, y: mousePos.y - docPos.y + windowPos.y};
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

  mouseUp = function(e) {
    draggable.dropzones = [];

    if (draggable.current_pad) {
      // Drop the object where we're hovering
      $(draggable.object).insertAfter($(draggable.current_pad.obj));
      Drupal.Panels.changed($(draggable.object));
    }
    else {
      // or put it back where it came from
      $(draggable.object).insertAfter(draggable.placeholder);
    }
    // remove the placeholder
    draggable.placeholder.remove();
   
    // restore original settings.
    $(draggable.object).css(draggable.original);
    if (draggable.current_dropzone) {
      draggable.unsetDropZone();
    }

    $(document).unbind('mouseup').unbind('mousemove');
    draggable.savePositions();
  }

  mouseMove = function(e) {
    var mousePos = getMousePos(e);
    draggable.findDropZone(mousePos.x, mousePos.y);

    var windowMoved = parseInt(draggable.offsetDivHeight - $(draggable.main).innerHeight());

    draggable.object.style.top = mousePos.y - draggable.mouseOffset.y + windowMoved + 'px';
    draggable.object.style.left = mousePos.x - draggable.mouseOffset.x  + 'px';
    $(draggable.object).toggleClass('moving');
  }

  mouseDown = function(e) {
    if (e.target.nodeName == 'A' || e.target.nodeName == 'INPUT') {
      return;
    }

    draggable.object = $(this).parent(draggable.draggable).get(0);

    // create a placeholder so we can put this object back if dropped in an invalid location.
    draggable.placeholder = $('<div class="draggable-placeholder-object" style="display:none"></div>"');
    $(draggable.object).after(draggable.placeholder);

    // Store original CSS so we can put it back.
    draggable.original = {
      position: $(draggable.object).css('position'),
      width: 'auto',
      left: $(draggable.object).css('left'),
      top: $(draggable.object).css('top'),
      'z-index': $(draggable.object).css('z-index'),
      'margin-bottom': $(draggable.object).css('margin-bottom'),
      'margin-top': $(draggable.object).css('margin-top'),
      'margin-left': $(draggable.object).css('margin-left'),
      'margin-right': $(draggable.object).css('margin-right'),
      'padding-bottom': $(draggable.object).css('padding-bottom'),
      'padding-top': $(draggable.object).css('padding-top'),
      'padding-left': $(draggable.object).css('padding-left'),
      'padding-right': $(draggable.object).css('padding-right')
    };

    var mousePos = getMousePos(e);
    var originalPos = $(draggable.object).offset();
    var width = Math.min($(draggable.object).innerWidth(), draggable.maxWidth);

    draggable.offsetDivHeight = $(draggable.main).innerHeight();
    draggable.findDropZone(mousePos.x, mousePos.y);

    // Make the draggable relative, get it out of the way and make it
    // invisible.
    $(draggable.object).css({
      position: 'relative',
      'z-index': 100,
      width: width + 'px',
      'margin-bottom': (-1 * parseInt($(draggable.object).outerHeight())) + 'px',
      'margin-top': 0,
      'margin-left': 0,
      'margin-right': (-1 * parseInt($(draggable.object).outerWidth())) + 'px',
      'padding-bottom': 0,
      'padding-top': 0,
      'padding-left': 0,
      'padding-right': 0
    })
      .insertAfter($(draggable.main));
    var newPos = $(draggable.object).offset();

    var windowOffset = { left: originalPos.left - newPos.left, top: originalPos.top - newPos.top }
    
    // if they grabbed outside the area where we make the draggable smaller, move it
    // closer to the cursor.
    if (e.layerX != 'undefined' && e.layerX > width) {
      windowOffset.left += e.layerX - 10;
    }
    else if (e.layerX != 'undefined' && e.offsetX > width) {
      windowOffset.left += e.offsetX - 10;
    }

    // This is stored so we can move with it.
    draggable.mouseOffset = { x: mousePos.x - windowOffset.left, y: mousePos.y - windowOffset.top};
    draggable.offsetDivHeight = $(draggable.main).innerHeight();

    draggable.object.style.top = windowOffset.top + 'px';
    draggable.object.style.left = windowOffset.left + 'px';
    $(document).unbind('mouseup').unbind('mousemove').mouseup(mouseUp).mousemove(mouseMove);

    draggable.calculateDropZones(mousePos, e);
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

  Drupal.Panels.bindClickCache();
  Drupal.Panels.bindClickConfigure();
  Drupal.Panels.bindClickDelete();
  Drupal.Panels.Draggable.savePositions();
}

Drupal.Panels.autoAttach = function() {
  // Show javascript only items.
  $('span#panels-js-only').css('display', 'inline');

  // Bind buttons.
  $('input.pane-add').click(Drupal.Panels.clickAdd);
  $('input#panels-hide-all').click(Drupal.Panels.clickHideAll);
  $('input#panels-show-all').click(Drupal.Panels.clickShowAll);
  $('input#panels-cache-settings').click(Drupal.Panels.clickCacheSettings);

  Drupal.Panels.attachPane(document);
}

$(Drupal.Panels.autoAttach);
