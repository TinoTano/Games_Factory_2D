#include "InputModule.h"
#include "ThirdParty/glfw-3.2.1/include/glfw3.h"
#include "Application.h"
#include "WindowModule.h"


InputModule::InputModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
}

InputModule::~InputModule()
{

}

bool InputModule::PreUpdate(float delta_time)
{
	bool ret = true;

	if (glfwWindowShouldClose(App->windowModule->engineWindow))
	{
		App->QuitEngine();
	}
	else
	{
		glfwPollEvents();
	}

	return ret;
}
