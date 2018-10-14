#include "EditorModule.h"
#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "Application.h"
#include "WindowModule.h"
#include "VulkanModule.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

EditorModule::EditorModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
    
}

EditorModule::~EditorModule()
{
    
}

bool EditorModule::Start()
{
	editorWindows.reserve(2);
	editorWindows.emplace_back(inspectorWindow = new InspectorWindow("Inspector Window"));
	editorWindows.emplace_back(hierarchyWindow = new HierarchyWindow("Hierarchy Window"));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForVulkan(App->windowModule->engineWindow, false);
	ImGui::StyleColorsDark();
	App->vulkanModule->InitImGui();

	return true;
}

bool EditorModule::PreUpdate(float deltaTime)
{
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	return true;
}

bool EditorModule::PostUpdate(float deltaTime)
{
    for (EditorWindow* window : editorWindows)
    {
        window->DrawWindow();
    }
	ImGui::Render();

    return true;
}

bool EditorModule::CleanUp()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    return true;
}
