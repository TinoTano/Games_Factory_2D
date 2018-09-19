#include "VulkanModule.h"
#include "ThirdParty\glfw-3.2.1\include\glfw3.h"
#include "Globals.h"
#include "Application.h"
#include "WindowModule.h"
#include <set>
#include <algorithm>
#include <array>
#include "FileSystemModule.h"

VulkanModule::VulkanModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
	vk_physical_device = VK_NULL_HANDLE;
	vk_instance = VK_NULL_HANDLE;
	vk_logical_device = VK_NULL_HANDLE;
	vk_graphics_queue = VK_NULL_HANDLE;
	vk_presentation_queue = VK_NULL_HANDLE;
	vk_allocator = VK_NULL_HANDLE;
	vk_debug_report = VK_NULL_HANDLE;

#if NDEBUG
	enable_validation_layers = false;
#else
	enable_validation_layers = true;
#endif

	device_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	validation_layers.emplace_back("VK_LAYER_LUNARG_standard_validation");
}


VulkanModule::~VulkanModule()
{
}

bool VulkanModule::Init()
{
	if (!glfwVulkanSupported())
	{
		CONSOLE_ERROR("Vulkan not supported!");
		return false;
	}

	if (!InitVulkan())
	{
		return false;
	}

	return true;
}

bool VulkanModule::CleanUp()
{
	return true;
}

bool VulkanModule::InitVulkan()
{
	if (!CreateVulkanInstance()) return false;
	if (!SetupDebugDrawCall()) return false;
	if (!CreateVulkanSurface()) return false;
	if (!PickPhysicalDevice()) return false;
	if (!CreateLogicalDevice()) return false;
	if (!CreateSwapChain()) return false;
	CreateImageViews();
	if (!CreateRenderPass()) return false;
	if (!CreateGraphicsPipeline()) return false;
	if (!CreateFramebuffers()) return false;
	if (!CreateCommandPool()) return false;
	if (!CreateVertexBuffer()) return false;
	if (!CreateCommandBuffers()) return false;
	if (!CreateSemaphores()) return false;

	return true;
}

bool VulkanModule::CreateVulkanInstance()
{
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;
	app_info.pApplicationName = "Games Factory 2D";
	app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	app_info.pEngineName = "Games Factory 2D";
	app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	app_info.apiVersion = VK_API_VERSION_1_1;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	VkInstanceCreateInfo instance_info;
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pNext = nullptr;
	instance_info.flags = 0;
	instance_info.pApplicationInfo = &app_info;
	instance_info.enabledExtensionCount = glfwExtensionCount;
	instance_info.ppEnabledExtensionNames = glfwExtensions;
	instance_info.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&instance_info, vk_allocator, &vk_instance);

	if (result != VK_SUCCESS)
	{
		PrintVkResults(result);
		return false;
	}

	return true;
}

bool VulkanModule::SetupDebugDrawCall()
{
	if (enable_validation_layers)
	{
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
	}

	return true;
}

bool VulkanModule::CreateVulkanSurface()
{
	VkResult result = glfwCreateWindowSurface(vk_instance, App->window_module->engine_window, nullptr, &vk_surface);
	if (result != VK_SUCCESS)
	{
		PrintVkResults(result);
		return false;
	}
	return true;
}

bool VulkanModule::PickPhysicalDevice()
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

bool VulkanModule::CreateLogicalDevice()
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

bool VulkanModule::CreateSwapChain()
{
	SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(vk_physical_device);

	VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
	VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes);
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

	VkResult result = vkCreateSwapchainKHR(vk_logical_device, &create_info, nullptr, &vk_swap_chain);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to create swap chain!");
		PrintVkResults(result);
		return false;
	}

	result = vkGetSwapchainImagesKHR(vk_logical_device, vk_swap_chain, &image_Count, nullptr);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to get swap chain images HKR!");
		PrintVkResults(result);
		return false;
	}
	swap_chain_images.resize(image_Count);
	result = vkGetSwapchainImagesKHR(vk_logical_device, vk_swap_chain, &image_Count, swap_chain_images.data());
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to get swap chain images HKR!");
		PrintVkResults(result);
		return false;
	}

	vk_swap_chain_image_format = surface_format.format;
	vk_swap_chain_extent = extent;

	return true;
}

void VulkanModule::CreateImageViews()
{
	swap_chain_image_views.resize(swap_chain_images.size());

	for (uint32_t i = 0; i < swap_chain_images.size(); i++) {
		swap_chain_image_views[i] = CreateImageView(swap_chain_images[i], vk_swap_chain_image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

bool VulkanModule::CreateRenderPass()
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

	VkResult result = vkCreateRenderPass(vk_logical_device, &renderPassInfo, nullptr, &vk_render_pass);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to create render pass!");
		PrintVkResults(result);
		return false;
	}

	return true;
}

bool VulkanModule::CreateGraphicsPipeline()
{
	auto vertShaderCode = App->file_system_module->LoadBinaryTextFile(DEFAULT_VERTEX_SHADER_PATH);
	auto fragShaderCode = App->file_system_module->LoadBinaryTextFile(DEFAULT_FRAGMENT_SHADER_PATH);

	VkShaderModule vert_shader_module = CreateShaderModule(vertShaderCode);
	VkShaderModule frag_shader_module = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
	vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_info.module = vert_shader_module;
	vert_shader_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
	frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_info.module = frag_shader_module;
	frag_shader_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vert_shader_stage_info, frag_shader_stage_info };

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertex_input_info.vertexBindingDescriptionCount = 1;
	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertex_input_info.pVertexBindingDescriptions = &bindingDescription;
	vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vk_swap_chain_extent.width;
	viewport.height = (float)vk_swap_chain_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = vk_swap_chain_extent;

	VkPipelineViewportStateCreateInfo viewport_state = {};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.pViewports = &viewport;
	viewport_state.scissorCount = 1;
	viewport_state.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
	depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil.depthTestEnable = VK_TRUE;
	depth_stencil.depthWriteEnable = VK_TRUE;
	depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil.depthBoundsTestEnable = VK_FALSE;
	depth_stencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo color_blending = {};
	color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable = VK_FALSE;
	color_blending.logicOp = VK_LOGIC_OP_COPY;
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &color_blend_attachment;
	color_blending.blendConstants[0] = 0.0f;
	color_blending.blendConstants[1] = 0.0f;
	color_blending.blendConstants[2] = 0.0f;
	color_blending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 1;
	pipeline_layout_info.pSetLayouts = &vk_descriptor_set_layout;

	VkResult result = vkCreatePipelineLayout(vk_logical_device, &pipeline_layout_info, nullptr, &vk_pipeline_layout);
	if(result != VK_SUCCESS) {
		CONSOLE_ERROR("failed to create pipeline layout!");
		PrintVkResults(result);
		return false;
	}

	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shaderStages;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterizer;
	pipeline_info.pMultisampleState = &multisampling;
	pipeline_info.pDepthStencilState = &depth_stencil;
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.layout = vk_pipeline_layout;
	pipeline_info.renderPass = vk_render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	result = vkCreateGraphicsPipelines(vk_logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &vk_graphics_pipeline);
	if(result != VK_SUCCESS) {
		CONSOLE_ERROR("failed to create graphics pipeline!");
		PrintVkResults(result);
		return false;
	}

	vkDestroyShaderModule(vk_logical_device, frag_shader_module, nullptr);
	vkDestroyShaderModule(vk_logical_device, vert_shader_module, nullptr);

	return true;
}

bool VulkanModule::CreateFramebuffers()
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

bool VulkanModule::CreateCommandPool()
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

bool VulkanModule::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();

	CreateImage(vk_swap_chain_extent.width, vk_swap_chain_extent.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

bool VulkanModule::CreateCommandBuffers()
{
	command_buffers.resize(swap_chain_framebuffers.size());

	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = vk_command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = (uint32_t)command_buffers.size();

	VkResult result = vkAllocateCommandBuffers(vk_logical_device, &alloc_info, command_buffers.data());
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to allocate command buffers!");
		PrintVkResults(result);
		return false;
	}

	for (size_t i = 0; i < command_buffers.size(); i++) {
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VkResult result = vkBeginCommandBuffer(command_buffers[i], &begin_info);
		if (result != VK_SUCCESS)
		{
			CONSOLE_ERROR("failed to begin recording command buffer!");
			PrintVkResults(result);
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

		VkResult result = vkEndCommandBuffer(command_buffers[i]);
		if (result != VK_SUCCESS)
		{
			CONSOLE_ERROR("failed to record command buffer!");
			PrintVkResults(result);
			return false;
		}
	}

	return true;
}

bool VulkanModule::CreateSemaphores()
{

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkResult result = vkCreateSemaphore(vk_logical_device, &semaphore_info, nullptr, &vk_image_available_semaphore);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to create image available semaphores!");
		PrintVkResults(result);
		return false;
	}

	VkResult result = vkCreateSemaphore(vk_logical_device, &semaphore_info, nullptr, &vk_render_finished_semaphore);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to create render finished semaphores!");
		PrintVkResults(result);
		return false;
	}

	return true;
}

bool VulkanModule::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamily family = FindQueueFamilies(device);
	if (!family.is_valid()) return false;

	bool extensions_supported = CheckDeviceExtensionSupport(device);
	if (!extensions_supported) return false;

	if (extensions_supported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		bool swap_chain_adequate = !swapChainSupport.formats.empty() && !swapChainSupport.present_modes.empty();
		if (!swap_chain_adequate) return false;
	}

	VkPhysicalDeviceFeatures supported_features;
	vkGetPhysicalDeviceFeatures(device, &supported_features);
	if (!supported_features.samplerAnisotropy) return false;

	return true;
}

QueueFamily VulkanModule::FindQueueFamilies(VkPhysicalDevice device)
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

SwapChainSupportDetails VulkanModule::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk_surface, &details.capabilities);
	if (result == VK_SUCCESS)
	{
		uint32_t format_count;
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &format_count, nullptr);
		if (result == VK_SUCCESS)
		{
			if (format_count != 0) {
				details.formats.resize(format_count);
				result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &format_count, details.formats.data());
				if (result == VK_SUCCESS)
				{
					uint32_t present_mode_count;
					result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface, &present_mode_count, nullptr);
					if (result == VK_SUCCESS)
					{
						if (present_mode_count != 0) {
							details.present_modes.resize(present_mode_count);
							result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface, &present_mode_count, details.present_modes.data());
						}
					}
				}
			}
		}
	}

	if (result != VK_SUCCESS)
	{
		PrintVkResults(result);
	}

	return details;
}

bool VulkanModule::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
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

VkSurfaceFormatKHR VulkanModule::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
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

VkPresentModeKHR VulkanModule::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> available_present_modes)
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

VkExtent2D VulkanModule::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
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

VkImageView VulkanModule::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
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
	VkResult result = vkCreateImageView(vk_logical_device, &view_info, nullptr, &image_view);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to create texture image view!");
		PrintVkResults(result);
		return false;
	}
	return image_view;
}

VkFormat VulkanModule::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

VkFormat VulkanModule::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
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

VkShaderModule VulkanModule::CreateShaderModule(std::string & shader_code)
{
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = shader_code.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

	VkShaderModule shader_module;
	VkResult result = vkCreateShaderModule(vk_logical_device, &create_info, nullptr, &shader_module);
	if(result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to create shader module!");
		PrintVkResults(result);
	}

	return shader_module;
}

bool VulkanModule::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & buffer_memory)
{
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(vk_logical_device, &buffer_info, nullptr, &buffer);
	if (result != VK_SUCCESS) 
	{
		CONSOLE_ERROR("failed to create buffer!");
		PrintVkResults(result);
		return false;
	}

	VkMemoryRequirements mem_requirements;
	vkGetBufferMemoryRequirements(vk_logical_device, buffer, &mem_requirements);
	uint32_t mem_type = FindMemoryType(mem_requirements.memoryTypeBits, properties);
	if (mem_type == -1) return;

	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = mem_type;

	VkResult result = vkAllocateMemory(vk_logical_device, &alloc_info, nullptr, &buffer_memory);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to allocate buffer memory!");
		PrintVkResults(result);
		return false;
	}

	vkBindBufferMemory(vk_logical_device, buffer, buffer_memory, 0);

	return true;
}

uint32_t VulkanModule::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
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

void VulkanModule::CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
	VkCommandBuffer command_buffer = BeginSingleTimeCommands();

	VkBufferCopy copy_region = {};
	copy_region.size = size;
	vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

	EndSingleTimeCommands(command_buffer);
}

VkCommandBuffer VulkanModule::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = vk_command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	VkResult result = vkAllocateCommandBuffers(vk_logical_device, &alloc_info, &commandBuffer);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to allocate buffer memory!");
		PrintVkResults(result);
		return false;
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanModule::EndSingleTimeCommands(VkCommandBuffer command_buffer)
{
	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vkQueueSubmit(vk_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(vk_graphics_queue);

	vkFreeCommandBuffers(vk_logical_device, vk_command_pool, 1, &command_buffer);
}

bool VulkanModule::CreateImage(uint32_t width, uint32_t height, uint32_t mipmap_levels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & image_memory)
{
	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = mipmap_levels;
	image_info.arrayLayers = 1;
	image_info.format = format;
	image_info.tiling = tiling;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = usage;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateImage(vk_logical_device, &image_info, nullptr, &image);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to create image!");
		PrintVkResults(result);
		return false;
	}

	VkMemoryRequirements mem_requirements;
	vkGetImageMemoryRequirements(vk_logical_device, image, &mem_requirements);

	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

	VkResult result = vkAllocateMemory(vk_logical_device, &alloc_info, nullptr, &image_memory);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to allocate image memory!");
		PrintVkResults(result);
		return false;
	}

	VkResult result = vkBindImageMemory(vk_logical_device, image, image_memory, 0);
	if (result != VK_SUCCESS)
	{
		CONSOLE_ERROR("failed to bind image!");
		PrintVkResults(result);
		return false;
	}

	return true;
}

void VulkanModule::RecreateSwapChain(VkSwapchainKHR swap_chain)
{
	int width, height;
	glfwGetWindowSize(App->window_module->engine_window, &width, &height);
	if (width == 0 || height == 0) return;

	vkDeviceWaitIdle(vk_logical_device);

	CleanupSwapChain(swap_chain);

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateDepthResources();
	CreateFramebuffers();
	CreateCommandBuffers();
}

void VulkanModule::CleanupSwapChain(VkSwapchainKHR swap_chain)
{
	vkDestroyImageView(vk_logical_device, depthImageView, nullptr);
	vkDestroyImage(vk_logical_device, depthImage, nullptr);
	vkFreeMemory(vk_logical_device, depthImageMemory, nullptr);

	for (auto framebuffer : swap_chain_framebuffers) {
		vkDestroyFramebuffer(vk_logical_device, framebuffer, nullptr);
	}

	vkFreeCommandBuffers(vk_logical_device, vk_command_pool, static_cast<uint32_t>(command_buffers.size()), command_buffers.data());

	vkDestroyPipeline(vk_logical_device, vk_graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(vk_logical_device, vk_pipeline_layout, nullptr);
	vkDestroyRenderPass(vk_logical_device, vk_render_pass, nullptr);

	for (auto imageView : swap_chain_image_views) {
		vkDestroyImageView(vk_logical_device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(vk_logical_device, swap_chain, nullptr);
}

void VulkanModule::PrintVkResults(VkResult result)
{
	const char* message = "";

	switch (result)
	{
	case VK_NOT_READY:
		message = "A fence or query has not yet completed";
		break;
	case VK_TIMEOUT:
		message = "A wait operation has not completed in the specified time";
		break;
	case VK_EVENT_SET:
		message = "An event is signaled";
		break;
	case VK_EVENT_RESET:
		message = "An event is unsignaled";
		break;
	case VK_INCOMPLETE:
		message = "A return array was too small for the result";
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		message = "A host memory allocation has failed";
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		message = "A device memory allocation has failed";
		break;
	case VK_ERROR_INITIALIZATION_FAILED:
		message = "Initialization of an object could not be completed for implementation-specific reasons";
		break;
	case VK_ERROR_DEVICE_LOST:
		message = "The logical or physical device has been lost";
		break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		message = "Mapping of a memory object has failed";
		break;
	case VK_ERROR_LAYER_NOT_PRESENT:
		message = "A requested layer is not present or could not be loaded";
		break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		message = "A requested extension is not supported";
		break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
		message = "A requested feature is not supported";
		break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		message = "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons";
		break;
	case VK_ERROR_TOO_MANY_OBJECTS:
		message = "Too many objects of the type have already been created";
		break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		message = "A requested format is not supported on this device";
		break;
	case VK_ERROR_FRAGMENTED_POOL:
		message = R"(A pool allocation has failed due to fragmentation of the pool’s memory.
					This must only be returned if no attempt to allocate host or device memory was made to accomodate the new allocation. 
					This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation)";
		break;
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		message = R"(A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accomodate the new allocation. 
					If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead)";
		break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
		message = "An external handle is not a valid handle of the specified type";
		break;
	case VK_ERROR_SURFACE_LOST_KHR:
		message = "A surface is no longer available";
		break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		message = "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again";
		break;
	case VK_SUBOPTIMAL_KHR:
		message = "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully";
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		message = R"(A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. 
					Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface)";
		break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		message = "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image";
		break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
		message = "VK_ERROR_VALIDATION_FAILED_EXT";
		break;
	case VK_ERROR_INVALID_SHADER_NV:
		message = "One or more shaders failed to compile or link. More details are reported back to the application via ../../html/vkspec.html#VK_EXT_debug_report if enabled";
		break;
	case VK_ERROR_FRAGMENTATION_EXT:
		message = "A descriptor pool creation has failed due to fragmentation";
		break;
	case VK_ERROR_NOT_PERMITTED_EXT:
		message = "VK_ERROR_NOT_PERMITTED_EXT";
		break;
	case VK_RESULT_RANGE_SIZE:
		message = "VK_RESULT_RANGE_SIZE";
		break;
	default:
		break;
	}

	CONSOLE_ERROR(message);
}
