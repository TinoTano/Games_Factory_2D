#pragma once
#include "Component.h"
#include <vec2.hpp>

class b2Fixture;
class ComponentPhysicsBody;

class ComponentCollider :
	public Component
{
public:
	ComponentCollider(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type);
	~ComponentCollider();

	void CleanUp();

	b2Fixture* GetFixture() const;

	bool IsSensor() const;
	void SetSensor(bool sensor);

	virtual void SetOffset(glm::vec2 offset) = 0;
	glm::vec2 GetOffset() const;

	void SetPlatform();
	bool IsPlatform() const;

	virtual void SaveData(Data& data) = 0;
	virtual void LoadData(Data& data) = 0;

protected:
	glm::vec2 offset;
	b2Fixture* fixture;
	ComponentPhysicsBody* physBody;

private:
	bool isPlatform;
};

