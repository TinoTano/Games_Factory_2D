#pragma once
#include "Module.h"
#include <vector>
#include "Application.h"

#define GLFW_INCLUDE_VULKAN
#include "ThirdParty/glfw-3.2.1/include/glfw3.h"

struct QueueFamily
{
	int graphics_index = 0;
	int present_index = 0;

	bool is_valid()
	{
		return graphics_index > 0 && present_index > 0;
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

	void CompileShader(std::string shader_file_path);

	void RecreateSwapChain();

private:

	bool SetupDebugDrawCall();
	bool CreateVulkanSurface();
	
	bool CreateLogicalDevice();
	bool CreateSwapChain();
	void CreateImageViews();
	bool CreateRenderPass();
	bool CreateGraphicsPipeline();
	bool CreateFramebuffers();
	bool CreateCommandPool();
	bool CreateVertexBuffer();
	bool CreateIndexBuffer();
	bool CreateCommandBuffers();
	bool CreateSemaphores();
	void CleanupSwapChain();

	bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamily FindQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	bool CheckValidationLayerSupport();
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> available_present_modes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	VkFormat FindDepthFormat();
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkShaderModule CreateShaderModule(std::string& shader_code);
	bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);
	uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

	void PrintVkResults(VkResult result);
	bool PrintVKDebugMessages(const char* msg);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
		return App->renderer_module->PrintVKDebugMessages(msg);
	}

private:
	
};

