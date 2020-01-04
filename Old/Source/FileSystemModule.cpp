#include "FileSystemModule.h"
#include <experimental/filesystem>
#include <fstream>
#include "Log.h"
#include <vector>
#include "AssetsWindow.h"
#include <algorithm>
#include "Application.h"
#include "Timer.h"
#include "md5.h"

namespace fs = std::experimental::filesystem;

FileSystemModule::FileSystemModule(const char* moduleName, bool gameModule) : Module(moduleName, gameModule)
{
	assetsPath = GetWorkingPath() + "/Data/Assets";
	libraryPath = GetWorkingPath() + "/Data/Library";
}

FileSystemModule::~FileSystemModule()
{
}

bool FileSystemModule::Init(Data& settings)
{
	if (App->IsEditor())
	{
		CheckAssetsFolder();
	}
	CheckLibraryFolder();

	return true;
}

std::string FileSystemModule::LoadBinaryTextFile(const char* filePath)
{
	std::string fileText;

	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		CONSOLE_ERROR("failed to open file!", NULL);
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

std::string FileSystemModule::LoadTextFile(const char * filePath)
{
	return std::string();
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

void FileSystemModule::FillDirectoryData(Directory& directory)
{
	//Erase directories or files that has been deleted in the folder
	for (std::vector<Directory>::iterator direct = directory.subDirectories.begin(); direct != directory.subDirectories.end();)
	{
		if (!fs::exists(direct->fullPath))
		{
			direct = directory.subDirectories.erase(direct);
			continue;
		}
		direct++;
	}

	for (std::vector<File>::iterator file = directory.files.begin(); file != directory.files.end();)
	{
		if (!fs::exists(file->path))
		{
			file = directory.files.erase(file);
			continue;
		}
		file++;
	}

	//Add new directories or files or update the changed files
	fs::path path(directory.fullPath);
	for (auto& p : fs::directory_iterator(path))
	{
		if (fs::is_directory(p.path()))
		{
			bool directoryAlreadyExist = false;
			for (Directory& direct : directory.subDirectories)
			{
				if (direct.fullPath == p.path().string())
				{
					directoryAlreadyExist = true;
					break;
				}
			}
			if (!directoryAlreadyExist)
			{
				Directory dir;
				dir.fullPath = p.path().string();
				dir.name = p.path().filename().string();
				dir.parent = &directory;
				directory.subDirectories.emplace_back(dir);
			}
		}
		else
		{
			bool fileAlreadyExist = false;
			for (File& file : directory.files)
			{
				long long currentTime = GetModifiedTime(file.path);
				if (currentTime > directory.modifiedTime)
				{
					//Update file
					int i = 0;
				}

				if (file.path == p.path().string())
				{
					fileAlreadyExist = true;
					break;
				}
			}
			if (!fileAlreadyExist)
			{
				File file;
				file.name = p.path().filename().string();
				file.path = p.path().string();
				file.type = GetFileType(p.path().extension().string());
				directory.files.emplace_back(file);
			}
		}
	}
}

std::string FileSystemModule::GetFileDirectory(std::string filePath)
{
	fs::path path(filePath);
	return path.parent_path().string();
}

long long FileSystemModule::GetModifiedTime(std::string path)
{
	fs::file_time_type time = fs::last_write_time(path);
	return time.time_since_epoch().count();
}

void FileSystemModule::CheckDirectoryChanges(Directory & directory)
{
	//Add or remove directory content
	FillDirectoryData(directory);

	for (Directory& directory : directory.subDirectories)
	{
		CheckDirectoryChanges(directory);
	}
}

void FileSystemModule::CheckAssetsFolder()
{
	if (!fs::exists(assetsPath))
	{
		fs::create_directory(assetsPath);
	}
}

void FileSystemModule::CheckLibraryFolder()
{
	if (!fs::exists(libraryPath))
	{
		fs::create_directory(libraryPath);
	}
}

bool FileSystemModule::CreateDirectory(std::string path)
{
	if (!FileExist(path))
	{
		if (!fs::create_directory(path))
		{
			CONSOLE_ERROR("Cannot create %s folder", path.c_str());
			return false;
		}
	}
	return true;
}

void FileSystemModule::Delete(std::string path)
{
	if (fs::exists(path))
	{
		if (fs::is_directory(path))
		{
			fs::remove_all(path);
		}
		else
		{
			fs::remove(path);
		}
	}
}

std::string FileSystemModule::CreateUID()
{
	Timer t;
	return md5(std::to_string(t.GetCurrentTime()));
}

bool FileSystemModule::FileExist(std::string path)
{
	return fs::exists(path);
}

std::string FileSystemModule::GetExtension(std::string path) const
{
	std::string extension = "";
	fs::path p(path);
	if (p.has_extension()) 
	{
		extension = p.extension().string();
	}
	return extension;
}

std::string FileSystemModule::GetName(std::string path) const
{
	fs::path p(path);
	return p.filename().string();
}

void FileSystemModule::GetDirectoryFilesPath(std::string directoryPath, std::vector<std::string>& files, bool recursive)
{
	fs::path path(directoryPath);
	for (auto& p : fs::directory_iterator(path))
	{
		if (!fs::is_directory(p.path()))
		{
			files.emplace_back(p.path().string());
		}
		else
		{
			if (recursive)
			{
				GetDirectoryFilesPath(p.path().string(), files, recursive);
			}
		}
	}
}

std::vector<std::string> FileSystemModule::GetAssetsFilesPaths()
{
	std::vector<std::string> paths;

	GetDirectoryFilesPath(assetsPath, paths, true);

	return paths;
}

std::vector<std::string> FileSystemModule::GetLibraryFilesPaths()
{
	std::vector<std::string> paths;

	GetDirectoryFilesPath(libraryPath, paths, true);

	return paths;
}

File::FileType FileSystemModule::GetFileType(std::string extension) const
{
	File::FileType type;

	if (extension == ".scene")
	{
		type = File::SCENE;
	}
	else if (extension == ".jpg" || extension == ".png")
	{
		type = File::SPRITE;
	}
	else
	{
		type = File::UNKNOWN;
	}

	return type;
}

std::string FileSystemModule::GetFileText() const
{
	return std::string();
}


