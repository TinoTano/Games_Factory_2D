#pragma once
#include "Module.h"
#include <vector>
#include <list>

class GameObject;

class SceneModule :
	public Module
{
public:
	SceneModule(const char* moduleName, bool gameModule = false);
	~SceneModule();

	bool Init();
	bool Update(float delta_time);

	void CreateNewObject(GameObject* parent);

public:
	std::vector<GameObject*> sceneGameObjects;

};

