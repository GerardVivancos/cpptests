#include "physicaldevice.h"

PhysicalDeviceBuilder::PhysicalDeviceBuilder(VkInstance instance) {
    this->instance = instance;
}

VkPhysicalDevice PhysicalDeviceBuilder::Build() {
    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);
    if (physicalDevicesCount == 0) {
        throw std::runtime_error("no physical devices found");
    }
    
    std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
    auto result = vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevices.data());

    std::cout << "Devices found: " << std::endl;
    VkPhysicalDeviceProperties deviceProperties;
    for (uint32_t i = 0; i < physicalDevicesCount; i++) {
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
        std::cout << deviceProperties.deviceID <<" "<< deviceProperties.deviceName <<" "<< deviceProperties.deviceType << std::endl;
    }

    for (auto physicalDevice: physicalDevices) {
        if (isDeviceSuitable(physicalDevice)) {
            vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
            std::cout << "Selected device: " << deviceProperties.deviceID <<" "<< deviceProperties.deviceName <<" "<< deviceProperties.deviceType << std::endl;
            return physicalDevice;
        }
    }
    
    throw std::runtime_error("no suitable physical device found");
}

PhysicalDeviceBuilder& PhysicalDeviceBuilder::RequireExtension(const char *extension) {
    this->requiredExtensions.push_back(extension);
    return *this;
}

PhysicalDeviceBuilder& PhysicalDeviceBuilder::PreferExtension(const char *extension) {
    this->preferredExtensions.push_back(extension);
    return *this;
}

PhysicalDeviceBuilder& PhysicalDeviceBuilder::RequireExtensions(std::vector<const char*> extensions) {
    this->requiredExtensions.insert(this->requiredExtensions.end(), extensions.begin(), extensions.end());
    return *this;
}

PhysicalDeviceBuilder& PhysicalDeviceBuilder::PreferExtensions(std::vector<const char*> extensions) {
    this->preferredExtensions.insert(this->preferredExtensions.end(), extensions.begin(), extensions.end());
    return *this;
}

PhysicalDeviceBuilder& PhysicalDeviceBuilder::RequireDiscrete() {
    this->require_discrete = true;
    return *this;
}

PhysicalDeviceBuilder& PhysicalDeviceBuilder::PreferDiscrete() {
    this->prefer_discrete = true;
    return *this;
}



bool PhysicalDeviceBuilder::isDeviceSuitable(VkPhysicalDevice device){
    // Get device properties
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    // If require_discrete and properties say this is not discrete, return false.
    if (this->require_discrete)
        return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    
    // TODO: support preference for discrete

    // Get device supported extensions
    std::vector<VkExtensionProperties> extensions = getPhysicalDeviceExtensions(device);
    // If any requiredExtensions are not supported return false
    for (auto req: this->requiredExtensions) {
        bool found = false;
        for (auto ext: extensions) {
            if (req == std::string(ext.extensionName)) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }

    // TODO: add preferredExtensions support
    
    // TODO: QueueFamilies

    return true;
}

std::vector<VkExtensionProperties> PhysicalDeviceBuilder::getPhysicalDeviceExtensions(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
    return extensions;
}
