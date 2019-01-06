#include "EditorModule.h"
#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "Application.h"
#include "WindowModule.h"
#include "VulkanModule.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include "AssetsWindow.h"
#include "SceneModule.h"
#include <tinyfiledialogs.h>
#include "Builder.h"
#include "Scene.h"
#include "AnimationWindow.h"
#include "FileSystemModule.h"

EditorModule::EditorModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
    
}

EditorModule::~EditorModule()
{
	inspectorWindow = nullptr;
	hierarchyWindow = nullptr;
	assetsWindow = nullptr;
	animationWindow = nullptr;
}

bool EditorModule::Start()
{
	//editorWindows.reserve(4);
	editorWindows.emplace_back(inspectorWindow = new InspectorWindow("Inspector Window"));
	editorWindows.emplace_back(hierarchyWindow = new HierarchyWindow("Hierarchy Window"));
	editorWindows.emplace_back(assetsWindow = new AssetsWindow("Assets Window"));
	editorWindows.emplace_back(animationWindow = new AnimationWindow("Animation Window"));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForVulkan(App->windowModule->engineWindow, true);
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
	DrawMainMenu();

	if (!App->IsPlaying())
	{
		for (EditorWindow* window : editorWindows)
		{
			if (window->IsEnabled())
			{
				window->DrawWindow();
			}
		}
	}

	ImGui::Render();

    return true;
}

bool EditorModule::CleanUp()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	for (EditorWindow* window : editorWindows)
	{
		delete window;
		window = nullptr;
	}

	editorWindows.clear();

    return true;
}

void EditorModule::DrawMainMenu()
{
	ImGui::BeginMainMenuBar();

	DrawFileMenu();
	DrawWindowsMenu();
	DrawDebugWindow();

	ImGui::SameLine(App->windowModule->GetWidth() - 75.f);
	ImGui::Text("FPS: %d", App->GetFPS());
	ImGui::EndMainMenuBar();
}

void EditorModule::DrawFileMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New Scene"))
		{
			App->sceneModule->ClearScene();
		}
		if (ImGui::MenuItem("Save Scene"))
		{
			char const * lFilterPatterns[1] = { "*.scene" };
			const char* path = tinyfd_saveFileDialog("Save Scene...", App->sceneModule->currentScene->GetName().c_str(), 1, lFilterPatterns, NULL);
			if (path != nullptr)
			{
				App->sceneModule->SaveScene(path);
			}
		}
		if (ImGui::MenuItem("Load Scene"))
		{
			char const * lFilterPatterns[1] = { "*.scene" };
			const char* path = tinyfd_openFileDialog("Load Scene...", NULL, 1, lFilterPatterns, NULL, 0);
			if (path != nullptr)
			{
				App->sceneModule->LoadScene(path);
				hierarchyWindow->RemoveSelectedGameObject();
			}
		}
		if (ImGui::BeginMenu("Build"))
		{
			if (ImGui::MenuItem("Windows"))
			{
				const char* path = tinyfd_selectFolderDialog("Select Folder...", NULL);
				if (path != nullptr)
				{
					App->builder->BuildForWindows(path);
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

void EditorModule::DrawWindowsMenu()
{
	if (ImGui::BeginMenu("Windows"))
	{
		if (ImGui::MenuItem("Animation"))
		{
			animationWindow->Enable(true);
		}
		ImGui::EndMenu();
	}
}

void EditorModule::DrawDebugWindow()
{
	if (ImGui::BeginMenu("Debug"))
	{
		bool isPlaying = App->IsPlaying();
		bool isPaused = App->IsPaused();

		if (ImGui::MenuItem("Play", nullptr, nullptr, !isPlaying))
		{
			if (App->fileSystemModule->CreateDirectory("./Data/Tmp"))
			{
				App->sceneModule->SaveScene("./Data/Tmp/backup.scene");
				App->Play();
			}
		}
		if (ImGui::MenuItem("Stop", nullptr, nullptr, isPlaying || isPaused))
		{
			App->Stop();
			hierarchyWindow->RemoveSelectedGameObject();
			App->sceneModule->LoadScene("./Data/Tmp/backup.scene");
		}
		if (ImGui::MenuItem("Pause", nullptr, nullptr, isPlaying))
		{
			App->Pause();
		}
		if (ImGui::MenuItem("Step", nullptr, nullptr, isPaused))
		{
			App->AdvanceStep();
		}
		ImGui::EndMenu();
	}
}
