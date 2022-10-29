//
// Created by izak on 10/29/22.
//

#ifndef GAME_EGL_SETUP_H
#define GAME_EGL_SETUP_H
#include <stdlib.h>
#include <EGL/egl.h>

struct egl_info {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
};

void init_egl(){

}

#endif //GAME_EGL_SETUP_H
