#include "SceneModule.h"
#include "GameObject.h"
#include "Application.h"
#include "CameraModule.h"
#include "Vertex.h"
#include "ComponentSprite.h"
#include "Globals.h"
#include "ComponentTransform.h"
#include <gtc/type_ptr.hpp>

SceneModule::SceneModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
    updateSceneVertices = false;
}

SceneModule::~SceneModule()
{
}

bool SceneModule::Init()
{
    GameObject* go = new GameObject();
    sceneGameObjects.emplace_back(go);
    return true;
}

bool SceneModule::Update(float delta_time)
{
    if(App->GetFPS() > 59)
    {
        CreateNewObject(nullptr);
    }
    return true;
}

void SceneModule::CreateNewObject(GameObject* parent)
{
    GameObject* go = new GameObject();
    go->SetParent(*parent);
    sceneGameObjects.emplace_back(go);
    updateSceneVertices = true;
}
