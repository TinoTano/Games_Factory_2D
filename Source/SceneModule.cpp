#include "SceneModule.h"
#include "GameObject.h"
#include "Application.h"
#include "CameraModule.h"
#include "Globals.h"
#include "ComponentTransform.h"

SceneModule::SceneModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
    updateSceneVertices = false;
	rootGameObject = nullptr;
	spritesNum = 0;
}

SceneModule::~SceneModule()
{
}

bool SceneModule::Init()
{
	rootGameObject = new GameObject("Root GameObject");
    return true;
}

bool SceneModule::Update(float delta_time)
{

    return true;
}

GameObject* SceneModule::CreateNewObject(GameObject* parent, std::string name)
{
	if (name.empty()) name = "New GameObject ";
	if (parent == nullptr) parent = rootGameObject;
    GameObject* go = new GameObject(name + std::to_string(rootGameObject->GetChilds().size() + 1), parent);

	return go;
}
