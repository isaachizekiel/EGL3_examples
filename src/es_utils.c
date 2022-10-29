#include <EGL/egl.h>
#include <stdio.h>
#include <stdlib.h>
#include "es_utils.h"

#ifdef ANDROID
#include <android/log.h>
#include <android_native_app_glue.h>
#endif

EGLint get_context_renderable_type(EGLDisplay egl_display) {
#ifdef EGL_KHR_create_context
  const char* extension = eglQueryString(egl_display, EGL_EXTENSIONS);

  if (extension != NULL && strstr(extensions, "EGL_KHR_create_context")) {
    return EGL_OPENGL_ES3_BIT_KHR;
  }
#endif
  
  return EGL_OPENGL_ES2_BIT;
}


EGLBoolean es_create_window(struct es_context *context, const char *title, GLint width, GLint height, GLuint flags) {
  EGLConfig config;
  EGLint major_version;
  EGLint minor_version;
  EGLint context_attribs [] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};

  if (context == NULL) {
    return GL_FALSE;
  }

#ifdef ANDROID
  context->width = ANativeWindow_getWidth(context->egl_native_window);
  context->height = ANativeWindow_getHeight(context->egl_native_window);
#else
  context->width = width;
  context->height = height;
#endif

#ifdef ANDROID
  // if android we don't need to create window manually
#else
  #if 0
  if (!WinCreate (context, title)) {
    return GL_FALSE;
  }
  #endif
#endif

  EGLint num_configs = 0;
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
  if (!eglChooseConfig(context->egl_display, attrib_list, &config, 1, &num_configs)) {
    return GL_FALSE;
  }

  if (num_configs < 1) {
    return GL_FALSE;
  }


#ifdef ANDROID
  EGLint format = 0;
  eglGetConfigAttrib(context->egl_display, config, EGL_NATIVE_VISUAL_ID, &format);
  ANativeWindow_setBuffersGeometry(context->egl_native_window, 0, 0, format);
#endif

  // create surface
  context->egl_surface = eglCreateWindowSurface(context->egl_display, config, context->egl_native_window, NULL);

  if (context->egl_surface == EGL_NO_SURFACE) {
    return EGL_FALSE;
  }

  // create GL context
  context->egl_context = eglCreateContext(context->egl_display, config, EGL_NO_CONTEXT/* dont share the context with another render context*/, context_attribs);

  if (context->egl_context == EGL_NO_CONTEXT) {
    return GL_FALSE;
  }

  // make the context current
  if (!eglMakeCurrent(context->egl_display, context->egl_surface, context->egl_surface, context->egl_context)) {
    return GL_FALSE;
  }

  return GL_TRUE;
}

void es_register_draw_cb(struct es_context * context, void (*draw_cb)(struct es_context *)) {
  context->draw_cb = draw_cb;
}

void es_register_shutdown_cb(struct es_context * context, void (*shutdown_cb)(struct es_context *)) {
  context->shutdown_cb = shutdown_cb;
}

void es_register_key_cb(struct es_context *context, void (*key_cb)(struct es_context *, unsigned char, int, int)) {
  key_cb = key_cb;
}

void es_register_update_cb(struct es_context * context, void (*update_cb)(struct es_context *, float)) {
  context->update_cb = update_cb;
}
