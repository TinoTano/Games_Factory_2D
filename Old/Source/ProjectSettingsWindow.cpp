#include "ProjectSettingsWindow.h"
#include <imgui.h>
#include "Application.h"
#include "CustomImGui.h"
#include "WindowModule.h"
#include "Physics2DModule.h"

ProjectSettingsWindow::ProjectSettingsWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
}

ProjectSettingsWindow::~ProjectSettingsWindow()
{
}

void ProjectSettingsWindow::DrawWindow()
{
	ImGui::SetNextWindowSize({ 500,700 });
	ImGui::Begin("Project Settings", &enabled,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse);

	DrawGeneralOptions();
	DrawDisplayOptions();
	DrawPhysicsOptions();
	
	ImGui::End();
}

void ProjectSettingsWindow::DrawGeneralOptions()
{
	if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static char* title;
		title = _strdup(App->appTitle);
		if (ImGui::InputText("Ttile", title, 100, 0))
		{
			App->appTitle = title;
		}
		static char* version;
		version = _strdup(App->appVersion);
		if (ImGui::InputText("Version", version, 100, 0))
		{
			App->appVersion = version;
		}
		ImGui::InputTexture("Icon", &App->icon);
		ImGui::InputScene("Start Scene", &App->startScene);
	}
}

void ProjectSettingsWindow::DrawDisplayOptions()
{
	if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen))
	{
		WindowModule& windowModule = *App->windowModule;
		int width = windowModule.GetGameWindowWidth();
		if(ImGui::InputInt("Width", &width))
		{
			windowModule.SetGameWindowWidth(width);
		}
		int height = windowModule.GetGameWindowHeight();
		if (ImGui::InputInt("Height", &height))
		{
			windowModule.SetGameWindowHeight(height);
		}
		bool fullscreen = windowModule.GetIsFullscreen();
		if (ImGui::Checkbox("Fullscreen", &fullscreen))
		{
			windowModule.SetFullScreen(fullscreen);
		}
		bool resizable = windowModule.GetIsResizable();
		if (ImGui::Checkbox("Resizable", &resizable))
		{
			windowModule.SetResizable(resizable);
		}
		bool borderless = windowModule.GetIsBorderless();
		if (ImGui::Checkbox("Borderless", &borderless))
		{
			windowModule.SetBorderless(borderless);
		}
		bool onTop = windowModule.GetIsAlwaysOnTop();
		if (ImGui::Checkbox("Always on top", &onTop))
		{
			windowModule.SetAlwaysOnTop(onTop);
		}
		float maxFPS = App->GetMaxFPS();
		if (ImGui::InputFloat("Max FPS", &maxFPS))
		{
			App->SetMaxFPS(maxFPS);
		}
	}
}

void ProjectSettingsWindow::DrawPhysicsOptions()
{
	if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen))
	{
		/*ImGui::InputFloat("Step", App->physics2DModule->);
		ImGui::InputFloat2("Gravity");
		ImGui::InputFloat("Velocity Iterations");
		ImGui::InputFloat("Position Iterations");*/
	}
}
