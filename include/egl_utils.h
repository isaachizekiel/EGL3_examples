#ifndef GAME_EGL_UTILS_H
#define GAME_EGL_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include <GLES3/gl3.h>
#include <GLES3/gl3platform.h>

#include <EGL/egl.h>
#include <EGL/eglplatform.h>


#ifdef ANDROID
#include <android_native_app_glue.h>
#endif

#include "utils.h"


#define ES_WINDOW_RGB 0
#define ES_WINDOW_ALPHA 1
#define ES_WINDOW_DEPTH 2
#define ES_WINDOW_STENCIL 4
#define ES_WINDOW_MULTISAMPLE 8

struct egl_context {
  GLint width, height;

  EGLNativeDisplayType egl_native_display;
  EGLNativeWindowType egl_native_window;

  EGLDisplay egl_display;
  EGLSurface egl_surface;
  EGLConfig egl_config;  
  EGLContext egl_context;

  // variables to track Android lifecycle:
  // bool mHasFocus, mIsVisible, mHasWindow;
  // 0 0 0 0 0 0 0 0 = 
  // 0 0 0 0 0 0 0 1 = mHasWindow;
  // 0 0 0 0 0 0 1 1 = mIsVisible, mHasWindow;
  // 0 0 0 0 0 1 1 1 = mHasFocus, mIsVisible, mHasWindow;    
  unsigned char app_life_cycle;

  void *shader_program_data;

  void ( *draw_cb ) ( struct egl_context * );
  void ( *update_cb ) ( struct egl_context *, float );
  void ( *shutdown_cb ) ( struct egl_context * );
};


// prepare to render
int prepare_egl(struct egl_context * context);

// clean egl context
void clean_egl_context();

// clean egl surface
void clean_egl_surface();

// clean egl display
void clean_egl_display();

// clean native window
void clean_native_window();

// callback for the draw function
void register_draw_cb(struct egl_context *context, void (*draw_cb)(struct egl_context *));

// callback for the shutdown function
void register_shutdown_cb(struct egl_context *context, void (*shutdown_cb)(struct egl_context *));

void register_update_cb(struct egl_context *context, void (*shutdown_cb)(struct egl_context *, float interval));

#endif
