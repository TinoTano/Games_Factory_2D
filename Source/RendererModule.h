#pragma once
#include "Module.h"
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include "ThirdParty/glfw-3.2.1/include/glfw3.h"

struct QueueFamily
{
	int graphics_index = 0;
	int present_index = 0;

	bool is_valid()
	{
		graphics_index > 0 && present_index > 0;
	}
};

struct SwapChainSupportDetails 
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class RendererModule :
	public Module
{
public:
	RendererModule(const char* module_name, bool game_module = false);
	~RendererModule();

	bool Init();
	bool PreUpdate(float delta_time);
	bool Update(float delta_time);
	bool PostUpdate(float delta_time);
	bool CleanUp();

private:
	bool InitVulkan();
	bool CreateVulkanInstance();
	bool SetupDebugDrawCall();
	bool CreateVulkanSurface();
	bool PickPhysicalDevice();
	bool CreateLogicalDevice();
	bool CreateSwapChain();

	bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamily FindQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	bool CheckValidationLayerSupport();
	void PrintVkResults(VkResult result);

private:
	VkInstance vk_instance;
	VkPhysicalDevice vk_physical_device;
	VkDevice vk_logical_device;
	VkQueue vk_graphics_queue;
	VkQueue vk_presentation_queue;
	VkSurfaceKHR vk_surface;
	VkAllocationCallbacks* vk_allocator;
	VkDebugReportCallbackEXT vk_debug_report;

	bool enable_validation_layers;

	std::vector<const char*> device_extensions;
	std::vector<const char*> validation_layers;
};

