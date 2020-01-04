#include "LuaScripting.h"
#include <lua.hpp>
#include "Log.h"
#include "LuaScript.h"
#include <future>
#include "Application.h"
#include "SceneModule.h"
#include "GameObject.h"
#include "ComponentScript.h"
#include "Resource.h"

LuaScripting::LuaScripting()
{
	
}

LuaScripting::~LuaScripting()
{

}

bool LuaScripting::InitScript(LuaScript& script, const char* code, GameObject& gameObject)
{
	CloseLua(script.luaState);
	
	bool ret = false;
	script.luaState = luaL_newstate();
	luaL_openlibs(script.luaState);
	GlobalFunctions(script.luaState);
	RegisterAPI(script.luaState);

	if (luaL_loadstring(script.luaState, code) == 0) {
		SetScriptContainerObject(script.luaState, gameObject, script);
		ret = LuaFunctions::CallFunction(script.luaState, NULL, 0, 0);
		if (ret) {
			script.hasStartFunction = LuaFunctions::FunctionExist(script.luaState, "Start");
			script.hasUpdateFunction = LuaFunctions::FunctionExist(script.luaState, "Update");
			script.hasOnCollisionEnter = LuaFunctions::FunctionExist(script.luaState, "OnCollisionEnter");
			script.hasOnCollisionStay = LuaFunctions::FunctionExist(script.luaState, "OnCollisionStay");
			script.hasOnCollisionExit = LuaFunctions::FunctionExist(script.luaState, "OnCollisionExit");
			script.hasOnSensorEnter = LuaFunctions::FunctionExist(script.luaState, "OnSensorEnter");
			script.hasOnSensorStay = LuaFunctions::FunctionExist(script.luaState, "OnSensorStay");
			script.hasOnSensorExit = LuaFunctions::FunctionExist(script.luaState, "OnSensorExit");
			script.hasOnEnable = LuaFunctions::FunctionExist(script.luaState, "OnEnable");
			script.hasOnDisable = LuaFunctions::FunctionExist(script.luaState, "OnDisable");
		}
	}
	else 
	{
		CONSOLE_ERROR("Cannot load lua script '%s': %s", script.GetName().c_str(), lua_tostring(script.luaState, -1));
	}
	return ret;
}

void LuaScripting::CloseLua(lua_State * luaState)
{
	if (luaState != nullptr)
	{
		lua_close(luaState);
		luaState = nullptr;
	}
}

void LuaScripting::SetInt(LuaScript & script, const char * valueName, int value)
{
	SetFloat(script, valueName, (float)value);
}

int LuaScripting::GetInt(LuaScript & script, const char * valueName)
{
	return (int)GetFloat(script, valueName);
}

void LuaScripting::SetDouble(LuaScript & script, const char * valueName, double value)
{
	SetFloat(script, valueName, (float)value);
}

double LuaScripting::GetDouble(LuaScript & script, const char * valueName)
{
	return (double)GetFloat(script, valueName);
}

void LuaScripting::SetFloat(LuaScript & script, const char * valueName, float value)
{
	bool exist = false;
	lua_getglobal(script.luaState, valueName);

	if (lua_isnumber(script.luaState, lua_gettop(script.luaState))) {
		exist = true;
	}
	lua_pop(script.luaState, 1);

	if (exist) {
		lua_pushnumber(script.luaState, value);
		lua_setglobal(script.luaState, valueName);
	}
	else {
		CONSOLE_WARNING("Global value '%s' is not a number.", valueName);
	}
}

float LuaScripting::GetFloat(LuaScript & script, const char * valueName)
{
	float ret = 0;

	lua_getglobal(script.luaState, valueName);

	if (lua_isnumber(script.luaState, lua_gettop(script.luaState)))
	{
		ret = (float)lua_tonumber(script.luaState, -1); // -1 = top of stack
	}
	else
	{
		CONSOLE_WARNING("Global value '%s' is not a number.", valueName);
	}

	lua_pop(script.luaState, 1);

	return ret;
}

void LuaScripting::SetBool(LuaScript & script, const char * valueName, bool value)
{
	bool exists = false;

	lua_getglobal(script.luaState, valueName);
	if (lua_isboolean(script.luaState, lua_gettop(script.luaState)))
	{
		exists = true;
	}

	// Pop after getglobal, since no other lua function is popping the stack
	lua_pop(script.luaState, 1);

	if (exists)
	{
		lua_pushboolean(script.luaState, (int)value);
		lua_setglobal(script.luaState, valueName); // setglobal pops the stack for us
	}
	else
	{
		CONSOLE_WARNING("Global value '%s' is not a boolean.", valueName);
	}
}

bool LuaScripting::GetBool(LuaScript & script, const char * valueName)
{
	bool ret = false;
	lua_getglobal(script.luaState, valueName);

	if (lua_isboolean(script.luaState, lua_gettop(script.luaState)))
	{
		ret = lua_toboolean(script.luaState, -1); // -1 = top of stack
	}
	else
	{
		CONSOLE_WARNING("Global value '%s' is not a boolean.", valueName);
	}

	lua_pop(script.luaState, 1);

	return ret;
}

void LuaScripting::SetString(LuaScript & script, const char * valueName, const char * value)
{
	bool exists = false;

	lua_getglobal(script.luaState, valueName);
	if (lua_isstring(script.luaState, lua_gettop(script.luaState)))
	{
		exists = true;
	}

	lua_pop(script.luaState, 1);

	if (exists)
	{
		lua_pushstring(script.luaState, value);
		lua_setglobal(script.luaState, valueName);
	}
	else {
		CONSOLE_WARNING("Global value '%s' is not a String.", valueName);
	}
}

const char* LuaScripting::GetString(LuaScript & script, const char * valueName)
{
	const char* ret = "";
	lua_getglobal(script.luaState, valueName);

	if (lua_isstring(script.luaState, lua_gettop(script.luaState)))
	{
		ret = lua_tostring(script.luaState, -1);
	}
	else
	{
		CONSOLE_WARNING("Global value '%s' is not a String.", valueName);
	}

	lua_pop(script.luaState, 1);

	return ret;
}

void LuaScripting::SetVec2(LuaScript & script, const char * valueName, glm::vec2 value)
{
	bool exist = false;
	lua_getglobal(script.luaState, valueName);
	if (lua_istable(script.luaState, lua_gettop(script.luaState))) {
		exist = true;
	}
	lua_pop(script.luaState, 1);

	if (exist) {
		lua_newtable(script.luaState);

		lua_pushnumber(script.luaState, value.x);
		lua_rawseti(script.luaState, -2, 1);

		lua_pushnumber(script.luaState, value.y);
		lua_rawseti(script.luaState, -2, 2);

		lua_setglobal(script.luaState, valueName);
	}
	else {
		CONSOLE_WARNING("Global value '%s' is nil.", valueName);
	}
}

glm::vec2 LuaScripting::GetVec2(LuaScript & script, const char * valueName)
{
	glm::vec2 v;

	lua_getglobal(script.luaState, valueName);

	if (lua_istable(script.luaState, lua_gettop(script.luaState))) {
		lua_pushinteger(script.luaState, 1);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.x = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'x' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);

		lua_pushinteger(script.luaState, 2);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.y = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'y' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);
	}

	lua_pop(script.luaState, 1);

	return v;
}

void LuaScripting::SetVec3(LuaScript & script, const char * valueName, glm::vec3 value)
{
	bool exist = false;
	lua_getglobal(script.luaState, valueName);
	if (lua_istable(script.luaState, lua_gettop(script.luaState))) {
		exist = true;
	}
	lua_pop(script.luaState, 1);

	if (exist) {
		lua_newtable(script.luaState);

		lua_pushnumber(script.luaState, value.x);
		lua_rawseti(script.luaState, -2, 1);

		lua_pushnumber(script.luaState, value.y);
		lua_rawseti(script.luaState, -2, 2);

		lua_pushnumber(script.luaState, value.z);
		lua_rawseti(script.luaState, -2, 3);

		lua_setglobal(script.luaState, valueName);
	}
	else {
		CONSOLE_WARNING("Global value '%s' is nil.", valueName);
	}
}

glm::vec3 LuaScripting::GetVec3(LuaScript & script, const char * valueName)
{
	glm::vec3 v;

	lua_getglobal(script.luaState, valueName);

	if (lua_istable(script.luaState, lua_gettop(script.luaState))) {
		lua_pushinteger(script.luaState, 1);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.x = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'x' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);

		lua_pushinteger(script.luaState, 2);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.y = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'y' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);

		lua_pushinteger(script.luaState, 3);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.z = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'z' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);
	}

	lua_pop(script.luaState, 1);

	return v;
}

void LuaScripting::SetVec4(LuaScript & script, const char * valueName, glm::vec4 value)
{
	bool exist = false;
	lua_getglobal(script.luaState, valueName);
	if (lua_istable(script.luaState, lua_gettop(script.luaState))) {
		exist = true;
	}
	lua_pop(script.luaState, 1);

	if (exist) {
		lua_newtable(script.luaState);

		lua_pushnumber(script.luaState, value.x);
		lua_rawseti(script.luaState, -2, 1);

		lua_pushnumber(script.luaState, value.y);
		lua_rawseti(script.luaState, -2, 2);

		lua_pushnumber(script.luaState, value.z);
		lua_rawseti(script.luaState, -2, 3);

		lua_pushnumber(script.luaState, value.w);
		lua_rawseti(script.luaState, -2, 4);

		lua_setglobal(script.luaState, valueName);
	}
	else {
		CONSOLE_WARNING("Global value '%s' is nil.", valueName);
	}
}

glm::vec4 LuaScripting::GetVec4(LuaScript & script, const char * valueName)
{
	glm::vec4 v;

	lua_getglobal(script.luaState, valueName);

	if (lua_istable(script.luaState, lua_gettop(script.luaState))) {
		lua_pushinteger(script.luaState, 1);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.x = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'x' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);

		lua_pushinteger(script.luaState, 2);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.y = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'y' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);

		lua_pushinteger(script.luaState, 3);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.z = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'z' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);

		lua_pushinteger(script.luaState, 4);
		lua_gettable(script.luaState, -2);
		if (lua_isnumber(script.luaState, -1)) {
			v.w = (float)lua_tonumber(script.luaState, -1);
		}
		else {
			CONSOLE_WARNING("Value 'w' of %s is not a number", valueName);
		}
		lua_pop(script.luaState, 1);
	}

	lua_pop(script.luaState, 1);

	return v;
}

void LuaScripting::SetUserData(LuaScript & script, const char * valueName, Resource * value)
{
	if (lua_istable(script.luaState, lua_gettop(script.luaState)))
	{
		lua_getglobal(script.luaState, valueName);

		lua_newtable(script.luaState);
		lua_pushstring(script.luaState, "Object");
		lua_pushlightuserdata(script.luaState, value);
		lua_settable(script.luaState, -3);
		lua_setglobal(script.luaState, valueName); // setglobal pops the stack for us
	}
	else
	{
		CONSOLE_WARNING("Global value '%s' is nil.", valueName);
	}
}

Resource * LuaScripting::GetUserData(LuaScript & script, const char * valueName)
{
	Resource* ret = nullptr;

	lua_getglobal(script.luaState, valueName);

	if (lua_istable(script.luaState, lua_gettop(script.luaState)))
	{
		lua_pushstring(script.luaState, "Object");
		lua_gettable(script.luaState, -2);
		if (lua_isuserdata(script.luaState, -1)) {
			ret = (Resource*)lua_touserdata(script.luaState, -1);
			lua_pop(script.luaState, 1);
		}
	}
	else
	{
		CONSOLE_WARNING("Global value '%s' is nil.", valueName);
	}

	lua_pop(script.luaState, 1);

	return ret;
}

void LuaScripting::GlobalFunctions(lua_State * luaState)
{
	const luaL_Reg globalFunctions[] =
	{
		{ "print", PrintToLog },
		{ "Invoke", Invoke },
		{ "Instantiate", Instantiate },
		{ "GameObject", AddGameObjectField },
		{ "Texture", AddTextureField },
		{ "Animation", AddAnimationField },
		{ "Script", AddScriptField },
		{ NULL, NULL }
	};

	lua_getglobal(luaState, "_G");
	luaL_setfuncs(luaState, globalFunctions, 0);
	lua_pop(luaState, 1);
}

void LuaScripting::RegisterAPI(lua_State * luaState)
{
	luaL_Reg GameObject[] =
	{
		{ NULL, NULL }
	};
	LuaFunctions::RegisterLibrary(luaState, GameObject, "GameObject");

	luaL_Reg Math[] =
	{
		{"Vector2", Vector2 },
		{"Vector3", Vector3 },
		{"Vector4", Vector4 },
		{NULL, NULL}
	};
	LuaFunctions::RegisterLibrary(luaState, Math, "Math");
}

int LuaScripting::PrintToLog(lua_State * luaState)
{
	int argCount = lua_gettop(luaState);
	std::string message;

	for (int argIndex = 1; argIndex <= argCount; ++argIndex)
	{
		if (lua_isstring(luaState, argIndex))
		{
			message += lua_tostring(luaState, argIndex);
		}
		else if (lua_isboolean(luaState, argIndex)) {
			bool boolArg = lua_toboolean(luaState, argIndex);
			if (boolArg) message += "true";
			else message += "false";
		}
		else if (lua_isnumber(luaState, argIndex)) {
			message += std::to_string(lua_tonumber(luaState, argIndex));
		}
		else if (lua_istable(luaState, argIndex)) {
			lua_pushstring(luaState, "Object");
			lua_gettable(luaState, -2);
			if (lua_isstring(luaState, -1)) {
				message += lua_tostring(luaState, -1);
			}
		}
		else {
			CONSOLE_ERROR("Don't recognise print function argument %d type", argIndex);
			return 0;
		}
		message += ", ";
	}
	CONSOLE_LOG("%s", message.c_str());
	return 0;
}

int LuaScripting::Invoke(lua_State * luaState)
{
	int arguments = lua_gettop(luaState);
	if (arguments != 2) {
		CONSOLE_WARNING("Invoke(string, float) takes 2 arguments!");
	}
	else {
		if (lua_isstring(luaState, 1)) {
			const char* function = lua_tostring(luaState, 1);
			if (LuaFunctions::FunctionExist(luaState, function)) {
				if (lua_isnumber(luaState, 2)) {
					float time = lua_tonumber(luaState, 2);
					if (time = 0) {
						LuaFunctions::CallFunction(luaState, function, 0, 0);
					}
					else {
						//call after time
						auto fut = std::async(std::launch::async, [&]() { LuaFunctions::CallFunction(luaState, function, 0, 0); });
						fut.wait_for(std::chrono::duration<float>(time));
					}
				}
				else {
					CONSOLE_WARNING("Invoke() float argument is not a number!");
				}
			}
			else {
				CONSOLE_WARNING("Function %s doesn't exist in the script!", function);
			}
		}
		else {
			CONSOLE_WARNING("Invoke() string argument is not a string!");
		}
	}
	return 0;
}

int LuaScripting::Instantiate(lua_State * luaState)
{
	int arguments = lua_gettop(luaState);
	if (arguments != 1) {
		CONSOLE_WARNING("Instantiate(GameObject) takes 1 argument!");
		return 0;
	}
	else {
		GameObject* go = nullptr;
		GameObject* duplicatedGO = nullptr;
		if (lua_istable(luaState, 1)) {
			lua_pushstring(luaState, "Object");
			lua_gettable(luaState, -2);
			Resource* res = (Resource*)lua_touserdata(luaState, -1);
			if (res->GetType() == Resource::RESOURCE_GAMEOBJECT)
			{
				lua_pop(luaState, 1);
				go = (GameObject*)res;
				if (go == nullptr) {
					CONSOLE_WARNING("Instantiate() GameObject argument is nil!");
					return 0;
				}
				else {
					duplicatedGO = App->sceneModule->DuplicateGameObject(*go);
					if (duplicatedGO == nullptr) {
						CONSOLE_WARNING("Failed to Instantiate %s!", go->GetName().c_str());
						return 0;
					}
					else {
						lua_newtable(luaState);
						lua_pushstring(luaState, "Object");
						lua_pushlightuserdata(luaState, duplicatedGO);
						lua_settable(luaState, -3);
						lua_pop(luaState, 1);
					}
				}
			}
			else {
				CONSOLE_WARNING("Instantiate() GameObject argument is not a GameObject!");
				return 0;
			}
		}
	}
	return 1;
}

int LuaScripting::AddGameObjectField(lua_State * luaState)
{
	return AddField(luaState, ScriptField::GAMEOBJECT);
}

int LuaScripting::AddTextureField(lua_State * luaState)
{
	return AddField(luaState, ScriptField::TEXTURE);
}

int LuaScripting::AddAnimationField(lua_State * luaState)
{
	return AddField(luaState, ScriptField::ANIMATION);
}

int LuaScripting::AddScriptField(lua_State * luaState)
{
	return AddField(luaState, ScriptField::SCRIPT);
}

int LuaScripting::AddField(lua_State * luaState, int type)
{
	const char* fieldName = "";

	int arguments = lua_gettop(luaState);
	if (arguments != 1) {
		CONSOLE_WARNING("AddField(string) takes 1 argument!");
	}
	else 
	{
		if (lua_isstring(luaState, 1)) {
			fieldName = lua_tostring(luaState, 1);
		}
		else
		{
			CONSOLE_WARNING("AddField() argument is not a string!");
		}
	}

	lua_getglobal(luaState, "this");
	if (lua_istable(luaState, lua_gettop(luaState)))
	{
		lua_pushstring(luaState, "Script");
		lua_gettable(luaState, -2);
		LuaScript& script = *(LuaScript*)lua_touserdata(luaState, -1);
		if (&script != nullptr)
		{
			ScriptField* field = new ScriptField();
			field->fieldName = fieldName;
			field->propertyType = (ScriptField::PropertyType)type;
			script.fields.emplace_back(field);

			lua_newtable(script.luaState);
			lua_pushstring(script.luaState, "Object");
			lua_pushlightuserdata(script.luaState, nullptr);
			lua_settable(script.luaState, -3);
			lua_setglobal(script.luaState, fieldName); // setglobal pops the stack for us
		}
	}

	lua_pop(luaState, 1);

	return 0;
}

int LuaScripting::Vector4(lua_State * luaState)
{
	int arguments = lua_gettop(luaState);
	if (arguments != 4 && arguments != 0) {
		CONSOLE_WARNING("Vector4 takes 4 numbers or can be empty");
		return 0;
	}
	else {
		if (arguments == 4) {
			for (int i = 1; i <= arguments; i++) {
				if (!lua_isnumber(luaState, i)) {
					CONSOLE_WARNING("Argument %d in Vector4 is not a number!", i);
					return 0;
				}
			}
			lua_newtable(luaState);
			lua_pushnumber(luaState, lua_tonumber(luaState, 1));
			lua_rawseti(luaState, -2, 1);
			lua_pushnumber(luaState, lua_tonumber(luaState, 2));
			lua_rawseti(luaState, -2, 2);
			lua_pushnumber(luaState, lua_tonumber(luaState, 3));
			lua_rawseti(luaState, -2, 3);
			lua_pushnumber(luaState, lua_tonumber(luaState, 4));
			lua_rawseti(luaState, -2, 4);
		}
		else {
			lua_newtable(luaState);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 1);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 2);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 3);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 4);
		}
	}
	return 1;
}

int LuaScripting::Vector3(lua_State * luaState)
{
	int arguments = lua_gettop(luaState);
	if (arguments != 3 && arguments != 0) {
		CONSOLE_WARNING("Vector3 takes 3 numbers or can be empty");
		return 0;
	}
	else {
		if (arguments == 3) {
			for (int i = 1; i <= arguments; i++) {
				if (!lua_isnumber(luaState, i)) {
					CONSOLE_WARNING("Argument %d in Vector3 is not a number!", i);
					return 0;
				}
			}
			lua_newtable(luaState);
			lua_pushnumber(luaState, lua_tonumber(luaState, 1));
			lua_rawseti(luaState, -2, 1);
			lua_pushnumber(luaState, lua_tonumber(luaState, 2));
			lua_rawseti(luaState, -2, 2);
			lua_pushnumber(luaState, lua_tonumber(luaState, 3));
			lua_rawseti(luaState, -2, 3);
		}
		else {
			lua_newtable(luaState);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 1);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 2);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 3);
		}
	}
	return 1;
}

int LuaScripting::Vector2(lua_State * luaState)
{
	int arguments = lua_gettop(luaState);
	if (arguments != 2 && arguments != 0) {
		CONSOLE_WARNING("Vector2 takes 2 numbers or can be empty");
		return 0;
	}
	else {
		if (arguments == 2) {
			for (int i = 1; i <= arguments; i++) {
				if (!lua_isnumber(luaState, i)) {
					CONSOLE_WARNING("Argument %d in Vector2 is not a number!", i);
					return 0;
				}
			}
			lua_newtable(luaState);
			lua_pushnumber(luaState, lua_tonumber(luaState, 1));
			lua_rawseti(luaState, -2, 1);
			lua_pushnumber(luaState, lua_tonumber(luaState, 2));
			lua_rawseti(luaState, -2, 2);
		}
		else {
			lua_newtable(luaState);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 1);
			lua_pushnumber(luaState, 0);
			lua_rawseti(luaState, -2, 2);
		}
	}
	return 1;
}

void LuaFunctions::RegisterLibrary(lua_State * luaState, luaL_Reg lib[], const char * libName)
{
	lua_getglobal(luaState, libName);
	if (lua_isnil(luaState, -1))
	{
		lua_pop(luaState, 1);
		lua_newtable(luaState);
	}
	luaL_setfuncs(luaState, lib, 0);
	lua_setglobal(luaState, libName);
}

bool LuaFunctions::CallFunction(lua_State * luaState, const char * functionName, int numArgs, int numResults)
{
	bool ret = true;

	if (functionName != NULL) {
		lua_getglobal(luaState, functionName);
	}

	if (lua_pcall(luaState, numArgs, numResults, 0) != 0) {
		CONSOLE_WARNING("Cannot call Lua function %s: %s", functionName, lua_tostring(luaState, -1));
		ret = false;
	}

	return ret;
}

bool LuaFunctions::CallUserDataFunction(lua_State * luaState, const char * functionName, int numArgs, int numResults, Resource * userData)
{
	bool ret = true;

	if (functionName != NULL) {
		lua_getglobal(luaState, functionName);
		
		lua_newtable(luaState);
		lua_pushstring(luaState, "Object");
		lua_pushlightuserdata(luaState, userData);
		lua_settable(luaState, -3);
		//lua_pop(luaState, 1);
	}

	if (lua_pcall(luaState, numArgs, numResults, 0) != 0) {
		CONSOLE_WARNING("Cannot call Lua function %s: %s", functionName, lua_tostring(luaState, -1));
		ret = false;
	}

	return ret;
}

bool LuaFunctions::FunctionExist(lua_State * luaState, const char * functionName)
{
	bool ret = true;

	lua_getglobal(luaState, functionName);
	if (!lua_isfunction(luaState, lua_gettop(luaState))) {
		ret = false;
	}
	lua_pop(luaState, 1);

	return ret;
}

void LuaScripting::SetScriptContainerObject(lua_State * luaState, GameObject & object, LuaScript& script)
{
	lua_newtable(luaState);
	lua_pushstring(luaState, "Object");
	lua_pushlightuserdata(luaState, &object); 
	lua_pushstring(luaState, "Script");
	lua_pushlightuserdata(luaState, &script);
	lua_settable(luaState, -5);
	lua_settable(luaState, -3);
	lua_setglobal(luaState, "this");
}

void LuaScripting::GetFields(LuaScript& script)
{
	//script.fields.clear();

	lua_pushglobaltable(script.luaState);
	lua_pushnil(script.luaState);

	while (lua_next(script.luaState, -2) != 0) {
		int keyType = lua_type(script.luaState, -2);
		if (keyType != LUA_TSTRING) {
			lua_pop(script.luaState, 1);
			continue;
		}

		int valueType = lua_type(script.luaState, -1);
		const char* keyName = lua_tostring(script.luaState, -2);
		if (keyName == "" || keyName[0] == '_' || keyName == "this") {
			lua_pop(script.luaState, 1);
			continue;
		}

		ScriptField* propertyField = new ScriptField();
		propertyField->fieldName = keyName;

		switch (valueType) {
		case LUA_TNUMBER:
		{
			double n = lua_tonumber(script.luaState, -1);
			if (n == (int)n) {
				propertyField->propertyType = propertyField->INT;
			}
			else {
				propertyField->propertyType = propertyField->FLOAT;
			}
		}
		break;
		case LUA_TSTRING:
			propertyField->propertyType = propertyField->STRING;
			break;
		case LUA_TBOOLEAN:
			propertyField->propertyType = propertyField->BOOL;
			break;
		//case LUA_TLIGHTUSERDATA:
		//	propertyField->propertyType = propertyField->USER_DATA;
		//	break;
		case LUA_TTABLE:
		{
			if (lua_rawlen(script.luaState, -1) == 3) {
				propertyField->propertyType = propertyField->VECTOR3;
			}
			else if (lua_rawlen(script.luaState, -1) == 2) {
				propertyField->propertyType = propertyField->VECTOR2;
			}
			else if (lua_rawlen(script.luaState, -1) == 4) {
				propertyField->propertyType = propertyField->VECTOR4;
			}
		}
		break;
		default:
			propertyField->propertyType = propertyField->NONE;
			break;
		}
		if (propertyField->propertyType != propertyField->NONE) {
			script.fields.emplace_back(propertyField);
		}

		lua_pop(script.luaState, 1);
	}
}

void LuaScripting::StackDump(lua_State * luaState)
{
	int i;
	int top = lua_gettop(luaState);

	CONSOLE_LOG("total in stack %d\n", top);

	for (i = 1; i <= top; i++)
	{  /* repeat for each level */
		int t = lua_type(luaState, i);
		switch (t) {
		case LUA_TSTRING:  /* strings */
			CONSOLE_LOG("string: '%s'\n", lua_tostring(luaState, i));
			break;
		case LUA_TBOOLEAN:  /* booleans */
			CONSOLE_LOG("boolean %s\n", lua_toboolean(luaState, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:  /* numbers */
			CONSOLE_LOG("number: %g\n", lua_tonumber(luaState, i));
			break;
		case LUA_TTABLE:
			CONSOLE_LOG("table\n");
			//PrintTable(luaState, i);
			break;
		case LUA_TUSERDATA:
			CONSOLE_LOG("user data\n");
			break;
		default:  /* other values */
			CONSOLE_LOG("%s\n", lua_typename(luaState, t));
			break;
		}
	}
	CONSOLE_LOG("\n");  /* end the listing */
}

void LuaScripting::PrintTable(lua_State * luaState, int index)
{
	/*if ((lua_type(luaState, -2) == LUA_TSTRING))
		CONSOLE_LOG("%s", lua_tostring(luaState, -2));

	lua_pushnil(luaState);*/
	while (lua_next(luaState, index - 1) != 0) {
		if (lua_isstring(luaState, -1))
		{
			CONSOLE_LOG("%s = %s\n", lua_tostring(luaState, index), lua_tostring(luaState, -1));
		}
		else if (lua_isnumber(luaState, -1))
		{
			CONSOLE_LOG("%s = %d\n", lua_tostring(luaState, index), lua_tonumber(luaState, -1));
		}
		else if (lua_istable(luaState, -1)) 
		{
			//PrintTable(luaState);
		}
		lua_pop(luaState, 1);
	}
}
