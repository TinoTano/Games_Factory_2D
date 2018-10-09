#include "CameraModule.h"
#include "Globals.h"
#include "Application.h"
#include "WindowModule.h"

CameraModule::CameraModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
	updateCamera = false;
	projMatrix = glm::mat4();
	viewMatrix = glm::mat4();
	cameraPosition = glm::vec2(0);
	cameraRotation = 0;
	cameraZoom = 1;
	cameraCenter = glm::vec2(0);
}

CameraModule::~CameraModule()
{
}

bool CameraModule::Init()
{
	UpdateCameraMatrix(App->windowModule->GetWidth(), App->windowModule->GetHeight());
	return true;
}

bool CameraModule::Update(float delta_time)
{
	if (updateCamera)
	{
		UpdateViewMatrix();
		updateCamera = false;
	}
	return true;
}

void CameraModule::UpdateCameraMatrix(float width, float height)
{
	SetOrthoprojection(width, height);
	UpdateViewMatrix();
}

void CameraModule::SetPosition(glm::vec2 position)
{
	cameraPosition = position;
	updateCamera = true;
}

void CameraModule::IncreasePosition(glm::vec2 increase)
{
	cameraPosition += increase;
	updateCamera = true;
}

glm::vec2 CameraModule::GetPosition() const
{
	return cameraPosition;
}

void CameraModule::SetRotation(float rotation)
{
	cameraRotation = rotation;
	updateCamera = true;
}

void CameraModule::IncreaseRotation(float increase)
{
	cameraRotation += increase;
	updateCamera = true;
}

float CameraModule::GetRotation() const
{
	return cameraRotation;
}

void CameraModule::SetZoom(float zoom)
{
	cameraZoom = zoom;
	updateCamera = true;
}

void CameraModule::IncreaseZoom(float increase)
{
	cameraZoom *= increase;
	updateCamera = true;
}

float CameraModule::GetZoom() const
{
	return cameraZoom;
}

void CameraModule::SetOrthoprojection(float width, float height)
{
	projMatrix = glm::ortho(0.0f, width, height, 0.0f);

	cameraCenter.x = width * 0.5f;
	cameraCenter.y = height * 0.5f;
}

glm::mat4 CameraModule::GetOrthoProjection() const
{
	return projMatrix;
}

glm::mat4 CameraModule::GetViewMatrix() const
{
	return viewMatrix;
}

glm::vec2 CameraModule::GetCameraCenter() const
{
	return cameraCenter;
}

void CameraModule::UpdateViewMatrix()
{
	glm::mat4 view(1.0f);
	view = glm::translate(view, glm::vec3(cameraCenter.x + cameraPosition.x, cameraCenter.y + cameraPosition.y, 0.0f));
	view = glm::rotate(view, glm::radians(cameraRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	view = glm::scale(view, glm::vec3(cameraZoom, cameraZoom, 0.0f));
	viewMatrix = view;
}
