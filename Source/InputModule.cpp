#include "InputModule.h"
#include "Application.h"
#include "WindowModule.h"
#include "CameraModule.h"
#include "EditorModule.h"
#include "AssetsWindow.h"

#define GLFW_INCLUDE_NONE
#include <glfw3.h>

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
	glfwSetWindowFocusCallback(App->windowModule->engineWindow, InputModule::WindowFocusCallback);

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
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_D:
			App->cameraModule->IncreasePosition({ 10.01f, 0 });
			break;
		case GLFW_KEY_S:
			App->cameraModule->IncreasePosition({ 0, -10.01f });
			break;
		case GLFW_KEY_A:
			App->cameraModule->IncreasePosition({ -10.01f, 0 });
			break;
		case GLFW_KEY_W:
			App->cameraModule->IncreasePosition({ 0, 10.01f });
			break;
		case GLFW_KEY_E:
			App->cameraModule->IncreaseZoom(0.90f);
			break;
		case GLFW_KEY_R:
			App->cameraModule->IncreaseZoom(1.10f);
			break;
		}
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

void InputModule::WindowFocusCallback(GLFWwindow * window, int focused)
{
	if (focused == 1)
	{
		if (App->IsEditor())
		{
			App->editorModule->assetsWindow->CheckDirectories();
		}
	}
}

