#pragma once
#include "EditorWindow.h"
#include <vector>

struct File
{
	enum FileType
	{
		SPRITE, SCENE
	};
	std::string name;
	std::string path;
	FileType type;
};

struct Directory
{
	std::string name;
	std::string path;
	std::vector<Directory> subDirectories;
	std::vector<File> files;
};

class AssetsWindow :
	public EditorWindow
{
public:
	AssetsWindow(const char* windowName, bool enabled = true);
	~AssetsWindow();

	void DrawWindow();

	void FillSubDirectoriesRecursively(Directory dir);

private:
	Directory* assetsDirectory;
};

