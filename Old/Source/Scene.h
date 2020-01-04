#pragma once
#include "Resource.h"
#include <vector>

class GameObject;

class Scene :
	public Resource
{
public:
	Scene(std::string name, std::string assetsPath, std::string fullPath);
	~Scene();

	GameObject* GetRootGameObject() const;

private:
	GameObject* rootGameObject;
	std::vector<GameObject*> sceneGameObjects;
};

