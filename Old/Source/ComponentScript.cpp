#include "ComponentScript.h"
#include "LuaScript.h"
#include "Data.h"
#include "Application.h"
#include "ResourceManagerModule.h"

ComponentScript::ComponentScript(GameObject& gameObject, const char* componentName) :
	Component(gameObject, componentName, COMPONENT_TYPE::SCRIPT)
{
	script = nullptr;
}

ComponentScript::~ComponentScript()
{
	delete script;
	script = nullptr;
}

void ComponentScript::SetScript(LuaScript& script)
{
	this->script = new LuaScript();
	if (this->script->LoadFromCode(script.scriptCode, gameObject))
	{
		//this->script->SetContainerObject(*gameObject);
	}
	else
	{
		delete this->script;
		this->script = nullptr;
	}
}

LuaScript * ComponentScript::GetScript() const
{
	return script;
}

void ComponentScript::SaveData(Data & data)
{
	data.AddInt("Type", GetComponentType());
	data.AddString("ScriptPath", (script) ? script->GetAssetsPath() : "");
}

void ComponentScript::LoadData(Data & data)
{
	SetScript(*App->resourceManagerModule->GetScript(data.GetString("ScriptPath")));
}
