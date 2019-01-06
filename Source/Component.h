#pragma once

#include <string>

class GameObject;
class Data;

class Component
{
public:
	enum COMPONENT_TYPE
	{
		TRANSFORM, SPRITE, SCRIPT, PHYSICS_BODY, BOX_COLLIDER, CIRCLE_COLLIDER, EDGE_COLLIDER, CHAIN_COLLIDER
	};

	Component(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type);
	virtual ~Component();

	virtual void CleanUp() {}

	GameObject* GetGameObject() const;
	COMPONENT_TYPE GetComponentType() const;

	bool GetActive() const;
	void SetActive(bool active);

	std::string GetName() const;
	std::string GetID() const;

	virtual void SaveData(Data& data) {}
	virtual void LoadData(Data& data) {}

protected:
	std::string componentName;
	GameObject* gameObject;
private:
	COMPONENT_TYPE componentType;
	bool active;
	std::string ID;
};

