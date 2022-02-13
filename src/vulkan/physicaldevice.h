#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <iostream>
#include <vector>

class PhysicalDeviceBuilder {
    private:
    VkInstance instance;
    // VkPhysicalDeviceProperties requiredProperties;
    // VkPhysicalDeviceProperties preferredProperties;
    // VkPhysicalDeviceFeatures requiredFeatures;
    // VkPhysicalDeviceFeatures preferredFeatures;
    std::vector<const char*> requiredExtensions;
    std::vector<const char*> preferredExtensions; // Not yet supported
    bool require_discrete = false;
    bool prefer_discrete = false;

    public:
    VkPhysicalDevice Build();
    PhysicalDeviceBuilder& RequireExtension(const char *extension);
    PhysicalDeviceBuilder& PreferExtension(const char *extension); // Not yet supported
    PhysicalDeviceBuilder& RequireExtensions(std::vector<const char*> extensions);
    PhysicalDeviceBuilder& PreferExtensions(std::vector<const char*> extensions); // Not yet supported
    PhysicalDeviceBuilder& RequireDiscrete();
    PhysicalDeviceBuilder& PreferDiscrete(); // Not yet supported
    PhysicalDeviceBuilder(VkInstance instance);

    private:
    bool isDeviceSuitable(VkPhysicalDevice device);
    std::vector<VkExtensionProperties> getPhysicalDeviceExtensions(VkPhysicalDevice device);
};


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> transferFamily;
    std::optional<uint32_t> sparsebindingFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && computeFamily.has_value() && transferFamily.has_value(); // && sparsebindingFamily.has_value();
    }
};
