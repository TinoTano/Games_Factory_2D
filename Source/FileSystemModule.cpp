#include "FileSystemModule.h"
#include <filesystem>
#include <fstream>


namespace fs = std::experimental::filesystem;

FileSystemModule::FileSystemModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
}

FileSystemModule::~FileSystemModule()
{
}

std::string FileSystemModule::LoadBinaryTextFile(const char* file_path)
{
	std::ifstream file(file_path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::string buffer;
	buffer.resize(fileSize);

	file.seekg(0);
	file.read((char*)buffer.data(), fileSize);

	file.close();

	return buffer;
}

void FileSystemModule::SaveFileTo(const char * final_path)
{
	fs::path path(old_path);
	if (path.compare(new_path) != 0)
		fs::copy(old_path, new_path, fs::copy_options::update_existing);
}


