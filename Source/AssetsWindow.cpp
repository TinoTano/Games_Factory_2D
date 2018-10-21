#include "AssetsWindow.h"
#include "Application.h"
#include "FileSystemModule.h"
#include <imgui.h>
#include <windows.h>
#include "Globals.h"

AssetsWindow::AssetsWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
	assetsDirectory = new Directory();
	assetsDirectory->fullPath = (App->fileSystemModule->GetWorkingPath() + "/Data/Assets").c_str();
	assetsDirectory->name = "Assets";
	currentDirectory = assetsDirectory;
	FillSubDirectoriesRecursively(*assetsDirectory);
}

AssetsWindow::~AssetsWindow()
{
}

void AssetsWindow::DrawWindow()
{
	bool open = true;
	ImGui::Begin("Assets", &open);
	ImGui::Columns(2);
	ImGui::BeginChild("child");
	DrawDirectories(*assetsDirectory);
	ImGui::EndChild();
	ImGui::NextColumn();
	DrawFolderContents(*currentDirectory);
	OpenAssetsOptions(selectedAssetPath.c_str());
	ImGui::End();
}

void AssetsWindow::DrawMenuBar()
{
	/*ImGui::BeginMenuBar();
	if (ImGui::ArrowButton("UpArrow", ImGuiDir_Up))
	{
		if (currentDirectory != assetsDirectory)
		{
			currentDirectory = currentDirectory->parent;
		}
	}
	ImGui::SameLine();
	currentAssetsPath = currentDirectory->enginePath + "/";
	ImGui::Text(currentAssetsPath.c_str());
	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("Path Options");
	}
	if (ImGui::BeginPopup("Path Options"))
	{
		if (ImGui::Selectable("Open Folder"))
		{
			OpenWindowsFolder(currentDirectory->fullPath.c_str());
		}
		ImGui::EndPopup();
	}
	ImGui::EndMenuBar();*/
}

void AssetsWindow::DrawDirectories(Directory& directory)
{
	int flags = 0;
	if (currentDirectory == &directory)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (directory.subDirectories.empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}
	
	if(ImGui::TreeNodeEx(directory.name.c_str(), flags))
	{
		if (ImGui::IsItemClicked(0))
		{
			currentDirectory = &directory;
		}
		for (Directory& dir : directory.subDirectories)
		{
			DrawDirectories(dir);
		}
		ImGui::TreePop();
	}
}

void AssetsWindow::DrawFolderContents(Directory & dir)
{
	for (Directory& directory : dir.subDirectories)
	{
		if (ImGui::GetItemRectMax().y > ImGui::GetWindowPos().y + ImGui::GetWindowSize().y) break;
		if (ImGui::Selectable(directory.name.c_str()))
		{
			currentDirectory = &directory;
			currentAssetsPath += directory.name + "/";
		}
		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup("Asset Options");
			selectedAssetPath = directory.fullPath;
		}
	}
	for (File& file : dir.files)
	{
		if (ImGui::GetItemRectMax().y > ImGui::GetWindowPos().y + ImGui::GetWindowSize().y) break;
		if (ImGui::Selectable(file.name.c_str()))
		{
			//Draw file options in Inspector window
		}
		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup("Asset Options");
			selectedAssetPath = file.path;
		}
	}
}

void AssetsWindow::OpenWindowsFolder(const char * path)
{
	ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWNORMAL);
}

void AssetsWindow::OpenAssetsOptions(const char* assetPath)
{
	if (ImGui::BeginPopup("Asset Options"))
	{
		if (ImGui::Selectable("Open Folder"))
		{
			std::string folderPath = App->fileSystemModule->GetFileDirectory(assetPath);
			OpenWindowsFolder(folderPath.c_str());
		}
		if (ImGui::Selectable("Open"))
		{
			OpenWindowsFolder(assetPath);
		}
		ImGui::EndPopup();
	}
}

void AssetsWindow::FillSubDirectoriesRecursively(Directory& dir)
{
	App->fileSystemModule->FillDirectoryData(dir);
	
	for (Directory& directory : dir.subDirectories)
	{
		FillSubDirectoriesRecursively(directory);
	}
}

void AssetsWindow::CheckDirectories()
{
	App->fileSystemModule->CheckDirectoryChanges(*assetsDirectory);
}
