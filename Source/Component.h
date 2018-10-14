#pragma once

#include <string>

class GameObject;

class Component
{
public:
	enum COMPONENT_TYPE
	{
		TRANSFORM, SPRITE
	};

	Component(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type);
	~Component();

	GameObject* GetGameObject() const;
	COMPONENT_TYPE GetComponentType() const;

	bool GetActive() const;
	void SetActive(bool active);

protected:
	std::string componentName;
	GameObject* gameObject;
private:
	COMPONENT_TYPE componentType;
	bool active;
};

