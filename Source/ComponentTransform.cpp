#include "ComponentTransform.h"
#include "GameObject.h"
#include "Globals.h"
#include "Data.h"
#include "ComponentPhysicsBody.h"
#include "Application.h"
#include "Physics2DModule.h"

ComponentTransform::ComponentTransform(GameObject& gameObject, const char* componentName) :
	Component(gameObject, componentName, COMPONENT_TYPE::TRANSFORM)
{
	localPosition = globalPosition = { 0,0 };
	localRotation = globalRotation = 0;
	localScale = globalScale = { 1,1 };

	modelMatrix = glm::mat4(1);
	UpdateMatrix();
}

ComponentTransform::~ComponentTransform()
{

}

void ComponentTransform::SetPosition(glm::vec2 position)
{
	localPosition = position;
	UpdateMatrix();
}

void ComponentTransform::IncreasePosition(glm::vec2 increase)
{
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
	localRotation = angle;
	UpdateMatrix();
}

void ComponentTransform::IncreaseRotation(float angle)
{
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
	if (scale.x < 0 || scale.y < 0)
	{
		if (scale.x < 0) scale.x = 0;
		if (scale.y < 0) scale.y = 0;
		return;
	}

	localScale = scale;
	UpdateMatrix();
}

void ComponentTransform::IncreaseScale(glm::vec2 increase)
{
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

void ComponentTransform::UpdateMatrix()
{
	GameObject* parent = gameObject->GetParent();
	glm::mat4 parentMatrix = (parent) ? parent->GetModelMatrix() : glm::mat4(1);

	glm::mat4 m = glm::mat4(1);
	m = glm::translate(m, glm::vec3(localPosition, 1.0f));
	m = glm::rotate(m, glm::radians(localRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	m = glm::scale(m, glm::vec3(localScale, 1.0f));
	modelMatrix = parentMatrix * m;
	
	globalPosition = { modelMatrix[3].x, modelMatrix[3].y };
	globalRotation = glm::degrees(glm::atan(modelMatrix[0][1], modelMatrix[0][0]));
	globalScale = { glm::length(modelMatrix[0]), glm::length(modelMatrix[1]) };

	ComponentPhysicsBody* compPhysBody = (ComponentPhysicsBody*)gameObject->GetComponentOfType(Component::PHYSICS_BODY);

	if (compPhysBody != nullptr)
	{
		App->physics2DModule->SetBodyTransform(compPhysBody->GetBody(), globalPosition, globalRotation, globalScale);
	}

	std::vector<GameObject*> childs = gameObject->GetChilds();
	for (GameObject* child : childs)
	{
		child->UpdateModelMatrix();
	}
}

glm::mat4 ComponentTransform::GetModelMatrix() const
{
	return modelMatrix;
}

void ComponentTransform::SaveData(Data & data)
{
	data.AddInt("Type", GetComponentType());
	data.AddFloat("LocalPositionX", localPosition.x);
	data.AddFloat("LocalPositionY", localPosition.y);
	data.AddFloat("Rotation", localRotation);
	data.AddFloat("LocalScaleX", localScale.x);
	data.AddFloat("LocalScaleY", localScale.y);
}

void ComponentTransform::LoadData(Data & data)
{
	localPosition = { data.GetFloat("LocalPositionX"), data.GetFloat("LocalPositionY") };
	localRotation = data.GetFloat("Rotation");
	localScale = { data.GetFloat("LocalScaleX"), data.GetFloat("LocalScaleY") };
	UpdateMatrix();
}
