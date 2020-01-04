#pragma once

#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>

struct lua_State;
struct luaL_Reg;
class GameObject;
class LuaScript;
class Resource;

struct LuaFunctions {
	static void RegisterLibrary(lua_State* luaState, luaL_Reg lib[], const char* libName);
	static bool CallFunction(lua_State* luaState, const char* functionName, int numArgs, int numResults);
	static bool CallUserDataFunction(lua_State* luaState, const char* functionName, int numArgs, int numResults, Resource* userData);
	static bool FunctionExist(lua_State* luaState, const char* functionName);
};

class LuaScripting
{
public:
	LuaScripting();
	~LuaScripting();

	bool InitScript(LuaScript& script, const char* code, GameObject& gameObject);
	void GetFields(LuaScript& script);
	void CloseLua(lua_State * luaState);

	void SetInt(LuaScript& script, const char* valueName, int value);
	int GetInt(LuaScript& script, const char* valueName);
	void SetDouble(LuaScript& script, const char* valueName, double value);
	double GetDouble(LuaScript& script, const char* valueName);
	void SetFloat(LuaScript& script, const char* valueName, float value);
	float GetFloat(LuaScript& script, const char* valueName);
	void SetBool(LuaScript& script, const char* valueName, bool value);
	bool GetBool(LuaScript& script, const char* valueName);
	void SetString(LuaScript& script, const char* valueName, const char* value);
	const char* GetString(LuaScript& script, const char* valueName);
	void SetVec2(LuaScript& script, const char* valueName, glm::vec2 value);
	glm::vec2 GetVec2(LuaScript& script, const char* valueName);
	void SetVec3(LuaScript& script, const char* valueName, glm::vec3 value);
	glm::vec3 GetVec3(LuaScript& script, const char* valueName);
	void SetVec4(LuaScript& script, const char* valueName, glm::vec4 value);
	glm::vec4 GetVec4(LuaScript& script, const char* valueName);
	
	void SetUserData(LuaScript& script, const char* valueName, Resource* value);
	Resource* GetUserData(LuaScript& script, const char* valueName);

private:
	void SetScriptContainerObject(lua_State* luaState, GameObject& object, LuaScript& script);

	void GlobalFunctions(lua_State* luaState);
	void RegisterAPI(lua_State* luaState);

	static int PrintToLog(lua_State* luaState);
	static int Invoke(lua_State* luaState);
	static int Instantiate(lua_State* luaState);

	static int AddGameObjectField(lua_State* luaState);
	static int AddTextureField(lua_State* luaState);
	static int AddAnimationField(lua_State* luaState);
	static int AddScriptField(lua_State* luaState);
	static int AddField(lua_State* luaState, int type);


	//Math library
	static int Vector4(lua_State* luaState);
	static int Vector3(lua_State* luaState);
	static int Vector2(lua_State* luaState);

	//Debug Lua Stack
	static void StackDump(lua_State* luaState);
	static void PrintTable(lua_State* luaState, int index);
};

