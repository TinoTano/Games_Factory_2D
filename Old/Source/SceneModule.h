#pragma once
#include "Module.h"
#include <vector>

class GameObject;
class Scene;
class Data;

class SceneModule :
	public Module
{
public:
	SceneModule(const char* moduleName, bool gameModule = false);
	~SceneModule();

	bool Init(Data& settings);
	bool PreUpdate(float deltaTime);
	bool Update(float deltaTime);
	bool CleanUp();

	void LoadScene(std::string path);
	void SaveScene(std::string path);
	void ClearScene();

	GameObject* CreateNewObject(GameObject* parent = nullptr, std::string name = "");
	GameObject* DuplicateGameObject(GameObject& go);
	void RemoveGameObject(GameObject& gameObject);

	GameObject* FindGameObject(std::string UID);

private:
	void NewScene();
	void DestroyScene();

public:
	Scene* currentScene;
    bool updateSceneVertices;
	std::vector<GameObject*> sceneGameObjects;
	bool clearScene;
};

