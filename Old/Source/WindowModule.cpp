#include "WindowModule.h"
#include <glfw3.h>
#include "CameraModule.h"
#include "Application.h"
#include "FileSystemModule.h"
#include "Data.h"
#include "Texture.h"

WindowModule::WindowModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
	engineWindowWidth = 0;
	engineWindowHeight = 0;
	gameWindowWidth = 0;
	gameWindowHeight = 0;

	isFullscreen = false;
	isResizable = true;
	isBorderless = false;
	isAlwaysOnTop = false;
}

WindowModule::~WindowModule()
{
}

bool WindowModule::Init(Data& settings)
{
	bool ret = true;

	ret = glfwInit();
	
	Data sectionData;
	if (settings.GetSectionData("WindowData", sectionData))
	{
		engineWindowWidth = settings.GetInt("EngineWindowWidth");
		engineWindowHeight = settings.GetInt("EngineWindowHeight");
		gameWindowWidth = settings.GetInt("GameWindowWidth");
		gameWindowHeight = settings.GetInt("GameWindowHeigth");

		isFullscreen = settings.GetBool("Fullscreen");
		isResizable = settings.GetBool("Resizable");
		isBorderless = settings.GetBool("Borderless");
		isAlwaysOnTop = settings.GetBool("AlwaysOnTop");
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, isResizable);
	glfwWindowHint(GLFW_FLOATING, isAlwaysOnTop);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	if (!isFullscreen) monitor = nullptr;
	if (engineWindowWidth <= 0 || engineWindowHeight <= 0)
	{
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		engineWindowWidth = mode->width;
		engineWindowHeight = mode->height;
	}

	window = glfwCreateWindow(engineWindowWidth, engineWindowHeight, App->appTitle, monitor, nullptr);

	glfwSetFramebufferSizeCallback(window, WindowModule::WindowResizeCallback);

	GLFWimage image;
	if (App->icon != nullptr)
	{
		image.pixels = App->icon->pixels;
		glfwSetWindowIcon(window, 1, &image);
	}

	return ret;
}

bool WindowModule::CleanUp()
{
	bool ret = true;

	glfwDestroyWindow(window);
	glfwTerminate();

	return ret;
}

void WindowModule::WindowResizeCallback(GLFWwindow *window, int width, int height)
{
	if (App->IsEditor())
	{
		App->windowModule->SetEngineWindowWidth(width);
		App->windowModule->SetEngineWindowHeight(height);
	}
	else
	{
		App->windowModule->SetGameWindowWidth(width);
		App->windowModule->SetGameWindowHeight(height);
	}
	
	App->cameraModule->UpdateCameraMatrix(width, height);
}

void WindowModule::SetEngineWindowWidth(int width)
{
	engineWindowWidth = width;
}

int WindowModule::GetEngineWindowWidth() const
{
	return engineWindowWidth;
}

void WindowModule::SetEngineWindowHeight(int height)
{
	engineWindowHeight = height;
}

int WindowModule::GetEngineWindowHeight() const
{
	return engineWindowHeight;
}

void WindowModule::SetGameWindowWidth(int width)
{
	gameWindowWidth = width;

	glfwSetWindowSize(window, width, gameWindowHeight);
}

int WindowModule::GetGameWindowWidth() const
{
	return gameWindowWidth;
}

void WindowModule::SetGameWindowHeight(int height)
{
	gameWindowHeight = height;

	glfwSetWindowSize(window, gameWindowWidth, height);
}

void WindowModule::SetGameWindowSize(int width, int height)
{
	glfwSetWindowSize(window, width, height);
}

int WindowModule::GetGameWindowHeight() const
{
	return gameWindowHeight;
}

void WindowModule::SetFullScreen(bool fullscreen)
{
	isFullscreen = fullscreen;
}

bool WindowModule::GetIsFullscreen() const
{
	return isFullscreen;
}

void WindowModule::SetResizable(bool resizable)
{
	isResizable = resizable;
}

bool WindowModule::GetIsResizable() const
{
	return isResizable;
}

void WindowModule::SetBorderless(bool borderless)
{
	isBorderless = borderless;
}

bool WindowModule::GetIsBorderless() const
{
	return isBorderless;
}

void WindowModule::SetAlwaysOnTop(bool onTop)
{
	isAlwaysOnTop = onTop;
}

bool WindowModule::GetIsAlwaysOnTop() const
{
	return isAlwaysOnTop;
}
