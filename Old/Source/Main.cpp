#include "Application.h"

#ifdef _DEBUG
#include "MemLeaks.h"
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Application* App = nullptr;

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
#ifdef _DEBUG
	ReportMemoryLeaks();
#endif

	App = new Application();

	if (App != nullptr) {
		if (App->Init()) {
			if (App->Start()) {
				while (App->DoUpdate()) {}
				App->CleanUp();
			}
		}
	}

	delete App;
	App = nullptr;

	return 0;
}
#else

std::unique_ptr<Application> App = nullptr;
int main()
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
#endif // _WIN32