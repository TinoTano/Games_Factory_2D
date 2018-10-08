#include "Application.h"

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
