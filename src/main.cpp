#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <vector>

using namespace std;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = false;
#endif

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
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    
    void createWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "My Title", NULL, NULL);
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
        if (enableValidationLayers)
            printSupportedLayers();
        createInstance();
        createPhysicalDevice();
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

        if (enableValidationLayers) {
            std::vector<const char*>  validationLayerNames = {
                "VK_LAYER_KHRONOS_validation"
            };

            bool allLayersSupported = true;
            cout << validationLayerNames.size() << " validation layers required" << endl;
            for (auto layer: validationLayerNames) {
                bool supported = supportsVkLayer(layer);
                cout << "- " << layer;
                if (supported) {
                    cout << " (Supported)" << endl;
                } else {
                    cout << " (NOT supported)" << endl;
                    allLayersSupported = false;
                }
            }

            if (!allLayersSupported) {
                throw std::runtime_error("error initializing validation layers: required extension not supported");
            }
            
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayerNames.size());
            createInfo.ppEnabledLayerNames = validationLayerNames.data();
            cout << "Validation layers enabled" << endl;
        }

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

    void createPhysicalDevice() {
        uint32_t physicalDevicesCount = 0;
        vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);
        if (physicalDevicesCount == 0) {
            throw std::runtime_error("no physical devices found");
        }
        
        vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
        auto result = vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevices.data());

        cout << "Devices found: " << endl;
        VkPhysicalDeviceProperties deviceProperties;
        for (int i = 0; i < physicalDevicesCount; i++) {
            vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
            cout << deviceProperties.deviceID <<" "<< deviceProperties.deviceName <<" "<< deviceProperties.deviceType << endl;
        }

    }

    void cleanup() {
        // physicalDevice is destroyed by vkDestroyInstance
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

    bool supportsVkLayer(const char *name) {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        vector<VkLayerProperties> layers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        for (const VkLayerProperties layer: layers) {
            if (strcmp(name, layer.layerName) == 0) {
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

    void printSupportedLayers() {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        vector<VkLayerProperties> layers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        cout << layerCount << " layers supported:" << endl;
        for (const VkLayerProperties layer: layers) {
            cout << "- " << layer.layerName << endl;
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
