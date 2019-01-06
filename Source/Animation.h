#pragma once
#include "Resource.h"
#include <vector>

class Texture;

class Animation :
	public Resource
{
public:
	Animation(std::string name, std::string assetsPath, std::string fullPath);
	virtual ~Animation();

	void AddTexture(Texture& texture);
	void RemoveTexture(Texture& texture);

	void SetSpeed(float speed);
	float GetSpeed() const;

	void SetLoop(bool loop);
	bool GetIsLoop() const;

	void StopAnimation();

	Texture* GetCurrentTexture() const;
	Texture* GetNextTexture();

	void UpdateTextures(std::vector<Texture*> textures);
	std::vector<Texture*> GetTextures() const;

	bool LoadFromFile(std::string path);

private:
	std::vector<Texture*> animationTextures;
	float speed;
	bool loop;
	float currentAnimTextureIndex;
};

