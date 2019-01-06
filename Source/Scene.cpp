#include "Scene.h"
#include "GameObject.h"

Scene::Scene(std::string name, std::string assetsPath, std::string fullPath) : 
	Resource(name, assetsPath, fullPath, Resource::RESOURCE_SCENE)
{
	rootGameObject = new GameObject("Root GameObject");
}

Scene::~Scene()
{
	delete rootGameObject;
	rootGameObject = nullptr;
}

GameObject * Scene::GetRootGameObject() const
{
	return rootGameObject;
}
