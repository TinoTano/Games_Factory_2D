#include "Texture.h"
#include "Application.h"
#include "FileSystemModule.h"
#include "VulkanModule.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(std::string name, std::string assetsPath, std::string fullPath) : 
	Resource(name, assetsPath, fullPath, Resource::RESOURCE_TEXTURE)
{
	width = 0;
	height = 0;
	mipmapLevels = 0;
	textureIndex = 0;
	textureImage = 0;
	textureImageMemory = 0;
	textureImageView = 0;
	textureSampler = 0;
}

Texture::~Texture()
{
}

bool Texture::LoadFromFile(std::string filePath)
{
	bool ret = false;

	if (App->fileSystemModule->FileExist(filePath))
	{
		stbi_uc* pixels = stbi_load(filePath.c_str(), &width, &height, &mipmapLevels, STBI_rgb_alpha);
		if (pixels != nullptr)
		{
			App->vulkanModule->CreateTexture(*this, pixels);
			ret = true;
		}
	}

	return ret;
}

int Texture::GetWidth() const
{
	return width;
}

int Texture::GetHeight() const
{
	return height;
}

int Texture::GetMipMapLevels() const
{
	return mipmapLevels;
}
