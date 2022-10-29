#ifndef GAME_EGL_UTILS_H
#define GAME_EGL_UTILS_H

#include <GLES3/gl3.h>
#include <GLES3/gl3platform.h>

#include <EGL/egl.h>
#include <EGL/eglplatform.h>


#define ES_WINDOW_RGB 0
#define ES_WINDOW_ALPHA 1
#define ES_WINDOW_DEPTH 2
#define ES_WINDOW_STENCIL 4
#define ES_WINDOW_MULTISAWMPLE 8


struct es_context {
  void* platform_data;
  void* user_data;
  GLint width;
  GLint height;

  EGLNativeDisplayType egl_native_display;
  EGLNativeWindowType egl_native_window;

  EGLDisplay egl_display;
  EGLContext egl_context;
  EGLSurface egl_surface;

  int animating;

  void (*draw_cb) ( struct es_context*);
  void (*shutdown_cb) ( struct es_context*);
  void (*key_cb) ( struct es_context*, unsigned char, int, int);
  void (*update_cb) ( struct es_context*, float);  
};

EGLBoolean es_create_window(struct es_context *, const char *title, GLint width, GLint height, GLuint flags);

void es_register_draw_cb(struct es_context *, void (*draw_cb)(struct es_context *));

void es_register_shutdown_cb(struct es_context *, void (*shutdown_cb)(struct es_context *));

void es_register_key_cb(struct es_context *, void (*key_cb)(struct es_context *, unsigned char, int, int));

void es_register_update_cb(struct es_context *, void (*update_cb)(struct es_context *, float));



#endif
