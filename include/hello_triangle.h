#ifndef GAME_HELLO_TRIANGLE_H
#define GAME_HELLO_TRIANGLE_H

#include "egl_utils.h"

GLuint load_shader(GLenum type, const char *shader_src);

int init(struct egl_context *context);

void draw(struct egl_context *context);

void shutdown(struct egl_context * context);

#endif
