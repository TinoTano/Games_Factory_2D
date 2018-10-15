#include "InspectorWindow.h"
#include "Application.h"
#include "SceneModule.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentSprite.h"
#include <imgui.h>

InspectorWindow::InspectorWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
    selectecGameObject = nullptr;
    showAddComponentsWindow = false;
}

InspectorWindow::~InspectorWindow()
{
    
}

void InspectorWindow::DrawWindow()
{
    ImGui::Begin("Inspector");
	if (selectecGameObject != nullptr)
	{
		DrawInfo(*selectecGameObject);

		std::vector<Component*> components = selectecGameObject->GetComponents();
		for (Component* component : components)
		{
			Component::COMPONENT_TYPE type = component->GetComponentType();
			switch (type)
			{
			case Component::TRANSFORM:
				DrawTransform(*(ComponentTransform*)component);
				break;
			case Component::SPRITE:
				DrawSprite(*(ComponentSprite*)component);
				break;
			default:
				break;
			}

			ImGui::Separator();
		}

		if (ImGui::Button("Add Component"))
		{
			showAddComponentsWindow = true;
			addComponentsWindowPos = ImGui::GetCursorScreenPos();
		}
	}
    ImGui::End();

	if (showAddComponentsWindow)
	{
		ShowAddComponentWindow();
	}
}

void InspectorWindow::SetSelectedGameObject(GameObject * go)
{
	selectecGameObject = go;
}

void InspectorWindow::DrawInfo(GameObject & go)
{
    char* name = strdup(go.GetName().c_str());
    if(ImGui::InputText("Name", name, go.GetName().size() + 1, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
    {
        go.SetName(name);
    }

	bool active = go.GetActive();
	if (ImGui::Checkbox("Active", &active))
	{
		go.SetActive(active);
	}

	ImGui::Separator();
}

void InspectorWindow::DrawTransform(ComponentTransform & transform)
{
	glm::vec2 pos = transform.GetLocalPosition();
	if (ImGui::DragFloat2("Position", (float*)&pos, 0.25f))
	{
		transform.SetPosition(pos);
	}

	float rot = transform.GetLocalRotation();
	if (ImGui::DragFloat("Rotation", &rot, 0.25f))
	{
		transform.SetRotation(rot);
	}

	glm::vec2 scale = transform.GetLocalScale();
	if (ImGui::DragFloat2("Scale", (float*)&scale, 0.25f))
	{
		transform.SetScale(scale);
	}
}

void InspectorWindow::DrawSprite(ComponentSprite & transform)
{
	bool active = transform.GetActive();
	if (ImGui::Checkbox("Active##ComponentSprite", &active))
	{
		transform.SetActive(active);
	}
}

void InspectorWindow::ShowAddComponentWindow()
{
	ImGui::SetNextWindowPos(addComponentsWindowPos);
	bool active = true;
	ImGui::Begin("##AddComponentWindow", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	if (ImGui::MenuItem("Sprite"))
	{
		selectecGameObject->AddComponent(Component::SPRITE);
		showAddComponentsWindow = false;
	}
	ImGui::End();
}
