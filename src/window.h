#pragma once

#include <GLFW/glfw3.h>

class Window {
    GLFWwindow *window;

    public:
        Window(int width, int height, const char *title);
        ~Window();

        bool ShouldClose();
        void PollEvents();

    private:
        void createWindow(int width, int height, const char *title);
};
