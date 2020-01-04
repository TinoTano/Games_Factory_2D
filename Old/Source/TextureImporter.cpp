#include "TextureImporter.h"
#include "ThirdParty/gli/gli.hpp"
#include "Texture.h"


TextureImporter::TextureImporter()
{
}

TextureImporter::~TextureImporter()
{
}

std::shared_ptr<Texture> TextureImporter::LoadImage(const char * path)
{
	std::shared_ptr<Texture> texture;
	gli::texture2d gliTexture(gli::load(path));
	//std::shared_ptr<Texture> texture = std::make_shared<Texture>(Texture(gliTexture.extent().x, gliTexture.extent().y,);
	return texture;
}
