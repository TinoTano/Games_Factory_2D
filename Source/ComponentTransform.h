#pragma once

#include "Component.h"
#include <vec2.hpp>
#include <gtc/matrix_transform.hpp>

class ComponentTransform :
	public Component
{
public:
	ComponentTransform(GameObject& gameObject, const char* componentName);
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
	void IncreaseRotation(float angle);

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

	void UpdateMatrix();

	glm::mat4 GetModelMatrix() const;

	void SaveData(Data& data);
	void LoadData(Data& data);

private:
	glm::vec2 localPosition;
	float localRotation;
	glm::vec2 localScale;

	glm::vec2 globalPosition;
	float globalRotation;
	glm::vec2 globalScale;

	glm::mat4 modelMatrix;
};

