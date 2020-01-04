#pragma once
#include "Module.h"

struct GLFWwindow;

class InputModule :
	public Module
{
public:
	InputModule(const char* moduleName, bool gameModule = false);
	~InputModule();

	bool Start();
	bool PreUpdate(float deltaTime);

public:
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void MouseCallback(GLFWwindow* window, int button, int action, int mods);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void WindowFocusCallback(GLFWwindow* window, int focused);

	int GetMouseButton(int button);

private:
	int mouseButtons[5];
};

