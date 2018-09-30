#pragma once

#include "Component.h"
#include "ThirdParty/glm/vec2.hpp"

class ComponentTransform :
	public Component
{
public:
	ComponentTransform(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type);
	~ComponentTransform();

	void SetPosition(glm::vec2 position);
	void IncreasePosition(glm::vec2 increase);

	glm::vec2 GetLocalPosition() const;
	float GetLocalPositionX() const;
	float GetLocalPositionY() const;

	glm::vec2 GetGlobalPosition() const;
	float GetGlobalPositionX() const;
	float GetGlobalPositionY() const;

	void SetRotation(float angle);
	float GetLocalRotation() const;
	float GetGlobalRotation() const;

	void SetScale(glm::vec2 scale);
	void IncreaseScale(glm::vec2 increase);

	glm::vec2 GetLocalScale() const;
	float GetLocalScaleX() const;
	float GetLocalScaleY() const;

	glm::vec2 GetGlobalScale() const;
	float GetGlobalScaleX() const;
	float GetGlobalScaleY() const;

private:
	glm::vec2 localPosition;
	float localRotation;
	glm::vec2 localScale;

	glm::vec2 globalPosition;
	float globalRotation;
	glm::vec2 globalScale;
};

