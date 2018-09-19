#pragma once
#include "Module.h"
#include <vector>

class FileSystemModule :
	public Module
{
public:
	FileSystemModule(const char* module_name, bool game_module = false);
	~FileSystemModule();

	std::string LoadBinaryTextFile(const char* file_path);
	void SaveFileTo(const char* final_path);
};

