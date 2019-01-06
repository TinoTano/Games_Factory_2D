#include "SceneModule.h"
#include "GameObject.h"
#include "Application.h"
#include "CameraModule.h"
#include "ComponentTransform.h"
#include "Scene.h"
#include "Data.h"
#include "Log.h"
#include "FileSystemModule.h"

SceneModule::SceneModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
    updateSceneVertices = false;
	currentScene = nullptr;
	clearScene = false;
}

SceneModule::~SceneModule()
{
}

bool SceneModule::Init()
{
	NewScene();
    return true;
}

bool SceneModule::PreUpdate(float delta_time)
{
	if (clearScene)
	{
		NewScene();
		clearScene = false;
	}
	return true;
}

bool SceneModule::Update(float delta_time)
{

    return true;
}

bool SceneModule::CleanUp()
{
	DestroyScene();
	return true;
}

void SceneModule::NewScene()
{
	DestroyScene();
	currentScene = new Scene("Default", "", "");
	sceneGameObjects.clear();
	sceneGameObjects.emplace_back(currentScene->GetRootGameObject());
}

void SceneModule::DestroyScene()
{
	if (currentScene != nullptr)
	{
		delete currentScene;
		currentScene = nullptr;
	}
}

void SceneModule::LoadScene(std::string path)
{
	Data data;

	if (data.LoadData(path))
	{
		int dataType = data.GetInt("Type");

		if (dataType != Resource::RESOURCE_SCENE)
		{
			CONSOLE_ERROR("%s", "Failed to load a Scene. " + path + " Is not a valid Scene Path");
			return;
		}

		NewScene();

		int gameObjectCount = data.GetInt("GameObjectCount");

		for (int i = 0; i < gameObjectCount; i++)
		{
			Data sectionData;
			if (data.GetSectionData("GameObject" + std::to_string(i), sectionData))
			{
				GameObject* gameObject = nullptr;
				if (i == 0)
				{
					gameObject = currentScene->GetRootGameObject();
				}
				else
				{
					gameObject = App->sceneModule->CreateNewObject();
				}
				gameObject->LoadData(sectionData);
			}
		}
	}
}

void SceneModule::SaveScene(std::string path)
{
	Data data;

	data.AddInt("Type", Resource::RESOURCE_SCENE);
	data.AddInt("GameObjectCount", sceneGameObjects.size());
	for (int i = 0; i < sceneGameObjects.size(); i++)
	{
		data.CreateSection("GameObject" + std::to_string(i));
		sceneGameObjects[i]->SaveData(data);
		data.CloseSection("GameObject" + std::to_string(i));
	}
	if (App->fileSystemModule->GetExtension(path) != ".scene")
	{
		path += ".scene";
	}
	data.SaveData(path);
}

void SceneModule::ClearScene()
{
	clearScene = true;
}

GameObject* SceneModule::CreateNewObject(GameObject* parent, std::string name)
{
	if (name.empty()) name = "GameObject ";
    GameObject* go = new GameObject(name + std::to_string(sceneGameObjects.size()), parent);
	sceneGameObjects.emplace_back(go);
	return go;
}

GameObject * SceneModule::DuplicateGameObject(GameObject & go)
{
	Data data;
	go.SaveData(data);

	GameObject* duplicated = CreateNewObject();
	std::string UID = duplicated->GetUID();
	duplicated->LoadData(data);
	duplicated->SetUID(UID);

	return duplicated;
}

void SceneModule::RemoveGameObject(GameObject & gameObject)
{
	GameObject* sceneRoot = currentScene->GetRootGameObject();
	sceneRoot->RemoveChild(&gameObject);

	for (int i = 0; i < sceneGameObjects.size(); i++)
	{
		if (sceneGameObjects[i] == &gameObject)
		{
			sceneGameObjects.erase(sceneGameObjects.begin() + i);
			break;
		}
	}
}

GameObject * SceneModule::FindGameObject(std::string UID)
{
	GameObject* ret = nullptr;

	for (GameObject* gameObject : sceneGameObjects)
	{
		if (gameObject->GetUID() == UID)
		{
			ret = gameObject;
		}
	}
	return ret;
}
