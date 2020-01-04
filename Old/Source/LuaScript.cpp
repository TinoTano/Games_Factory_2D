#include "LuaScript.h"
#include "FileSystemModule.h"
#include "Application.h"
#include "LuaScripting.h"
#include "Log.h"

LuaScript::LuaScript()
{
}

LuaScript::LuaScript(std::string name, std::string assetsPath, std::string fullPath) :
	Resource(name, assetsPath, fullPath, Resource::RESOURCE_LUASCRIPT)
{
}

LuaScript::~LuaScript()
{
	App->luaScripting->CloseLua(luaState);
	scriptCode.clear();
	fields.clear();
}

bool LuaScript::LoadFromFile(std::string path)
{
	bool ret = false;

	scriptCode = App->fileSystemModule->LoadBinaryTextFile(path.c_str());

	ret = LoadFromCode(scriptCode, nullptr);

	return ret;
}

bool LuaScript::LoadFromCode(std::string code, GameObject* gameObject)
{
	bool ret = false;

	scriptCode = code;

	if (!scriptCode.empty())
	{
		ret = App->luaScripting->InitScript(*this, scriptCode.c_str(), *gameObject);
		if (ret)
		{
			App->luaScripting->GetFields(*this);
		}
	}
	else
	{
		CONSOLE_ERROR("Script %s is empty", GetName().c_str());
	}

	if (!ret)
	{
		CONSOLE_ERROR("Script %s code is invalid", GetName().c_str());
		scriptCode.clear();
	}

	return ret;
}
