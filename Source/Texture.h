#pragma once
#include "Resource.h"

class Texture :
	public Resource
{
public:
	Texture();
	~Texture();

	bool LoadFromFile(const char* filePath);

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	//uint8_t* GetData() const;
	uint32_t GetMipMapLevels() const;

public:
	uint64_t textureImage;
	uint64_t textureImageMemory;
	uint64_t textureImageView;
	uint64_t textureSampler;

private:
	uint32_t width;
	uint32_t height;
	//uint8_t* textureData;
	uint32_t mipmapLevels;
};

