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



