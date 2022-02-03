#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <vector>

using namespace std;

class VulkanApp {
public:
    void run() {
        initVulkan();
    };

private:
    VkInstance instance;

    void initVulkan() {
        createInstance();
    }

    void createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = "G's Vulkan Test";
        appInfo.applicationVersion = 0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // Vulkan has no concept of window et al, so it need extensions to interact with it. GLFW conveniently provides some help
        uint32_t glfwExtensionCount = 1;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        bool allExtSupported = true;
        cout << glfwExtensionCount << " extensions required:" << endl;
        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            bool supported = supportsVkExtension(glfwExtensions[i]);
            cout << "- " << glfwExtensions[i];
            if (supported) {
                cout << " (Supported)" << endl;
            } else {
                cout << " (NOT supported)" << endl;
                allExtSupported = false;
            }
        }

        if (!allExtSupported) {
            throw std::runtime_error("unsupported Vulkan extension/s");
        }

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("error creating instance");
        }
    }

    bool supportsVkExtension(const char *name) {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        for (uint32_t i = 0; i < extensionCount; i++) {
            if (strcmp(name, extensions[i].extensionName) == 0) {
                return true;
            }
        }
        return false;
    }
};


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window) {
        cout << "No window" << endl;
        const char *description;
        int error_code = glfwGetError(&description);
        if (error_code != GLFW_NO_ERROR) {
            if (description)
                cout << description << endl;
        }
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    cout << extensionCount << " extensions supported:" << endl;
    for (uint32_t i = 0; i < extensionCount; i++) {
        cout << "- " << extensions[i].extensionName << endl;
    }
    
    try {
    VulkanApp app = VulkanApp();
    app.run();
    } catch(exception e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
