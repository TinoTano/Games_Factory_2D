#pragma once

#include <memory>

class Texture;

class TextureImporter
{
public:
	TextureImporter();
	~TextureImporter();

	std::shared_ptr<Texture> LoadImage(const char* path);
};

