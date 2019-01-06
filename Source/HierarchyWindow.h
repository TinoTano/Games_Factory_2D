#pragma once
#include "EditorWindow.h"
#include <imgui.h>

class GameObject;

class HierarchyWindow :
	public EditorWindow
{
public:
	HierarchyWindow(const char* windowName, bool enabled = true);
	~HierarchyWindow();

	void DrawWindow();

	void RemoveSelectedGameObject();

private:
	void ShowGameObjectOptions();
	void CheckMouseSelect(GameObject& gameObject);

private:
	void DrawNodes(GameObject* gameObject);
	GameObject* selectedGameObject;
	bool showGameObjectOptions;
	ImVec2 optionsWindowPos;
};

