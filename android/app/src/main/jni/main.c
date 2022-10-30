//
// Created by izak on 10/28/22.
//

#include <stdlib.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include "es_utils.h"

#include "util.h"

// the game state storage, this is like the saved state bundle
struct saved_state {
    float angle;
    float x;
    float y;
};

// all application lifecycle callbacks are handled here
static void handle_cmd(struct android_app* app, int32_t cmd) {
    struct egl_context * context = app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW: %d", cmd);
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW: %d", cmd);
            clean_egl_surface(context);
            break;
        case APP_CMD_WINDOW_RESIZED:
            LOGI("APP_CMD_WINDOW_RESIZED: %d", cmd);
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            LOGI("APP_CMD_WINDOW_REDRAW_NEEDED: %d", cmd);
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            LOGI("APP_CMD_CONTENT_RECT_CHANGED: %d", cmd);
            break;
        case APP_CMD_GAINED_FOCUS:
            context->app_life_cycle &= 0x4;
            LOGI("APP_CMD_GAINED_FOCUS: %d - %02x", cmd, context->app_life_cycle);
            break;
        case APP_CMD_LOST_FOCUS:
            context->app_life_cycle ^= 0x4;
            LOGI("APP_CMD_GAINED_FOCUS: %d - %02x", cmd, context->app_life_cycle);
            break;
        case APP_CMD_CONFIG_CHANGED:
            LOGI("APP_CMD_CONFIG_CHANGED: %d", cmd);
            break;
        case APP_CMD_LOW_MEMORY:
            LOGI("APP_CMD_LOW_MEMORY: %d", cmd);
            break;
        case APP_CMD_START:
            LOGI("APP_CMD_START: %d", cmd);
            break;
        case APP_CMD_RESUME:
            LOGI("APP_CMD_RESUME: %d", cmd);
            break;
        case APP_CMD_SAVE_STATE:
            // system ahs asked us to save out current state
            // TODO finish this
            // ((struct android_app *)context->platform_data)->savedState = malloc(sizeof (struct  saved_state));
            // *((struct saved_state *)e->app->savedState) = e->state;
            // ((struct android_app *)context->platform_data)->savedStateSize = sizeof (struct saved_state);
            LOGI("APP_CMD_SAVE_STATE: %d", cmd);
            break;
        case APP_CMD_PAUSE:
            LOGI("APP_CMD_PAUSE: %d", cmd);
            break;
        case APP_CMD_STOP:
            LOGI("APP_CMD_STOP: %d", cmd);
            break;
        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY: %d", cmd);
            break;
        default:
            LOGI("Unknown CMD: %d", cmd);
            break;
    }
}

// input events are collected using this android_app callback function
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    switch (AInputEvent_getType(event)) {
        case AINPUT_EVENT_TYPE_MOTION:
        case AINPUT_SOURCE_TOUCHSCREEN:
            // todo handle this
            //context->is_animating = 1;
            //context->state.x = AMotionEvent_getX(event, 0);
            //context->state.y = AMotionEvent_getY(event, 0);
            return 1;
    }
    return 0;
}

static int is_animating(struct android_app *app) {
    struct egl_context * context = app->userData;
    return context->app_life_cycle ^ 0x7 ? 0 : 1;
}

static void do_frame(struct android_app *app) {
    struct egl_context *context = app->userData;
    prepare_egl(context);
}

static void game_loop(struct android_app* app) {
    while(1) {
        int events;
        struct android_poll_source* source;
        // if not animating, block until we get event; if animating don't block;
        while((ALooper_pollAll(is_animating(app) ? 0 : -1, NULL, &events, (void **) &source)) >= 0) {
            // processes event
            if (source != NULL) {
                source->process(app, source);
            }
            // are we exiting
            if (app->destroyRequested) return;
        }
        if (is_animating(app)) do_frame(app);
    }
}

void android_main(struct android_app* app) {
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;

    struct es_context * context = malloc (sizeof (struct egl_context));
    app->userData = context;

    // main window loop
    game_loop(app);

    // clean memory
    free(context);
}
