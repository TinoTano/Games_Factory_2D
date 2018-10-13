#include "InspectorWindow.h"
#include <imgui.h>

InspectorWindow::InspectorWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
    
}

InspectorWindow::~InspectorWindow()
{
    
}

void InspectorWindow::DrawWindow()
{
    ImGui::Begin(windowName);
    ImGui::End();
}
