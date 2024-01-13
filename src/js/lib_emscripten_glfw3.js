// Javascript implementation called from cpp. These functions are considered
// implementation details and should NOT be used outside.
let impl = {
  $GLFW3__deps: ['$GL'],
  $GLFW3__postset: `
    // exports
    Module["requestFullscreen"] = (lockPointer, resizeCanvas) => { GLFW3.requestFullscreen(null, lockPointer, resizeCanvas); }
    Module["glfwGetWindow"] = (any) => { const ctx = GLFW3.findContext(any); return ctx ? ctx.glfwWindow : null; };
    Module["glfwGetCanvasSelector"] = (any) => { const ctx = GLFW3.findContext(any); return ctx ? ctx.selector : null; };
    Module["glfwSetCanvasResizableSelector"] = GLFW3.glfwSetCanvasResizableSelector;
    Module["glfwRequestFullscreen"] = GLFW3.requestFullscreen;
    `,
  $GLFW3: {
    fWindowContexts: null,
    fScaleMQL: null,
    fScaleChangeCallback: null,
    fWindowResizeCallback: null,
    fRequestFullscreen: null,
    fContext: null,

    //! onScaleChange
    onScaleChange() {
      if(GLFW3.fScaleChangeCallback) {
        {{{ makeDynCall('vp', 'GLFW3.fScaleChangeCallback') }}}(GLFW3.fContext);
      }
    },

    //! onWindowResize
    onWindowResize(glfwWindow, width, height) {
      if(GLFW3.fWindowResizeCallback) {
        {{{ makeDynCall('vppii', 'GLFW3.fWindowResizeCallback') }}}(GLFW3.fContext, glfwWindow, width, height);
      }
    },

    //! findContextByCanvas
    findContextByCanvas(canvas) {
      for(let w in GLFW3.fWindowContexts) {
        if(GLFW3.fWindowContexts[w].canvas === canvas) {
          return GLFW3.fWindowContexts[w];
        }
      }
      return null;
    },

    //! findContextBySelector
    findContextBySelector__deps: ['$findEventTarget'],
    findContextBySelector(canvasSelector) {
      return GLFW3.findContextByCanvas(findEventTarget(canvasSelector));
    },

    //! findContext
    findContext(any) {
      if(!any)
        return null;

      // is any a glfwWindow?
      if(GLFW3.fWindowContexts[any])
        return GLFW3.fWindowContexts[any];

      // is any a canvas?
      if(any instanceof HTMLCanvasElement)
        return GLFW3.findContextByCanvas(canvas);

      // is any a selector?
      return GLFW3.findContextBySelector(any);
    },

    //! requestFullscreen
    requestFullscreen(target, lockPointer, resizeCanvas) {
      if(GLFW3.fRequestFullscreen) {
        const ctx = GLFW3.findContext(target);
        {{{ makeDynCall('vppii', 'GLFW3.fRequestFullscreen') }}}(GLFW3.fContext, ctx ? ctx.glfwWindow : 0, lockPointer, resizeCanvas);
      }
    },

    //! setCanvasResizableSelector
    setCanvasResizableSelector__deps: ['$findEventTarget'],
    setCanvasResizableSelector: (any, canvasResizeSelector) => {
      const ctx = GLFW3.findContext(any);
      if(!ctx)
        return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};

      if(ctx.fCanvasResize)
      {
        ctx.fCanvasResize.observer.disconnect();
        delete ctx.fCanvasResize;
      }

      if(canvasResizeSelector) {
        const canvasResize =  findEventTarget(canvasResizeSelector);

        if(!canvasResize)
          return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};

        const glfwWindow = ctx.glfwWindow;

        ctx.fCanvasResize = { target: canvasResize };

        if(canvasResize === window) {
          ctx.fCanvasResize.computeSize = () => { return { width: window.innerWidth, height: window.innerHeight }; };
          const listener = (e) => {
            const size = ctx.fCanvasResize.computeSize();
            GLFW3.onWindowResize(glfwWindow, size.width, size.height);
          };
          ctx.fCanvasResize.observer = {
            observe: (elt) => { window.addEventListener('resize', listener); },
            disconnect: () => { window.removeEventListener('resize', listener) }
          }
        } else {
          ctx.fCanvasResize.computeSize = () => {
            const style = getComputedStyle(canvasResize);
            return {width: parseInt(style.width, 10), height: parseInt(style.height, 10)}
          };
          ctx.fCanvasResize.observer = new ResizeObserver((entries) => {
            const ctx = GLFW3.fWindowContexts[glfwWindow];
            if(ctx.fCanvasResize) {
              for(const entry of entries) {
                if(entry.target === canvasResize) {
                  const size = ctx.fCanvasResize.computeSize();
                  GLFW3.onWindowResize(glfwWindow, size.width, size.height);
                }
              }
            }
          });
        }
        ctx.fCanvasResize.observer.observe(canvasResize);
        const size = ctx.fCanvasResize.computeSize();
        GLFW3.onWindowResize(glfwWindow, size.width, size.height);
      }

      return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
    },

    //! glfwSetCanvasResizableSelector
    glfwSetCanvasResizableSelector: (any, canvasResizeSelector) => {
      return GLFW3.setCanvasResizableSelector(any, canvasResizeSelector) === {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
    }
  },

  //! emscripten_glfw3_context_to_codepoint
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

  //! emscripten_glfw3_context_init
  emscripten_glfw3_context_init__deps: ['$specialHTMLTargets'],
  emscripten_glfw3_context_init: (scale, scaleChangeCallback, windowResizeCallback, requestFullscreen, context) => {
    console.log("emscripten_glfw3_context_init()");
    // For backward compatibility with emscripten, defaults to getting the canvas from Module
    specialHTMLTargets["Module['canvas']"] = Module.canvas;
    specialHTMLTargets["window"] = window;
    GLFW3.fWindowContexts = {};

    GLFW3.fScaleChangeCallback = scaleChangeCallback;
    GLFW3.fWindowResizeCallback = windowResizeCallback;
    GLFW3.fRequestFullscreen = requestFullscreen;
    GLFW3.fContext = context;
    GLFW3.fScaleMQL = window.matchMedia('(resolution: ' + scale + 'dppx)');
    GLFW3.fScaleMQL.addEventListener('change', GLFW3.onScaleChange);
  },

  //! emscripten_glfw3_context_is_any_element_focused
  emscripten_glfw3_context_is_any_element_focused: () => {
    return document.activeElement !== document.body;
  },

  //! emscripten_glfw3_context_get_fullscreen_window
  emscripten_glfw3_context_get_fullscreen_window: () => {
    const ctx = GLFW3.findContextByCanvas(document.fullscreenElement);
    return ctx ? ctx.glfwWindow : null;
  },

  //! emscripten_glfw3_context_get_pointer_lock_window
  emscripten_glfw3_context_get_pointer_lock_window: () => {
    const ctx = GLFW3.findContextByCanvas(document.pointerLockElement);
    return ctx ? ctx.glfwWindow : null;
  },

  //! emscripten_glfw3_context_is_extension_supported (copied from library_glfw.js)
  emscripten_glfw3_context_is_extension_supported: (extension) => {
    extension = UTF8ToString(extension);
    if(!GLFW3.fGLExtensions)
      GLFW3.fGLExtensions = GL.getExtensions();

    if(GLFW3.fGLExtensions) {
      if(GLFW3.fGLExtensions.includes(extension))
        return true;
      return (GLFW3.fGLExtensions.includes("GL_" + extension));
    }
    return false;
  },

  //! emscripten_glfw3_context_set_title
  emscripten_glfw3_context_set_title: (title) => {
    if(title)
      document.title = UTF8ToString(title);
  },

  //! emscripten_glfw3_context_destroy
  emscripten_glfw3_context_destroy: () => {
    console.log("emscripten_glfw3_context_destroy()");

    GLFW3.fWindowContexts = null;
    GLFW3.fScaleChangeCallback = null;
    if(GLFW3.fScaleMQL) {
      GLFW3.fScaleMQL.removeEventListener('change', GLFW3.onScaleChange);
    }
    GLFW3.fContext = null;
  },

  //! emscripten_glfw3_window_init
  emscripten_glfw3_window_init__deps: ['$findEventTarget'],
  emscripten_glfw3_window_init: (glfwWindow, canvasSelector) => {
    canvasSelector = UTF8ToString(canvasSelector);

    const canvas =  findEventTarget(canvasSelector);

    if(!canvas)
      return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};

    // check for duplicate
    if(GLFW3.findContextByCanvas(canvas)) {
      return {{{ cDefs.EMSCRIPTEN_RESULT_INVALID_TARGET }}};
    }

    var canvasCtx = {};
    canvasCtx.glfwWindow = glfwWindow;
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

    GLFW3.fWindowContexts[canvasCtx.glfwWindow] = canvasCtx;
    return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
  },

  //! emscripten_glfw3_window_on_created
  emscripten_glfw3_window_on_created: (glfwWindow) => {
    if(Module.glfwOnWindowCreated) {
      Module.glfwOnWindowCreated(glfwWindow, GLFW3.fWindowContexts[glfwWindow].selector);
    }
  },

  //! emscripten_glfw3_window_destroy
  emscripten_glfw3_window_destroy: (glfwWindow) => {
    if(GLFW3.fWindowContexts)
    {
      const ctx = GLFW3.fWindowContexts[glfwWindow];
      const canvas = ctx.canvas;

      ctx.restoreCSSValues();

      canvas.width = ctx.originalSize.width;
      canvas.height = ctx.originalSize.height;

      if(ctx.fCanvasResize)
      {
        ctx.fCanvasResize.observer.disconnect();
        delete ctx.fCanvasResize;
      }

      delete GLFW3.fWindowContexts[glfwWindow];
    }
  },

  //! emscripten_glfw3_window_focus
  emscripten_glfw3_window_focus: (glfwWindow) => {
    const canvas = GLFW3.fWindowContexts[glfwWindow].canvas;
    canvas.focus();
  },

  //! emscripten_glfw3_window_set_size
  emscripten_glfw3_window_set_size: (glfwWindow, width, height, fbWidth, fbHeight) => {
    const ctx = GLFW3.fWindowContexts[glfwWindow];
    const canvas = ctx.canvas;

    if(canvas.width !== fbWidth) canvas.width = fbWidth;
    if(canvas.height !== fbHeight) canvas.height = fbHeight;

    // this will (on purpose) override any css setting
    ctx.setCSSValue("width",   width + "px", "important");
    ctx.setCSSValue("height", height + "px", "important");
  },

  //! emscripten_glfw3_window_get_position
  emscripten_glfw3_window_get_position: (glfwWindow, x, y) => {
    const canvas = GLFW3.fWindowContexts[glfwWindow].canvas;
    const rect = getBoundingClientRect(canvas);
    {{{ makeSetValue('x', '0', 'rect.x', 'i32') }}};
    {{{ makeSetValue('y', '0', 'rect.y', 'i32') }}};
  },

  //! emscripten_glfw3_window_set_cursor
  emscripten_glfw3_window_set_cursor: (glfwWindow, cursor) => {
    const ctx = GLFW3.fWindowContexts[glfwWindow];
    if(cursor)
      ctx.setCSSValue("cursor", UTF8ToString(cursor));
    else
      ctx.restoreCSSValue("cursor");
  },

  //! emscripten_glfw3_window_get_computed_opacity
  emscripten_glfw3_window_get_computed_opacity: (glfwWindow) => {
    return GLFW3.fWindowContexts[glfwWindow].getComputedCSSValue("opacity");
  },

  //! emscripten_glfw3_window_set_opacity
  emscripten_glfw3_window_set_opacity: (glfwWindow, opacity) => {
    const ctx = GLFW3.fWindowContexts[glfwWindow];
    ctx.setCSSValue("opacity", opacity);
  },

  //! emscripten_glfw3_window_get_computed_visibility
  emscripten_glfw3_window_get_computed_visibility: (glfwWindow) => {
    return GLFW3.fWindowContexts[glfwWindow].getComputedCSSValue("display") !== "none";
  },

  //! emscripten_glfw3_window_set_visibility
  emscripten_glfw3_window_set_visibility: (glfwWindow, visible) => {
    const ctx = GLFW3.fWindowContexts[glfwWindow];
    if(!visible)
      ctx.setCSSValue("display", "none");
    else
      ctx.restoreCSSValue("display");
  },

  //! emscripten_glfw3_window_set_canvas_resizable_selector
  emscripten_glfw3_window_set_canvas_resizable_selector: (glfwWindow, canvasResizeSelector) => {
    return GLFW3.setCanvasResizableSelector(glfwWindow, UTF8ToString(canvasResizeSelector));
  },

  //! emscripten_glfw3_context_gl_init
  emscripten_glfw3_context_gl_init: (glfwWindow) => {
    const canvasCtx = GLFW3.fWindowContexts[glfwWindow];
    if(!canvasCtx)
      return;
    canvasCtx.glAttributes = {};
  },

  //! emscripten_glfw3_context_gl_bool_attribute
  emscripten_glfw3_context_gl_bool_attribute: (glfwWindow, attributeName, attributeValue) => {
    const canvasCtx = GLFW3.fWindowContexts[glfwWindow];
    if(!canvasCtx)
      return;
    canvasCtx.glAttributes[UTF8ToString(attributeName)] = !!attributeValue;
  },

  //! emscripten_glfw3_context_gl_create_context
  emscripten_glfw3_context_gl_create_context: (glfwWindow) => {
    const canvasCtx = GLFW3.fWindowContexts[glfwWindow];
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

  //! emscripten_glfw3_context_gl_make_context_current
  emscripten_glfw3_context_gl_make_context_current: (glfwWindow) => {
    const canvasCtx = GLFW3.fWindowContexts[glfwWindow];
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
