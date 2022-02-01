#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>

using namespace std;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window) {
        cout << "No window" << endl;
        const char* description;
        int error_code = glfwGetError(&description);
        if (error_code != GLFW_NO_ERROR) {
            if (description)
                cout << description << endl;
        }
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

     while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
