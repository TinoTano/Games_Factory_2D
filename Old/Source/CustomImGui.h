#pragma once

#include <vector>

class Resource;
class Texture;
class Animation;
class LuaScript;
class GameObject;
class Scene;

namespace ImGui
{
	bool InputTexture(const char* label, Texture** texture);
	bool InputAnimation(const char* label, Animation** animation);
	bool InputLuaScript(const char* label, LuaScript** script);
	bool InputGameObject(const char* label, GameObject** gameObject);
	bool InputScene(const char* label, Scene** scene);

	void Image(Texture& texture);

	bool EditorDragInt(const char* label, int* value, float v_speed = 1.0f, int v_min = 0, int v_max = 0);
	bool EditorDragFloat(const char* label, float* value, float v_speed = 1.0f, int v_min = 0, int v_max = 0);
	bool EditorDragFloat2(const char* label, float value[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0);
	bool EditorDragFloat3(const char* label, float value[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0);
	bool EditorInputInt(const char* label, int* value, float v_speed = 1.0f, int v_min = 0, int v_max = 0);
	bool EditorInputFloat(const char* label, float* value, float step = 0.0f, float step_fast = 0.0f);
	bool EditorInputFloat2(const char* label, float value[2]);
	bool EditorInputFloat3(const char* label, float value[3]);
	bool EditorBool(const char* label, bool* value);

	void EditableList(std::vector<Resource*>& resourceList);
}

