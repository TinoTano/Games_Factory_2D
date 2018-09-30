#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentSprite.h"

GameObject::GameObject()
{
	parent = nullptr;

	components.emplace_back(new ComponentTransform(*this, "Transform", Component::TRANSFORM));
	components.emplace_back(new ComponentSprite(*this, "Sprite", Component::SPRITE));
}

GameObject::~GameObject()
{
}

void GameObject::SetParent(GameObject& parent)
{
	this->parent = &parent;
}

GameObject* GameObject::GetParent() const
{
	return parent;
}

void GameObject::SetPosition(float x, float y)
{
	ComponentTransform* transform = GetTransform();
	transform->SetPosition({ x, y });
}

void GameObject::IncreasePosition(float x, float y)
{
	ComponentTransform* transform = GetTransform();
	transform->IncreasePosition({ x, y });
}

void GameObject::SetRotation(float angle)
{
	ComponentTransform* transform = GetTransform();
	transform->SetRotation(angle);
}

void GameObject::IncreaseScale(float x, float y)
{
	ComponentTransform* transform = GetTransform();
	transform->IncreaseScale({ x, y });
}

void GameObject::SetScale(float x, float y)
{
	ComponentTransform* transform = GetTransform();
	transform->SetScale({ x, y });
}

ComponentTransform* GameObject::GetTransform() const
{
	ComponentTransform* transform = (ComponentTransform*)(GetComponentOfType(Component::TRANSFORM));

	return transform;
}

ComponentSprite* GameObject::GetSprite() const
{
	ComponentSprite* sprite = (ComponentSprite*)(GetComponentOfType(Component::SPRITE));

	return sprite;
}

Component* GameObject::GetComponentOfType(Component::COMPONENT_TYPE type) const
{
	Component* component = nullptr;

	for (Component* c : components)
	{
		if (c->GetComponentType() == type)
		{
			component = c;
			break;
		}
	}

	return component;
}

std::vector<Component*> GameObject::GetComponentsOfType(Component::COMPONENT_TYPE type) const
{
	std::vector<Component*> components;

	for (Component* c : components)
	{
		if (c->GetComponentType() == type)
		{
			components.emplace_back(c);
		}
	}

	return components;
}
