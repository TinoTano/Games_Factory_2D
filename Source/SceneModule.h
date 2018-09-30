#pragma once
#include "Module.h"
#include <vector>

class GameObject;

class SceneModule :
	public Module
{
public:
	SceneModule(const char* moduleName, bool gameModule = false);
	~SceneModule();

	bool Init();
	bool Update(float delta_time);

	std::vector<GameObject*> sceneGameObjects;
};

