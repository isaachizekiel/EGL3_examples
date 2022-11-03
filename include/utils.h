#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#ifdef ANDROID
#include <android/log.h>
#else
#include <stdio.h>
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

#endif // GAME_UTILS_H
