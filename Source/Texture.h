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
	//uint8_t* GetData() const;
	int GetMipMapLevels() const;

public:
	uint64_t textureImage;
	uint64_t textureImageMemory;
	uint64_t textureImageView;
	uint64_t textureSampler;
	int textureIndex;

private:
	int width;
	int height;
	//uint8_t* textureData;
	int mipmapLevels;
};

