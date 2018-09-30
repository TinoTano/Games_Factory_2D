#include "ComponentTransform.h"
#include "GameObject.h"
#include "ComponentSprite.h"
#include "Globals.h"

ComponentTransform::ComponentTransform(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type) :
	Component(gameObject, componentName, type)
{
	localPosition = { 0,0 };
	localRotation = 0;
	localScale = { 1,1 };

	globalPosition = { 0,0 };
	globalRotation = 0;
	globalScale = { 1,1 };
}

ComponentTransform::~ComponentTransform()
{

}

void ComponentTransform::SetPosition(glm::vec2 position)
{
	GameObject* parent = gameObject->GetParent();
	ComponentSprite* sprite = gameObject->GetSprite();
	if (parent != nullptr)
	{
		localPosition = position;
		globalPosition = position + parent->GetTransform()->GetGlobalPosition();
	}
	else
	{
		localPosition = position;
		globalPosition = position;

		if (sprite != nullptr)
		{
			sprite->UpdateVerticesPositions(globalPosition.x, globalPosition.y);
		}
	}
}

void ComponentTransform::IncreasePosition(glm::vec2 increase)
{
	float i = globalPosition.x + increase.x;
	SetPosition({ i, globalPosition.y });
}

glm::vec2 ComponentTransform::GetLocalPosition() const
{
	return localPosition;
}

float ComponentTransform::GetLocalPositionX() const
{
	return localPosition.x;
}

float ComponentTransform::GetLocalPositionY() const
{
	return localPosition.y;
}

glm::vec2 ComponentTransform::GetGlobalPosition() const
{
	return globalPosition;
}

float ComponentTransform::GetGlobalPositionX() const
{
	return globalPosition.x;
}

float ComponentTransform::GetGlobalPositionY() const
{
	return globalPosition.y;
}

void ComponentTransform::SetRotation(float angle)
{
	GameObject* parent = gameObject->GetParent();
	ComponentSprite* sprite = gameObject->GetSprite();
	if (parent != nullptr)
	{
		localRotation += angle;
		globalRotation = localRotation + parent->GetTransform()->GetGlobalRotation();
	}
	else
	{
		localPosition += angle;
		globalPosition += angle;

		if (sprite != nullptr)
		{
			sprite->UpdateVerticesRotation(angle);
		}
	}
}

float ComponentTransform::GetLocalRotation() const
{
	return localRotation;
}

float ComponentTransform::GetGlobalRotation() const
{
	return globalRotation;
}

void ComponentTransform::SetScale(glm::vec2 scale)
{
	GameObject* parent = gameObject->GetParent();
	ComponentSprite* sprite = gameObject->GetSprite();
	if (parent != nullptr)
	{
		localScale = scale;
		globalScale = localScale + parent->GetTransform()->GetGlobalScale();
	}
	else
	{
		localScale = scale;
		globalScale = scale;

		if (sprite != nullptr)
		{
			sprite->UpdateVerticesScale(scale.x, scale.y);
		}
	}
}

void ComponentTransform::IncreaseScale(glm::vec2 increase)
{
	CONSOLE_LOG("%.3f, %.3f", (localScale + increase).x, (localScale + increase).y);
	float i = 1 + increase.x;
	SetScale({ i, i });
}

glm::vec2 ComponentTransform::GetLocalScale() const
{
	return localScale;
}

float ComponentTransform::GetLocalScaleX() const
{
	return localScale.x;
}

float ComponentTransform::GetLocalScaleY() const
{
	return localScale.y;
}

glm::vec2 ComponentTransform::GetGlobalScale() const
{
	return globalScale;
}

float ComponentTransform::GetGlobalScaleX() const
{
	return globalScale.x;
}

float ComponentTransform::GetGlobalScaleY() const
{
	return globalScale.y;
}
