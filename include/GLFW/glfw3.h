// Stub GLFW header for basic compilation
#pragma once

#ifndef GLFW_VERSION_MAJOR
#define GLFW_VERSION_MAJOR 3
#define GLFW_VERSION_MINOR 4
#define GLFW_VERSION_REVISION 0
#endif

// Basic GLFW types
typedef struct GLFWwindow GLFWwindow;
typedef void (*GLFWkeyfun)(GLFWwindow *, int, int, int, int);

// Basic constants
#define GLFW_PRESS 0x00000001
#define GLFW_RELEASE 0x00000000
#define GLFW_KEY_ESCAPE 256

// Stub functions - these will need proper linking later
extern "C" {
int glfwInit(void);
void glfwTerminate(void);
GLFWwindow *glfwCreateWindow(int width, int height, const char *title,
                             void *monitor, void *share);
void glfwDestroyWindow(GLFWwindow *window);
int glfwWindowShouldClose(GLFWwindow *window);
void glfwPollEvents(void);
void glfwSwapBuffers(GLFWwindow *window);
GLFWkeyfun glfwSetKeyCallback(GLFWwindow *window, GLFWkeyfun callback);
void glfwMakeContextCurrent(GLFWwindow *window);
void glfwSetWindowUserPointer(GLFWwindow *window, void *pointer);
void *glfwGetWindowUserPointer(GLFWwindow *window);
}