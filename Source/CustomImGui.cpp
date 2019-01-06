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

namespace ImGui
{
	bool InputResource(const char * label, Resource ** resource, Resource::ResourceType resType)
	{
		bool ret = false;
		std::string tag = "##Resources" + std::to_string(resType);

		std::string resourceName = "";
		if (*resource != nullptr)
		{
			resourceName = (*resource)->GetName();
		}
		ImGui::InputText(label, resourceName.data(), resourceName.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button(("..." + tag).c_str()))
		{
			ImGui::OpenPopup(("Resources Window" + tag).c_str());
		}

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

	void ImGui::EditableList(std::vector<Resource*>& resourceList)
	{
		/*for (auto& resource : resourceList)
		{
			std::string name = "##" + resource->GetName();
			ImGui::InputResource(name.c_str(), &resource);
		}*/
	}
}