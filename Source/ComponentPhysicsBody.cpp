#include "ComponentPhysicsBody.h"
#include "Physics2DModule.h"
#include "Application.h"
#include <Dynamics/b2Body.h>
#include "Data.h"

ComponentPhysicsBody::ComponentPhysicsBody(GameObject& gameObject, const char* componentName) :
	Component(gameObject, componentName, COMPONENT_TYPE::PHYSICS_BODY)
{
	physicsBody = App->physics2DModule->CreateBody(&gameObject);
}

ComponentPhysicsBody::~ComponentPhysicsBody()
{
	App->physics2DModule->DestroyBody(physicsBody);
	physicsBody = nullptr;
}

void ComponentPhysicsBody::SetType(BODY_TYPE bodyType)
{
	switch (bodyType)
	{
	case STATIC:
		physicsBody->SetType(b2_staticBody);
		break;
	case KINEMATIC:
		physicsBody->SetType(b2_kinematicBody);
		break;
	case DYNAMIC:
		physicsBody->SetType(b2_dynamicBody);
		break;
	default:
		break;
	}
}

ComponentPhysicsBody::BODY_TYPE ComponentPhysicsBody::GetType() const
{
	return (BODY_TYPE)physicsBody->GetType();
}

b2Body * ComponentPhysicsBody::GetBody() const
{
	return physicsBody;
}

void ComponentPhysicsBody::SetGravityScale(float use)
{
	physicsBody->SetGravityScale(use);
}

float ComponentPhysicsBody::GetGravityScale() const
{
	return physicsBody->GetGravityScale();
}

void ComponentPhysicsBody::SaveData(Data & data)
{
	data.AddInt("Type", GetComponentType());
}

void ComponentPhysicsBody::LoadData(Data & data)
{
}
