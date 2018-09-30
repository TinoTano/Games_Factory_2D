#include "InputModule.h"
#include "Application.h"
#include "WindowModule.h"
#include "CameraModule.h"
#include "Globals.h"

#define GLFW_INCLUDE_NONE
#include "ThirdParty/glfw-3.2.1/include/glfw3.h"

InputModule::InputModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
	
}

InputModule::~InputModule()
{
	
}

bool InputModule::Start()
{
	glfwSetKeyCallback(App->windowModule->engineWindow, InputModule::KeyCallback);
	glfwSetScrollCallback(App->windowModule->engineWindow, InputModule::ScrollCallback);

	return true;
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

void InputModule::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		App->cameraModule->IncreasePosition({ 1.01f, 1.01f });
		//App->cameraModule->IncreaseRotation(0.1f);
	}
}

void InputModule::ScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
	if (yoffset == -1)
	{
		App->cameraModule->IncreaseZoom(-0.01f);
	}
	else
	{
		App->cameraModule->IncreaseZoom(0.01f);
	}
}
