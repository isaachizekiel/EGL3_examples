#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include "egl_utils.h"


static EGLint get_context_renderable_type(EGLDisplay egl_display) {
#ifdef EGL_KHR_create_context
  const char* extension = eglQueryString(egl_display, EGL_EXTENSIONS);
  if (extension != NULL && strstr(extensions, "EGL_KHR_create_context")) {
    return EGL_OPENGL_ES3_BIT_KHR;
  }
#endif  
  return EGL_OPENGL_ES2_BIT;
}


// init native window
static void initialize_native_window() {
#ifdef ANDROID
  // if android we don't need to create window manually
#else
#if 0
  if (!WinCreate (context, title)) {
    return GL_FALSE;
  }
#endif
#endif
}

// initialize display
static EGLBoolean initialize_egl_display(struct egl_context * context) {
  EGLint major_version;
  EGLint minor_version;

  // check if we already have a display
  if (context->egl_display != EGL_NO_DISPLAY) return EGL_TRUE;
  
  context->egl_display = eglGetDisplay(context->egl_native_display);
  if (context->egl_display == EGL_NO_DISPLAY) {
    return EGL_FALSE;
  }

  // initialize egl;
  if (!eglInitialize(context->egl_display, &major_version, &minor_version)) {
    return EGL_FALSE;
  }

  return EGL_TRUE;    
}

// initialize surface
static EGLBoolean initialize_egl_surface(struct egl_context * context, GLuint flags) {  
  EGLint num_configs = 0;
  
  // check if we already have a surface
  if (context->egl_surface != EGL_NO_SURFACE) return EGL_TRUE;
  
  EGLint attrib_list[] = {
    EGL_RED_SIZE, 5,
    EGL_GREEN_SIZE, 6,
    EGL_BLUE_SIZE, 5,
    EGL_ALPHA_SIZE, (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
    EGL_DEPTH_SIZE, (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
    EGL_STENCIL_SIZE, (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
    EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
    EGL_RENDERABLE_TYPE, get_context_renderable_type(context->egl_display),
    EGL_NONE
  };

  // choose config
  if (!eglChooseConfig(context->egl_display, attrib_list, &(context->egl_config), 1, &num_configs)) {
    switch (eglGetError()) {
    case EGL_BAD_DISPLAY:
      // is generated if display is not an EGL display connection.
      LOGE("EGL_BAD_DISPLAY");
      return GL_FALSE;    
    case EGL_BAD_ATTRIBUTE:
      // is generated if attribute_list contains an invalid frame buffer configuration attribute or
      // an attribute value that is unrecognized or out of range.
      LOGE("EGL_BAD_ATTRIBUTE");
      return GL_FALSE;    
    case EGL_NOT_INITIALIZED:
      // is generated if display has not been initialized.
      LOGE("EGL_NOT_INITIALIZED");
      return GL_FALSE;    
    case EGL_BAD_PARAMETER:
      LOGE("EGL_BAD_PARAMETER");
      // is generated if num_config is NULL.
      return GL_FALSE;    
    }    
    return GL_FALSE;    
  }

  if (num_configs < 1) {
    return GL_FALSE;
  }


#ifdef ANDROID
  EGLint format = 0;
  if (!eglGetConfigAttrib(context->egl_display, context->egl_config, EGL_NATIVE_VISUAL_ID, &format)){
    switch (eglGetError()) {
    case EGL_BAD_DISPLAY:
      // is generated if display is not an EGL display connection. 
      break;
    case EGL_NOT_INITIALIZED:
      // is generated if display has not been initialized.
      break;
    case EGL_BAD_CONFIG:
      // is generated if config is not an EGL frame buffer configuration. 
      break;
    case EGL_BAD_ATTRIBUTE:
      // is generated if attribute is not a valid frame buffer configuration attribute. 
      break;
    }      
  } 
  ANativeWindow_setBuffersGeometry(context->egl_native_window, 0, 0, format);
#endif

  return EGL_TRUE;
}

// initialize context
static EGLBoolean initialize_egl_context(struct egl_context *context) {
  EGLint context_attribs [] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};

  // check if we already have a display
  if (context->egl_context != EGL_NO_CONTEXT) return EGL_TRUE;

  // create surface
  context->egl_surface = eglCreateWindowSurface(context->egl_display, context->egl_config, context->egl_native_window, NULL);
  if (context->egl_surface == EGL_NO_SURFACE) {
    // no error is returned
    return EGL_FALSE;
  }

  // create GL context
  context->egl_context = eglCreateContext(context->egl_display, context->egl_config, EGL_NO_CONTEXT/* dont share the context with another render context*/, context_attribs);

  if (context->egl_context == EGL_NO_CONTEXT) {
    return GL_FALSE;
  }

  // make the context current
  if (!eglMakeCurrent(context->egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
    switch (eglGetError()) {
    case  EGL_BAD_MATCH:
      // If draw or read are not compatible with context, then an EGL_BAD_MATCH error is generated.
      LOGE("EGL_BAD_MATCH");
      return GL_FALSE;
    case EGL_BAD_ACCESS:
      // If context is current to some other thread, or if either draw or read are bound to contexts in another thread,
      // an EGL_BAD_ACCESS error is generated.
      // If binding context would exceed the number of current contexts of that client API type supported by the implementation,
      // an EGL_BAD_ACCESS error is generated.
      // If either draw or read are pbuffers created with eglCreatePbufferFromClientBuffer,
      // and the underlying bound client API buffers are in use by the client API that created them,
      // an EGL_BAD_ACCESS error is generated.
      LOGE("EGL_BAD_ACCESS");
      return GL_FALSE;
    case EGL_BAD_CONTEXT:
      // If context is not a valid context and is not EGL_NO_CONTEXT,
      // an EGL_BAD_CONTEXT error is generated.
      LOGE("EGL_BAD_CONTEXT");
      return GL_FALSE;
    case  EGL_BAD_SURFACE:
      // If either draw or read are not valid EGL surfaces
      // and are not EGL_NO_SURFACE, an EGL_BAD_SURFACE error is generated.
      LOGE("EGL_BAD_SURFACE");
      return GL_FALSE;
    case EGL_BAD_NATIVE_WINDOW:
      // If a native window underlying either draw or read is no longer valid,
      // an EGL_BAD_NATIVE_WINDOW error is generated.       
      LOGE("EGL_BAD_NATIVE_WINDOW");
      return GL_FALSE;
    case EGL_BAD_CURRENT_SURFACE:
      // If the previous context of the calling thread has unflushed commands,
      // and the previous surface is no longer valid, an EGL_BAD_CURRENT_SURFACE error is generated.      
      LOGE("EGL_BAD_CURRENT_SURFACE");
      return GL_FALSE;      
    }
    return GL_FALSE;
  }
  return GL_TRUE;
}

static int initialize_egl_objects(struct egl_context *context) { return 1; }


int prepare_egl(struct egl_context *context) {

  if (!initialize_egl_display(context)) {
    LOGD("Display is already initialized");
    return -1;
  }

  if (!initialize_egl_surface(context, ES_WINDOW_RGB)) {
    LOGD("Surface is already initialized");
    return -2;
  }

  if (!initialize_egl_context(context)) {
    LOGD("Context is already initialized");
    return -3;
  }
  
  return 0;
}


// clean egl context
void clean_egl_context(struct egl_context* context) {
  // todo check if there are egl objects  
  if (!eglMakeCurrent(context->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
    switch (eglGetError()) {
    case EGL_BAD_MATCH:
      LOGE("EGL_BAD_MATCH");
      break;
    case EGL_BAD_ACCESS:
      LOGE("EGL_BAD_ACCESS");
      break;
    case EGL_BAD_CONTEXT:
      LOGE("EGL_BAD_CONTEXT");
      break;
    case EGL_BAD_SURFACE:
      LOGE("EGL_BAD_SURFACE");
      break;
    case EGL_BAD_NATIVE_WINDOW:
      LOGE("EGL_BAD_NATIVE_WINDOW");
      break;
    case EGL_BAD_CURRENT_SURFACE:
      LOGE("EGL_BAD_CURRENT_SURFACE");
      break;
    }
  }

  if (context->egl_context == EGL_NO_CONTEXT) {
    eglDestroyContext(context->egl_display, context->egl_context);
    context->egl_context = EGL_NO_CONTEXT;
  }  
}

// clean egl surface
void clean_egl_surface(struct egl_context * context) {
  eglMakeCurrent(context->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

  if (context->egl_surface != EGL_NO_SURFACE) {
    eglDestroySurface(context->egl_display, context->egl_surface);
    context->egl_surface = EGL_NO_SURFACE;    
  }
}

// clean egl display
void clean_egl_display(struct egl_context * context) {
  clean_egl_context(context);
  clean_egl_surface(context);

  if (context->egl_display != EGL_NO_DISPLAY) {
    eglTerminate(context->egl_display);
    context->egl_display = EGL_NO_DISPLAY;
  }  
}

// clean native window
void clean_native_window() {
  // todo 
}

void register_draw_cb(struct egl_context *context, void (*draw_cb)(struct egl_context *)) {
  context->draw_cb = draw_cb;
}

void register_shutdown_cb(struct egl_context *context, void (*shutdown_cb)(struct egl_context *)) {
  context->shutdown_cb = shutdown_cb;
}
