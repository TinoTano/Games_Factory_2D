#pragma once

#include "Component.h"
#include "Vertex.h"

class Texture;

class ComponentSprite :
	public Component
{
public:
	ComponentSprite(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type);
	~ComponentSprite();

	void UpdateVerticesPositions(float x, float y);
	void UpdateVerticesRotation(float angle);
	void UpdateVerticesScale(float x, float y);

	void SetPivot(float x, float y);
	glm::vec2 GetPivot() const;

	void SetTexture(Texture& texture);
	Texture* GetTexture() const;

	void SetVertices(std::array<Vertex, 4> vertices);
	std::array<Vertex, 4> GetVertices() const;

private:
	Texture* spriteTexture;
	std::array<Vertex, 4> vertices;
	glm::vec2 pivot;
};

