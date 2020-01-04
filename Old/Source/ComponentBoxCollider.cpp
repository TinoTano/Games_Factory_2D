#include "ComponentBoxCollider.h"
#include "Physics2DModule.h"
#include "Application.h"
#include <Dynamics/b2Fixture.h>
#include <Collision/Shapes/b2PolygonShape.h>
#include "ComponentPhysicsBody.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Data.h"

ComponentBoxCollider::ComponentBoxCollider(GameObject& gameObject, const char* componentName) :
	ComponentCollider(gameObject, componentName, COMPONENT_TYPE::BOX_COLLIDER)
{
	if (physBody != nullptr)
	{
		ComponentTransform* transform = (ComponentTransform*)gameObject.GetComponentOfType(Component::TRANSFORM);
		glm::vec2 scale = transform->GetGlobalScale();
		fixture = App->physics2DModule->CreateBoxCollider(physBody->GetBody(), scale.x * 1.f, scale.y * 1.f);
	}
	size = { 1.f, 1.f };
}

ComponentBoxCollider::~ComponentBoxCollider()
{
}

void ComponentBoxCollider::SetSize(glm::vec2 size)
{
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponentOfType(Component::TRANSFORM);
	b2PolygonShape& shape = *(b2PolygonShape*)fixture->GetShape();
	glm::vec2 scale = transform->GetGlobalScale();
	shape.SetAsBox(0.02f * scale.x * size.x, 0.02f * scale.y * size.y);
	this->size = size;
}

glm::vec2 ComponentBoxCollider::GetSize() const
{
	return size;
}

void ComponentBoxCollider::SetOffset(glm::vec2 offset)
{
	glm::vec2 diff = offset - this->offset;
	b2PolygonShape& shape = *(b2PolygonShape*)fixture->GetShape();

	for (int i = 0; i < 4; i++)
	{
		shape.m_vertices[i].x += diff.x;
		shape.m_vertices[i].y += diff.y;
	}
	shape.m_centroid.x += diff.x;
	shape.m_centroid.y += diff.y;

	this->offset = offset;
}

void ComponentBoxCollider::SaveData(Data & data)
{
	data.AddInt("Type", GetComponentType());
}

void ComponentBoxCollider::LoadData(Data & data)
{
}
