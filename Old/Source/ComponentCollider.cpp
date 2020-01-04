#include "ComponentCollider.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentPhysicsBody.h"
#include "Application.h"
#include "Physics2DModule.h"
#include <vector>
#include <Dynamics/b2Fixture.h>

ComponentCollider::ComponentCollider(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type) :
	Component(gameObject, componentName, type)
{
	physBody = nullptr;

	std::vector<Component*> comp = gameObject.GetComponentsOfType(Component::PHYSICS_BODY);

	if (!comp.empty())
	{
		physBody = (ComponentPhysicsBody*)comp[0];
	}
	else
	{
		physBody = (ComponentPhysicsBody*)gameObject.AddComponent(Component::PHYSICS_BODY);
	}
}

ComponentCollider::~ComponentCollider()
{
	physBody = nullptr;
	fixture = nullptr;
}

void ComponentCollider::CleanUp()
{
	b2Body* body = physBody->GetBody();
	if (body != nullptr)
	{
		body->DestroyFixture(fixture);
	}
}

b2Fixture * ComponentCollider::GetFixture() const
{
	return fixture;
}

bool ComponentCollider::IsSensor() const
{
	return fixture->IsSensor();
}

void ComponentCollider::SetSensor(bool sensor)
{
	fixture->SetSensor(sensor);
}

glm::vec2 ComponentCollider::GetOffset() const
{
	return offset;
}

void ComponentCollider::SetPlatform()
{
	isPlatform = true;
}

bool ComponentCollider::IsPlatform() const
{
	return isPlatform;
}
