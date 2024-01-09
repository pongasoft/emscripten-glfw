// Javascript implementation called from cpp. These functions are considered
// implementation details and should NOT be used outside.
let impl = {
  $GLFW3__deps: ['$GL'],
  $GLFW3__postset: `
    // exports
    Module["requestFullscreen"] = (lockPointer, resizeCanvas) => { GLFW3.requestFullscreen(null, lockPointer, resizeCanvas); }
    Module["glfwGetWindow"] = (canvasSelector) => { const ctx = GLFW3.findContextBySelector(canvasSelector); return ctx ? ctx.id : null; };
    Module["glfwRequestFullscreen"] = GLFW3.requestFullscreen;
    `,
  $GLFW3: {
    fCanvasContexts: null,
    fCurrentCanvasContext: null,
    fScaleMQL: null,
    fScaleChangeCallback: null,
    fRequestFullscreen: null,
    fContext: null,

    onScaleChange() {
      if(GLFW3.fScaleChangeCallback) {
        {{{ makeDynCall('vp', 'GLFW3.fScaleChangeCallback') }}}(GLFW3.fContext);
      }
    },

    findContext(canvas) {
      for(let id in GLFW3.fCanvasContexts) {
        if(GLFW3.fCanvasContexts[id].canvas === canvas) {
          return GLFW3.fCanvasContexts[id];
        }
      }
      return null;
    },

    findContextBySelector__deps: ['$findEventTarget'],
    findContextBySelector(canvasSelector) {
      return GLFW3.findContext(findEventTarget(canvasSelector));
    },

    requestFullscreen(target, lockPointer, resizeCanvas) {
      if(GLFW3.fRequestFullscreen) {
        const ctx = target ? GLFW3.findContext(findEventTarget(target)) : null;
        {{{ makeDynCall('vppii', 'GLFW3.fRequestFullscreen') }}}(GLFW3.fContext, ctx ? ctx.id : 0, lockPointer, resizeCanvas);
      }
    }
  },

  // see https://developer.mozilla.org/en-US/docs/Web/API/UI_Events/Keyboard_event_key_values
  emscripten_glfw3_context_to_codepoint: (eventKey) => {
    // TODO: the eventKey gets copied back and forth between C and javascript a few too many times IMO (try to fix)
    eventKey = UTF8ToString(eventKey);
    const codepoint = eventKey.charCodeAt(0);
    if(codepoint < 0x7f && eventKey.length > 1)
      // case when eventKey is something like "Tab" (eventKey.charCodeAt(0) would be "T")
      return 0;
    else
      return codepoint;
  },

  emscripten_glfw3_context_init__deps: ['$specialHTMLTargets'],
  emscripten_glfw3_context_init: (scale, scaleChangeCallback, requestFullscreen, context) => {
    console.log("emscripten_glfw3_context_init()");
    // For backward compatibility with emscripten, defaults to getting the canvas from Module
    specialHTMLTargets["Module['canvas']"] = Module.canvas;
    GLFW3.fCanvasContexts = {};
    GLFW3.fCurrentCanvasContext = null;

    GLFW3.fScaleChangeCallback = scaleChangeCallback;
    GLFW3.fRequestFullscreen = requestFullscreen;
    GLFW3.fContext = context;
    GLFW3.fScaleMQL = window.matchMedia('(resolution: ' + scale + 'dppx)');
    GLFW3.fScaleMQL.addEventListener('change', GLFW3.onScaleChange);
  },

  emscripten_glfw3_context_is_any_element_focused: () => {
    return document.activeElement !== document.body;
  },

  emscripten_glfw3_context_get_fullscreen_window: () => {
    const ctx = GLFW3.findContext(document.fullscreenElement);
    return ctx ? ctx.id : null;
  },

  emscripten_glfw3_context_get_pointer_lock_window: () => {
    const ctx = GLFW3.findContext(document.pointerLockElement);
    return ctx ? ctx.id : null;
  },

  emscripten_glfw3_context_destroy: () => {
    console.log("emscripten_glfw3_context_destroy()");

    GLFW3.fCanvasContexts = null;
    GLFW3.fCurrentCanvasContext = null;
    GLFW3.fScaleChangeCallback = null;
    if(GLFW3.fScaleMQL) {
      GLFW3.fScaleMQL.removeEventListener('change', GLFW3.onScaleChange);
    }
    GLFW3.fContext = null;
  },

  emscripten_glfw3_context_window_init__deps: ['$findEventTarget'],
  emscripten_glfw3_context_window_init: (canvasId, canvasSelector) => {
    canvasSelector = UTF8ToString(canvasSelector);

    const canvas =  findEventTarget(canvasSelector);

    if(!canvas)
      return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};

    // check for duplicate
    if(GLFW3.findContext(canvas)) {
      return {{{ cDefs.EMSCRIPTEN_RESULT_INVALID_TARGET }}};
    }

    var canvasCtx = {};
    canvasCtx.id = canvasId;
    canvasCtx.selector = canvasSelector;
    canvasCtx.canvas = canvas;
    canvasCtx.originalSize = { width: canvas.width, height: canvas.height};

    canvasCtx.originalCSS = {};
    ["width", "height", "opacity", "cursor", "display"].forEach((name) => {
      canvasCtx.originalCSS[name] = canvas.style.getPropertyValue(name);
    });
    canvasCtx.restoreCSSValue = (name) => {
      const value = canvasCtx.originalCSS[name];
      if(!value)
        canvas.style.removeProperty(name);
      else
        canvas.style.setProperty(name, value);
    };
    canvasCtx.restoreCSSValues = () => {
      Object.entries(canvasCtx.originalCSS).forEach(([name, value]) => {
        if(!value)
          canvas.style.removeProperty(name);
        else
          canvas.style.setProperty(name, value);
      });
    };
    canvasCtx.setCSSValue = (name, value) => {
      canvas.style.setProperty(name, value);
    };
    canvasCtx.getComputedCSSValue = (name) => {
      return window.getComputedStyle(canvas).getPropertyValue(name);
    };

    GLFW3.fCanvasContexts[canvasCtx.id] = canvasCtx;
    return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
  },

  emscripten_glfw3_context_window_destroy: (canvasId) => {
    if(GLFW3.fCanvasContexts)
    {
      const ctx = GLFW3.fCanvasContexts[canvasId];
      const canvas = ctx.canvas;

      ctx.restoreCSSValues();

      canvas.width = ctx.originalSize.width;
      canvas.height = ctx.originalSize.height;

      if(ctx.fResizeObserver)
      {
        ctx.fResizeObserver.disconnect();
        delete ctx.fResizeObserver;
      }

      delete GLFW3.fCanvasContexts[canvasId];
    }
  },

  emscripten_glfw3_context_window_focus: (canvasId) => {
    const canvas = GLFW3.fCanvasContexts[canvasId].canvas;
    canvas.focus();
  },

  emscripten_glfw3_context_window_set_size: (canvasId, width, height, fbWidth, fbHeight) => {
    const ctx = GLFW3.fCanvasContexts[canvasId];
    const canvas = ctx.canvas;

    if(canvas.width !== fbWidth) canvas.width = fbWidth;
    if(canvas.height !== fbHeight) canvas.height = fbHeight;

    // this will (on purpose) override any css setting
    ctx.setCSSValue("width",   width + "px", "important");
    ctx.setCSSValue("height", height + "px", "important");
  },

  emscripten_glfw3_context_window_get_resize: (canvasId, width, height) => {
    const ctx = GLFW3.fCanvasContexts[canvasId];

    if(!ctx.fCanvasResize)
      return;

    if(ctx.fCanvasResize === window) {
      {{{ makeSetValue('width', '0', 'window.innerWidth', 'double') }}};
      {{{ makeSetValue('height', '0', 'window.innerHeight', 'double') }}};
    } else {
      const target = ctx.fCanvasResize;
      const style = getComputedStyle(target);
      const targetWidth = target.clientWidth - parseFloat(style.paddingLeft) - parseFloat(style.paddingRight);
      const targetHeight = target.clientHeight - parseFloat(style.paddingTop) - parseFloat(style.paddingBottom);
      {{{ makeSetValue('width', '0', 'targetWidth', 'double') }}};
      {{{ makeSetValue('height', '0', 'targetHeight', 'double') }}};
    }
  },

  emscripten_glfw3_context_window_set_resize: (canvasId, width, height) => {
    const ctx = GLFW3.fCanvasContexts[canvasId];

    // can't resize window
    if(!ctx.fCanvasResize || ctx.fCanvasResize === window)
      return {{{ cDefs.EMSCRIPTEN_RESULT_INVALID_PARAM }}};

    const target = ctx.fCanvasResize;
    target.style.setProperty('width', width + 'px');
    target.style.setProperty('height', height + 'px');

    return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
  },

  emscripten_glfw3_context_window_set_cursor: (canvasId, cursor) => {
    const ctx = GLFW3.fCanvasContexts[canvasId];
    if(cursor)
      ctx.setCSSValue("cursor", UTF8ToString(cursor));
    else
      ctx.restoreCSSValue("cursor");
  },

  emscripten_glfw3_context_window_get_computed_opacity: (canvasId) => {
    return GLFW3.fCanvasContexts[canvasId].getComputedCSSValue("opacity");
  },

  emscripten_glfw3_context_window_set_opacity: (canvasId, opacity) => {
    const ctx = GLFW3.fCanvasContexts[canvasId];
    ctx.setCSSValue("opacity", opacity);
  },

  emscripten_glfw3_context_window_get_computed_visibility: (canvasId) => {
    return GLFW3.fCanvasContexts[canvasId].getComputedCSSValue("display") !== "none";
  },

  emscripten_glfw3_context_window_set_visibility: (canvasId, visible) => {
    const ctx = GLFW3.fCanvasContexts[canvasId];
    if(!visible)
      ctx.setCSSValue("display", "none");
    else
      ctx.restoreCSSValue("display");
  },

  emscripten_glfw3_context_window_set_resize_callback__deps: ['$findEventTarget'],
  emscripten_glfw3_context_window_set_resize_callback: (canvasId, canvasResizeSelector, resizeCallback, resizeCallbackUserData) => {
    const ctx = GLFW3.fCanvasContexts[canvasId];

    if(ctx.fResizeObserver)
    {
      ctx.fResizeObserver.disconnect();
      delete ctx.fCanvasResize;
      delete ctx.fResizeCallback;
      delete ctx.fResizeObserver;
    }

    if(canvasResizeSelector) {
      canvasResizeSelector = UTF8ToString(canvasResizeSelector);

      const canvasResize =  canvasResizeSelector === "window" ? window : findEventTarget(canvasResizeSelector);

      if(!canvasResize)
        return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};

      ctx.fCanvasResize = canvasResize;
      ctx.fResizeCallback = resizeCallback;

      if(canvasResize === window) {
        const listener = (e) => {
          {{{ makeDynCall('vp', 'ctx.fResizeCallback') }}}(resizeCallbackUserData);
        };
        ctx.fResizeObserver = {
          observe: (elt) => { window.addEventListener('resize', listener); },
          disconnect: () => { window.removeEventListener('resize', listener) }
        }
      } else {
        ctx.fResizeObserver = new ResizeObserver((entries) => {
          const ctx = GLFW3.fCanvasContexts[canvasId];
          if(ctx.fResizeCallback) {
            for(const entry of entries) {
              if(entry.target === canvasResize) {
                {{{ makeDynCall('vp', 'ctx.fResizeCallback') }}}(resizeCallbackUserData);
              }
            }
          }
        });
      }
      ctx.fResizeObserver.observe(canvasResize);
    }

    return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
  },

  emscripten_glfw3_context_gl_init: (canvasId) => {
    const canvasCtx = GLFW3.fCanvasContexts[canvasId];
    if(!canvasCtx)
      return;
    canvasCtx.glAttributes = {};
  },

  emscripten_glfw3_context_gl_bool_attribute: (canvasId, attributeName, attributeValue) => {
    const canvasCtx = GLFW3.fCanvasContexts[canvasId];
    if(!canvasCtx)
      return;
    canvasCtx.glAttributes[UTF8ToString(attributeName)] = !!attributeValue;
  },

  emscripten_glfw3_context_gl_create_context: (canvasId) => {
    const canvasCtx = GLFW3.fCanvasContexts[canvasId];
    if(!canvasCtx)
      return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};
    const contextHandle = GL.createContext(canvasCtx.canvas, canvasCtx.glAttributes);
    if(contextHandle) {
      canvasCtx.glContextHandle = contextHandle;
      return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
    } else {
      return {{{ cDefs.EMSCRIPTEN_RESULT_FAILED }}};
    }
  },

  emscripten_glfw3_context_gl_make_context_current: (canvasId) => {
    const canvasCtx = GLFW3.fCanvasContexts[canvasId];
    if(!canvasCtx)
      return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};
    if(!canvasCtx.glContextHandle)
      return {{{ cDefs.EMSCRIPTEN_RESULT_FAILED }}};
    if(GL.makeContextCurrent(canvasCtx.glContextHandle))
      return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
    else
      return {{{ cDefs.EMSCRIPTEN_RESULT_FAILED }}};
  },

}

autoAddDeps(impl, '$GLFW3')
mergeInto(LibraryManager.library, impl);
