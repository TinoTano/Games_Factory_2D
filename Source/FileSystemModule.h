#pragma once
#include "Module.h"
#include <vector>

struct File
{
	enum FileType
	{
		SPRITE, SCENE, UNKNOWN
	};
	std::string name;
	std::string path;
	FileType type;
	long long modifiedTime = 0;
};

struct Directory
{
	std::string name;
	std::string fullPath;
	std::vector<Directory> subDirectories;
	std::vector<File> files;
	Directory* parent = nullptr;
	long long modifiedTime = 0;
};

class FileSystemModule :
	public Module
{
public:
	FileSystemModule(const char* moduleName, bool gameModule = false);
	~FileSystemModule();

	bool Init();

	std::string LoadBinaryTextFile(const char* filePath);
	void SaveFileTo(const char* finalPath);
	std::string GetWorkingPath();
	void FillDirectoryData(Directory& directory);
	std::string GetFileDirectory(std::string filePath);
	long long GetModifiedTime(std::string path);
	void CheckDirectoryChanges(Directory& directory);
	void CheckAssetsFolder();
	void CheckLibraryFolder();
	void CreateDirectory(std::string path);
	void Delete(std::string path);
	std::string CreateUID();
	bool FileExist(std::string path);

private:
	File::FileType GetFileType(std::string extension) const;
};

