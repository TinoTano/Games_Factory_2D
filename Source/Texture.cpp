#include "Texture.h"
#include "ThirdParty/gli/gli.hpp"
#include "Application.h"
#include "FileSystemModule.h"

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
