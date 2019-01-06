#pragma once
#include "Module.h"
#include <vector>

struct File
{
	enum FileType
	{
		SPRITE, SCENE, ANIMATION, LUASCRIPT, UNKNOWN
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
	std::string LoadTextFile(const char* filePath);
	void SaveFileTo(const char* finalPath);
	std::string GetWorkingPath();
	void FillDirectoryData(Directory& directory);
	std::string GetFileDirectory(std::string filePath);
	long long GetModifiedTime(std::string path);
	void CheckDirectoryChanges(Directory& directory);
	void CheckAssetsFolder();
	void CheckLibraryFolder();
	bool CreateDirectory(std::string path);
	void Delete(std::string path);
	std::string CreateUID();
	bool FileExist(std::string path);
	std::string GetExtension(std::string path) const;
	std::string GetName(std::string path) const;
	void GetDirectoryFilesPath(std::string directoryPath, std::vector<std::string>& files, bool recursive);
	std::vector<std::string> GetAssetsFilesPaths();
	std::vector<std::string> GetLibraryFilesPaths();

private:
	File::FileType GetFileType(std::string extension) const;
	std::string GetFileText() const;

public:
	std::string assetsPath;
	std::string libraryPath;
};

