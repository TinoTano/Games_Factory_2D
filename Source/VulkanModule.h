#pragma once
#include "Module.h"
#include <vector>
#include "Application.h"

#define GLFW_INCLUDE_VULKAN
#include "ThirdParty/glfw-3.2.1/include/glfw3.h"

class VulkanModule : public Module
{
public:

	struct QueueFamily
	{
		int graphicsIndex = 0;
		int presentIndex = 0;

		bool IsValid()
		{
			return graphicsIndex > 0 && presentIndex > 0;
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	VulkanModule(const char* moduleName, bool gameModule = false);
	~VulkanModule();

	bool Init();
	bool CleanUp();

	bool Render();

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

	bool CheckValidationLayerSupport();

	bool CreateVertexBuffer();

	bool CreateIndexBuffer();

	//Return better swap chain format from available formats
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	//Return better swap chain presentation mode from available modes 
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

	//Return swap chain extent
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	VkFormat FindDepthFormat();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkShaderModule CreateShaderModule(std::string& shaderCode);

	bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkCommandBuffer BeginSingleTimeCommands();

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	bool CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void RecreateSwapChain(VkSwapchainKHR swapChain);
	void CleanupSwapChain(VkSwapchainKHR swapCshain);

	void PrintVkResults(VkResult result);

	bool PrintVKDebugMessages(const char* msg);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
		return App->vulkanModule->PrintVKDebugMessages(msg);
	}

private:
	//The instance is the connection between your application and the Vulkan library
	VkInstance vkInstance;

	//Physical Device is the GPU used
	VkPhysicalDevice vkPhysicalDevice;

	//Interface with the physical device
	VkDevice vkLogicalDevice;

	//graphics queue from logical device
	VkQueue vkGraphicsQueue;

	//presentation queue from logical device
	VkQueue vkPresentationQueue;

	//Surface where the images are rendered
	VkSurfaceKHR vkSurface;

	VkAllocationCallbacks* vkAllocator;
	VkDebugReportCallbackEXT vkDebugReport;

	//The swap chain is essentially a queue of images that are waiting to be presented to the screen
	VkSwapchainKHR vkSwapchain;

	//Format of the images in the swap chain
	VkFormat vkSwapchainImageFormat;

	//Swap chain extent (x,y,w,h)
	VkExtent2D vkSwapchainExtent;

	//we need to tell Vulkan about the framebuffer attachments that will be used while rendering. 
	//We need to specify how many color and depth buffers there will be, how many samples to use for each of them and how their contents should be handled 
	//throughout the rendering operations. All of this information is wrapped in a render pass object
	VkRenderPass vkRenderPass;

	VkDescriptorSetLayout vkDescriptorSetLayout;
	VkPipelineLayout vkPipelineLayout;
	VkPipeline vkGraphicsPipeline;
	VkCommandPool vkCommandPool;
	VkSemaphore vkImageAvailableSemaphore;
	VkSemaphore vkRenderFinishedSemaphore;

	bool enableValidationLayers;

	std::vector<const char*> deviceExtensions;
	std::vector<const char*> validationLayers;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkDescriptorSet> vkDescriptorSets;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
};

