#include "TextureImporter.h"
#include "ThirdParty/stb/stb_image.h"
#include "Texture.h"


TextureImporter::TextureImporter()
{
}

TextureImporter::~TextureImporter()
{
}

std::shared_ptr<Texture> TextureImporter::LoadImage(const char * path)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();

	stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	return texture;
}
