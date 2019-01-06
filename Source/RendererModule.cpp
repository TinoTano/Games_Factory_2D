#include "RendererModule.h"
#include "Globals.h"
#include "WindowModule.h"
#include "VulkanModule.h"
#include "Application.h"
#include "ComponentSprite.h"
#include "SceneModule.h"

RendererModule::RendererModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{

}

RendererModule::~RendererModule()
{
}

bool RendererModule::Init()
{


	return true;
}

bool RendererModule::PreUpdate(float delta_time)
{
	return true;
}

bool RendererModule::Update(float delta_time)
{
	return true;
}

bool RendererModule::PostUpdate(float delta_time)
{
	bool ret = false;

	ret = App->vulkanModule->Render();

	return ret;
}

bool RendererModule::CleanUp()
{
	return true;
}

void RendererModule::AddSpriteToRender(ComponentSprite & sprite)
{
	spritesToRender.emplace_back(&sprite);
	App->sceneModule->updateSceneVertices = true;
}

void RendererModule::RemoveSpriteToRender(ComponentSprite & sprite)
{
	std::vector<ComponentSprite*>::iterator it = std::find(spritesToRender.begin(), spritesToRender.end(), &sprite);
	if (it != spritesToRender.end())
	{
		spritesToRender.erase(it);
	}
}

std::vector<ComponentSprite*> RendererModule::GetSpritesToRender() const
{
	return spritesToRender;
}

void RendererModule::AddDebugVertex(Vertex & vertex)
{
	debugVertices.emplace_back(vertex);
}

void RendererModule::CleanDebugVertex()
{
	debugVertices.clear();
}

std::vector<Vertex> RendererModule::GetDebugVertexToRender() const
{
	return debugVertices;
}
