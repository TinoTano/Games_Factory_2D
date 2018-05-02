#pragma once
#include "Module.h"

struct GLFWwindow;

class WindowModule :
	public Module
{
public:
	WindowModule(const char* module_name, bool game_module = false);
	~WindowModule();

	bool Init();
	bool CleanUp();

public:
	GLFWwindow* engine_window;
};

