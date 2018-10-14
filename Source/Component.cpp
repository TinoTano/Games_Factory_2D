#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type) :
	gameObject(&gameObject), componentName(componentName), componentType(type)
{
	active = true;
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
