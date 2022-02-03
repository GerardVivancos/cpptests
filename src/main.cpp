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
        createWindow();
        initVulkan();
        mainLoop();
        cleanup();
    };

private:
    GLFWwindow *window;
    VkInstance instance;
    
    void createWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
        if (!window) {
            const char *description;
            int error_code = glfwGetError(&description);
            if (error_code != GLFW_NO_ERROR && description) {
                cout << description << endl;
            }
            throw std::runtime_error("could not create glfw window");
        }
    }
    
    void initVulkan() {
        printSupportedExtensions();
        createInstance();
    }

    void mainLoop() {
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
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

    void cleanup() {
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool supportsVkExtension(const char *name) {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        for (const VkExtensionProperties extension: extensions) {
            if (strcmp(name, extension.extensionName) == 0) {
                return true;
            }
        }
        return false;
    }

    void printSupportedExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        cout << extensionCount << " extensions supported:" << endl;
        for (const VkExtensionProperties extension: extensions) {
            cout << "- " << extension.extensionName << endl;
        }
    }
};


int main()
{
    try {
        VulkanApp app = VulkanApp();
        app.run();
    } catch(exception e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
