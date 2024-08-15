// Javascript implementation called from cpp. These functions are considered
// implementation details and should NOT be used outside.
let emscripten_glfw3_impl = {
  $GLFW3__deps: ['$GL', '$stringToNewUTF8', 'free'],
  $GLFW3__postset: `
    // exports
    Module["requestFullscreen"] = (lockPointer, resizeCanvas) => { GLFW3.requestFullscreen(null, lockPointer, resizeCanvas); }
    Module["glfwGetWindow"] = (any) => { const ctx = GLFW3.findContext(any); return ctx ? ctx.glfwWindow : null; };
    Module["glfwGetCanvas"] = (any) => { const ctx = GLFW3.findContext(any); return ctx ? ctx.canvas : null; };
    Module["glfwGetCanvasSelector"] = (any) => { const ctx = GLFW3.findContext(any); return ctx ? ctx.selector : null; };
    Module["glfwMakeCanvasResizable"] = (any, resizableSelector, handleSelector) => { GLFW3.makeCanvasResizable(any, resizableSelector, handleSelector); };
    Module["glfwUnmakeCanvasResizable"] = (any) => { GLFW3.unmakeCanvasResizable(any); };
    Module["glfwRequestFullscreen"] = GLFW3.requestFullscreen;
    Module["glfwIsRuntimePlatformApple"] = () => { return _emscripten_glfw3_context_is_runtime_platform_apple() };
    `,
  $GLFW3: {
    fDestructors: [],
    fWindowContexts: null,
    fScaleMQL: null,
    fScaleChangeCallback: null,
    fWindowResizeCallback: null,
    fKeyboardCallback: null,
    fClipboardCallback: null,
    fRequestFullscreen: null,
    fDeferredActions: [],
    fContext: null,
    fCSSValues: null, // key is element, value is {property_name: property_value}
    fErrorCodes: {GLFW_INVALID_VALUE: 0x00010004, GLFW_PLATFORM_ERROR: 0x00010008},

    //! onError
    onError(errorCode, errorMessage) {
      if(GLFW3.fErrorHandler) {
        const ptr = stringToNewUTF8(errorMessage);
        {{{ makeDynCall('vip', 'GLFW3.fErrorHandler') }}}(GLFW3.fErrorCodes[errorCode], ptr);
        _free(ptr);
      }
    },

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

    //! onPaste
    onPaste(e) {
      e.preventDefault();
      let clipboardData = e.clipboardData || window.clipboardData;
      let pastedData = clipboardData.getData('text/plain');
      if(pastedData !== '' && GLFW3.fClipboardCallback) {
        const pastedString = stringToNewUTF8(pastedData);
        {{{ makeDynCall('vpipp', 'GLFW3.fClipboardCallback') }}}(GLFW3.fContext, 0, pastedString, null);
        _free(pastedString);
      }
    },

    //! onCutOrCopy
    onCutOrCopy(e) {
      if(GLFW3.fClipboardCallback) {
        const windowSelection = window.getSelection();
        if(windowSelection && windowSelection.toString() !== '') {
          const selection = stringToNewUTF8(windowSelection.toString());
          {{{ makeDynCall('vpipp', 'GLFW3.fClipboardCallback') }}}(GLFW3.fContext, 0, selection, null);
          _free(selection);
        } else {
          // this is to prevent the browser to beep on empty clipboard
          e.clipboardData.setData('text/plain', ' ');
          e.preventDefault();
        }
      }
    },

    // onMouseEvent
    onMouseEvent(e) {
      requestAnimationFrame(GLFW3.executeDeferredActions);
    },

    // onKeyboardEvent
    onKeyboardEvent(e) {
      if(e.type === 'keydown')
        requestAnimationFrame(GLFW3.executeDeferredActions);

      if(GLFW3.fKeyboardCallback)
      {
        const code = stringToNewUTF8(e.code);
        const key = stringToNewUTF8(e.key);
        // see https://developer.mozilla.org/en-US/docs/Web/API/UI_Events/Keyboard_event_key_values
        var codepoint = e.key.charCodeAt(0);
        if(codepoint < 0x7f && e.key.length > 1)
          // case when eventKey is something like "Tab" (eventKey.charCodeAt(0) would be "T")
          codepoint = 0;
        var modifierBits = 0;
        if(e.shiftKey) modifierBits |= 0x0001;
        if(e.ctrlKey) modifierBits |= 0x0002;
        if(e.altKey) modifierBits |= 0x0004;
        if(e.metaKey) modifierBits |= 0x0008;
        // if(e.getModifierState('CapsLock')) modifierBits |= 0x0010;
        // if(e.getModifierState('NumLock')) modifierBits |= 0x0020;
        if({{{ makeDynCall('ipippiii', 'GLFW3.fKeyboardCallback') }}}(GLFW3.fContext, e.type === 'keydown', code, key, e.repeat, codepoint, modifierBits))
          e.preventDefault();
        _free(key);
        _free(code);
      }
    },

    //! executeDeferredActions
    executeDeferredActions() {
      if(GLFW3.fDeferredActions.length > 0)
      {
        for(let action of GLFW3.fDeferredActions)
          action();
        GLFW3.fDeferredActions.length = 0;
      }
    },

    //! deferAction
    deferAction(action) {
      GLFW3.fDeferredActions.push(action);
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
        {{{ makeDynCall('ipii', 'GLFW3.fRequestFullscreen') }}}(ctx ? ctx.glfwWindow : 0, lockPointer, resizeCanvas);
      }
    },

    //! backupCSSValues
    backupCSSValues(element, ...names) {
      if(!GLFW3.fCSSValues.has(element)) {
        GLFW3.fCSSValues.set(element, {});
      }
      const properties = GLFW3.fCSSValues.get(element);
      names.forEach(name => properties[name] = element.style.getPropertyValue(name));
    },

    //! setCSSValue
    setCSSValue(element, name, value) {
      if(!GLFW3.fCSSValues.get(element)?.hasOwnProperty(name)) {
        GLFW3.backupCSSValues(element, name);
      }
      element.style.setProperty(name, value);
    },

    //! restoreCSSValues
    restoreCSSValues(element, ...names) {
      const properties = GLFW3.fCSSValues.get(element);
      if(!properties)
        return;
      if(names.length === 0)
        names = Object.keys(properties);
      names.forEach(name => {
        const value = properties[name];
        if(!value)
          element.style.removeProperty(name);
        else
          element.style.setProperty(name, value);
        delete properties[name];
      });
      if(Object.keys(properties).length === 0) {
        GLFW3.fCSSValues.delete(element);
      }
    },

    //! setCanvasResizeSelector
    setCanvasResizeSelector__deps: ['$findEventTarget'],
    setCanvasResizeSelector: (any, canvasResizeSelector) => {
      const ctx = GLFW3.findContext(any);
      if(!ctx) {
        GLFW3.onError('GLFW_INVALID_VALUE', `Cannot find canvas [${any !== null ? any.toString() : 'nullptr'}]`);
        return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};
      }

      if(ctx.fCanvasResize)
      {
        ctx.fCanvasResize.destroy();
        delete ctx.fCanvasResize;
      }

      if(canvasResizeSelector) {
        const canvasResize =  findEventTarget(canvasResizeSelector);

        if(!canvasResize) {
          GLFW3.onError('GLFW_INVALID_VALUE', `Cannot find canvas resize selector [${canvasResizeSelector}]`);
          return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};
        }

        const glfwWindow = ctx.glfwWindow;

        ctx.fCanvasResize = { target: canvasResize, destructors: [] };
        ctx.fCanvasResize.destroy = () => {
          for(let destructor of ctx.fCanvasResize.destructors)
            destructor();
        }

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
          ctx.fCanvasResize.destructors.push(() => { GLFW3.restoreCSSValues(canvasResize); });
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
        ctx.fCanvasResize.destructors.push(() => { ctx.fCanvasResize.observer.disconnect(); });
        ctx.fCanvasResize.observer.observe(canvasResize);
        const size = ctx.fCanvasResize.computeSize();
        GLFW3.onWindowResize(glfwWindow, size.width, size.height);
      }

      return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
    },

    //! makeCanvasResizable
    makeCanvasResizable__deps: ['$findEventTarget'],
    makeCanvasResizable: (any, resizableSelector, handleSelector) => {
      if(!resizableSelector) {
        GLFW3.onError('GLFW_INVALID_VALUE', `canvas resize selector is required`);
        return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};
      }

      // first we set the canvas resize selector
      const res = GLFW3.setCanvasResizeSelector(any, resizableSelector);
      if(res !== {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}})
        return res;

      // no handle, no need to continue
      if(!handleSelector)
        return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};

      const ctx = GLFW3.findContext(any);
      const resizable = findEventTarget(resizableSelector);
      const handle = findEventTarget(handleSelector);

      if(!handle) {
        GLFW3.onError('GLFW_INVALID_VALUE', `Cannot find handle element with selector [${handleSelector}]`);
        return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};
      }

      var lastDownX = 0;
      var lastDownY = 0;
      var size = undefined;

      ctx.fCanvasResize.onSizeChanged = (width, height) => {
        if (!size) { // while not resizing (otherwise it conflicts)
          GLFW3.setCSSValue(resizable, 'width', `${width}px`);
          GLFW3.setCSSValue(resizable, 'height', `${height}px`);
        }
      }

      function computeSize(element) {
        const style = getComputedStyle(element);
        return { width: parseFloat(style.width), height: parseFloat(style.height) };
      }

      // mouse down (target handle) => record size + location
      const onMouseDown = (e) => {
        e.preventDefault();
        size = computeSize(resizable);
        lastDownX = e.clientX;
        lastDownY = e.clientY;
      };

      handle.addEventListener('mousedown', onMouseDown);
      ctx.fCanvasResize.destructors.push(() => { handle.removeEventListener('mousedown', onMouseDown); });

      // mouse move (target window) => if resizing, compute new size and make resizable this size
      const onMouseMove = (e) => {
        if (!size) {
          return;
        }
        var offsetX = lastDownX - e.clientX;
        var offsetY = lastDownY - e.clientY;
        size.width -= offsetX;
        size.height -= offsetY;
        if (size.width < 0)
          size.width = 0;
        if (size.height < 0)
          size.height = 0;
        GLFW3.setCSSValue(resizable, 'width', `${size.width}px`);
        GLFW3.setCSSValue(resizable, 'height', `${size.height}px`);
        lastDownX = e.clientX;
        lastDownY = e.clientY;
      };

      window.addEventListener('mousemove', onMouseMove);
      ctx.fCanvasResize.destructors.push(() => { window.removeEventListener('mousemove', onMouseMove); });

      // mouse up (target window) => if resizing, compute canvas size and adjust resizable accordingly
      const onMouseUp = (e) => {
        if (size) {
          const canvasSize = computeSize(ctx.canvas);
          GLFW3.setCSSValue(resizable, 'width', `${canvasSize.width}px`);
          GLFW3.setCSSValue(resizable, 'height', `${canvasSize.height}px`);
          size = undefined;
        }
      };

      window.addEventListener('mouseup', onMouseUp);
      ctx.fCanvasResize.destructors.push(() => { window.removeEventListener('mouseup', onMouseUp); });

      return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
    },

    //! unmakeCanvasResizable
    unmakeCanvasResizable: (any) => {
      return GLFW3.setCanvasResizeSelector(any, null);
    }
  },

  //! emscripten_glfw3_context_init
  emscripten_glfw3_context_init__deps: ['$specialHTMLTargets'],
  emscripten_glfw3_context_init: (context, scale, scaleChangeCallback, windowResizeCallback, keyboardCallback, clipboardCallback, requestFullscreen, errorHandler) => {
    // For backward compatibility with emscripten, defaults to getting the canvas from Module
    specialHTMLTargets["Module['canvas']"] = Module.canvas;
    specialHTMLTargets["window"] = window;

    GLFW3.fWindowContexts = {};
    GLFW3.fCSSValues = new Map();
    GLFW3.fScaleChangeCallback = scaleChangeCallback;
    GLFW3.fWindowResizeCallback = windowResizeCallback;
    GLFW3.fKeyboardCallback = keyboardCallback;
    GLFW3.fClipboardCallback = clipboardCallback;
    GLFW3.fRequestFullscreen = requestFullscreen;
    GLFW3.fErrorHandler = errorHandler;
    GLFW3.fContext = context;

    // handle scale change
    GLFW3.fScaleMQL = window.matchMedia('(resolution: ' + scale + 'dppx)');
    GLFW3.fScaleMQL.addEventListener('change', GLFW3.onScaleChange);
    GLFW3.fDestructors.push(() => {
      if(GLFW3.fScaleMQL) {
        GLFW3.fScaleMQL.removeEventListener('change', GLFW3.onScaleChange);
      }
    });

    // handle mouse
    document.addEventListener('mouseup', GLFW3.onMouseEvent);
    GLFW3.fDestructors.push(() => { document.removeEventListener('mouseup', GLFW3.onMouseEvent); });
    document.addEventListener('mousedown', GLFW3.onMouseEvent);
    GLFW3.fDestructors.push(() => { document.removeEventListener('mousedown', GLFW3.onMouseEvent); });

    // handle keyboard
    document.addEventListener('keydown', GLFW3.onKeyboardEvent);
    GLFW3.fDestructors.push(() => { document.removeEventListener('keydown', GLFW3.onKeyboardEvent); });
    document.addEventListener('keyup', GLFW3.onKeyboardEvent);
    GLFW3.fDestructors.push(() => { document.removeEventListener('keyup', GLFW3.onKeyboardEvent); });

    // handle clipboard
    document.addEventListener('cut', GLFW3.onCutOrCopy);
    GLFW3.fDestructors.push(() => { document.removeEventListener('cut', GLFW3.onCutOrCopy); });
    document.addEventListener('copy', GLFW3.onCutOrCopy);
    GLFW3.fDestructors.push(() => { document.removeEventListener('copy', GLFW3.onCutOrCopy); });
    document.addEventListener('paste', GLFW3.onPaste);
    GLFW3.fDestructors.push(() => { document.removeEventListener('paste', GLFW3.onPaste); });
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

  //! emscripten_glfw3_context_get_now
  emscripten_glfw3_context_get_now: () => {
    return performance.now();
  },

  //! emscripten_glfw3_context_set_title
  emscripten_glfw3_context_set_title: (title) => {
    if(title)
      document.title = UTF8ToString(title);
  },

  //! emscripten_glfw3_context_destroy
  emscripten_glfw3_context_destroy: () => {
    GLFW3.fWindowContexts = null;
    GLFW3.fCSSValues = null;
    GLFW3.fScaleChangeCallback = null;
    GLFW3.fWindowResizeCallback = null;
    GLFW3.fKeyboardCallback = null;
    GLFW3.fClipboardCallback = null;
    GLFW3.fRequestFullscreen = null;
    for(let destructor of GLFW3.fDestructors)
      destructor();
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

    canvasCtx.restoreCSSValue = (name) => { GLFW3.restoreCSSValues(canvas, name); };
    canvasCtx.restoreCSSValues = () => { GLFW3.restoreCSSValues(canvas); };
    canvasCtx.setCSSValue = (name, value) => { GLFW3.setCSSValue(canvas, name, value); };
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

      if(!ctx) // could happen if error during init
        return;

      const canvas = ctx.canvas;

      ctx.restoreCSSValues();

      canvas.width = ctx.originalSize.width;
      canvas.height = ctx.originalSize.height;

      if(ctx.fCanvasResize)
      {
        ctx.fCanvasResize.destroy();
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

    if(ctx.fCanvasResize && ctx.fCanvasResize.onSizeChanged)
      ctx.fCanvasResize.onSizeChanged(width, height);
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

  //! emscripten_glfw3_context_make_canvas_resizable
  emscripten_glfw3_context_make_canvas_resizable: (glfwWindow, resizableSelector, handleSelector) => {
    resizableSelector = resizableSelector ? UTF8ToString(resizableSelector) : null;
    handleSelector = handleSelector ? UTF8ToString(handleSelector) : null;
    return GLFW3.makeCanvasResizable(glfwWindow, resizableSelector, handleSelector);
  },

  //! emscripten_glfw3_context_unmake_canvas_resizable
  emscripten_glfw3_context_unmake_canvas_resizable: (glfwWindow) => {
    return GLFW3.unmakeCanvasResizable(glfwWindow);
  },

  // emscripten_glfw3_context_set_clipboard_string
  emscripten_glfw3_context_set_clipboard_string: (content) => {
    content = content ? UTF8ToString(content): '';
    const errorHandler = (err) => {
      if(GLFW3.fClipboardCallback) {
        const errorString = stringToNewUTF8(`${err}`);
        {{{ makeDynCall('vpipp', 'GLFW3.fClipboardCallback') }}}(GLFW3.fContext, 2, null, errorString);
        _free(errorString);
      } else {
        GLFW3.onError('GLFW_PLATFORM_ERROR', `Cannot set clipboard string [${err}]`);
      }
    };
    if(navigator.clipboard) {
      GLFW3.deferAction(() => {
        navigator.clipboard.writeText(content)
          .then(() => {
            if(GLFW3.fClipboardCallback) {
              const string = stringToNewUTF8(content);
              {{{ makeDynCall('vpipp', 'GLFW3.fClipboardCallback') }}}(GLFW3.fContext, 2, string, null);
              _free(string);
            }
          })
          .catch(errorHandler);
      });
    } else {
      errorHandler('Missing navigator.clipboard');
    }
  },

  // emscripten_glfw3_context_open_url
  emscripten_glfw3_context_open_url: (url, target) => {
    if(url) {
      url = UTF8ToString(url);
      target = target ? UTF8ToString(target) : null;
      GLFW3.deferAction(() => {
        window.open(url, target);
      });
    }
  },

  // emscripten_glfw3_context_is_runtime_platform_apple
  emscripten_glfw3_context_is_runtime_platform_apple: () => {
    return navigator.platform.indexOf("Mac") === 0 || navigator.platform === "iPhone";
  },

}

autoAddDeps(emscripten_glfw3_impl, '$GLFW3')
mergeInto(LibraryManager.library, emscripten_glfw3_impl);
