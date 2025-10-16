#pragma once

#include <GLFW/glfw3.h>

class Window {
public:
  Window(int width, int height, const char *title);
  ~Window();

  bool shouldClose() const;
  void pollEvents() const;
  GLFWwindow* handle() const;

private:
  GLFWwindow* _window;
  
};
