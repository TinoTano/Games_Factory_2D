#include "WindowModule.h"

#include "ThirdParty/glfw-3.2.1/include/glfw3.h"


WindowModule::WindowModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
}

WindowModule::~WindowModule()
{
}

bool WindowModule::Init()
{
	bool ret = true;

	ret = glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	engineWindow = glfwCreateWindow(1200, 900, "Games Factory 2D", nullptr, nullptr);

	return ret;
}

bool WindowModule::CleanUp()
{
	bool ret = true;

	glfwDestroyWindow(engineWindow);
	glfwTerminate();

	return ret;
}
