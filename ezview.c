#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ppmread.h"

#define GLFW_TRUE 1

typedef struct {
  float Position[2];
  float TexCoord[2];
} Vertex;

typedef struct{
  float rotate[3];
  float translate[2];
  float scale;
  float shear[2];
} TransformInfo;

Vertex vertexes[] = {
  {{1, 1},  {1, 0}},
  {{1, -1}, {1, 1}},
  {{-1, 1}, {0, 0}},
  {{-1, -1}, {0, 1}}
};


static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec2 TexCoordIn;\n"
"attribute vec2 vPos;\n"
"varying vec2 TexCoordOut;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    TexCoordOut = TexCoordIn;\n"
"}\n";

static const char* fragment_shader_text =
"varying vec2 TexCoordOut;\n"
"uniform sampler2D Texture;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(Texture, TexCoordOut);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  TransformInfo *info = glfwGetWindowUserPointer(window);
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  else if(key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    if(mods == GLFW_MOD_CONTROL){
      (*info).shear[1] -= 0.025;
    }
    else{
      (*info).translate[0] -= 0.025;
    }
  }
  else if(key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    if(mods == GLFW_MOD_CONTROL){
      (*info).shear[1] += 0.025;
    }
    else{
      (*info).translate[0] += 0.025;
    }
  }
  else if(key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    if(mods == GLFW_MOD_CONTROL){
      (*info).shear[0] -= 0.025;
    }
    else{
      (*info).translate[1] -= 0.025;
    }
  }
  else if(key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    if(mods == GLFW_MOD_CONTROL){
      (*info).shear[0] += 0.025;
    }
    else{
      (*info).translate[1] += 0.025;
    }
  }
  else if((key == GLFW_KEY_1 || key == GLFW_KEY_KP_1) && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    (*info).rotate[2] += 0.025;
  }
  else if((key == GLFW_KEY_3 || key == GLFW_KEY_KP_3) && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    (*info).rotate[2] -= 0.025;
  }
  else if((key == GLFW_KEY_4 || key == GLFW_KEY_KP_4) && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    (*info).rotate[1] += 0.025;
  }
  else if((key == GLFW_KEY_6 || key == GLFW_KEY_KP_6) && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    (*info).rotate[1] -= 0.025;
  }
  else if((key == GLFW_KEY_8 || key == GLFW_KEY_KP_8) && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    (*info).rotate[0] += 0.025;
  }
  else if((key == GLFW_KEY_2 || key == GLFW_KEY_KP_2) && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    (*info).rotate[0] -= 0.025;
  }
  else if(key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    (*info).scale += 0.025;
  }
  else if(key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    (*info).scale -= 0.025;
    if((*info).scale <= 0){
      (*info).scale = 0.01;
    }
  }
}

void glCompileShaderOrDie(GLuint shader) {
  GLint compiled;
  glCompileShader(shader);
  glGetShaderiv(shader,
		GL_COMPILE_STATUS,
		&compiled);
  if (!compiled) {
    GLint infoLen = 0;
    glGetShaderiv(shader,
		  GL_INFO_LOG_LENGTH,
		  &infoLen);
    char* info = malloc(infoLen+1);
    GLint done;
    glGetShaderInfoLog(shader, infoLen, &done, info);
    printf("Unable to compile shader: %s\n", info);
    exit(1);
  }
}

int main(int argc, char* argv[]){

  if(argc != 2){
    fprintf(stderr, "Error : Wrong number of argument, expected ./ezview inputImageName\n");
    exit(1);
  }
  ppmImage inputImage = openImage(argv[1]);
  unsigned char* imagedata = getImageData(inputImage);

  GLFWwindow* window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  GLint mvp_location, vpos_location;//, vcol_location;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
  exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  // gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);

  // NOTE: OpenGL error checks have been omitted for brevity

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShaderOrDie(vertex_shader);

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShaderOrDie(fragment_shader);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  // more error checking! glLinkProgramOrDie!

  mvp_location = glGetUniformLocation(program, "MVP");
  assert(mvp_location != -1);

  vpos_location = glGetAttribLocation(program, "vPos");
  assert(vpos_location != -1);

  GLint texcoord_location = glGetAttribLocation(program, "TexCoordIn");
  assert(texcoord_location != -1);

  GLint tex_location = glGetUniformLocation(program, "Texture");
  assert(tex_location != -1);

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

  glEnableVertexAttribArray(texcoord_location);
  glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 2));

  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inputImage->width, inputImage->height, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);
  glUniform1i(tex_location, 0);

  TransformInfo info = {{0,0,0},{0,0},1,{0,0}};
  glfwSetWindowUserPointer(window, &info);

  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;


    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_identity(m);
    mat4x4_translate(m, info.translate[0], info.translate[1], 0);
    mat4x4_rotate_Z(m, m, info.rotate[2]);
    mat4x4_rotate_Y(m, m, info.rotate[1]);
    mat4x4_rotate_X(m, m, info.rotate[0]);
    mat4x4_scale_aniso(m, m, info.scale, info.scale, 1);
    mat4x4_shear(m, m, info.shear[0], info.shear[1], 0, 0, 0, 0);
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

    mat4x4_mul(mvp, p, m);

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDrawArrays(GL_TRIANGLES, 1, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
