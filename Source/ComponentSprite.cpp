#include "ComponentSprite.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/rotate_vector.hpp>
#include "Application.h"
#include "RendererModule.h"
#include "ResourceManagerModule.h"
#include "Texture.h"
#include "Data.h"
#include "SceneModule.h"
#include "Animation.h"

ComponentSprite::ComponentSprite(GameObject& gameObject, const char* componentName) :
	Component(gameObject, componentName, COMPONENT_TYPE::SPRITE)
{
    vertices[0].pos = { -1, -1 };
	vertices[1].pos = {  1, -1 };
	vertices[2].pos = {  1,  1 };
	vertices[3].pos = { -1,  1 };

	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1.0f, 1.0f, 1.0f, 1.0f };

	vertices[0].texCoord = { 0.0f, 1.0f };
	vertices[1].texCoord = { 1.0f, 1.0f };
	vertices[2].texCoord = { 1.0f, 0.0f };
	vertices[3].texCoord = { 0.0f, 0.0f };

	SetPivot((vertices[0].pos.x + vertices[1].pos.x + vertices[2].pos.x + vertices[3].pos.x) / 3,
		(vertices[0].pos.y + vertices[1].pos.y + vertices[2].pos.y + vertices[3].pos.y) / 3);

	flippedX = false;
	flippedY = false;

	playingAnimation = nullptr;
}

ComponentSprite::~ComponentSprite()
{
	if (spriteTexture != nullptr)
	{
		App->rendererModule->RemoveSpriteToRender(*this);
	}
	spriteTexture = nullptr;

	animations.clear();
	playingAnimation = nullptr;
}

void ComponentSprite::SetTexture(Texture* texture)
{
	if (spriteTexture == texture) return;

	if (texture != nullptr)
	{
		if (spriteTexture == nullptr)
		{
			App->rendererModule->AddSpriteToRender(*this);
		}

		int width = texture->GetWidth();
		int height = texture->GetHeight();

		if (spriteTexture == nullptr || (spriteTexture->GetWidth() != width || spriteTexture->GetHeight() != height))
		{
			vertices[0].pos = { -(width * 0.5f), -(height * 0.5f) };
			vertices[1].pos = { (width * 0.5f), -(height * 0.5f) };
			vertices[2].pos = { (width * 0.5f),  (height * 0.5f) };
			vertices[3].pos = { -(width * 0.5f),  (height * 0.5f) };

			SetPivot((vertices[0].pos.x + vertices[1].pos.x + vertices[2].pos.x + vertices[3].pos.x) / 3,
				(vertices[0].pos.y + vertices[1].pos.y + vertices[2].pos.y + vertices[3].pos.y) / 3);

			App->sceneModule->updateSceneVertices = true;
		}
	}
	else
	{
		if (spriteTexture != nullptr)
		{
			App->rendererModule->RemoveSpriteToRender(*this);
		}
	}
	
	spriteTexture = texture;
}

Texture* ComponentSprite::GetTexture()
{
	if (playingAnimation != nullptr)
	{
		if (App->IsPlaying())
		{
			SetTexture(animations[0]->GetNextTexture());
		}
	}
	
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

void ComponentSprite::FlipX()
{
	std::swap(vertices[0].texCoord.x, vertices[1].texCoord.x);
	std::swap(vertices[2].texCoord.x, vertices[3].texCoord.x);

	flippedX = !flippedX;
	App->sceneModule->updateSceneVertices = true;
}

void ComponentSprite::FlipY()
{
	std::swap(vertices[0].texCoord.y, vertices[2].texCoord.y);
	std::swap(vertices[1].texCoord.y, vertices[3].texCoord.y);

	flippedY = !flippedY;
	App->sceneModule->updateSceneVertices = true;
}

bool ComponentSprite::IsFlippedX() const
{
	return flippedX;
}

bool ComponentSprite::IsFlippedY() const
{
	return flippedY;
}

void ComponentSprite::AddAnimation(Animation * animation)
{
	if (animation != nullptr)
	{
		if (std::find(animations.begin(), animations.end(), animation) == animations.end())
		{
			animations.emplace_back(animation);
		}
	}
}

void ComponentSprite::RemoveAnimation(int animationIndex)
{
	animations.erase(animations.begin() + animationIndex);
}

std::vector<Animation*> ComponentSprite::GetAnimations() const
{
	return animations;
}

void ComponentSprite::PlayAnimation(std::string animationName)
{
	for (Animation* animation : animations)
	{
		if (animation->GetName() == animationName)
		{
			playingAnimation = animation;
			break;
		}
	}
}

void ComponentSprite::StopAnimation()
{
	if (playingAnimation != nullptr)
	{
		playingAnimation->StopAnimation();
		playingAnimation = nullptr;
	}
}

void ComponentSprite::PauseAnimation()
{
	playingAnimation = nullptr;
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

void ComponentSprite::SaveData(Data & data)
{
	data.AddInt("Type", GetComponentType());
	//data.AddString("TextureUID", (spriteTexture) ? spriteTexture->GetUID() : "");
	data.AddString("TexturePath", (spriteTexture) ? spriteTexture->GetAssetsPath() : "");
	data.AddInt("AnimCount", animations.size());
	data.CreateSection("Animations");
	for (int i = 0; i < animations.size(); i++)
	{
		data.AddString("Anim" + std::to_string(i), animations[i]->GetAssetsPath());
	}
	data.CloseSection("Animations");
}

void ComponentSprite::LoadData(Data & data)
{
	SetTexture(App->resourceManagerModule->GetTexture(data.GetString("TexturePath")));
	int animsCount = data.GetInt("AnimCount");
	Data animData;
	if (data.GetSectionData("Animations", animData))
	{
		for (int i = 0; i < animsCount; i++)
		{
			AddAnimation(App->resourceManagerModule->GetAnimation(data.GetString("Anim" + std::to_string(i))));
		}
	}
}