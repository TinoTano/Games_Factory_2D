#include "ComponentCircleCollider.h"
#include "Physics2DModule.h"
#include "Application.h"
#include <Dynamics/b2Fixture.h>
#include <Collision/Shapes/b2CircleShape.h>
#include "ComponentPhysicsBody.h"
#include "GameObject.h"
#include "Component.h"
#include "Data.h"
#include "ComponentTransform.h"
#include <algorithm>

ComponentCircleCollider::ComponentCircleCollider(GameObject& gameObject, const char* componentName) :
	ComponentCollider(gameObject, componentName, COMPONENT_TYPE::CIRCLE_COLLIDER)
{
	if (physBody != nullptr)
	{
		ComponentTransform* transform = (ComponentTransform*)gameObject.GetComponentOfType(Component::TRANSFORM);
		glm::vec2 scale = transform->GetGlobalScale();
		float maxScale = std::max(scale.x, scale.y);
		fixture = App->physics2DModule->CreateCircleCollider(physBody->GetBody(), maxScale * 0.5f);
	}

	radius = 0.5f;
}

ComponentCircleCollider::~ComponentCircleCollider()
{
}

void ComponentCircleCollider::SetRadius(float radius)
{
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponentOfType(Component::TRANSFORM);
	b2CircleShape& shape = *(b2CircleShape*)fixture->GetShape();
	glm::vec2 scale = transform->GetGlobalScale();
	shape.m_radius = 0.02f * scale.x * radius;
	this->radius = radius;
}

float ComponentCircleCollider::GetRadius() const
{
	return radius;
}

void ComponentCircleCollider::SetOffset(glm::vec2 offset)
{
	glm::vec2 diff = offset - this->offset;
	b2CircleShape& shape = *(b2CircleShape*)fixture->GetShape();

	shape.m_p.x += diff.x;
	shape.m_p.y += diff.y;

	this->offset = offset;
}

void ComponentCircleCollider::SaveData(Data & data)
{
	data.AddInt("Type", GetComponentType());
}

void ComponentCircleCollider::LoadData(Data & data)
{
}
