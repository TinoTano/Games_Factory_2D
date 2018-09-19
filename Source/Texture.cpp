#include "Texture.h"

Texture::Texture(uint32_t width, uint32_t height, uint8_t* texture_data, std::string name, std::string assets_path, std::string library_path, ResourceType type = RESOURCE_TEXTURE) :
	width(width), height(height), texture_data(texture_data), Resource(name, assets_path, library_path, type)
{
}

Texture::~Texture()
{
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
	return texture_data;
}
