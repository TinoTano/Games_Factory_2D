#pragma once

#include <string>

class Module
{
public:
	Module(const char* module_name, bool game_module = false) : module_name(module_name), game_module(game_module) {}
	virtual ~Module() {}

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool PreUpdate(float deltaTime)
	{
		return true;
	}

	virtual bool Update(float deltaTime)
	{
		return true;
	}

	virtual bool PostUpdate(float deltaTime)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

private:
	bool game_module;
	const char* module_name;
};