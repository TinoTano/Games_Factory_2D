#include "FileSystemModule.h"
#include <experimental/filesystem>
#include <fstream>
#include "Globals.h"
#include <vector>
#include "AssetsWindow.h"


namespace fs = std::experimental::filesystem;

FileSystemModule::FileSystemModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
}

FileSystemModule::~FileSystemModule()
{
}

std::string FileSystemModule::LoadBinaryTextFile(const char* filePath)
{
	std::string fileText;

	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		const char* s = "failed to open file!";
		printf("%s\n", s);
		//CONSOLE_ERROR("%s", s);
	}
	else
	{
		size_t fileSize = (size_t)file.tellg();
		fileText.resize(fileSize);

		file.seekg(0);
		file.read((char*)fileText.data(), fileSize);

		file.close();
	}

	return fileText;
}

void FileSystemModule::SaveFileTo(const char * final_path)
{
	/*fs::path path(old_path);
	if (path.compare(new_path) != 0)
		fs::copy(old_path, new_path, fs::copy_options::update_existing);*/
}

std::string FileSystemModule::GetWorkingPath()
{
	return fs::current_path().string();
}

void FileSystemModule::GetSubDirectories(std::string directoryPath, std::vector<std::string>& directories, std::vector<std::string>& files)
{
	fs::path path(directoryPath);
	for (auto& p : fs::directory_iterator(path))
	{
		if (fs::is_directory(p.path()))
		{
			directories.emplace_back(p.path().string());
		}
		else
		{
			files.emplace_back(p.path().string());
		}
	}
}
