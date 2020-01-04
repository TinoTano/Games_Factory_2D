#pragma once

#include "Component.h"
#include <array>
#include <vector>
#include "Vertex.h"

class Texture;
class GameObject;
class Animation;

class ComponentSprite :
	public Component
{
public:
	ComponentSprite(GameObject& gameObject, const char* componentName);
	~ComponentSprite();

	void UpdateVerticesPositions(float x, float y);
	void UpdateVerticesRotation(float angle);
	void UpdateVerticesScale(float x, float y);

	void SetPivot(float x, float y);
	glm::vec2 GetPivot() const;

	void SetTexture(Texture* texture);
	Texture* GetTexture();

	void SetVertices(std::array<Vertex, 4> vertices);
	std::array<Vertex, 4> GetVertices() const;

	void FlipX();
	void FlipY();

	bool IsFlippedX() const;
	bool IsFlippedY() const;

	void AddAnimation(Animation* animation);
	void RemoveAnimation(int animationIndex);
	std::vector<Animation*> GetAnimations() const;
	void PlayAnimation(std::string animationName);
	void StopAnimation();
	void PauseAnimation();

	void SaveData(Data& data);
	void LoadData(Data& data);

private:
	Texture* spriteTexture;
	std::array<Vertex, 4> vertices;
	glm::vec2 pivot;
	bool flippedX;
	bool flippedY;

	std::vector<Animation*> animations;
	Animation* playingAnimation;
};

