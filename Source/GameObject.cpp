#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentSprite.h"
#include "Globals.h"
#include "Application.h"
#include "FileSystemModule.h"

GameObject::GameObject(std::string name, GameObject* parent) : name(name)
{
	components.emplace_back(new ComponentTransform(*this, "Transform", Component::TRANSFORM));

	if (parent != nullptr)
	{
		SetParent(*parent);
	}
	active = true;

	UID = App->fileSystemModule->CreateUID();
}

GameObject::~GameObject()
{
}

void GameObject::SetParent(GameObject& parent)
{
	if (this->parent == &parent) return;

	if (this->parent != nullptr)
	{
		this->parent->RemoveChild(this);
	}
	parent.AddChild(this);
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

void GameObject::IncreaseRotation(float angle)
{
	ComponentTransform* transform = GetTransform();
	transform->IncreaseRotation(angle);
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

glm::mat4 GameObject::GetModelMatrix() const
{
	ComponentTransform* transform = (ComponentTransform*)(GetComponentOfType(Component::TRANSFORM));

	return transform->GetModelMatrix();
}

std::vector<Component*> GameObject::GetComponents()
{
	return components;
}

void GameObject::AddComponent(Component::COMPONENT_TYPE type)
{
	switch (type)
	{
	case Component::TRANSFORM:
		CONSOLE_ERROR("GameObjects cannot have more than 1 transform component", NULL);
		break;
	case Component::SPRITE:
		if (GetSprite() != nullptr)
		{
			CONSOLE_ERROR("GameObjects cannot have more than 1 sprite component", NULL);
		}
		else
		{
			components.emplace_back(new ComponentSprite(*this, "Sprite", Component::SPRITE));
		}
		break;
	default:
		break;
	}
}

std::vector<GameObject*> GameObject::GetChilds() const
{
	return childs;
}

void GameObject::AddChild(GameObject* gameObject)
{
	childs.emplace_back(gameObject);
}

void GameObject::RemoveChild(GameObject * gameObject)
{
	childs.erase(std::remove(childs.begin(), childs.end(), gameObject));
}

void GameObject::SetName(const char * name)
{
	this->name = name;
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}

bool GameObject::GetActive() const
{
	return active;
}

std::string GameObject::GetUID() const
{
	return UID;
}
