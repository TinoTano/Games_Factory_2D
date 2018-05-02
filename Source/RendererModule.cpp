#include "RendererModule.h"
#include "Globals.h"
#include "Application.h"
#include "WindowModule.h"
#include <set>

RendererModule::RendererModule(const char* module_name, bool game_module) : Module(module_name, game_module)
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

RendererModule::~RendererModule()
{
}

bool RendererModule::Init()
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

	glfwSwapBuffers(App->window_module->engine_window);
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
	if(!CreateVulkanInstance()) return false;
	if(!SetupDebugDrawCall()) return false;
	if(!CreateVulkanSurface()) return false;
	if(!PickPhysicalDevice()) return false;
	if(!CreateLogicalDevice()) return false;
	if(!CreateSwapChain()) return false;
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
}

bool RendererModule::SetupDebugDrawCall()
{
	if (!enable_validation_layers) return;

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

bool RendererModule::CreateVulkanSurface()
{
	VkResult result = glfwCreateWindowSurface(vk_instance, App->window_module->engine_window, nullptr, &vk_surface);
	if (result != VK_SUCCESS)
	{
		PrintVkResults(result);
		return false;
	}
}

bool RendererModule::PickPhysicalDevice()
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

	if (device_count == 0) {
		CONSOLE_ERROR("failed to find GPUs with Vulkan support!");
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
	}
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
}

bool RendererModule::CreateSwapChain()
{
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

void RendererModule::PrintVkResults(VkResult result)
{
	if (result == VK_SUCCESS)
		return;

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

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
	CONSOLE_ERROR("validation layer: %s", msg);

	return VK_FALSE;
}