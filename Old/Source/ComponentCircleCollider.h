#pragma once
#include "ComponentCollider.h"

class ComponentCircleCollider :
	public ComponentCollider
{
public:
	ComponentCircleCollider(GameObject& gameObject, const char* componentName);
	~ComponentCircleCollider();

	void SetRadius(float radius);
	float GetRadius() const;

	void SetOffset(glm::vec2 offset);

	void SaveData(Data& data);
	void LoadData(Data& data);

private:
	float radius;
};

