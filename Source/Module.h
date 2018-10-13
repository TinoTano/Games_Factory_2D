#pragma once

#include <string>

class Module
{
public:
	Module(const char* moduleName, bool gameModule = false) : moduleName(moduleName), gameModule(gameModule) {}
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

	const char* GetName() const
	{
		return moduleName;
	}

private:
	bool gameModule;
	const char* moduleName;
};
