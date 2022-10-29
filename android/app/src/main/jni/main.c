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

// global pointers are dangerous but let's take our chances
struct es_context * context = {0};

// all application lifecycle callbacks are handled here
static void handle_cmd(struct android_app* app, int32_t cmd) {
    context = ( struct es_context *) app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW: %d", cmd);
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW: %d", cmd);
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
            ///context->animating = 1;
            LOGI("APP_CMD_GAINED_FOCUS: %d", cmd);
            break;
        case APP_CMD_LOST_FOCUS:
            //context->animating = 0;
            LOGI("APP_CMD_LOST_FOCUS: %d", cmd);
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

//
static void do_frame() {
    // LOGI("doing frame");
}

static void game_loop() {
    while(1) {
        int events;
        struct android_poll_source* source;
        // if not animating, block until we get event; if animating don't block;
        while((ALooper_pollAll(context->animating ? 0 : -1, NULL, &events, (void **) &source)) >= 0) {
            // processes event
            if (source != NULL) {
                source->process(context->platform_data, source);
            }
            // are we exiting
            if (((struct android_app *)context->platform_data)->destroyRequested) return;
        }
        if (context->animating) do_frame();
    }
}

void android_main(struct android_app* app) {
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;

    context = malloc (sizeof (struct es_context));

    context->platform_data = app;

    // main window loop
    game_loop();

    // release the resource used by the window
    free(context);
}
