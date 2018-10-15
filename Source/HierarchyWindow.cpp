#include "HierarchyWindow.h"
#include <imgui.h>
#include "SceneModule.h"
#include "Application.h"
#include "GameObject.h"
#include "EditorModule.h"
#include "InspectorWindow.h"

HierarchyWindow::HierarchyWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
	selectedGameObject = nullptr;
	showGameObjectOptions = false;
	nodeNum = 0;
}

HierarchyWindow::~HierarchyWindow()
{
}

void HierarchyWindow::DrawWindow()
{
	nodeNum = 0;
	ImGui::Begin("Hierarchy");
	if (ImGui::Button("Create GameObject"))
	{
		App->sceneModule->CreateNewObject();
	}
	std::vector<GameObject*> childs = App->sceneModule->rootGameObject->GetChilds();
	for (GameObject* go : childs)
	{
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
            selectedGameObject = nullptr;
            App->editorModule->inspectorWindow->SetSelectedGameObject(nullptr);
        }
    }
	ImGui::End();
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
	nodeNum++;
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
		if (ImGui::IsItemClicked())
		{
			selectedGameObject = gameObject;
			App->editorModule->inspectorWindow->SetSelectedGameObject(gameObject);
		}
		if (ImGui::IsItemClicked(1))
		{
			showGameObjectOptions = true;
			optionsWindowPos = ImGui::GetCursorPos();
		}
		std::vector<GameObject*> childs = gameObject->GetChilds();
		for (GameObject* child : childs)
		{
			DrawNodes(child);
		}
		ImGui::TreePop();
	}
}
