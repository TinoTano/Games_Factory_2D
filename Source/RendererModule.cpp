#include "RendererModule.h"
#include "Globals.h"
#include "WindowModule.h"
#include "VulkanModule.h"


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
