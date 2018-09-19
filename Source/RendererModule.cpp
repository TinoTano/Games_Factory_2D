#include "RendererModule.h"
#include "Globals.h"
#include "WindowModule.h"
#include <set>
#include <algorithm>
#include <array>
#include "FileSystemModule.h"


RendererModule::RendererModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{

}

RendererModule::~RendererModule()
{
}

bool RendererModule::Init()
{
	

	return true;
}

bool RendererModule::PreUpdate(float delta_time)
{
	return true;
}

bool RendererModule::Update(float delta_time)
{
	return true;
}

bool RendererModule::PostUpdate(float delta_time)
{
	vkQueueWaitIdle(vk_presentation_queue);

	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(vk_logical_device, vk_swap_chain, std::numeric_limits<uint64_t>::max(), vk_image_available_semaphore, VK_NULL_HANDLE, &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return true;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		CONSOLE_ERROR("failed to acquire swap chain image!");
		return true;
	}

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = { vk_image_available_semaphore };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffers[image_index];

	VkSemaphore signal_semaphores[] = { vk_render_finished_semaphore };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	if (vkQueueSubmit(vk_graphics_queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to submit draw command buffer!");
		return true;
	}

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swap_chains[] = { vk_swap_chain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;

	present_info.pImageIndices = &image_index;

	result = vkQueuePresentKHR(vk_presentation_queue, &present_info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		CONSOLE_ERROR("failed to present swap chain image!");
		return true;
	}

	return true;
}

bool RendererModule::CleanUp()
{
#if _DEBUG
	PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugReportCallbackEXT");
	vkDestroyDebugReportCallbackEXT(vk_instance, vk_debug_report, vk_allocator);
#endif

	vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);

	vkDestroyDevice(vk_logical_device, nullptr);
	vkDestroyInstance(vk_instance, nullptr);
	return true;
}

bool RendererModule::InitVulkan()
{
	
}

bool RendererModule::CreateVulkanInstance()
{
	if (enable_validation_layers && !CheckValidationLayerSupport()) {
		return false;
	}

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Games Factory 2D";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 1, 73);
	app_info.pEngineName = "Games Factory 2D";
	app_info.engineVersion = VK_MAKE_VERSION(1, 1, 73);
	app_info.apiVersion = VK_API_VERSION_1_1;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	VkInstanceCreateInfo instance_create_info = {};
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pApplicationInfo = &app_info;
	instance_create_info.enabledExtensionCount = glfwExtensionCount;
	instance_create_info.ppEnabledExtensionNames = glfwExtensions;
	instance_create_info.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&instance_create_info, vk_allocator, &vk_instance);
	if (result)
	{
		PrintVkResults(result);
		return false;
	}

	return true;
}

bool RendererModule::SetupDebugDrawCall()
{
	if (!enable_validation_layers) return false;

	VkDebugReportCallbackCreateInfoEXT debug_report_callback = {};
	debug_report_callback.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debug_report_callback.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	debug_report_callback.pfnCallback = DebugCallback;

	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
		(PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vk_instance, "vkCreateDebugReportCallbackEXT");

	VkResult result = vkCreateDebugReportCallbackEXT(vk_instance, &debug_report_callback, vk_allocator, &vk_debug_report);
	if (result != VK_SUCCESS)
	{
		PrintVkResults(result);
		return false;
	}

	return true;
}

bool RendererModule::CreateVulkanSurface()
{
	VkResult result = glfwCreateWindowSurface(vk_instance, App->window_module->engine_window, nullptr, &vk_surface);
	if (result != VK_SUCCESS)
	{
		PrintVkResults(result);
		return false;
	}
	return true;
}

bool RendererModule::PickPhysicalDevice()
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

	if (device_count == 0) {
		CONSOLE_ERROR("failed to find GPUs with Vulkan support!");
		return false;
	}

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(vk_instance, &device_count, devices.data());

	for (const auto& device : devices) {
		if (IsDeviceSuitable(device)) {
			vk_physical_device = device;
			break;
		}
	}

	if (vk_physical_device == VK_NULL_HANDLE) {
		CONSOLE_ERROR("failed to find a suitable GPU!");
		return false;
	}

	return true;
}

bool RendererModule::CreateLogicalDevice()
{
	QueueFamily family = FindQueueFamilies(vk_physical_device);

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	std::set<int> unique_queue_families = { family.graphics_index, family.present_index };

	float queue_priority = 1.0f;
	for (int queue_family : unique_queue_families) {
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = queue_family;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_priority;
		queue_create_infos.push_back(queue_create_info);
	}

	VkPhysicalDeviceFeatures device_features = {};
	device_features.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.pEnabledFeatures = &device_features;
	create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
	create_info.ppEnabledExtensionNames = device_extensions.data();

	if (enable_validation_layers) {
		create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();
	}
	else {
		create_info.enabledLayerCount = 0;
	}

	VkResult result = vkCreateDevice(vk_physical_device, &create_info, nullptr, &vk_logical_device);
	if (result != VK_SUCCESS)
	{
		PrintVkResults(result);
		return false;
	}

	vkGetDeviceQueue(vk_logical_device, family.graphics_index, 0, &vk_graphics_queue);
	vkGetDeviceQueue(vk_logical_device, family.present_index, 0, &vk_presentation_queue);

	return true;
}

bool RendererModule::CreateSwapChain()
{
	SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(vk_physical_device);

	VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
	VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities);

	uint32_t image_Count = swap_chain_support.capabilities.minImageCount + 1;
	if (swap_chain_support.capabilities.maxImageCount > 0 && image_Count > swap_chain_support.capabilities.maxImageCount) {
		image_Count = swap_chain_support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = vk_surface;
	create_info.minImageCount = image_Count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamily indices = FindQueueFamilies(vk_physical_device);
	uint32_t queue_family_indices[] = { (uint32_t)indices.graphics_index, (uint32_t)indices.present_index };

	if (indices.graphics_index != indices.present_index) {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	}
	else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	create_info.preTransform = swap_chain_support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(vk_logical_device, &create_info, nullptr, &vk_swap_chain) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to create swap chain!");
		return false;
	}

	vkGetSwapchainImagesKHR(vk_logical_device, vk_swap_chain, &image_Count, nullptr);
	swap_chain_images.resize(image_Count);
	vkGetSwapchainImagesKHR(vk_logical_device, vk_swap_chain, &image_Count, swap_chain_images.data());

	vk_swap_chain_image_format = surface_format.format;
	vk_swap_chain_extent = extent;

	return true;
}

bool RendererModule::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamily family = FindQueueFamilies(device);
	if (!family.is_valid()) return false;

	bool extensions_supported = CheckDeviceExtensionSupport(device);
	if (!extensions_supported) return false;

	if (extensions_supported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		bool swap_chain_adequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		if (!swap_chain_adequate) return false;
	}

	VkPhysicalDeviceFeatures supported_features;
	vkGetPhysicalDeviceFeatures(device, &supported_features);
	if (!supported_features.samplerAnisotropy) return false;

	return true;
}

QueueFamily RendererModule::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamily ret;

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

	int i = 0;
	for (const auto& queue_family : queue_families) {
		if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			ret.graphics_index = i;
		}

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_surface, &present_support);

		if (queue_family.queueCount > 0 && present_support) {
			ret.present_index = i;
		}

		if (ret.is_valid()) {
			break;
		}

		i++;
	}

	return ret;
}

SwapChainSupportDetails RendererModule::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk_surface, &details.capabilities);

	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &format_count, nullptr);

	if (format_count != 0) {
		details.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &format_count, details.formats.data());
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface, &present_mode_count, nullptr);

	if (present_mode_count != 0) {
		details.presentModes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface, &present_mode_count, details.presentModes.data());
	}

	return details;
}

bool RendererModule::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
	
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

	std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

	for (const auto& extension : available_extensions) {
		required_extensions.erase(extension.extensionName);
	}

	return required_extensions.empty();
}

bool RendererModule::CheckValidationLayerSupport() {
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> available_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

	const char* layers[] = { "VK_LAYER_LUNARG_standard_validation" };

	for (const char* layer_name : layers) {
		bool layer_found = false;

		for (const auto& layer_properties : available_layers) {
			if (strcmp(layer_name, layer_properties.layerName) == 0) {
				layer_found = true;
				break;
			}
		}

		if (!layer_found) {
			return false;
		}
	}

	return true;
}

VkSurfaceFormatKHR RendererModule::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
	if (available_formats.size() == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& format : available_formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	return available_formats[0];
}

VkPresentModeKHR RendererModule::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> available_present_modes)
{
	VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& present_mode : available_present_modes) {
		if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return present_mode;
		}
		else if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			best_mode = present_mode;
		}
	}

	return best_mode;
}

VkExtent2D RendererModule::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetWindowSize(App->window_module->engine_window, &width, &height);

		VkExtent2D actual_extent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
		actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

		return actual_extent;
	}
}

void RendererModule::CreateImageViews()
{
	swap_chain_image_views.resize(swap_chain_images.size());

	for (uint32_t i = 0; i < swap_chain_images.size(); i++) {
		swap_chain_image_views[i] = CreateImageView(swap_chain_images[i], vk_swap_chain_image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

bool RendererModule::PrintVKDebugMessages(const char * msg)
{
	CONSOLE_ERROR("validation layer: %s", msg);

	return VK_FALSE;
}

void RendererModule::CompileShader(std::string shader_file_path)
{
	/*int result = system((GLSL_VALIDATOR_COMMAND + shader_file_path).c_str());
	int s;*/
}

void RendererModule::RecreateSwapChain()
{
	int width, height;
	glfwGetWindowSize(App->window_module->engine_window, &width, &height);
	if (width == 0 || height == 0) return;

	vkDeviceWaitIdle(vk_logical_device);

	CleanupSwapChain();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateDepthResources();
	CreateFramebuffers();
	CreateCommandBuffers();
}

VkImageView RendererModule::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;
	view_info.subresourceRange.aspectMask = aspectFlags;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = mipLevels;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	VkImageView image_view;
	if (vkCreateImageView(vk_logical_device, &view_info, nullptr, &image_view) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to create texture image view!");
	}

	return image_view;
}

bool RendererModule::CreateRenderPass()
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = vk_swap_chain_image_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depth_attachment = {};
	depth_attachment.format = FindDepthFormat();
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { color_attachment, depth_attachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(vk_logical_device, &renderPassInfo, nullptr, &vk_render_pass) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to create render pass!");
		return false;
	}

	return true;
}

VkFormat RendererModule::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

VkFormat RendererModule::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	VkFormat ret = VkFormat::VK_FORMAT_UNDEFINED;

	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(vk_physical_device, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	CONSOLE_ERROR("failed to find supported format!");
	return ret;
}

VkShaderModule RendererModule::CreateShaderModule(std::string& shader_code)
{
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = shader_code.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

	VkShaderModule shader_module;
	if (vkCreateShaderModule(vk_logical_device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to create shader module!");
	}

	return shader_module;
}

bool RendererModule::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & buffer_memory)
{
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(vk_logical_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to create buffer!");
		return false;
	}

	VkMemoryRequirements mem_requirements;
	vkGetBufferMemoryRequirements(vk_logical_device, buffer, &mem_requirements);
	uint32_t mem_type = FindMemoryType(mem_requirements.memoryTypeBits, properties);
	if (mem_type == -1) return;

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = mem_requirements.size;
	allocInfo.memoryTypeIndex = mem_type;

	if (vkAllocateMemory(vk_logical_device, &allocInfo, nullptr, &buffer_memory) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to allocate buffer memory!");
		return false;
	}

	vkBindBufferMemory(vk_logical_device, buffer, buffer_memory, 0);

	return true;
}

uint32_t RendererModule::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties mem_properties;
	vkGetPhysicalDeviceMemoryProperties(vk_physical_device, &mem_properties);

	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
		if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	CONSOLE_ERROR("failed to find suitable memory type!");
	return -1;
}

void RendererModule::copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
	VkCommandBuffer command_buffer = beginSingleTimeCommands();

	VkBufferCopy copy_region = {};
	copy_region.size = size;
	vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

	endSingleTimeCommands(command_buffer);
}

bool RendererModule::CreateGraphicsPipeline()
{
	
}

bool RendererModule::CreateFramebuffers()
{
	swap_chain_framebuffers.resize(swap_chain_image_views.size());

	for (size_t i = 0; i < swap_chain_image_views.size(); i++) {
		VkImageView attachments[] = {
			swap_chain_image_views[i]
		};

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = vk_render_pass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = vk_swap_chain_extent.width;
		framebuffer_info.height = vk_swap_chain_extent.height;
		framebuffer_info.layers = 1;

		if (vkCreateFramebuffer(vk_logical_device, &framebuffer_info, nullptr, &swap_chain_framebuffers[i]) != VK_SUCCESS) {
			CONSOLE_ERROR("failed to create framebuffer!");
			return false;
		}
	}
	return true;
}

bool RendererModule::CreateCommandPool()
{
	QueueFamily queueFamilyIndices = FindQueueFamilies(vk_physical_device);

	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = queueFamilyIndices.graphics_index;

	if (vkCreateCommandPool(vk_logical_device, &pool_info, nullptr, &vk_command_pool) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to create graphics command pool!");
		return false;
	}
	return true;
}

bool RendererModule::CreateVertexBuffer()
{
	VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	if (!CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory))
	{
		CONSOLE_ERROR("failed to create vertex staging buffer");
		return false;
	}

	void* data;
	vkMapMemory(vk_logical_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, vertices.data(), (size_t)buffer_size);
	vkUnmapMemory(vk_logical_device, staging_buffer_memory);

	if(!CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory))
	{
		CONSOLE_ERROR("failed to create vertex buffer");
		return false;
	}

	CopyBuffer(staging_buffer, vertexBuffer, buffer_size);

	vkDestroyBuffer(vk_logical_device, staging_buffer, nullptr);
	vkFreeMemory(vk_logical_device, staging_buffer_memory, nullptr);

	return true;
}

bool RendererModule::CreateIndexBuffer()
{
	VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	if(!CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory))
	{
		CONSOLE_ERROR("failed to create index staging buffer");
		return false;
	}

	void* data;
	vkMapMemory(vk_logical_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, indices.data(), (size_t)buffer_size);
	vkUnmapMemory(vk_logical_device, staging_buffer_memory);

	if(!CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory))
	{
		CONSOLE_ERROR("failed to create index buffer");
		return false;
	}

	copyBuffer(staging_buffer, indexBuffer, buffer_size);

	vkDestroyBuffer(vk_logical_device, staging_buffer, nullptr);
	vkFreeMemory(vk_logical_device, staging_buffer_memory, nullptr);

	return true;
}

bool RendererModule::CreateCommandBuffers()
{
	command_buffers.resize(swap_chain_framebuffers.size());

	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = vk_command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = (uint32_t)command_buffers.size();

	if (vkAllocateCommandBuffers(vk_logical_device, &alloc_info, command_buffers.data()) != VK_SUCCESS) {
		CONSOLE_ERROR("failed to allocate command buffers!");
		return false;
	}

	for (size_t i = 0; i < command_buffers.size(); i++) {
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(command_buffers[i], &begin_info) != VK_SUCCESS) {
			CONSOLE_ERROR("failed to begin recording command buffer!");
			return false;
		}

		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = vk_render_pass;
		render_pass_info.framebuffer = swap_chain_framebuffers[i];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = vk_swap_chain_extent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		render_pass_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
		render_pass_info.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphics_pipeline);

		VkBuffer vertex_buffers[] = { vertex_buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffers[i], 0, 1, vertex_buffers, offsets);

		vkCmdBindIndexBuffer(command_buffers[i], index_buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline_layout, 0, 1, &vk_descriptor_set, 0, nullptr);

		vkCmdDrawIndexed(command_buffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(command_buffers[i]);

		if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
			CONSOLE_ERROR("failed to record command buffer!");
			return false;
		}
	}

	return true;
}

bool RendererModule::CreateSemaphores()
{
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(vk_logical_device, &semaphore_info, nullptr, &vk_image_available_semaphore) != VK_SUCCESS ||
		vkCreateSemaphore(vk_logical_device, &semaphore_info, nullptr, &vk_render_finished_semaphore) != VK_SUCCESS) {

		CONSOLE_ERROR("failed to create semaphores!");
		return false;
	}

	return true;
}

void RendererModule::CleanupSwapChain()
{
	for (size_t i = 0; i < swap_chain_framebuffers.size(); i++) {
		vkDestroyFramebuffer(vk_logical_device, swap_chain_framebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(vk_logical_device, vk_command_pool, static_cast<uint32_t>(command_buffers.size()), command_buffers.data());

	vkDestroyPipeline(vk_logical_device, vk_graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(vk_logical_device, vk_pipeline_layout, nullptr);
	vkDestroyRenderPass(vk_logical_device, vk_render_pass, nullptr);

	for (size_t i = 0; i < swap_chain_image_views.size(); i++) {
		vkDestroyImageView(vk_logical_device, swap_chain_image_views[i], nullptr);
	}

	vkDestroySwapchainKHR(vk_logical_device, vk_swap_chain, nullptr);
}
