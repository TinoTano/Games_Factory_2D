#pragma once
#include "Module.h"
#include "ThirdParty\Vulkan_1.1.82.1\Include\vulkan\vulkan.hpp"
#include <vector>

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
	std::vector<VkPresentModeKHR> present_modes;
};

class VulkanModule : public Module
{
public:
	VulkanModule(const char* module_name, bool game_module = false);
	~VulkanModule();

	bool Init();
	bool CleanUp();

private:

	bool InitVulkan();

	bool CreateVulkanInstance();
	bool SetupDebugDrawCall();
	bool CreateVulkanSurface();
	bool PickPhysicalDevice();
	bool CreateLogicalDevice();
	bool CreateSwapChain();
	void CreateImageViews();
	bool CreateRenderPass();
	bool CreateGraphicsPipeline();
	bool CreateCommandPool();
	bool CreateDepthResources();
	bool CreateFramebuffers();
	bool CreateCommandBuffers();
	bool CreateSemaphores();

	//Check if gpu is good for us
	bool IsDeviceSuitable(VkPhysicalDevice device);

	//Find the queue family that support graphics and presentation
	QueueFamily FindQueueFamilies(VkPhysicalDevice device);

	//Check if gpu supports swap chain configuration
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	//Check if gpu supports necessary extensions
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	//Return better swap chain format from available formats
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);

	//Return better swap chain presentation mode from available modes 
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> available_present_modes);

	//Return swap chain extent
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	VkFormat FindDepthFormat();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkShaderModule CreateShaderModule(std::string& shader_code);

	bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);

	uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

	void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

	VkCommandBuffer BeginSingleTimeCommands();

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	bool CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void RecreateSwapChain(VkSwapchainKHR swap_chain);
	void CleanupSwapChain(VkSwapchainKHR swap_chain);

	void PrintVkResults(VkResult result);

private:
	//The instance is the connection between your application and the Vulkan library
	VkInstance vk_instance;

	//Physical Device is the GPU used
	VkPhysicalDevice vk_physical_device;

	//Interface with the physical device
	VkDevice vk_logical_device;

	//graphics queue from logical device
	VkQueue vk_graphics_queue;

	//presentation queue from logical device
	VkQueue vk_presentation_queue;

	//Surface where the images are rendered
	VkSurfaceKHR vk_surface;

	VkAllocationCallbacks* vk_allocator;
	VkDebugReportCallbackEXT vk_debug_report;

	//The swap chain is essentially a queue of images that are waiting to be presented to the screen
	VkSwapchainKHR vk_swap_chain;

	//Format of the images in the swap chain
	VkFormat vk_swap_chain_image_format;

	//Swap chain extent (x,y,w,h)
	VkExtent2D vk_swap_chain_extent;

	//we need to tell Vulkan about the framebuffer attachments that will be used while rendering. 
	//We need to specify how many color and depth buffers there will be, how many samples to use for each of them and how their contents should be handled 
	//throughout the rendering operations. All of this information is wrapped in a render pass object
	VkRenderPass vk_render_pass;

	VkDescriptorSetLayout vk_descriptor_set_layout;
	VkPipelineLayout vk_pipeline_layout;
	VkPipeline vk_graphics_pipeline;
	VkCommandPool vk_command_pool;
	VkSemaphore vk_image_available_semaphore;
	VkSemaphore vk_render_finished_semaphore;

	bool enable_validation_layers;

	std::vector<const char*> device_extensions;
	std::vector<const char*> validation_layers;
	std::vector<VkImage> swap_chain_images;
	std::vector<VkImageView> swap_chain_image_views;
	std::vector<VkFramebuffer> swap_chain_framebuffers;
	std::vector<VkCommandBuffer> command_buffers;
};

