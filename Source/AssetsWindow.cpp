#include "AssetsWindow.h"
#include "Application.h"
#include "FileSystemModule.h"

AssetsWindow::AssetsWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
	assetsDirectory = new Directory();
	assetsDirectory->path = (App->fileSystemModule->GetWorkingPath() + "\\Data\\Assets").c_str();
	assetsDirectory->name = "Assets";
	FillSubDirectoriesRecursively(*assetsDirectory);
}


AssetsWindow::~AssetsWindow()
{
}

void AssetsWindow::DrawWindow()
{
}

void AssetsWindow::FillSubDirectoriesRecursively(Directory dir)
{
	std::vector<std::string> subdirectories;
	std::vector<std::string> files;
	App->fileSystemModule->GetSubDirectories(dir.path, subdirectories, files);
	for (std::string directory : subdirectories)
	{
		Directory direct;
		direct.path = directory.c_str();
		//direct.name = directory.
		dir.subDirectories.emplace_back(direct);
		for (std::string file : files)
		{
			File f;
			f.name = 
		}
		FillSubDirectoriesRecursively(direct);
	}
}
