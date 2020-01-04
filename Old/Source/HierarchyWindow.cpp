#include "HierarchyWindow.h"
#include <imgui.h>
#include "SceneModule.h"
#include "Application.h"
#include "GameObject.h"
#include "EditorModule.h"
#include "InspectorWindow.h"
#include "Scene.h"
#include "RendererModule.h"
#include "Physics2DModule.h"

HierarchyWindow::HierarchyWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
	selectedGameObject = nullptr;
	showGameObjectOptions = false;
}

HierarchyWindow::~HierarchyWindow()
{
}

void HierarchyWindow::DrawWindow()
{
	ImGui::Begin("Hierarchy");
	if (ImGui::Button("Create GameObject"))
	{
		App->sceneModule->CreateNewObject(App->sceneModule->currentScene->GetRootGameObject());
	}
	std::vector<GameObject*> childs = App->sceneModule->currentScene->GetRootGameObject()->GetChilds();
	for (GameObject* go : childs)
	{
		if (ImGui::GetItemRectMax().y > ImGui::GetWindowPos().y + ImGui::GetWindowSize().y) break;
		DrawNodes(go);
	}

	if (showGameObjectOptions)
	{
		ShowGameObjectOptions();
	}
    if(ImGui::IsMouseClicked(0))
    {
        if(!ImGui::IsAnyItemHovered() && ImGui::IsWindowFocused())
        {
			RemoveSelectedGameObject();
        }
    }

	if (childs.empty())
	{
		RemoveSelectedGameObject();
		showGameObjectOptions = false;
	}

	ImGui::End();
}

void HierarchyWindow::RemoveSelectedGameObject()
{
	selectedGameObject = nullptr;
	App->editorModule->inspectorWindow->SetSelectedGameObject(nullptr);
	App->rendererModule->CleanDebugVertex();
}

void HierarchyWindow::ShowGameObjectOptions()
{
	ImGui::SetNextWindowPos(optionsWindowPos);
	bool active = true;
	ImGui::Begin("##optionsWindow", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	if (ImGui::Selectable("Create Child"))
	{
		App->sceneModule->CreateNewObject(selectedGameObject);
		showGameObjectOptions = false;
	}
	ImGui::End();
}

void HierarchyWindow::DrawNodes(GameObject * gameObject)
{
	uint32_t flags = 0;
	if (gameObject->GetChilds().empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	flags |= ImGuiTreeNodeFlags_OpenOnArrow;

	if (selectedGameObject != nullptr)
	{
		if (selectedGameObject == gameObject)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
	}

	if (ImGui::TreeNodeEx(gameObject->GetName().c_str(), flags))
	{
		CheckMouseSelect(*gameObject);
		std::vector<GameObject*> childs = gameObject->GetChilds();
		for (GameObject* child : childs)
		{
			DrawNodes(child);
		}
		ImGui::TreePop();
	}
	else
	{
		CheckMouseSelect(*gameObject);
	}
}

void HierarchyWindow::CheckMouseSelect(GameObject& gameObject)
{
	if (ImGui::IsItemClicked())
	{
		if (selectedGameObject != &gameObject)
		{
			selectedGameObject = &gameObject;
			App->editorModule->inspectorWindow->SetSelectedGameObject(&gameObject);
			App->physics2DModule->SetGameObjectToDebug(gameObject);
		}
	}
	if (ImGui::IsItemClicked(1))
	{
		showGameObjectOptions = true;
		optionsWindowPos = ImGui::GetCursorPos();
	}
}
