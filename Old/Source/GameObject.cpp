#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentSprite.h"
#include "Log.h"
#include "Application.h"
#include "FileSystemModule.h"
#include "Data.h"
#include "SceneModule.h"
#include "ComponentScript.h"
#include "ComponentPhysicsBody.h"
#include "ComponentBoxCollider.h"
#include "ComponentCircleCollider.h"

GameObject::GameObject(std::string name, GameObject* parent) :
	Resource(name, "", "", Resource::RESOURCE_GAMEOBJECT)
{
	components.emplace_back(transform = new ComponentTransform(*this, "Transform"));

	if (parent != nullptr)
	{
		SetParent(*parent);
	}
	active = true;
}

GameObject::~GameObject()
{
	for (Component* component : components)
	{
		delete component;
		component = nullptr;
	}
	components.clear();

	for (GameObject* child : childs)
	{
		delete child;
		child = nullptr;
	}

	childs.clear();
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
	transform->SetPosition({ x, y });
}

void GameObject::IncreasePosition(float x, float y)
{
	transform->IncreasePosition({ x, y });
}

void GameObject::SetRotation(float angle)
{
	transform->SetRotation(angle);
}

void GameObject::IncreaseRotation(float angle)
{
	transform->IncreaseRotation(angle);
}

void GameObject::IncreaseScale(float x, float y)
{
	transform->IncreaseScale({ x, y });
}

void GameObject::SetScale(float x, float y)
{
	transform->SetScale({ x, y });
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
	std::vector<Component*> cmps;

	for (Component* c : components)
	{
		if (c->GetComponentType() == type)
		{
			cmps.emplace_back(c);
		}
	}

	return cmps;
}

glm::mat4 GameObject::GetModelMatrix() const
{
	return transform->GetModelMatrix();
}

void GameObject::UpdateModelMatrix()
{
	transform->UpdateMatrix();
}

std::vector<Component*> GameObject::GetComponents()
{
	return components;
}

Component* GameObject::AddComponent(Component::COMPONENT_TYPE type)
{
	Component* component = nullptr;

	switch (type)
	{
	case Component::TRANSFORM:
		component = transform;
		break;
	case Component::SPRITE:
		component = GetComponentOfType(Component::SPRITE);
		if (component == nullptr)
		{
			component = new ComponentSprite(*this, "Sprite");
		}
		break;
	case Component::SCRIPT:
		component = new ComponentScript(*this, "Script");
		break;
	case Component::PHYSICS_BODY:
		component = GetComponentOfType(Component::PHYSICS_BODY);
		if (component == nullptr)
		{
			component = new ComponentPhysicsBody(*this, "PhysBody");
		}
		break;
	case Component::BOX_COLLIDER:
		component = new ComponentBoxCollider(*this, "Box Collider");
		break;
	case Component::CIRCLE_COLLIDER:
		component = new ComponentCircleCollider(*this, "Circle Collider");
		break;
	default:
		break;
	}

	if (component != nullptr && type != Component::TRANSFORM)
	{
		components.emplace_back(component);
	}

	return component;
}

void GameObject::RemoveComponent(Component * component)
{
	std::remove(components.begin(), components.end(), component);

	delete component;
	component = nullptr;
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
	std::remove(childs.begin(), childs.end(), gameObject);
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}

bool GameObject::GetActive() const
{
	return active;
}

void GameObject::SaveData(Data & data)
{
	data.AddString("Parent", (parent) ? parent->GetUID() : "");
	data.AddString("Name", GetName());
	data.AddString("UID", GetUID());
	data.AddBool("Active", active);

	data.AddInt("ComponentsCount", components.size());
	for (int i = 0; i < components.size(); i++)
	{
		data.CreateSection(GetUID() + "Component" + std::to_string(i));
		components[i]->SaveData(data);
		data.CloseSection(GetUID() + "Component" + std::to_string(i));
	}
}

void GameObject::LoadData(Data & data)
{
	SetName(data.GetString("Name"));
	SetUID(data.GetString("UID"));
	active = data.GetBool("Active");

	GameObject* parent = App->sceneModule->FindGameObject(data.GetString("Parent"));
	if (parent != nullptr)
	{
		SetParent(*parent);
	}

	if (parent == nullptr) return; //Means is the root gameobject and there is no need to load components data

	int componentSize = data.GetInt("ComponentsCount");

	for (int i = 0; i < componentSize; i++)
	{
		Data sectionData;
		if (data.GetSectionData(GetUID() + "Component" + std::to_string(i), sectionData))
		{
			int compType = sectionData.GetInt("Type");
			Component* new_comp = AddComponent((Component::COMPONENT_TYPE)compType);

			if (new_comp != nullptr)
			{
				new_comp->LoadData(sectionData);
			}
		}
	}
}
