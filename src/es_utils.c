#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include "es_utils.h"

#ifdef ANDROID
#include <android/log.h>
#include <android_native_app_glue.h>
#endif

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
    EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAWMPLE) ? 1 : 0,
    EGL_RENDERABLE_TYPE, get_context_renderable_type(context->egl_display),
    EGL_NONE
  };

  // choose config
  if (!eglChooseConfig(context->egl_display, attrib_list, context->egl_config, 1, &num_configs)) {
    return GL_FALSE;
  }

  if (num_configs < 1) {
    return GL_FALSE;
  }


#ifdef ANDROID
  EGLint format = 0;
  eglGetConfigAttrib(context->egl_display, *context->egl_config, EGL_NATIVE_VISUAL_ID, &format);
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
  context->egl_surface = eglCreateWindowSurface(context->egl_display, *context->egl_config, context->egl_native_window, NULL);

  if (context->egl_surface == EGL_NO_SURFACE) {
    return EGL_FALSE;
  }

  // create GL context
  context->egl_context = eglCreateContext(context->egl_display, *context->egl_config, EGL_NO_CONTEXT/* dont share the context with another render context*/, context_attribs);

  if (context->egl_context == EGL_NO_CONTEXT) {
    return GL_FALSE;
  }

  // make the context current
  if (!eglMakeCurrent(context->egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
    return GL_FALSE;
  }

  return GL_TRUE;
}

int prepare_egl(struct egl_context *context) {

  if (!initialize_egl_display(context)) {
    return -1;
  }

  if (!initialize_egl_surface(context, 0)) {
    return -2;
  }

  if (!initialize_egl_context(context)) {
    return -3;
  }
  
  return 0;
}


// clean egl context
void clean_egl_context(struct egl_context* context) {

  // todo check if there are egl objects
  
  eglMakeCurrent(context->egl_context, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

  if (context->egl_context == EGL_NO_CONTEXT) {
    eglDestroyContext(context->egl_display, context->egl_context);
    context->egl_context = EGL_NO_CONTEXT;
  }  
}

// clean egl surface
void clean_egl_surface(struct egl_context * context) {
  eglMakeCurrent(context->egl_context, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

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
