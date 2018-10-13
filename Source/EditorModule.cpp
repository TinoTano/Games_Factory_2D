#include "EditorModule.h"
#include "InspectorWindow.h"
#include "Application.h"
#include "WindowModule.h"
#include <imgui_impl_glfw.h>

EditorModule::EditorModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
    
}

EditorModule::~EditorModule()
{
    
}

bool EditorModule::Init()
{
    editorWindows.reserve(1);
    editorWindows.emplace_back(inspectorWindow = new InspectorWindow("Inspector Window"));
    
    ImGui_ImplGlfw_InitForVulkan(App->windowModule->engineWindow, false);
    
    return true;
}

bool EditorModule::PostUpdate(float deltaTime)
{
    for (EditorWindow* window : editorWindows)
    {
        window->DrawWindow();
    }
    
    return true;
}

bool EditorModule::CleanUp()
{
    
    return true;
}
