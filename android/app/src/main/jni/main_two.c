//
// Created by izak on 11/1/22.
//

#include <stdlib.h>
#include "es_utils.h"

// the game state storage, this is like the saved state bundle
struct saved_state {
    float angle;
    float x;
    float y;
};

struct egl_context * context;

// all application lifecycle callbacks are handled here
static void handle_cmd(struct android_app* app, int32_t cmd) {
    //struct egl_context * context = app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            context->egl_native_window = app->window;
            context->app_life_cycle ^= 0x1;
            LOGE("APP_CMD_INIT_WINDOW: %d", cmd);
            break;
        case APP_CMD_TERM_WINDOW:
            clean_egl_surface(context);
            context->app_life_cycle ^= 0x1;
            LOGE("APP_CMD_TERM_WINDOW: %d", cmd);
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
            context->app_life_cycle ^= 0x4;
            LOGE("APP_CMD_GAINED_FOCUS: %d - %02x", cmd, context->app_life_cycle);
            break;
        case APP_CMD_LOST_FOCUS:
            context->app_life_cycle ^= 0x4;
            LOGE("APP_CMD_LOST_FOCUS: %d - %02x", cmd, context->app_life_cycle);
            break;
        case APP_CMD_CONFIG_CHANGED:
            LOGI("APP_CMD_CONFIG_CHANGED: %d", cmd);
            break;
        case APP_CMD_LOW_MEMORY:
            LOGI("APP_CMD_LOW_MEMORY: %d", cmd);
            break;
        case APP_CMD_START:
            context->app_life_cycle ^= 0x2;
            LOGE("APP_CMD_START: %d - %02x", cmd, context->app_life_cycle);
            break;
        case APP_CMD_RESUME:
            LOGE("APP_CMD_RESUME: %d - %02x", cmd, context->app_life_cycle);
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
            context->app_life_cycle ^= 0x2;
            LOGE("APP_CMD_STOP: %d - %02x", cmd, context->app_life_cycle);
            break;
        case APP_CMD_DESTROY:
            LOGE("APP_CMD_DESTROY: %d - %02x", cmd, context->app_life_cycle);
            break;
        default:
            LOGI("Unknown CMD: %d", cmd);
            break;
    }
}

static int is_animating() {
    LOGE("-- %d", context->app_life_cycle);
    return context->app_life_cycle == 0x7 ? 1 : 0;
}

static void do_frame(struct android_app *app) {
    LOGE("do frame");
    prepare_egl(context);
}

static void game_loop(struct android_app* app) {
    while(1) {
        int events;
        struct android_poll_source* source;
        // if not animating, block until we get event; if animating don't block;
        while((ALooper_pollAll(is_animating() ? 0 : -1, NULL, &events, (void **) &source)) >= 0) {
            // processes event
            if (source != NULL) {
                source->process(app, source);
            }
            // are we exiting
            if (app->destroyRequested) return;
        }
        if (is_animating()) do_frame(app);
    }
}

void android_main(struct android_app* app) {
    app->onAppCmd = handle_cmd;
    // app->onInputEvent = handle_input;

    context = malloc (sizeof (struct egl_context));
    app->userData = context;

    // main window loop
    game_loop(app);

    // clean memory
    free(context);
}