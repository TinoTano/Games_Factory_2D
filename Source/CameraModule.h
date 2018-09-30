#pragma once
#include "Module.h"
#include "ThirdParty/glm/vec2.hpp"
#include "ThirdParty/glm/gtc/matrix_transform.hpp"

class CameraModule :
	public Module
{
public:
	CameraModule(const char* moduleName, bool gameModule = false);
	~CameraModule();

	bool Init();
	bool Update(float delta_time);

	void SetPosition(glm::vec2 position);
	void IncreasePosition(glm::vec2 increase);
	glm::vec2 GetPosition() const;

	void SetRotation(float rotation);
	void IncreaseRotation(float increase);
	float GetRotation() const;

	void SetZoom(float zoom);
	void IncreaseZoom(float increase);
	float GetZoom() const;

	glm::mat4 GetOrthoProjection() const;

private:
	void CalculeOrthoProjection();

private:
	glm::vec2 cameraPosition;
	float cameraRotation;
	float cameraZoom;
	glm::mat4 orthoProjection;
	bool updateCamera;
};

