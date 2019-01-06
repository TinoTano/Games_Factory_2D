#include "Animation.h"
#include "Texture.h"
#include "Data.h"
#include "Application.h"
#include "FileSystemModule.h"
#include "ResourceManagerModule.h"

Animation::Animation(std::string name, std::string assetsPath, std::string fullPath) : 
	Resource(name, assetsPath, fullPath, Resource::RESOURCE_ANIM)
{
	speed = 1.0f;
	loop = false;
	currentAnimTextureIndex = 0;
}

Animation::~Animation()
{
	animationTextures.clear();
}

void Animation::AddTexture(Texture & texture)
{
	animationTextures.emplace_back(&texture);
}

void Animation::RemoveTexture(Texture & texture)
{
	for (int i = 0; i < animationTextures.size(); i++)
	{
		if (animationTextures[i] == &texture)
		{
			animationTextures.erase(animationTextures.begin() + i);
			break;
		}
	}
}

void Animation::SetSpeed(float speed)
{
	this->speed = speed;
}

float Animation::GetSpeed() const
{
	return speed;
}

void Animation::SetLoop(bool loop)
{
	this->loop = loop;
}

bool Animation::GetIsLoop() const
{
	return loop;
}

void Animation::StopAnimation()
{
	currentAnimTextureIndex = 0;
}

Texture * Animation::GetCurrentTexture() const
{
	return animationTextures[(int)currentAnimTextureIndex];
}

Texture * Animation::GetNextTexture()
{
	currentAnimTextureIndex += speed * App->GetDeltaTime();
	if (currentAnimTextureIndex >= animationTextures.size())
	{
		currentAnimTextureIndex = (loop) ? 0.0f : animationTextures.size() - 1;
	}

	return animationTextures[(int)currentAnimTextureIndex];
}

void Animation::UpdateTextures(std::vector<Texture*> textures)
{
	animationTextures.clear();
	animationTextures = textures;
}

std::vector<Texture*> Animation::GetTextures() const
{
	return animationTextures;
}

bool Animation::LoadFromFile(std::string path)
{
	bool ret = false;

	if (App->fileSystemModule->FileExist(path))
	{
		Data data;
		if (data.LoadData(path))
		{
			speed = data.GetFloat("Speed");
			loop = data.GetBool("Loop");
			int imagesCount = data.GetInt("ImagesCount");
			for (int i = 0; i < imagesCount; i++)
			{
				std::string assetPath = data.GetString("ImagePath" + std::to_string(i));
				std::string fullPath = App->fileSystemModule->GetWorkingPath() + "/Data/" + assetPath;
				Texture* texture = (Texture*)App->resourceManagerModule->CreateResource(fullPath);
				if(texture != nullptr)
				{
					animationTextures.emplace_back(texture);
				}
			}
			ret = true;
		}
	}

	return ret;
}
