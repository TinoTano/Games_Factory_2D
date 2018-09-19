#pragma once
#include "Module.h"
#include <map>

class Resource;

class ResourceManagerModule :
	public Module
{
public:
	ResourceManagerModule(const char* module_name, bool game_module = false);
	~ResourceManagerModule();

private:

	void SaveResourceToLibrary(Resource* res);
};

