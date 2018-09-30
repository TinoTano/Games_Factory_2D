#include "Application.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::unique_ptr<Application> App = nullptr;

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	App = std::make_unique<Application>();

	if (App != nullptr) {
		if (App->Init()) {
			if (App->Start()) {
				while (App->DoUpdate()) {}
				App->CleanUp();
			}
		}
	}

	App = nullptr;
	return 0;
}