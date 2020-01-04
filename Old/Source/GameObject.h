#pragma once

#include "Resource.h"
#include <vector>
#include "Component.h"
#include <gtc/matrix_transform.hpp>

class ComponentTransform;
class ComponentSprite;
class ComponentScript;
class ComponentPhysicsBody;
class ComponentBoxCollider;
class Data;

class GameObject :
	public Resource
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

	Component* GetComponentOfType(Component::COMPONENT_TYPE type) const;
	std::vector<Component*> GetComponentsOfType(Component::COMPONENT_TYPE type) const;

	glm::mat4 GetModelMatrix() const;
	void UpdateModelMatrix();

	std::vector<Component*> GetComponents();
	Component* AddComponent(Component::COMPONENT_TYPE type);
	void RemoveComponent(Component* component);

	std::vector<GameObject*> GetChilds() const;
	void AddChild(GameObject* gameObject);
	void RemoveChild(GameObject* gameObject);

	void SetActive(bool active);
	bool GetActive() const;

	void SaveData(Data& data);
	void LoadData(Data& data);

private:
	GameObject* parent;
	ComponentTransform* transform;
	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	bool active;
};

