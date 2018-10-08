#pragma once
#include "Resource.h"
#include <vec2.hpp>
#include <vec3.hpp>
#include <vulkan/vulkan.h>
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec2 uvs;
	glm::vec3 normal;
};

class Texture :
	public Resource
{
public:
	Texture();
	Texture(uint32_t width, uint32_t height, uint8_t* texture_data, std::string name, std::string assets_path, std::string library_path, ResourceType type = RESOURCE_TEXTURE);
	~Texture();

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint8_t* GetData() const;

private:
	uint32_t width;
	uint32_t height;
	uint8_t* texture_data;
	uint32_t mipmap_levels;

	VkImage texture_image;
	VkDeviceMemory texture_image_memory;
	VkImageView texture_image_view;
	VkSampler texture_sampler;

	std::vector<Vertex> texture_vertices;
	std::vector<uint32_t> indices;
};

