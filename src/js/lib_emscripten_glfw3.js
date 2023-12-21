// Javascript implementation called from cpp. These functions are considered
// implementation details and should NOT be used outside.
let impl = {
  $GLFW3__deps: ['$GL'],
  $GLFW3: {
    fCanvasContexts: null,
    fCurrentCanvasContext: null,
    fScaleMQL: null,
    fScaleChangeCallback: null,
    fContext: null,

    onScaleChange() {
      if(GLFW3.fScaleChangeCallback) {
        {{{ makeDynCall('vp', 'GLFW3.fScaleChangeCallback') }}}(GLFW3.fContext);
      }
    },
  },

  emscripten_glfw3_context_init__deps: ['$specialHTMLTargets'],
  emscripten_glfw3_context_init: (scale, scaleChangeCallback, context) => {
    console.log("emscripten_glfw3_context_init()");
    // For backward compatibility with emscripten, defaults to getting the canvas from Module
    specialHTMLTargets["Module['canvas']"] = Module.canvas;
    GLFW3.fCanvasContexts = {};
    GLFW3.fCurrentCanvasContext = null;

    GLFW3.fScaleChangeCallback = scaleChangeCallback;
    GLFW3.fContext = context;
    GLFW3.fScaleMQL = window.matchMedia('(resolution: ' + scale + 'dppx)');
    GLFW3.fScaleMQL.addEventListener('change', GLFW3.onScaleChange);
  },

  emscripten_glfw3_context_is_any_element_focused: () => {
    return document.activeElement !== document.body;
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
    for(let id in GLFW3.fCanvasContexts) {
      if(GLFW3.fCanvasContexts[id].canvas === canvas) {
        return {{{ cDefs.EMSCRIPTEN_RESULT_INVALID_TARGET }}};
      }
    }

    var canvasCtx = {};
    canvasCtx.id = canvasId;
    canvasCtx.selector = canvasSelector;
    canvasCtx.canvas = canvas;

    // check for css override: if there is a css rule (ex: width: 100%), then clientWidth will not match width
    canvasCtx.originalSize = { width: canvas.width, height: canvas.height};
    canvas.width = 1;
    canvas.height = 1;
    canvasCtx.hasCSSOverride = Math.floor(canvas.clientWidth) !== 1 || Math.floor(canvas.clientHeight) !== 1;

    GLFW3.fCanvasContexts[canvasCtx.id] = canvasCtx;
    return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
  },

  emscripten_glfw3_context_window_destroy: (canvasId) => {
    if(GLFW3.fCanvasContexts)
    {
      const ctx = GLFW3.fCanvasContexts[canvasId];
      const canvas = ctx.canvas;

      if(!ctx.hasCSSOverride) {
        canvas.style.removeProperty("width");
        canvas.style.removeProperty("height");
      }

      canvas.width = ctx.originalSize.width;
      canvas.height = ctx.originalSize.height;

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
    if(!ctx.hasCSSOverride) {
      canvas.style.setProperty( "width", width + "px", "important");
      canvas.style.setProperty("height", height + "px", "important");
    }
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
