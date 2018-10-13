#include "SceneModule.h"
#include "GameObject.h"
#include "Application.h"
#include "CameraModule.h"
#include "Globals.h"
#include "ComponentTransform.h"

SceneModule::SceneModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
    updateSceneVertices = false;
}

SceneModule::~SceneModule()
{
}

bool SceneModule::Init()
{

    return true;
}

bool SceneModule::Update(float delta_time)
{

    return true;
}

void SceneModule::CreateNewObject(GameObject* parent)
{
    GameObject* go = new GameObject();
    go->SetParent(*parent);
    sceneGameObjects.emplace_back(go);
    updateSceneVertices = true;
}
