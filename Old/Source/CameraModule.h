#pragma once
#include "Module.h"
#include <vec2.hpp>
#include <gtc/matrix_transform.hpp>

class Data;

class CameraModule :
	public Module
{
public:
	CameraModule(const char* moduleName, bool gameModule = false);
	~CameraModule();

	bool Init(Data& settings);
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

	void SetOrthoprojection(float width, float height);
	glm::mat4 GetOrthoProjection() const;
	glm::mat4 GetViewMatrix() const;
	glm::vec2 GetCameraCenter() const;

	void UpdateCameraMatrix(float width, float height);

private:
	void UpdateViewMatrix();

private:
	glm::vec2 cameraPosition;
	float cameraRotation;
	float cameraZoom;
	glm::mat4 projMatrix;
	glm::mat4 viewMatrix;
	glm::vec2 cameraCenter;
	bool updateCamera;
};

