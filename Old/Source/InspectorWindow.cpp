#include "InspectorWindow.h"
#include "Application.h"
#include "SceneModule.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentSprite.h"
#include "Texture.h"
#include <imgui.h>
#include "CustomImGui.h"
#include <float.h>
#include "Animation.h"
#include "EditorModule.h"
#include "AnimationWindow.h"
#include "ComponentScript.h"
#include "ResourceManagerModule.h"
#include "LuaScript.h"
#include "LuaScripting.h"
#include "ComponentPhysicsBody.h"
#include "ComponentBoxCollider.h"
#include "ComponentCircleCollider.h"

InspectorWindow::InspectorWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
    selectecGameObject = nullptr;
    showAddComponentsWindow = false;
}

InspectorWindow::~InspectorWindow()
{
	selectecGameObject = nullptr;
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
			case Component::SCRIPT:
				DrawScript(*(ComponentScript*)component);
				break;
			case Component::PHYSICS_BODY:
				DrawPhysicsBody(*(ComponentPhysicsBody*)component);
				break;
			case Component::BOX_COLLIDER:
				DrawBoxCollider(*(ComponentBoxCollider*)component);
				break;
			case Component::CIRCLE_COLLIDER:
				DrawCircleCollider(*(ComponentCircleCollider*)component);
				break;
			default:
				break;
			}

			ImGui::Separator();
		}

		ImGui::Columns(1);

		if (ImGui::Button("Add Component"))
		{
			showAddComponentsWindow = true;
			addComponentsWindowPos = ImGui::GetCursorScreenPos();
			ImGui::OpenPopup("AddComponentWindow");
		}

		if (ImGui::IsPopupOpen("AddComponentWindow"))
		{
			ShowAddComponentWindow();
		}
	}
    ImGui::End();
}

void InspectorWindow::SetSelectedGameObject(GameObject * go)
{
	selectecGameObject = go;
}

void InspectorWindow::DrawInfo(GameObject & go)
{
	static char* name = _strdup(go.GetName().c_str());

//#ifdef _WIN32
//	name = strdup(go.GetName().c_str());
//#else
//	name = strdup(go.GetName().c_str());
//#endif

    if(ImGui::InputText("Name", name, 40, ImGuiInputTextFlags_AutoSelectAll))
    {
        go.SetName(name);
    }

	bool active = go.GetActive();
	if (ImGui::EditorBool("Active", &active))
	{
		go.SetActive(active);
	}

	ImGui::Separator();
	ImGui::Columns(1);
}

void InspectorWindow::DrawTransform(ComponentTransform & compTransform)
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		glm::vec2 pos = compTransform.GetLocalPosition();
		if (ImGui::EditorDragFloat2("Position", (float*)&pos, 0.025f))
		{
			compTransform.SetPosition(pos);
		}
		float rot = compTransform.GetLocalRotation();
		if (ImGui::EditorDragFloat("Rotation", &rot, 0.25f))
		{
			compTransform.SetRotation(rot);
		}

		glm::vec2 scale = compTransform.GetLocalScale();
		if (ImGui::EditorDragFloat2("Scale", (float*)&scale, 0.025f/*, 0.0f, FLT_MAX*/))
		{
			compTransform.SetScale(scale);
		}
	}
	ImGui::Columns(1);
}

void InspectorWindow::DrawSprite(ComponentSprite & compSprite)
{
	if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool active = compSprite.GetActive();
		if (ImGui::EditorBool("Active##ComponentSprite", &active))
		{
			compSprite.SetActive(active);
		}

		Texture* texture = compSprite.GetTexture();
		if (ImGui::InputTexture("Image", &texture))
		{
			compSprite.SetTexture(texture);
		}
		
		bool flippedX = compSprite.IsFlippedX();
		if (ImGui::EditorBool("Flip X", &flippedX))
		{
			compSprite.FlipX();
		}

		bool flippedY = compSprite.IsFlippedY();
		if (ImGui::EditorBool("Flip Y", &flippedY))
		{
			compSprite.FlipY();
		}

		ImGui::Spacing();
		ImGui::Text("Animations:");

		std::vector<Animation*> animations = compSprite.GetAnimations();
		for (int i = 0; i < animations.size(); i++)
		{
			std::string name = "##" + std::to_string(i);
			ImGui::InputText(name.c_str(), animations[i]->GetName().data(), animations[i]->GetName().size(), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button(("Edit" + name).c_str()))
			{
				App->editorModule->animationWindow->SetAnimationToEdit(&animations[i]);
			}
			ImGui::SameLine();
			if (ImGui::Button(("Remove" + name).c_str()))
			{
				compSprite.RemoveAnimation(i);
			}
		}

		Animation* anim = nullptr;
		if (ImGui::InputAnimation(("##" + std::to_string(animations.size())).c_str(), &anim))
		{
			compSprite.AddAnimation(anim);
		}
	}
	ImGui::Columns(1);
}

void InspectorWindow::DrawScript(ComponentScript & compScript)
{
	if (ImGui::CollapsingHeader((compScript.GetName() + "##" + compScript.GetID()).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		LuaScript& script = *compScript.GetScript();
		std::vector<ScriptField*> fields = script.fields;

		for (ScriptField* field : fields)
		{
			switch (field->propertyType)
			{
			case ScriptField::INT:
			{
				int value = App->luaScripting->GetInt(script, field->fieldName);
				if (ImGui::EditorInputInt(field->fieldName, &value))
				{
					App->luaScripting->SetInt(script, field->fieldName, value);
				}
			}
			break;
			case ScriptField::FLOAT:
			{
				float value = App->luaScripting->GetFloat(script, field->fieldName);
				if (ImGui::EditorInputFloat(field->fieldName, &value))
				{
					App->luaScripting->SetFloat(script, field->fieldName, value);
				}
			}
			break;
			case ScriptField::DOUBLE:
			{
				double value = App->luaScripting->GetDouble(script, field->fieldName);
				/*if (ImGui::EditorInputDouble(field->fieldName, &value))
				{
					App->luaScripting->SetDouble(script, field->fieldName, value);
				}*/
			}
			break;
			case ScriptField::BOOL:
			{
				bool value = App->luaScripting->GetBool(script, field->fieldName);
				if (ImGui::EditorBool(field->fieldName, &value))
				{
					App->luaScripting->SetBool(script, field->fieldName, value);
				}
			}
			break;
			case ScriptField::STRING:
			{
				static char* value = _strdup(App->luaScripting->GetString(script, field->fieldName));
				if (ImGui::InputText(field->fieldName, value, SIZE_MAX, ImGuiInputTextFlags_AutoSelectAll))
				{
					App->luaScripting->SetString(script, field->fieldName, value);
				}
			}
			break;
			case ScriptField::VECTOR2:
			{
				glm::vec2 value = App->luaScripting->GetVec2(script, field->fieldName);
				if (ImGui::EditorInputFloat2(field->fieldName, (float*)&value))
				{
					App->luaScripting->SetVec2(script, field->fieldName, value);
				}
			}
			break;
			case ScriptField::VECTOR3:
			{
				glm::vec3 value = App->luaScripting->GetVec3(script, field->fieldName);
				if (ImGui::EditorInputFloat3(field->fieldName, (float*)&value))
				{
					App->luaScripting->SetVec3(script, field->fieldName, value);
				}
			}
			break;
			case ScriptField::VECTOR4:
			{
				glm::vec4 value = App->luaScripting->GetVec4(script, field->fieldName);
				/*if (ImGui::EditorInputFloat4(field->fieldName, (float*)&value))
				{
					App->luaScripting->SetVec4(script, field->fieldName, value);
				}*/
			}
			break;
			case ScriptField::GAMEOBJECT:
			{
				GameObject* value = (GameObject*)App->luaScripting->GetUserData(script, field->fieldName);
				if (ImGui::InputGameObject(field->fieldName, &value))
				{
					App->luaScripting->SetUserData(script, field->fieldName, (Resource*)value);
				}
			}
			break;
			case ScriptField::TEXTURE:
			{
				Texture* value = (Texture*)App->luaScripting->GetUserData(script, field->fieldName);
				if (ImGui::InputTexture(field->fieldName, &value))
				{
					App->luaScripting->SetUserData(script, field->fieldName, (Resource*)value);
				}
			}
			break;
			case ScriptField::ANIMATION:
			{
				Animation* value = (Animation*)App->luaScripting->GetUserData(script, field->fieldName);
				if (ImGui::InputAnimation(field->fieldName, &value))
				{
					App->luaScripting->SetUserData(script, field->fieldName, (Resource*)value);
				}
			}
			break;
			case ScriptField::SCRIPT:
			{
				LuaScript* value = (LuaScript*)App->luaScripting->GetUserData(script, field->fieldName);
				if (ImGui::InputLuaScript(field->fieldName, &value))
				{
					App->luaScripting->SetUserData(script, field->fieldName, (Resource*)value);
				}
			}
			break;
			}
		}
	}
	ImGui::Columns(1);
}

void InspectorWindow::DrawPhysicsBody(ComponentPhysicsBody & compPhysBody)
{
	if (ImGui::CollapsingHeader("Physics Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ComponentPhysicsBody::BODY_TYPE bodyType = compPhysBody.GetType();
		const char* strType[3] = { "Static", "Kinematic", "Dynamic" };

		if (ImGui::BeginCombo("Body Type", strType[bodyType]))
		{
			for (int i = 0; i < 3; i++)
			{
				if (ImGui::Selectable(strType[i]))
				{
					compPhysBody.SetType((ComponentPhysicsBody::BODY_TYPE)i);
				}
			}

			ImGui::EndCombo();
		}

		float gravityScale = compPhysBody.GetGravityScale();
		if(ImGui::EditorInputFloat("Gravity Scale", &gravityScale))
		{
			compPhysBody.SetGravityScale(gravityScale);
		}
	}
	ImGui::Columns(1);
}

void InspectorWindow::DrawBoxCollider(ComponentBoxCollider & compBoxColl)
{
	if (ImGui::CollapsingHeader(("Box Collider##" + compBoxColl.GetID()).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::string ID = compBoxColl.GetID();

		bool isSensor = compBoxColl.IsSensor();
		if (ImGui::EditorBool(("Sensor##"+ ID).c_str(), &isSensor))
		{
			compBoxColl.SetSensor(isSensor);
		}

		glm::vec2 offset = compBoxColl.GetOffset();
		if (ImGui::EditorDragFloat2(("Offset##" + ID).c_str(), (float*)&offset, 0.25f))
		{
			compBoxColl.SetOffset(offset);
		}

		glm::vec2 size = compBoxColl.GetSize();
		if (ImGui::EditorDragFloat2("Size", (float*)&size, 0.25f, 0))
		{
			compBoxColl.SetSize(size);
		}
	}
	ImGui::Columns(1);
}

void InspectorWindow::DrawCircleCollider(ComponentCircleCollider & compCircleColl)
{
	if (ImGui::CollapsingHeader(("Circle Collider##" + compCircleColl.GetID()).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::string ID = compCircleColl.GetID();

		bool isSensor = compCircleColl.IsSensor();
		if (ImGui::EditorBool(("Sensor##" + ID).c_str(), &isSensor))
		{
			compCircleColl.SetSensor(isSensor);
		}

		glm::vec2 offset = compCircleColl.GetOffset();
		if (ImGui::EditorDragFloat2(("Offset##" + ID).c_str(), (float*)&offset, 0.25f))
		{
			compCircleColl.SetOffset(offset);
		}

		float radius = compCircleColl.GetRadius();
		if (ImGui::EditorDragFloat("Radius", &radius, 0.25f, 0))
		{
			compCircleColl.SetRadius(radius);
		}
	}
	ImGui::Columns(1);
}

void InspectorWindow::ShowAddComponentWindow()
{
	bool active = true;
	
	ImGui::SetNextWindowPos(addComponentsWindowPos);
	ImGui::Begin("##AddComponentWindow", &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup);
	if (ImGui::MenuItem("Sprite"))
	{
		selectecGameObject->AddComponent(Component::SPRITE);
		showAddComponentsWindow = false;
	}
	if (ImGui::BeginMenu("Scripts"))
	{
		std::vector<LuaScript*> scripts = App->resourceManagerModule->GetScripts();
		for (LuaScript* script : scripts)
		{
			if (ImGui::MenuItem(script->GetName().c_str()))
			{
				ComponentScript* compScript = (ComponentScript*)selectecGameObject->AddComponent(Component::SCRIPT);
				compScript->SetScript(*script);
				showAddComponentsWindow = false;
				break;
			}
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Physics"))
	{
		if (ImGui::MenuItem("Body"))
		{
			selectecGameObject->AddComponent(Component::PHYSICS_BODY);
			showAddComponentsWindow = false;
		}
		if (ImGui::MenuItem("Box Collider"))
		{
			selectecGameObject->AddComponent(Component::BOX_COLLIDER);
			showAddComponentsWindow = false;
		}
		if (ImGui::MenuItem("Circle Collider"))
		{
			selectecGameObject->AddComponent(Component::CIRCLE_COLLIDER);
			showAddComponentsWindow = false;
		}
		ImGui::EndMenu();
	}
	ImGui::End();
	
	if (!showAddComponentsWindow)
	{
		ImGui::CloseCurrentPopup();
	}
}
