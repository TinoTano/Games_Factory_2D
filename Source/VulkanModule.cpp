#include "VulkanModule.h"
#include "Globals.h"
#include "Application.h"
#include "WindowModule.h"
#include <set>
#include <algorithm>
#include <array>
#include "FileSystemModule.h"
#include "Vertex.h"
#include "ComponentSprite.h"
#include "SceneModule.h"
#include "GameObject.h"
#include "CameraModule.h"
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include "Texture.h"

#define GLFW_INCLUDE_NONE
#include <glfw3.h>

std::vector<uint16_t> indices = {
    0, 1, 2, 0, 2, 3
};

//std::vector<uint16_t> indices;

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

//ImGui
static void check_vk_result(VkResult err)
{
	if (err == 0) return;
	CONSOLE_ERROR("VkResult %d\n", err);
	if (err < 0)
		abort();
}

const int MAX_FRAMES_IN_FLIGHT = 2;

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    // Select prefix depending on flags passed to the callback
    // Note that multiple flags may be set for a single validation message
    std::string prefix("");
    
    // Error that may result in undefined behaviour
    if (pCallbackData->flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        prefix += "ERROR: ";
    };
    // Warnings may hint at unexpected / non-spec API usage
    if (pCallbackData->flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        prefix += "WARNING: ";
    };
    // May indicate sub-optimal usage of the API
    if (pCallbackData->flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        prefix += "PERFORMANCE: ";
    };
    // Informal messages that may become handy during debugging
    if (pCallbackData->flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        prefix += "INFO: ";
    }
    // Diagnostic info from the Vulkan loader and layers
    // Usually not helpful in terms of API usage, but may help to debug layer and loader problems
    if (pCallbackData->flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        prefix += "DEBUG: ";
    }
    
    prefix += pCallbackData->pMessage;
    return App->vulkanModule->PrintVKDebugMessages(prefix.c_str());
}

VulkanModule::VulkanModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
    vkInstance = VK_NULL_HANDLE;
    vkPhysicalDevice = VK_NULL_HANDLE;
    vkLogicalDevice = VK_NULL_HANDLE;
    vkGraphicsQueue = VK_NULL_HANDLE;
    vkPresentationQueue = VK_NULL_HANDLE;
    callback = VK_NULL_HANDLE;
    vkSurface = VK_NULL_HANDLE;
    vkSwapchain = VK_NULL_HANDLE;
    vkPipelineLayout = VK_NULL_HANDLE;
	vertexBuffer = VK_NULL_HANDLE;
	vertexBufferMemory = VK_NULL_HANDLE;
	indexBuffer = VK_NULL_HANDLE;
	indexBufferMemory = VK_NULL_HANDLE;
    
#ifdef NDEBUG
    enableValidationLayers = false;
#else
    enableValidationLayers = true;
#endif
    
    deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    validationLayers.emplace_back("VK_LAYER_LUNARG_standard_validation");
}

VulkanModule::~VulkanModule()
{
}

bool VulkanModule::Init()
{
    if (!glfwVulkanSupported())
    {
        const char* s = "Vulkan not supported!";
        CONSOLE_ERROR("%s", s);
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
    vkDeviceWaitIdle(vkLogicalDevice);
    
    CleanupSwapChain(vkSwapchain);
    
    vkDestroyDescriptorPool(vkLogicalDevice, vkDescriptorPool, nullptr);
    
    vkDestroyDescriptorSetLayout(vkLogicalDevice, vkDescriptorSetLayout, nullptr);
    
    for (size_t i = 0; i < swapchainImages.size(); i++) {
        vkDestroyBuffer(vkLogicalDevice, uniformBuffers[i], nullptr);
        vkFreeMemory(vkLogicalDevice, uniformBuffersMemory[i], nullptr);
    }
    
	if(indexBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(vkLogicalDevice, indexBuffer, nullptr);
	}
	if (indexBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(vkLogicalDevice, indexBufferMemory, nullptr);
	}
    
	if (vertexBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(vkLogicalDevice, vertexBuffer, nullptr);
	}
	if (vertexBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(vkLogicalDevice, vertexBufferMemory, nullptr);
	}
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vkLogicalDevice, vkRenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(vkLogicalDevice, vkImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(vkLogicalDevice, vkInFlightFences[i], nullptr);
    }
    
    vkDestroyCommandPool(vkLogicalDevice, vkCommandPool, nullptr);
    
    vkDestroyDevice(vkLogicalDevice, nullptr);
    
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(vkInstance, callback, nullptr);
    }
    
    vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
    
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
    if (!CreateDescriptorSetLayout()) return false;
    if (!CreateGraphicsPipeline()) return false;
    if (!CreateFramebuffers()) return false;
    if (!CreateCommandPool()) return false;
    //if (!CreateVertexBuffer()) return false;
    if (!CreateIndexBuffer()) return false;
    if (!CreateUniformBuffer()) return false;
    if (!CreateDescriptorPool()) return false;
    if (!CreateDescriptorSets()) return false;
    if (!CreateCommandBuffers()) return false;
    if (!CreateSyncObjects()) return false;
    
    return true;
}

bool VulkanModule::Render()
{
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(vkLogicalDevice, vkSwapchain, std::numeric_limits<uint64_t>::max(), vkImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    
    vkWaitForFences(vkLogicalDevice, 1, &vkInFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain(vkSwapchain);
        return true;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        CONSOLE_LOG("failed to acquire swap chain image!", NULL);
        return true;
    }
    
    vkResetCommandPool(vkLogicalDevice, vkCommandPool, 0);
    
    UpdateCommandBuffers();
    
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = { vkImageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    
    VkSemaphore signalSemaphores[] = { vkRenderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    vkResetFences(vkLogicalDevice, 1, &vkInFlightFences[currentFrame]);
    
    if (vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, vkInFlightFences[currentFrame]) != VK_SUCCESS) {
        CONSOLE_ERROR("failed to submit draw command buffer!", NULL);
        return true;
    }
    
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapchains[] = { vkSwapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    
    presentInfo.pImageIndices = &imageIndex;
    
    result = vkQueuePresentKHR(vkPresentationQueue, &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapChain(vkSwapchain);
    }
    else if (result != VK_SUCCESS) {
        CONSOLE_ERROR("failed to present swap chain image!", NULL);
        return true;
    }
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    
    vkQueueWaitIdle(vkPresentationQueue);

    return true;
}

bool VulkanModule::UpdateCommandBuffers()
{
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        VkResult result = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
        if (result != VK_SUCCESS)
        {
            CONSOLE_ERROR("failed to begin recording command buffer!", NULL);
            PrintVkResults(result);
            return false;
        }
        
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vkRenderPass;
        renderPassInfo.framebuffer = swapchainFramebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = vkSwapchainExtent;
        
        /*std::array<VkClearValue, 1> clearValues = {};
         clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
         clearValues[1].depthStencil = { 1.0f, 0 };*/
        
        /*renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
         renderPassInfo.pClearValues = clearValues.data();*/
        
        VkClearValue clearValues;
        clearValues.color = { 0.0f, 0.0f, 0.0f, 1.0f };
        
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValues;
        
        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        
		std::vector<GameObject*> sceneGameObjects = App->sceneModule->rootGameObject->GetChilds();
		int count = App->sceneModule->spritesNum;
        if(count > 0)
        {
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline);
            
            if(App->sceneModule->updateSceneVertices)
            {
				if (vertexBuffer != VK_NULL_HANDLE)
				{
					vkDestroyBuffer(vkLogicalDevice, vertexBuffer, nullptr);
				}
				if (vertexBufferMemory != VK_NULL_HANDLE)
				{
					vkFreeMemory(vkLogicalDevice, vertexBufferMemory, nullptr);
				}
                CreateVertexBuffer();
                App->sceneModule->updateSceneVertices = false;
            }
            VkBuffer vertexBuffers[] = { vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            
            vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
            
            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
            
            int vertexOffset = 0;
            for(int j = 0; j < count; j++)
            {
				if (sceneGameObjects[j]->GetActive() && sceneGameObjects[j]->GetSprite()->GetActive())
				{
					UniformBufferObject ubo = {};

					ubo.view = App->cameraModule->GetViewMatrix();
					ubo.proj = App->cameraModule->GetOrthoProjection();
					ubo.model = sceneGameObjects[j]->GetModelMatrix();

					vkCmdPushConstants(
						commandBuffers[i],
						vkPipelineLayout,
						VK_SHADER_STAGE_VERTEX_BIT,
						0,
						sizeof(UniformBufferObject),
						&ubo
					);

					vkCmdDrawIndexed(commandBuffers[i], static_cast<uint16_t>(indices.size()), 1, 0, vertexOffset, 0);
				}
                
                vertexOffset += 4;
            }
        }

		//Draw editor
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[i]);
        
        vkCmdEndRenderPass(commandBuffers[i]);
        
        result = vkEndCommandBuffer(commandBuffers[i]);
        if (result != VK_SUCCESS)
        {
            CONSOLE_ERROR("failed to record command buffer!", NULL);
            PrintVkResults(result);
            return false;
        }
    }
    
    return true;
}

void VulkanModule::UpdateUniformBuffer(uint32_t imageIndex)
{
   /* UniformBufferObject ubo = {};
    
    ubo.view = App->cameraModule->GetViewMatrix();
    ubo.proj = App->cameraModule->GetOrthoProjection();
    ubo.model = App->sceneModule->sceneGameObjects[0]->GetModelMatrix();
    
    void* data;
    vkMapMemory(vkLogicalDevice, uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(vkLogicalDevice, uniformBuffersMemory[imageIndex]);*/
}

bool VulkanModule::CreateVulkanInstance()
{
    if (enableValidationLayers && !CheckValidationLayerSupport()) {
        return false;
    }
    
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Games Factory 2D";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "Games Factory 2D";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;
    
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    
    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        instanceInfo.enabledLayerCount = 0;
    }
    
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();
    
    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vkInstance);
    
    if (result != VK_SUCCESS)
    {
        PrintVkResults(result);
        return false;
    }
    
    return true;
}

bool VulkanModule::SetupDebugDrawCall()
{
    if (enableValidationLayers)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        
        VkResult result = CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &callback);
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
    VkResult result = glfwCreateWindowSurface(vkInstance, App->windowModule->engineWindow, nullptr, &vkSurface);
    if (result != VK_SUCCESS)
    {
        PrintVkResults(result);
        return false;
    }
    return true;
}

bool VulkanModule::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
    
    if (deviceCount == 0) {
        CONSOLE_ERROR("failed to find GPUs with Vulkan support!", NULL);
        return false;
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
    
    for (const auto& device : devices) {
        if (IsDeviceSuitable(device)) {
            vkPhysicalDevice = device;
            break;
        }
    }
    
    if (vkPhysicalDevice == VK_NULL_HANDLE) {
        CONSOLE_ERROR("failed to find a suitable GPU!", NULL);
        return false;
    }
    
    return true;
}

bool VulkanModule::CreateLogicalDevice()
{
    QueueFamily family = FindQueueFamilies(vkPhysicalDevice);
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { family.graphicsIndex.value(), family.presentIndex.value() };
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    
    if (enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        deviceCreateInfo.enabledLayerCount = 0;
    }
    
    VkResult result = vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkLogicalDevice);
    if (result != VK_SUCCESS)
    {
        PrintVkResults(result);
        return false;
    }
    
    vkGetDeviceQueue(vkLogicalDevice, family.graphicsIndex.value(), 0, &vkGraphicsQueue);
    vkGetDeviceQueue(vkLogicalDevice, family.presentIndex.value(), 0, &vkPresentationQueue);
    
    return true;
}

bool VulkanModule::CreateSwapChain()
{
    SwapChainSupportDetails swapchainSupport = QuerySwapChainSupport(vkPhysicalDevice);
    
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities);
    
    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR swapchainKHRCreateInfo = {};
    swapchainKHRCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainKHRCreateInfo.surface = vkSurface;
    swapchainKHRCreateInfo.minImageCount = imageCount;
    swapchainKHRCreateInfo.imageFormat = surfaceFormat.format;
    swapchainKHRCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainKHRCreateInfo.imageExtent = extent;
    swapchainKHRCreateInfo.imageArrayLayers = 1;
    swapchainKHRCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamily indices = FindQueueFamilies(vkPhysicalDevice);
    uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsIndex.value(), (uint32_t)indices.presentIndex.value() };
    
    if (indices.graphicsIndex != indices.presentIndex) {
        swapchainKHRCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainKHRCreateInfo.queueFamilyIndexCount = 2;
        swapchainKHRCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapchainKHRCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    
    swapchainKHRCreateInfo.preTransform = swapchainSupport.capabilities.currentTransform;
    swapchainKHRCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainKHRCreateInfo.presentMode = presentMode;
    swapchainKHRCreateInfo.clipped = VK_TRUE;
    
    VkResult result = vkCreateSwapchainKHR(vkLogicalDevice, &swapchainKHRCreateInfo, nullptr, &vkSwapchain);
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to create swap chain!", NULL);
        PrintVkResults(result);
        return false;
    }
    
    result = vkGetSwapchainImagesKHR(vkLogicalDevice, vkSwapchain, &imageCount, nullptr);
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to get swap chain images HKR!", NULL);
        PrintVkResults(result);
        return false;
    }
    swapchainImages.resize(imageCount);
    result = vkGetSwapchainImagesKHR(vkLogicalDevice, vkSwapchain, &imageCount, swapchainImages.data());
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to get swap chain images HKR!", NULL);
        PrintVkResults(result);
        return false;
    }
    
    vkSwapchainImageFormat = surfaceFormat.format;
    vkSwapchainExtent = extent;
    
    return true;
}

void VulkanModule::CreateImageViews()
{
    swapchainImageViews.resize(swapchainImages.size());
    
    for (uint32_t i = 0; i < swapchainImages.size(); i++) {
        swapchainImageViews[i] = CreateImageView(swapchainImages[i], vkSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

bool VulkanModule::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = vkSwapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = FindDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    //subpass.pDepthStencilAttachment = &depthAttachmentRef;
    
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    std::array<VkAttachmentDescription, 1> attachments = { colorAttachment/*, depthAttachment*/ };
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    VkResult result = vkCreateRenderPass(vkLogicalDevice, &renderPassInfo, nullptr, &vkRenderPass);
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to create render pass!", NULL);
        PrintVkResults(result);
        return false;
    }
    
    return true;
}

bool VulkanModule::CreateDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
     samplerLayoutBinding.binding = 1;
     samplerLayoutBinding.descriptorCount = 1;
     samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
     samplerLayoutBinding.pImmutableSamplers = nullptr;
     samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;
    
    VkResult result = vkCreateDescriptorSetLayout(vkLogicalDevice, &layoutInfo, nullptr, &vkDescriptorSetLayout);
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to create descriptor layout!", NULL);
        PrintVkResults(result);
        return false;
    }
    
    return true;
}

bool VulkanModule::CreateGraphicsPipeline()
{
    auto vertShaderCode = App->fileSystemModule->LoadBinaryTextFile("Data/Shaders/vert.spv");
    auto fragShaderCode = App->fileSystemModule->LoadBinaryTextFile("Data/Shaders/frag.spv");
    
    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);
    
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    
    auto bindingDescription = Vertex::bindingDescription();
    auto attributeDescriptions = Vertex::attributesDescriptions();
    
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vkSwapchainExtent.width;
    viewport.height = (float)vkSwapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = vkSwapchainExtent;
    
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    
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
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    
    VkPushConstantRange pushConstantRange {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(UniformBufferObject);
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &vkDescriptorSetLayout;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    
    if (vkCreatePipelineLayout(vkLogicalDevice, &pipelineLayoutInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = vkPipelineLayout;
    pipelineInfo.renderPass = vkRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    if (vkCreateGraphicsPipelines(vkLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    
    vkDestroyShaderModule(vkLogicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(vkLogicalDevice, vertShaderModule, nullptr);
    
    return true;
}

bool VulkanModule::CreateFramebuffers()
{
    swapchainFramebuffers.resize(swapchainImageViews.size());
    
    for (size_t i = 0; i < swapchainImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapchainImageViews[i]
        };
        
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vkRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = vkSwapchainExtent.width;
        framebufferInfo.height = vkSwapchainExtent.height;
        framebufferInfo.layers = 1;
        
        if (vkCreateFramebuffer(vkLogicalDevice, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
            CONSOLE_ERROR("failed to create framebuffer!", NULL);
            return false;
        }
    }
    
    return true;
}

bool VulkanModule::CreateCommandPool()
{
    QueueFamily queueFamilyIndices = FindQueueFamilies(vkPhysicalDevice);
    
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsIndex.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    if (vkCreateCommandPool(vkLogicalDevice, &poolInfo, nullptr, &vkCommandPool) != VK_SUCCESS) {
        CONSOLE_ERROR("failed to create graphics command pool!", NULL);
        return false;
    }
    
    return true;
}

bool VulkanModule::CreateDepthResources()
{
    /*VkFormat depthFormat = FindDepthFormat();
     
     CreateImage(vkSwapchainExtent.width, vkSwapchainExtent.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
     depthImageView = CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);*/
    
    //transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    
    return true;
}

bool VulkanModule::CreateCommandBuffers()
{
    commandBuffers.resize(swapchainFramebuffers.size());
    
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
    
    VkResult result = vkAllocateCommandBuffers(vkLogicalDevice, &allocInfo, commandBuffers.data());
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to allocate command buffers!", NULL);
        PrintVkResults(result);
        return false;
    }
    
    return true;
}

bool VulkanModule::CreateSyncObjects()
{
    vkImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    vkRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    vkInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = vkCreateSemaphore(vkLogicalDevice, &semaphoreInfo, nullptr, &vkImageAvailableSemaphores[i]);
        if (result != VK_SUCCESS)
        {
            CONSOLE_ERROR("failed to create Image available semaphore!", NULL);
            PrintVkResults(result);
            return false;
        }
        else
        {
            result = vkCreateSemaphore(vkLogicalDevice, &semaphoreInfo, nullptr, &vkRenderFinishedSemaphores[i]);
            if (result != VK_SUCCESS)
            {
                CONSOLE_ERROR("failed to create render available semaphore!", NULL);
                PrintVkResults(result);
                return false;
            }
            else
            {
                result = vkCreateFence(vkLogicalDevice, &fenceInfo, nullptr, &vkInFlightFences[i]);
                if (result != VK_SUCCESS)
                {
                    CONSOLE_ERROR("failed to create in flight fences!", NULL);
                    PrintVkResults(result);
                    return false;
                }
            }
        }
    }
    
    return true;
}

bool VulkanModule::CreateVertexBuffer()
{
    std::vector<Vertex> vertices;
    std::vector<GameObject*> gos = App->sceneModule->rootGameObject->GetChilds();
    for(GameObject* go : gos)
    {
		ComponentSprite* sprite = go->GetSprite();
		if (sprite)
		{
			std::array<Vertex, 4> goVertices = sprite->GetVertices();
			for (Vertex v : goVertices)
			{
				vertices.emplace_back(v);
			}
		}
    }

	if (vertices.empty()) return false;
        
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory))
    {
        CONSOLE_ERROR("failed to create vertex staging buffer", NULL);
        return false;
    }
    
    void* data;
    vkMapMemory(vkLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(vkLogicalDevice, stagingBufferMemory);
    
    if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory))
    {
        CONSOLE_ERROR("failed to create vertex buffer", NULL);
        return false;
    }
    
    CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);
    
    vkDestroyBuffer(vkLogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(vkLogicalDevice, stagingBufferMemory, nullptr);
    
    return true;
}

bool VulkanModule::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory))
    {
        CONSOLE_ERROR("failed to create index staging buffer", NULL);
        return false;
    }
    
    void* data;
    vkMapMemory(vkLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(vkLogicalDevice, stagingBufferMemory);
    
    if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory))
    {
        CONSOLE_ERROR("failed to create index buffer", NULL);
        return false;
    }
    
    CopyBuffer(stagingBuffer, indexBuffer, bufferSize);
    
    vkDestroyBuffer(vkLogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(vkLogicalDevice, stagingBufferMemory, nullptr);
    
    return true;
}

bool VulkanModule::CreateUniformBuffer()
{
    bool ret = false;
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    
    uniformBuffers.resize(swapchainImages.size());
    uniformBuffersMemory.resize(swapchainImages.size());
    
    for (size_t i = 0; i < swapchainImages.size(); i++) {
        ret = CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
    }
    
    return ret;
}

bool VulkanModule::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(swapchainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(swapchainImages.size());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(swapchainImages.size());
    
    VkResult result = vkCreateDescriptorPool(vkLogicalDevice, &poolInfo, nullptr, &vkDescriptorPool);
    if(result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to create descriptor pool!", NULL);
        return false;
    }
    
    return true;
}

bool VulkanModule::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(swapchainImages.size(), vkDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vkDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainImages.size());
    allocInfo.pSetLayouts = layouts.data();
    
    descriptorSets.resize(swapchainImages.size());
    
    VkResult result = vkAllocateDescriptorSets(vkLogicalDevice, &allocInfo, descriptorSets.data());
    if(result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to allocate descriptor sets!", NULL);
        return false;
    }
    
    for (size_t i = 0; i < swapchainImages.size(); i++) {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);
        
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        
        vkUpdateDescriptorSets(vkLogicalDevice, 1, &descriptorWrite, 0, nullptr);
    }
    
    return true;
}

bool VulkanModule::IsDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamily family = FindQueueFamilies(device);
    if (!family.IsValid()) return false;
    
    bool extensionsSupported = CheckDeviceExtensionSupport(device);
    if (!extensionsSupported) return false;
    
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
        bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        if (!swapChainAdequate) return false;
    }
    
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    if (!supportedFeatures.samplerAnisotropy) return false;
    
    return true;
}

VulkanModule::QueueFamily VulkanModule::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamily ret;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            ret.graphicsIndex = static_cast<uint32_t>(i);
        }
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vkSurface, &presentSupport);
        
        if (queueFamily.queueCount > 0 && presentSupport) {
            ret.presentIndex = static_cast<uint32_t>(i);
        }
        
        if (ret.IsValid()) {
            break;
        }
        
        i++;
    }
    
    return ret;
}

VulkanModule::SwapChainSupportDetails VulkanModule::QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vkSurface, &details.capabilities);
    if (result == VK_SUCCESS)
    {
        uint32_t formatCount;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, nullptr);
        if (result == VK_SUCCESS)
        {
            if (formatCount != 0) {
                details.formats.resize(formatCount);
                result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, details.formats.data());
                if (result == VK_SUCCESS)
                {
                    uint32_t presentModeCount;
                    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, nullptr);
                    if (result == VK_SUCCESS)
                    {
                        if (presentModeCount != 0) {
                            details.presentModes.resize(presentModeCount);
                            result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, details.presentModes.data());
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
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    
    return requiredExtensions.empty();
}

bool VulkanModule::CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    const char* layers[] = { "VK_LAYER_LUNARG_standard_validation" };
    
    for (const char* layerName : layers) {
        bool layerFound = false;
        
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        
        if (!layerFound) {
            return false;
        }
    }
    
    return true;
}

VkSurfaceFormatKHR VulkanModule::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }
    
    for (const auto& format : availableFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    
    return availableFormats[0];
}

VkPresentModeKHR VulkanModule::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
    
    for (const auto& presentMode : availablePresentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
        else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = presentMode;
        }
    }
    
    return bestMode;
}

VkExtent2D VulkanModule::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetWindowSize(App->windowModule->engineWindow, &width, &height);
        
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        
        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
        
        return actualExtent;
    }
}

VkImageView VulkanModule::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    
    VkImageView imageView;
    VkResult result = vkCreateImageView(vkLogicalDevice, &viewInfo, nullptr, &imageView);
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to create texture image view!", NULL);
        PrintVkResults(result);
        return VK_NULL_HANDLE;
    }
    return imageView;
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
        vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, format, &props);
        
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    
    CONSOLE_ERROR("failed to find supported format!", NULL);
    return ret;
}

VkShaderModule VulkanModule::CreateShaderModule(std::string shaderCode)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
    
    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(vkLogicalDevice, &createInfo, nullptr, &shaderModule);
    if(result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to create shader module!", NULL);
        PrintVkResults(result);
    }
    
    return shaderModule;
}

bool VulkanModule::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult result = vkCreateBuffer(vkLogicalDevice, &bufferInfo, nullptr, &buffer);
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to create buffer!", NULL);
        PrintVkResults(result);
        return false;
    }
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkLogicalDevice, buffer, &memRequirements);
    uint32_t memType = FindMemoryType(memRequirements.memoryTypeBits, properties);
    if (memType == -1) return false;
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memType;
    
    result = vkAllocateMemory(vkLogicalDevice, &allocInfo, nullptr, &bufferMemory);
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to allocate buffer memory!", NULL);
        PrintVkResults(result);
        return false;
    }
    
    vkBindBufferMemory(vkLogicalDevice, buffer, bufferMemory, 0);
    
    return true;
}

uint32_t VulkanModule::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    CONSOLE_ERROR("failed to find suitable memory type!", NULL);
    return -1;
}

void VulkanModule::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
    
    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    
    EndSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VulkanModule::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkCommandPool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer;
    VkResult result = vkAllocateCommandBuffers(vkLogicalDevice, &allocInfo, &commandBuffer);
    if (result != VK_SUCCESS)
    {
        CONSOLE_ERROR("failed to allocate buffer memory!", NULL);
        PrintVkResults(result);
        return VK_NULL_HANDLE;
    }
    
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
    return commandBuffer;
}

void VulkanModule::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkGraphicsQueue);
    
    vkFreeCommandBuffers(vkLogicalDevice, vkCommandPool, 1, &commandBuffer);
}

void VulkanModule::RecreateSwapChain(VkSwapchainKHR swapchain)
{
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(App->windowModule->engineWindow, &width, &height);
        glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(vkLogicalDevice);
    
    CleanupSwapChain(swapchain);
    
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    //CreateDepthResources();
    CreateFramebuffers();
    CreateCommandBuffers();
}

void VulkanModule::CleanupSwapChain(VkSwapchainKHR swapchain)
{
    for (auto framebuffer : swapchainFramebuffers) {
        vkDestroyFramebuffer(vkLogicalDevice, framebuffer, nullptr);
    }
    
    vkFreeCommandBuffers(vkLogicalDevice, vkCommandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    
    vkDestroyPipeline(vkLogicalDevice, vkGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(vkLogicalDevice, vkPipelineLayout, nullptr);
    vkDestroyRenderPass(vkLogicalDevice, vkRenderPass, nullptr);
    
    for (auto imageView : swapchainImageViews) {
        vkDestroyImageView(vkLogicalDevice, imageView, nullptr);
    }
    
    vkDestroySwapchainKHR(vkLogicalDevice, swapchain, nullptr);
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
    
    CONSOLE_ERROR("%s", message);
}

bool VulkanModule::PrintVKDebugMessages(const char * msg)
{
    CONSOLE_ERROR("validation layer: %s", msg);
    
    return VK_FALSE;
}

void VulkanModule::InitImGui()
{
	VkDescriptorPool ImGuiDescriptorPool;
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	VkResult err = vkCreateDescriptorPool(vkLogicalDevice, &pool_info, nullptr, &ImGuiDescriptorPool);
	check_vk_result(err);

	VkRenderPass ImGuiRenderPass;
	VkAttachmentDescription attachment = {};
	attachment.format = vkSwapchainImageFormat;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentReference color_attachment = {};
	color_attachment.attachment = 0;
	color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment;
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	VkRenderPassCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.attachmentCount = 1;
	info.pAttachments = &attachment;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = 1;
	info.pDependencies = &dependency;
	err = vkCreateRenderPass(vkLogicalDevice, &info, nullptr, &ImGuiRenderPass);
	check_vk_result(err);

	// Setup Vulkan binding
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = vkInstance;
	init_info.PhysicalDevice = vkPhysicalDevice;
	init_info.Device = vkLogicalDevice;
	init_info.QueueFamily = FindQueueFamilies(vkPhysicalDevice).graphicsIndex.value();
	init_info.Queue = vkGraphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = ImGuiDescriptorPool;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, ImGuiRenderPass);

	err = vkResetCommandPool(vkLogicalDevice, vkCommandPool, 0);
	check_vk_result(err);
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	err = vkBeginCommandBuffer(commandBuffers[0], &begin_info);
	check_vk_result(err);

	ImGui_ImplVulkan_CreateFontsTexture(commandBuffers[0]);

	VkSubmitInfo end_info = {};
	end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	end_info.commandBufferCount = 1;
	end_info.pCommandBuffers = &commandBuffers[0];
	err = vkEndCommandBuffer(commandBuffers[0]);
	check_vk_result(err);
	err = vkQueueSubmit(vkGraphicsQueue, 1, &end_info, VK_NULL_HANDLE);
	check_vk_result(err);

	err = vkDeviceWaitIdle(vkLogicalDevice);
	check_vk_result(err);
	ImGui_ImplVulkan_InvalidateFontUploadObjects();
}

void VulkanModule::CreateTexture(Texture & texture, void* pixels)
{
	uint32_t width = texture.GetWidth();
	uint32_t height = texture.GetHeight();

	VkDeviceSize imageSize = width * height * 4;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vkLogicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(vkLogicalDevice, stagingBufferMemory);

	delete pixels;
	pixels = nullptr;

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = texture.GetMipMapLevels();
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(vkLogicalDevice, &imageInfo, nullptr, &texture.textureImage) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(vkLogicalDevice, texture.textureImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(vkLogicalDevice, &allocInfo, nullptr, &texture.textureImageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(vkLogicalDevice, texture.textureImage, texture.textureImageMemory, 0);

	TransitionImageLayout(texture.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, texture.textureImage, width, height);
	TransitionImageLayout(texture.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(vkLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(vkLogicalDevice, stagingBufferMemory, nullptr);

	texture.textureImageView = CreateImageView(texture.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, texture.GetMipMapLevels());
	
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(vkLogicalDevice, &samplerInfo, nullptr, &texture.textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void VulkanModule::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	EndSingleTimeCommands(commandBuffer);
}

void VulkanModule::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	EndSingleTimeCommands(commandBuffer);
}

void VulkanModule::DestroyTexture(Texture & texture)
{
	vkDestroySampler(vkLogicalDevice, texture.textureSampler, nullptr);
	vkDestroyImageView(vkLogicalDevice, texture.textureImageView, nullptr);
	vkDestroyImage(vkLogicalDevice, texture.textureImage, nullptr);
	vkFreeMemory(vkLogicalDevice, texture.textureImageMemory, nullptr);
}
