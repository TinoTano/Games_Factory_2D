#pragma once
#include "Resource.h"

class Texture :
	public Resource
{
public:
	Texture(std::string name, std::string assetsPath, std::string fullPath);
	~Texture();

	bool LoadFromFile(std::string filePath);

	int GetWidth() const;
	int GetHeight() const;
	int GetMipMapLevels() const;

public:
	uint64_t textureImage;
	uint64_t textureImageMemory;
	uint64_t textureImageView;
	uint64_t textureSampler;
	int textureImageLayout;
	int textureIndex;
	unsigned char* pixels;

private:
	int width;
	int height;
	int mipmapLevels;
};

