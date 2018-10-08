#pragma once
#include "Module.h"

class FileSystemModule :
	public Module
{
public:
	FileSystemModule(const char* module_name, bool game_module = false);
	~FileSystemModule();

	std::string LoadBinaryTextFile(const char* filePath);
	void SaveFileTo(const char* finalPath);
	std::string GetFullPath(std::string fileName);
};

