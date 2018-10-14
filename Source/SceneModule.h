#pragma once
#include "Module.h"

class GameObject;

class SceneModule :
	public Module
{
public:
	SceneModule(const char* moduleName, bool gameModule = false);
	~SceneModule();

	bool Init();
	bool Update(float delta_time);

	GameObject* CreateNewObject(GameObject* parent = nullptr, std::string name = "");

public:
	GameObject* rootGameObject;
    bool updateSceneVertices;
	int spritesNum;
};

