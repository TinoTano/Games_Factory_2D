#pragma once
#include "Module.h"
#include <vector>
#include "AssetsWindow.h"

class FileSystemModule :
	public Module
{
public:
	FileSystemModule(const char* moduleName, bool gameModule = false);
	~FileSystemModule();

	std::string LoadBinaryTextFile(const char* filePath);
	void SaveFileTo(const char* finalPath);
	std::string GetWorkingPath();
	void GetSubDirectories(std::string directoryPath, std::vector<std::string>& directories, std::vector<std::string>& files);
};

