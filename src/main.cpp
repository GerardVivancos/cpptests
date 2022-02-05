#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <vector>
#include <string>
#include "window.h"

using namespace std;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
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
    Window *window;
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    
    void createWindow() {
        window = new Window(WIDTH, HEIGHT, "Finestra");
    }
    
    void initVulkan() {
        printSupportedExtensions();
        if (enableValidationLayers)
            printSupportedLayers();
        createInstance();
        createPhysicalDevice();
    }

    void mainLoop() {
        while(!window->ShouldClose()) {
            window->PollEvents();
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
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

        std::vector<const char*> validationLayerNames = {
            "VK_LAYER_KHRONOS_validation"
        };
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
        
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
            
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
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

        for (auto device: physicalDevices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                cout << "Selected device: " << deviceProperties.deviceID <<" "<< deviceProperties.deviceName <<" "<< deviceProperties.deviceType << endl;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("no suitable physical device found");
        }

    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        //here's where we'd check for specific properties and features but right now we don't care
        return true;
    }

    void cleanup() {
        // physicalDevice is destroyed by vkDestroyInstance
        vkDestroyInstance(instance, nullptr);
        delete window;
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
    } catch(exception& e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
