#include "ResourceManagerModule.h"
#include "Resource.h"
#include "Application.h"
#include "FileSystemModule.h"
#include "Texture.h"
#include "Animation.h"
#include "LuaScript.h"

ResourceManagerModule::ResourceManagerModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
}

ResourceManagerModule::~ResourceManagerModule()
{
	
}

bool ResourceManagerModule::Init()
{
	std::vector<std::string> resourcesPaths;

	if (App->IsEditor())
	{
		resourcesPaths = App->fileSystemModule->GetAssetsFilesPaths();
	}
	else
	{
		resourcesPaths = App->fileSystemModule->GetLibraryFilesPaths();
	}

	for (std::string resourcePath : resourcesPaths)
	{
		CreateResource(resourcePath);
	}
	return true;
}

bool ResourceManagerModule::CleanUp()
{
	for (Texture* texture : textures)
	{
		delete texture;
		texture = nullptr;
	}
	textures.clear();

	for (Animation* animation : animations)
	{
		delete animation;
		animation = nullptr;
	}
	animations.clear();

	return true;
}

std::vector<Texture*> ResourceManagerModule::GetTextures() const
{
	return textures;
}

Texture * ResourceManagerModule::GetTexture(std::string assetPath) const
{
	for (Texture* texture : textures)
	{
		if (texture->GetAssetsPath() == assetPath) return texture;
	}
	return nullptr;
}

std::vector<Animation*> ResourceManagerModule::GetAnimations() const
{
	return animations;
}

Animation * ResourceManagerModule::GetAnimation(std::string assetPath) const
{
	for (Animation* animation : animations)
	{
		if (animation->GetAssetsPath() == assetPath) return animation;
	}
	return nullptr;
}

std::vector<LuaScript*> ResourceManagerModule::GetScripts() const
{
	return scripts;
}

LuaScript * ResourceManagerModule::GetScript(std::string assetPath) const
{
	for (LuaScript* script : scripts)
	{
		if (script->GetAssetsPath() == assetPath) return script;
	}
	return nullptr;
}

Resource * ResourceManagerModule::CreateResource(std::string path)
{
	Resource::ResourceType type = (Resource::ResourceType)GetResourceType(App->fileSystemModule->GetExtension(path));
	std::string name = App->fileSystemModule->GetName(path);
	std::string assetsPath = path.substr(path.find("Assets"));
	Resource* resource = nullptr;

	switch (type)
	{
	case Resource::RESOURCE_TEXTURE:
		resource = (Resource*)CreateTexture(name, assetsPath, path);
		break;
	case Resource::RESOURCE_ANIM:
		resource = (Resource*)CreateAnimation(name, assetsPath, path);
		break;
	case Resource::RESOURCE_LUASCRIPT:
		resource = (Resource*)CreateScript(name, assetsPath, path);
		break;
	default:
		break;
	}

	return resource;
}

void ResourceManagerModule::SaveResourceToLibrary(Resource & res)
{
	App->fileSystemModule->SaveFileTo(res.GetLibraryPath().c_str());
}

int ResourceManagerModule::GetResourceType(std::string extension)
{
	int ret = 0;

	if (extension == ".jpg" || extension == ".png")
	{
		ret = Resource::RESOURCE_TEXTURE;
	}
	else if (extension == ".animation")
	{
		ret = Resource::RESOURCE_ANIM;
	}
	else if (extension == ".lua")
	{
		ret = Resource::RESOURCE_LUASCRIPT;
	}
	else
	{
		ret = Resource::RESOURCE_NONE;
	}

	return ret;
}

Texture * ResourceManagerModule::CreateTexture(std::string name, std::string assetsPath, std::string fullPath)
{
	Texture* texture = GetTexture(assetsPath);

	if (texture == nullptr)
	{
		texture = new Texture(name, assetsPath, fullPath);
		if (texture->LoadFromFile(fullPath))
		{
			textures.emplace_back(texture);
		}
		else
		{
			delete texture;
			texture = nullptr;
		}
	}
	
	return texture;
}

Animation * ResourceManagerModule::CreateAnimation(std::string name, std::string assetsPath, std::string fullPath)
{
	Animation* animation = GetAnimation(assetsPath);

	if (animation == nullptr)
	{
		animation = new Animation(name, assetsPath, fullPath);
		if (animation->LoadFromFile(fullPath))
		{
			animations.emplace_back(animation);
		}
		else
		{
			delete animation;
			animation = nullptr;
		}
	}

	return animation;
}

LuaScript * ResourceManagerModule::CreateScript(std::string name, std::string assetsPath, std::string fullPath)
{
	LuaScript* script = GetScript(assetsPath);

	if (script == nullptr)
	{
		script = new LuaScript(name, assetsPath, fullPath);
		if (script->LoadFromFile(fullPath))
		{
			scripts.emplace_back(script);
		}
		else
		{
			delete script;
			script = nullptr;
		}
	}

	return script;
}
