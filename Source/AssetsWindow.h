#pragma once
#include "EditorWindow.h"
#include <string>

struct Directory;

class AssetsWindow :
	public EditorWindow
{
public:
	AssetsWindow(const char* windowName, bool enabled = true);
	~AssetsWindow();

	void DrawWindow();
	void FillSubDirectoriesRecursively(Directory& dir);
	void CheckDirectories();

private:
	void DrawMenuBar();
	void DrawDirectories(Directory& directory);
	void DrawFolderContents(Directory& dir);
	void OpenWindowsFolder(const char* path);
	void OpenAssetsOptions(const char* assetPath);

private:
	Directory* assetsDirectory;
	Directory* currentDirectory;
	std::string currentAssetsPath;
	std::string selectedAssetPath;
};

