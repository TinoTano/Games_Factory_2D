#include "Texture.h"
#include "ThirdParty/gli/gli.hpp"
#include "Application.h"
#include "FileSystemModule.h"
#include "VulkanModule.h"

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::LoadFromFile(const char * filePath)
{
	bool ret = false;

	if (App->fileSystemModule->FileExist(filePath))
	{
		gli::texture2d texture(gli::load(filePath));
		width = texture.extent().x;
		height = texture.extent().y;
		mipmapLevels = texture.levels();
		App->vulkanModule->CreateTexture(*this, texture.data());
		texture.clear();
		ret = true;
	}

	return ret;
}

uint32_t Texture::GetWidth() const
{
	return width;
}

uint32_t Texture::GetHeight() const
{
	return height;
}

uint8_t * Texture::GetData() const
{
	return textureData;
}

uint32_t Texture::GetMipMapLevels() const
{
	return mipmapLevels;
}
