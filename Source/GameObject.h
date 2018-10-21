#pragma once

#include <vector>
#include "Component.h"
#include <gtc/matrix_transform.hpp>

class ComponentTransform;
class ComponentSprite;

class GameObject
{
public:
	GameObject(std::string name, GameObject* parent = nullptr);
	~GameObject();

	void SetParent(GameObject& parent);
	GameObject* GetParent() const;

	void SetPosition(float x, float y);
	void IncreasePosition(float x, float y);
	void SetRotation(float angle);
	void IncreaseRotation(float angle);
	void IncreaseScale(float x, float y);
	void SetScale(float x, float y);

	ComponentTransform* GetTransform() const;
	ComponentSprite* GetSprite() const;
	glm::mat4 GetModelMatrix() const;

	std::vector<Component*> GetComponents();
	void AddComponent(Component::COMPONENT_TYPE type);

	std::vector<GameObject*> GetChilds() const;
	void AddChild(GameObject* gameObject);
	void RemoveChild(GameObject* gameObject);

	void SetName(const char* name);
	std::string GetName() const;

	void SetActive(bool active);
	bool GetActive() const;

	std::string GetUID() const;

private:
	Component* GetComponentOfType(Component::COMPONENT_TYPE type) const;
	std::vector<Component*> GetComponentsOfType(Component::COMPONENT_TYPE type) const;

private:
	GameObject* parent;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	std::string name;
	bool active;
	std::string UID;
};

