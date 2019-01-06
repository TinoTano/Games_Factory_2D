#pragma once
#include "ComponentCollider.h"

class ComponentBoxCollider :
	public ComponentCollider
{
public:
	ComponentBoxCollider(GameObject& gameObject, const char* componentName);
	~ComponentBoxCollider();

	void SetSize(glm::vec2 size);
	glm::vec2 GetSize() const;

	void SetOffset(glm::vec2 offset);

	void SaveData(Data& data);
	void LoadData(Data& data);

private:
	glm::vec2 size;
};

