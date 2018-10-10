#pragma once
#include "Resource.h"
#include <vec2.hpp>
#include <vec3.hpp>
#include <vulkan/vulkan.h>
#include <vector>

class Texture :
	public Resource
{
public:
	Texture();
	~Texture();

	bool LoadFromFile(const char* filePath);

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint8_t* GetData() const;

private:
	uint32_t width;
	uint32_t height;
	uint8_t* textureData;
	uint32_t mipmapLevels;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};

