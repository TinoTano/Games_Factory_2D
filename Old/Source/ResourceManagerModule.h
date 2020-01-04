#pragma once
#include "Module.h"
#include <vector>

class Resource;
class Texture;
class Animation;
class LuaScript;
class Scene;
class Data;

class ResourceManagerModule :
	public Module
{
public:
	ResourceManagerModule(const char* module_name, bool game_module = false);
	~ResourceManagerModule();

	bool Init(Data& settings);
	bool CleanUp();

	Resource* CreateResource(std::string path);

	std::vector<Texture*> GetTextures() const;
	Texture* GetTexture(std::string assetPath) const;

	std::vector<Animation*> GetAnimations() const;
	Animation* GetAnimation(std::string assetPath) const;

	std::vector<LuaScript*> GetScripts() const;
	LuaScript* GetScript(std::string assetPath) const;

	std::vector<Scene*> GetScenes() const;
	Scene* GetScene(std::string assetPath) const;

private:
	void SaveResourceToLibrary(Resource& res);
	int GetResourceType(std::string extension);

	Texture* CreateTexture(std::string name, std::string assetsPath, std::string fullPath);
	Animation* CreateAnimation(std::string name, std::string assetsPath, std::string fullPath);
	LuaScript* CreateScript(std::string name, std::string assetsPath, std::string fullPath);

private:
	std::vector<Texture*> textures;
	std::vector<Animation*> animations;
	std::vector<LuaScript*> scripts;
	std::vector<Scene*> scenes;
};

