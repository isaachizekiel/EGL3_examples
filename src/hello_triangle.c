/* hello_triangle.c*/

#include "es_utils.h"
#include <stdlib.h>



struct user_data {
  // handle for the sharder program object
  GLuint program_object;
};

GLuint load_shader(GLenum type, const char * shader_src) {
  GLuint shader;
  GLint compiled;

  shader = glCreateShader(type);

  if (shader == 0) {
    return 0;
  }

  // load the shader
  glShaderSource(shader, 1, &shader_src, 0x00);

  // compile the shader
  glCompileShader(shader);

  // check the compile status  
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  if (!compiled) {
    GLint infoLen = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1) {
      char *info_log = malloc (sizeof(char) * infoLen);
      // LOGE("");
      free(info_log);
    }


    glDeleteShader(shader);
    return 0;
  }

  return shader;
  
}


// initialize the shader and program object

int init(struct egl_context * context) { 
  struct user_data * ud = context->user_data;

  char vertex_shader_str [] =
    "#version 300 es\n"
    "layout(location - 0) in vec4 vPosition;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vPosition;\n"
    "}\n";

  char fragment_shader_str [] =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 fragColor;\n"
    "void main()\n"
    "void main()\n"
    "{\n"
    "fragColor = vec4 (1.0, 1.0, 1.0, 1.0);\n"
    "}\n";


  GLuint vertex_shader, fragment_shader, program_object;
  GLint linked;

  // load the vertex and fragment shader
  vertex_shader = load_shader(GL_VERTEX_SHADER, vertex_shader_str) ;
  fragment_shader = load_shader(GL_FRAGMENT_SHADER, fragment_shader_str);

  // create the program pbject
  program_object = glCreateProgram();

  if (program_object == 0) {
    return 0;
  }

  glAttachShader(program_object, vertex_shader);
  glAttachShader(program_object, fragment_shader);

  // link the program
  glGetProgramiv(program_object, GL_LINK_STATUS, &linked);

  if (!linked) {
    GLint info_len = 0;
    glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &info_len);

    if (info_len > 1) {
      char * info_log = malloc (sizeof(char) * info_len);
      glGetProgramInfoLog(program_object, info_len, 0x00, info_log);
      // todo log message

      free(info_log);      
    }

    glDeleteProgram(program_object);
    return 0;
  }

  ud->program_object = program_object;

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  return 0;
}


// draw the triangle using the above triangle shader
void draw(struct egl_context * context) {
  struct user_data * ud = context->user_data;

  GLfloat vector_vertices [] =
    {
      0.0f, 0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f
    };

  // set the view port
  glViewport (0, 0, context->width, context->height);

  // clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(ud->program_object);

  // load the vertex data
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, vector_vertices);
  glEnableVertexAttribArray(0);

  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void shutdown(struct egl_context * context) {
  struct user_data * ud = context->user_data;

  glDeleteProgram(ud->program_object);  
}

