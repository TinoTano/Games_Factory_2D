#pragma once
#include "Resource.h"
#include <vector>

struct lua_State;
class GameObject;

struct ScriptField {
	enum PropertyType {
		INT, 
		DOUBLE, 
		FLOAT, 
		BOOL, 
		STRING,
		TEXTURE,
		GAMEOBJECT,
		ANIMATION,
		SCRIPT,
		VECTOR2, 
		VECTOR3, 
		VECTOR4,
		NONE
	};
	const char* fieldName;
	PropertyType propertyType = NONE;
};

class LuaScript :
	public Resource
{
public:
	LuaScript();
	LuaScript(std::string name, std::string assetsPath, std::string fullPath);
	virtual ~LuaScript();

	bool LoadFromFile(std::string path);
	bool LoadFromCode(std::string code, GameObject* gameObject);

public:
	lua_State* luaState;
	std::string scriptCode;
	bool hasStartFunction;
	bool hasUpdateFunction;
	bool hasOnCollisionEnter;
	bool hasOnCollisionStay;
	bool hasOnCollisionExit;
	bool hasOnSensorEnter;
	bool hasOnSensorStay;
	bool hasOnSensorExit;
	bool hasOnEnable;
	bool hasOnDisable;
	
	std::vector<ScriptField*> fields;
};

