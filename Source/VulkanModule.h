#pragma once
#include "Module.h"
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

class Texture;

class VulkanModule : public Module
{
public:

	struct QueueFamily
	{
		std::optional<uint32_t> graphicsIndex;
		std::optional<uint32_t> presentIndex;

		bool IsValid()
		{
			return graphicsIndex.has_value() && presentIndex.has_value();
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
	bool PrintVKDebugMessages(const char * msg);

	void InitImGui();
	void CreateTexture(Texture& texture, void* pixels);

	void DestroyTexture(Texture& texture);

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
	bool CreateDescriptorSetLayout();
	bool CreateGraphicsPipeline();
	bool CreateCommandPool();
	bool CreateDepthResources();
	bool CreateFramebuffers();
	bool CreateCommandBuffers();
	bool CreateSyncObjects();

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

	bool CreateDebugVertexBuffer();

	bool CreateIndexBuffer();

	bool CreateUniformBuffer();

	bool CreateDescriptorPool();

	bool CreateDescriptorSets();
    
    bool UpdateCommandBuffers();

	void UpdateUniformBuffer(uint32_t imageIndex);

	//Return better swap chain format from available formats
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	//Return better swap chain presentation mode from available modes
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

	//Return swap chain extent
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	VkFormat FindDepthFormat();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkShaderModule CreateShaderModule(std::string shaderCode);

	bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkCommandBuffer BeginSingleTimeCommands();

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void RecreateSwapChain(VkSwapchainKHR swapChain);
	void CleanupSwapChain(VkSwapchainKHR swapCshain);

	void PrintVkResults(VkResult result);

	void UpdateDescriptorSets();

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

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

	VkDebugUtilsMessengerEXT callback;

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
	VkPipeline vkDebugPipeline;
	VkCommandPool vkCommandPool;

	bool enableValidationLayers;

	std::vector<const char*> deviceExtensions;
	std::vector<const char*> validationLayers;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkDescriptorSet> vkDescriptorSets;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorImageInfo> samplerDescriptors;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceSize vertexBufferSize;

	VkBuffer debugVertexBuffer;
	VkDeviceMemory debugVertexBufferMemory;
	VkDeviceSize debugVertexBufferSize;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<VkSemaphore> vkImageAvailableSemaphores;
	std::vector<VkSemaphore> vkRenderFinishedSemaphores;
	std::vector<VkFence> vkInFlightFences;

	VkDescriptorPool vkDescriptorPool;

	int currentFrame = 0;
};

