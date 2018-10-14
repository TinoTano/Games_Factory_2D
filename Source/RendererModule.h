#pragma once
#include "Module.h"

class RendererModule :
	public Module
{
public:
	RendererModule(const char* module_name, bool game_module = false);
	~RendererModule();

	bool Init();
	bool PreUpdate(float delta_time);
	bool Update(float delta_time);
	bool PostUpdate(float delta_time);
	bool CleanUp();

};

