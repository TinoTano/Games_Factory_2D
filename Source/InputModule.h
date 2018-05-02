#pragma once
#include "Module.h"

class InputModule :
	public Module
{
public:
	InputModule(const char* module_name, bool game_module = false);
	~InputModule();

	bool PreUpdate(float delta_time);
};

