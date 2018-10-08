#include "WindowModule.h"
#include <glfw3.h>
#include "CameraModule.h"
#include "Application.h"


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
    width = 1200.0f;
    height = 900.0f;
    
    glfwSetFramebufferSizeCallback(engineWindow, WindowModule::WindowResizeCallback);

	return ret;
}

bool WindowModule::CleanUp()
{
	bool ret = true;

	glfwDestroyWindow(engineWindow);
	glfwTerminate();

	return ret;
}

void WindowModule::WindowResizeCallback(GLFWwindow *window, int width, int height)
{
    App->windowModule->SetWidth((float)width);
    App->windowModule->SetHeight((float)height);
    App->cameraModule->UpdateCameraMatrix(width, height);
}

void WindowModule::SetWidth(float width)
{
    this->width = width;
}

float WindowModule::GetWidth() const
{
    return width;
}

void WindowModule::SetHeight(float height)
{
    this->height = height;
}

float WindowModule::GetHeight() const
{
    return height;
}
