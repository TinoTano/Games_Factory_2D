#include "ComponentSprite.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/rotate_vector.hpp>
#include "Globals.h"
#include <gtc/random.hpp>
#include <time.h>

ComponentSprite::ComponentSprite(GameObject& gameObject, const char* componentName, COMPONENT_TYPE type) :
	Component(gameObject, componentName, type)
{
	srand(time(NULL));
	glm::vec2 lt(glm::linearRand(-600, 0), glm::linearRand(-450, 0));
	glm::vec2 rt(lt.x + 10, lt.y);
	glm::vec2 lb(rt.x, lt.y + 10);
	glm::vec2 rb(lt.x, lt.y + 10);

	/*vertices[0].pos = { -20.50f, -20.50f };
	vertices[1].pos = { 20.50f, -20.50f };
	vertices[2].pos = { 20.50f, 20.50f };
	vertices[3].pos = { -20.50f, 20.50f };*/

	vertices[0].pos = lt;
	vertices[1].pos = rt;
	vertices[2].pos = lb;
	vertices[3].pos = rb;

	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1.0f, 1.0f, 1.0f, 1.0f };

	SetPivot((vertices[0].pos.x + vertices[1].pos.x + vertices[2].pos.x + vertices[3].pos.x) / 3,
		(vertices[0].pos.y + vertices[1].pos.y + vertices[2].pos.y + vertices[3].pos.y) / 3);
}

ComponentSprite::~ComponentSprite()
{
}

void ComponentSprite::SetTexture(Texture& texture)
{
	spriteTexture = &texture;
}

Texture* ComponentSprite::GetTexture() const
{
	return spriteTexture;
}

void ComponentSprite::SetVertices(std::array<Vertex, 4> vertices)
{
	for (int i = 0; i < 4; i++)
	{
		this->vertices[i] = vertices[i];
	}
}

std::array<Vertex, 4> ComponentSprite::GetVertices() const
{
	return vertices;
}

void ComponentSprite::UpdateVerticesPositions(float x, float y)
{
	for (int i = 0; i < 4; i++)
	{
		vertices[i].pos.x += x;
		vertices[i].pos.y += y;
	}
}

void ComponentSprite::UpdateVerticesRotation(float angle)
{
	for (int i = 0; i < 4; i++)
	{
		vertices[i].pos -= pivot;
		glm::vec2 newRotation = glm::rotate(vertices[i].pos, angle);
		vertices[i].pos = newRotation;
		vertices[i].pos += pivot;
	}
}

void ComponentSprite::UpdateVerticesScale(float x, float y)
{
	for (int i = 0; i < 4; i++)
	{
		vertices[i].pos.x *= x;
		vertices[i].pos.y *= y;
	}
}

void ComponentSprite::SetPivot(float x, float y)
{
	pivot.x = x;
	pivot.y = y;
}

glm::vec2 ComponentSprite::GetPivot() const
{
	return pivot;
}
