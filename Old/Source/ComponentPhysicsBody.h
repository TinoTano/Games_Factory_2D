#pragma once
#include "Component.h"

class b2Body;

class ComponentPhysicsBody :
	public Component
{
public:
	enum BODY_TYPE
	{
		STATIC, KINEMATIC, DYNAMIC
	};

	ComponentPhysicsBody(GameObject& gameObject, const char* componentName);
	~ComponentPhysicsBody();

	void SetType(BODY_TYPE bodyType);
	BODY_TYPE GetType() const;

	b2Body* GetBody() const;

	void SetGravityScale(float use);
	float GetGravityScale() const;

	void SaveData(Data& data);
	void LoadData(Data& data);

private:
	b2Body* physicsBody;
};

