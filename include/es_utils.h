#ifndef GAME_EGL_UTILS_H
#define GAME_EGL_UTILS_H

#include <GLES3/gl3.h>
#include <GLES3/gl3platform.h>

#include <EGL/egl.h>
#include <EGL/eglplatform.h>

#ifdef ANDROID
#include <android/log.h>
#include <android_native_app_glue.h>
#endif

#define DEBUG_TAG "EGL3_examples"

#ifdef ANDROID
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, DEBUG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, DEBUG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, DEBUG_TAG, __VA_ARGS__))
#define ABORT_GAME { LOGE("*** GAME ABORTING."); *((volatile char*)0) = 'a'; }
#else
#define LOGD(...) ((void)fprintf(stdout, __VA_ARGS__))
#define LOGI(...) ((void)fprintf(stdout, __VA_ARGS__))
#define LOGW(...) ((void)fprintf(stdout, __VA_ARGS__))
#define LOGE(...) ((void)fprintf(stderr, __VA_ARGS__))
#define ABORT_GAME { LOGE("*** GAME ABORTING."); *((volatile char*)0) = 'a'; }
#endif


#define ES_WINDOW_RGB 0
#define ES_WINDOW_ALPHA 1
#define ES_WINDOW_DEPTH 2
#define ES_WINDOW_STENCIL 4
#define ES_WINDOW_MULTISAWMPLE 8

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

#endif
