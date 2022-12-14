//
// Created by izak on 11/1/22.
//

#include <stdlib.h>

#include "egl_utils.h"
#include "hello_triangle.h"

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
            context->egl_native_display = EGL_DEFAULT_DISPLAY;
            context->app_life_cycle ^= 0x1;
            // todo find a new place for this
            if (!init(context)) { //
                LOGE("can not initialize vertex and fragment shaders");
                return;
            }
            LOGE("APP_CMD_INIT_WINDOW: %d", cmd);
            break;
        case APP_CMD_TERM_WINDOW:
            if (context->shutdown_cb != NULL) context->shutdown_cb(context);
            clean_egl_surface(context);
            context->app_life_cycle ^= 0x1;
            context->shutdown_cb(context);
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



static int is_animating() {
    //LOGE("-- %d", context->app_life_cycle);
    return context->app_life_cycle == 0x7 ? 1 : 0;
}

static void do_frame(struct android_app *app) {
    // LOGE("do frame");
    prepare_egl(context);

    // Call app update function
    if ( context->update_cb != NULL ) {
        struct timespec res;
        long curTime = clock_gettime(CLOCK_MONOTONIC, &res);
        float deltaTime =  ( 0 );
        context->update_cb(context, 0);
    }

    if ( context->draw_cb != NULL ) {
        context->draw_cb (context);
        eglSwapBuffers ( context->egl_display, context->egl_surface );
    }
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
    app->onInputEvent = handle_input;

    context = malloc (sizeof (struct egl_context));
    app->userData = context;

    context->shader_program_data = malloc(sizeof (struct program_data));

    register_draw_cb(context, draw);
    register_shutdown_cb(context, shutdown);

    // main window loop
    game_loop(app);

    // clean memory
    free(context);
}