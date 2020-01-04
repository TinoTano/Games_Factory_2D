#pragma once
#include "EditorWindow.h"
#include <imgui.h>

class GameObject;
class ComponentTransform;
class ComponentSprite;
class ComponentScript;
class ComponentPhysicsBody;
class ComponentBoxCollider;
class ComponentCircleCollider;

class InspectorWindow : public EditorWindow
{
public:
    InspectorWindow(const char* windowName, bool enabled = true);
    virtual ~InspectorWindow();
    
    void DrawWindow();
	
	void SetSelectedGameObject(GameObject* go);

private:
	void DrawInfo(GameObject& go);
	void DrawTransform(ComponentTransform& compTransform);
	void DrawSprite(ComponentSprite& compSprite);
	void DrawScript(ComponentScript& compScript);
	void DrawPhysicsBody(ComponentPhysicsBody& compPhysBody);
	void DrawBoxCollider(ComponentBoxCollider& compBoxColl);
	void DrawCircleCollider(ComponentCircleCollider& compCircleColl);

	void ShowAddComponentWindow();

private:
	GameObject* selectecGameObject;
	bool showAddComponentsWindow;
	ImVec2 addComponentsWindowPos;
};
