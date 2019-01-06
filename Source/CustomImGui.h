#pragma once

#include <vector>

class Resource;
class Texture;
class Animation;
class LuaScript;
class GameObject;

namespace ImGui
{
	bool InputTexture(const char* label, Texture** texture);
	bool InputAnimation(const char* label, Animation** animation);
	bool InputLuaScript(const char* label, LuaScript** script);
	bool InputGameObject(const char* label, GameObject** gameObject);

	void EditableList(std::vector<Resource*>& resourceList);
}

