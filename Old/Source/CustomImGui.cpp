#include "CustomImGui.h"
#include <imgui.h>
#include "Application.h"
#include "Texture.h"
#include "ResourceManagerModule.h"
#include "SceneModule.h"
#include "Animation.h"
#include "LuaScript.h"
#include "GameObject.h"
#include <vector>
#include <imgui_impl_vulkan.h>
#include "Scene.h"
#include "Log.h"

namespace ImGui
{
	bool InputResource(std::string label, Resource ** resource, Resource::ResourceType resType)
	{
		bool ret = false;
		std::string tag = "##Resources" + std::to_string(resType);

		std::string resourceName = "";
		if (*resource != nullptr)
		{
			resourceName = (*resource)->GetName();
		}
		ImGui::InputText(("##" + label).c_str(), resourceName.data(), resourceName.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button(("..." + tag).c_str()))
		{
			ImGui::OpenPopup(("Resources Window" + tag).c_str());
		}
		ImGui::SameLine();
		ImGui::Text(label.c_str());

		if (ImGui::IsPopupOpen(("Resources Window" + tag).c_str()))
		{
			bool open = true;
			ImGui::Begin("Resources", &open, ImGuiWindowFlags_Popup);

			if (ImGui::Selectable("None"))
			{
				*resource = nullptr;
				ret = true;
			}

			if (!ret)
			{
				switch (resType)
				{
				case Resource::RESOURCE_TEXTURE:
				{
					std::vector<Texture*> textures = App->resourceManagerModule->GetTextures();

					for (Texture* texture : textures)
					{
						if (ImGui::Selectable(texture->GetName().c_str()))
						{
							*resource = (Resource*)texture;
							ret = true;
							break;
						}
					}
				}
				break;
				case Resource::RESOURCE_ANIM:
				{
					std::vector<Animation*> anims = App->resourceManagerModule->GetAnimations();

					for (Animation* anim : anims)
					{
						if (ImGui::Selectable(anim->GetName().c_str()))
						{
							*resource = (Resource*)anim;
							ret = true;
							break;
						}
					}
				}
				break;
				case Resource::RESOURCE_LUASCRIPT:
				{
					std::vector<LuaScript*> scripts = App->resourceManagerModule->GetScripts();

					for (LuaScript* script : scripts)
					{
						if (ImGui::Selectable(script->GetName().c_str()))
						{
							*resource = (Resource*)script;
							ret = true;
							break;
						}
					}
				}
				break;
				case Resource::RESOURCE_GAMEOBJECT:
				{
					std::vector<GameObject*> gameObjects = App->sceneModule->sceneGameObjects;
					gameObjects.erase(gameObjects.begin());

					for (GameObject* gameObject : gameObjects)
					{
						if (ImGui::Selectable(gameObject->GetName().c_str()))
						{
							*resource = (Resource*)gameObject;
							ret = true;
							break;
						}
					}
				}
				break;
				case Resource::RESOURCE_SCENE:
				{
					std::vector<Scene*> scenes = App->resourceManagerModule->GetScenes();

					for (Scene* scene : scenes)
					{
						if (ImGui::Selectable(scene->GetName().c_str()))
						{
							*resource = (Resource*)scene;
							ret = true;
							break;
						}
					}
				}
				break;
				default:
					break;
				}
			}

			if (ret == true)
			{
				CloseCurrentPopup();
			}

			ImGui::End();
		}

		return ret;
	}

	bool InputTexture(const char * label, Texture ** texture)
	{
		return InputResource(label, (Resource**)texture, Resource::RESOURCE_TEXTURE);
	}

	bool InputAnimation(const char * label, Animation ** animation)
	{
		return InputResource(label, (Resource**)animation, Resource::RESOURCE_ANIM);
	}

	bool InputLuaScript(const char * label, LuaScript ** script)
	{
		return InputResource(label, (Resource**)script, Resource::RESOURCE_LUASCRIPT);
	}

	bool InputGameObject(const char * label, GameObject ** gameObject)
	{
		return InputResource(label, (Resource**)gameObject, Resource::RESOURCE_GAMEOBJECT);
	}

	bool InputScene(const char * label, Scene ** scene)
	{
		return InputResource(label, (Resource**)scene, Resource::RESOURCE_SCENE);
	}

	void Image(Texture & texture)
	{
		ImTextureID id = ImGui_ImplVulkan_AddTexture(texture.textureSampler, texture.textureImageView, (VkImageLayout)texture.textureImageLayout);
		ImVec2 size(texture.GetWidth(), texture.GetHeight());
		ImGui::Image(id, size);
	}

	bool EditorDragInt(const char * label, int * value, float v_speed, int v_min, int v_max)
	{
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::DragInt(("##" + std::string(label)).c_str(), value, v_speed, v_min, v_max);
		ImGui::NextColumn();
		return true;
	}

	bool EditorDragFloat(const char * label, float * value, float v_speed, int v_min, int v_max)
	{
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::DragFloat(("##" + std::string(label)).c_str(), value, v_speed, v_min, v_max);
		ImGui::NextColumn();
		return true;
	}

	bool EditorDragFloat2(const char * label, float value[2], float v_speed, int v_min, int v_max)
	{
		bool ret = false;
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ret = ImGui::DragFloat2(("##" + std::string(label)).c_str(), value, v_speed, v_min, v_max);
		ImGui::NextColumn();
		return ret;
	}

	bool EditorDragFloat3(const char * label, float value[3], float v_speed, int v_min, int v_max)
	{
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::DragFloat3(("##" + std::string(label)).c_str(), value, v_speed, v_min, v_max);
		ImGui::NextColumn();
		return true;
	}

	bool EditorInputInt(const char * label, int * value, float v_speed, int v_min, int v_max)
	{
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::InputInt(("##" + std::string(label)).c_str(), value, v_speed, v_min, v_max);
		ImGui::NextColumn();
		return true;
	}

	bool EditorInputFloat(const char * label, float * value, float step, float step_fast)
	{
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::InputFloat(("##" + std::string(label)).c_str(), value, step, step_fast);
		ImGui::NextColumn();
		return true;
	}

	bool EditorInputFloat2(const char * label, float value[2])
	{
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::InputFloat2(("##" + std::string(label)).c_str(), value);
		ImGui::NextColumn();
		return true;
	}

	bool EditorInputFloat3(const char * label, float value[3])
	{
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::InputFloat3(("##" + std::string(label)).c_str(), value);
		ImGui::NextColumn();
		return true;
	}

	bool EditorBool(const char * label, bool * value)
	{
		if (ImGui::GetColumnsCount() == 1) ImGui::Columns(2);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::Checkbox(("##" + std::string(label)).c_str(), value);
		ImGui::NextColumn();
		return true;
	}

	void ImGui::EditableList(std::vector<Resource*>& resourceList)
	{
		/*for (auto& resource : resourceList)
		{
			std::string name = "##" + resource->GetName();
			ImGui::InputResource(name.c_str(), &resource);
		}*/
	}
}