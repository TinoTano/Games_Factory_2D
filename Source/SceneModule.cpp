#include "SceneModule.h"
#include "GameObject.h"
#include "Application.h"
#include "CameraModule.h"
#include "Vertex.h"
#include "ComponentSprite.h"
#include "Globals.h"

SceneModule::SceneModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{

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
	glm::mat4 cameraProjection = App->cameraModule->GetOrthoProjection();

	for (GameObject* go : sceneGameObjects)
	{
		std::array<Vertex, 4> vertices;
		ComponentSprite* sprite = go->GetSprite();
		vertices = sprite->GetVertices();
		glm::mat4 model = glm::mat4(1);
		glm::vec2 camPos = App->cameraModule->GetPosition();
		model = glm::translate(model, glm::vec3(camPos, 0.0f));
		for (int i = 0; i < 4; i++)
		{
			Vertex v = vertices[i];
			glm::vec4 pos;
			pos = v.currentPos * cameraProjection * glm::inverse(model);
			/*v.pos.x = pos.x;
			v.pos.y = pos.y;*/
			v.pos = pos;
			vertices[i] = v;
		}

		for (int i = 0; i < 4; i++)
		{
			Vertex v = vertices[i];
			//CONSOLE_LOG("Vertex %d: %.3f, %.3f", i, v.pos.x, v.pos.y);
		}
		sprite->SetVertices(vertices);
	}
	return true;
}
