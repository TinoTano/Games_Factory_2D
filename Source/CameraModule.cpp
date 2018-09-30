#include "CameraModule.h"
#include "Globals.h"

CameraModule::CameraModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
	updateCamera = false;
	orthoProjection = glm::mat4();
	cameraPosition = glm::vec2(0);
	cameraRotation = 0;
	cameraZoom = 1;
}

CameraModule::~CameraModule()
{
}

bool CameraModule::Init()
{
	CalculeOrthoProjection();
	return true;
}

bool CameraModule::Update(float delta_time)
{
	if (updateCamera)
	{
		CalculeOrthoProjection();
		updateCamera = false;
	}
	return true;
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
	cameraZoom += increase;
	updateCamera = true;
}

float CameraModule::GetZoom() const
{
	return cameraZoom;
}

glm::mat4 CameraModule::GetOrthoProjection() const
{
	return orthoProjection;
}

void CameraModule::CalculeOrthoProjection()
{
	glm::vec2 cameraViewSize = glm::vec2(static_cast<float>(1) / (cameraZoom), static_cast<float>(1) / (cameraZoom));
	glm::vec2 viewStart(cameraPosition.x - cameraViewSize.x, cameraPosition.y - cameraViewSize.y);
	glm::vec2 viewEnd(cameraPosition.x + cameraViewSize.x, cameraPosition.y + cameraViewSize.y);

	//CONSOLE_LOG("%.3f, %.3f, %.3f, %3.f", viewStart.x, viewEnd.x, viewEnd.y, viewStart.y);

	orthoProjection = glm::ortho(0.0f, 1 * 1200 * cameraZoom, 1 * 900 * cameraZoom, 0.0f);
	//orthoProjection = glm::ortho(viewStart.x, 1 * viewEnd.x * cameraZoom, 1 * viewEnd.y * cameraZoom, viewStart.y, -100.0f, 0.1f);
	orthoProjection = glm::rotate(orthoProjection, glm::radians(cameraRotation), glm::vec3(0.f, 0.f, 1.f));
}
