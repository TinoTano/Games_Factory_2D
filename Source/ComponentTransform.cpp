#include "ComponentTransform.h"
#include "GameObject.h"
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

	modelMatrix = glm::mat4(1);
}

ComponentTransform::~ComponentTransform()
{

}

void ComponentTransform::SetPosition(glm::vec2 position)
{
	GameObject* parent = gameObject->GetParent();

	if (parent != nullptr)
	{
		localPosition = position;
		globalPosition = position + parent->GetTransform()->GetGlobalPosition();
	}
	else
	{
		localPosition = position;
		globalPosition = position;
	}

	glm::vec2 diff = localPosition - prevPos;

	modelMatrix = glm::translate(modelMatrix, glm::vec3(diff, 0.0f));
}

void ComponentTransform::IncreasePosition(glm::vec2 increase)
{
	prevPos = localPosition;
	localPosition += increase;
	SetPosition(localPosition);
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

	if (parent != nullptr)
	{
		localRotation = angle;
		globalRotation = localRotation + parent->GetTransform()->GetGlobalRotation();
	}
	else
	{
		localRotation = angle;
		globalRotation = angle;
	}

	float diff = localRotation - prevRot;
	CONSOLE_LOG("%.3f", diff);

	modelMatrix = glm::translate(modelMatrix, glm::vec3(-globalPosition.x, -globalPosition.y, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(diff), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(globalPosition.x, globalPosition.y, 0.0f));
}

void ComponentTransform::IncreaseRotation(float angle)
{
	prevRot = localRotation;
	localRotation += angle;
	SetRotation(localRotation);
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

	if (parent != nullptr)
	{
		localScale = scale;
		globalScale = localScale + parent->GetTransform()->GetGlobalScale();
	}
	else
	{
		localScale = scale;
		globalScale = scale;
	}

	glm::vec2 origin(0, 0);

	float angle = -globalRotation * 3.141592654f / 180.f;
	float cosine = static_cast<float>(std::cos(angle));
	float sine = static_cast<float>(std::sin(angle));
	float sxc = globalScale.x * cosine;
	float syc = globalScale.y * cosine;
	float sxs = globalScale.x * sine;
	float sys = globalScale.y * sine;
	float tx = -origin.x * sxc - origin.y * sys + globalPosition.x;
	float ty = origin.x * sxs - origin.y * syc + globalPosition.y;

	modelMatrix = glm::mat4((glm::mat3(sxc, sys, tx,
		-sxs, syc, ty,
		0.f, 0.f, 1.f)));

	glm::vec2 diff = localScale - prevScale;
	/*diff.x = 1 * diff.x;
	diff.y = 1 * diff.y;*/
	//CONSOLE_LOG("%.3f, %.3f", diff.x, diff.y);
	//modelMatrix = glm::scale(modelMatrix, glm::vec3(globalScale, 1.0f));
}

void ComponentTransform::IncreaseScale(glm::vec2 increase)
{
	prevScale = localScale;
	localScale += increase;
	SetScale(localScale);
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

glm::mat4 ComponentTransform::GetModelMatrix() const
{
	return modelMatrix;
}
