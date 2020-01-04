#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "FileSystemModule.h"

Component::Component(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type) :
	gameObject(&gameObject), componentName(componentName), componentType(type)
{
	active = true;
	ID = App->fileSystemModule->CreateUID();
}

Component::~Component()
{
}

GameObject* Component::GetGameObject() const
{
	return gameObject;
}

Component::COMPONENT_TYPE Component::GetComponentType() const
{
	return componentType;
}

bool Component::GetActive() const
{
	return active;
}

void Component::SetActive(bool active)
{
	this->active = active;
}

std::string Component::GetName() const
{
	return componentName;
}

std::string Component::GetID() const
{
	return ID;
}
