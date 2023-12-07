// Javascript implementation called from cpp. These functions are considered
// implementation details and should NOT be used outside.
let impl = {
  $GLFW3__deps: ['$GL'],
  $GLFW3: {
    fCanvasContexts: null,
    fCurrentCanvasContext: null,
    fScaleMQL: null,
    fScaleChangeCallback: null,
    fScaleChangeCallbackUserData: null,

    onScaleChange() {
      if(GLFW3.fScaleChangeCallback) {
        {{{ makeDynCall('vp', 'GLFW3.fScaleChangeCallback') }}}(GLFW3.fScaleChangeCallbackUserData);
      }
    }
  },

  emscripten_glfw3_context_init: (scale, scaleChangeCallback, scaleChangeCallbackUserData) => {
    console.log("emscripten_glfw3_context_init()");
    GLFW3.fCanvasContexts = {};
    GLFW3.fCurrentCanvasContext = null;

    GLFW3.fScaleChangeCallback = scaleChangeCallback;
    GLFW3.fScaleChangeCallbackUserData = scaleChangeCallbackUserData;
    GLFW3.fScaleMQL = window.matchMedia('(resolution: ' + scale + 'dppx)');
    GLFW3.fScaleMQL.addEventListener('change', GLFW3.onScaleChange);
  },

  emscripten_glfw3_context_destroy: () => {
    console.log("emscripten_glfw3_context_destroy()");

    GLFW3.fCanvasContexts = null;
    GLFW3.fCurrentCanvasContext = null;
    GLFW3.fScaleChangeCallback = null;
    if(GLFW3.fScaleMQL) {
      GLFW3.fScaleMQL.removeEventListener('change', GLFW3.onScaleChange);
    }
    GLFW3.fScaleChangeCallbackUserData = null;
  },

  emscripten_glfw3_context_window_init: (canvasId, canvasSelector) => {
    canvasSelector = UTF8ToString(canvasSelector);
    const canvas = document.querySelector(canvasSelector);
    if(!canvas)
      return {{{ cDefs.EMSCRIPTEN_RESULT_UNKNOWN_TARGET }}};
    var canvasCtx = {};
    canvasCtx.id = canvasId;
    canvasCtx.selector = canvasSelector;
    canvasCtx.canvas = canvas;
    GLFW3.fCanvasContexts[canvasCtx.id] = canvasCtx;
    return {{{ cDefs.EMSCRIPTEN_RESULT_SUCCESS }}};
  },

  emscripten_glfw3_context_window_destroy: (canvasId) => {
    GLFW3.fCanvasContexts[canvasId] = null;
  },

  emscripten_glfw3_context_window_set_size: (canvasId, width, height, fbWidth, fbHeight) => {
    const canvas = GLFW3.fCanvasContexts[canvasId].canvas;
    if(canvas.width !== fbWidth) canvas.width = fbWidth;
    if(canvas.height !== fbHeight) canvas.height = fbHeight;
    if (fbWidth !== width || fbHeight !== height) {
      canvas.style.setProperty( "width", width + "px", "important");
      canvas.style.setProperty("height", height + "px", "important");
    } else {
      canvas.style.removeProperty( "width");
      canvas.style.removeProperty("height");
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
