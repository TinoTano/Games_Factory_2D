#pragma once
#include "Component.h"

class LuaScript;
class Data;

class ComponentScript :
	public Component
{
public:
	ComponentScript(GameObject& gameObject, const char* componentName);
	~ComponentScript();

	void SetScript(LuaScript& script);
	LuaScript* GetScript() const;

	void SaveData(Data& data);
	void LoadData(Data& data);

private:
	LuaScript* script;
};

