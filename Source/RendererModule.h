#pragma once
#include "Module.h"
#include <vector>

class ComponentSprite;
struct Vertex;

class RendererModule :
	public Module
{
public:
	RendererModule(const char* module_name, bool game_module = true);
	~RendererModule();

	bool Init();
	bool PreUpdate(float delta_time);
	bool Update(float delta_time);
	bool PostUpdate(float delta_time);
	bool CleanUp();

	void AddSpriteToRender(ComponentSprite& sprite);
	void RemoveSpriteToRender(ComponentSprite& sprite);

	std::vector<ComponentSprite*> GetSpritesToRender() const;

	void AddDebugVertex(Vertex& vertex);
	void CleanDebugVertex();

	std::vector<Vertex> GetDebugVertexToRender() const;

public:
	std::vector<Vertex> debugVertices;

private:
	std::vector<ComponentSprite*> spritesToRender;
	
};

