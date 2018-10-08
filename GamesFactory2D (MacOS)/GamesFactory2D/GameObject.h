#pragma once

#include <vector>
#include "Component.h"
#include <gtc/matrix_transform.hpp>

class ComponentTransform;
class ComponentSprite;

class GameObject
{
public:
	GameObject();
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

private:
	Component* GetComponentOfType(Component::COMPONENT_TYPE type) const;
	std::vector<Component*> GetComponentsOfType(Component::COMPONENT_TYPE type) const;

private:
	GameObject* parent;
	std::vector<Component*> components;
};

