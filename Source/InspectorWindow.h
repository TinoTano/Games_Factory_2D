#pragma once
#include "EditorWindow.h"
#include <imgui.h>

class GameObject;
class ComponentTransform;
class ComponentSprite;

class InspectorWindow : public EditorWindow
{
public:
    InspectorWindow(const char* windowName, bool enabled = true);
    ~InspectorWindow();
    
    void DrawWindow();
	
	void SetSelectedGameObject(GameObject* go);

private:
	void DrawInfo(GameObject& go);
	void DrawTransform(ComponentTransform& transform);
	void DrawSprite(ComponentSprite& transform);

	void ShowAddComponentWindow();

private:
	GameObject* selectecGameObject;
	bool showAddComponentsWindow;
	ImVec2 addComponentsWindowPos;
};
