// Stub GLAD header for basic compilation
#pragma once

#ifndef __glad_h_
#define __glad_h_

#ifdef __gl_h_
#error OpenGL header already included, remove this include, glad already provides it
#endif

#define __gl_h_

#include <windows.h>

// Basic OpenGL types
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef int GLsizei;
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// Basic OpenGL constants
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_DEPTH_TEST 0x0B71
#define GL_CULL_FACE 0x0B44
#define GL_BACK 0x0405
#define GL_CCW 0x0901
#define GL_TRIANGLES 0x0004
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_FLOAT 0x1406

// Stub functions - these will need proper implementation later
extern "C" {
int gladLoadGL(void);
void glClear(GLbitfield mask);
void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLuint glCreateShader(GLenum type);
void glDeleteShader(GLuint shader);
GLint glGetUniformLocation(GLuint program, const GLchar *name);
void glGetShaderiv(GLuint shader, GLenum pname, GLint *param);
void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length,
                        GLchar *infoLog);
void glEnable(GLenum cap);
void glCullFace(GLenum mode);
void glFrontFace(GLenum mode);
void glBindVertexArray(GLuint array);
void glDrawArrays(GLenum mode, GLint first, GLsizei count);
void glGenVertexArrays(GLsizei n, GLuint *arrays);
void glGenBuffers(GLsizei n, GLuint *buffers);
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
void glDeleteBuffers(GLsizei n, const GLuint *buffers);
void glBindBuffer(GLenum target, GLuint buffer);
void glBufferData(GLenum target, GLsizeiptr size, const void *data,
                  GLenum usage);
void glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                           GLboolean normalized, GLsizei stride,
                           const void *pointer);
void glEnableVertexAttribArray(GLuint index);
}

#endif /* __glad_h_ */