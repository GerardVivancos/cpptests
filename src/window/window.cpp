#include "window.h"
#include <iostream>

Window::Window(int width, int height, const char *title) {
        createWindow(width, height, title);
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "Closing window" << std::endl;
}

void Window::createWindow(int width, int height, const char *title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        // TODO: return an error value that can be handled by the caller?
        const char *description;
        int error_code = glfwGetError(&description);
        if (error_code != GLFW_NO_ERROR && description) {
            std::cout << description << std::endl;
        }

        glfwTerminate();
        throw std::runtime_error("could not create glfw window");
    }
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(this->window);
}

void Window::PollEvents() {
    glfwPollEvents();
}
