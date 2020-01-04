#include "Resource.h"
#include <fstream>
#include <ctime>
#include "FileSystemModule.h"
#include "Application.h"

Resource::Resource()
{
}

Resource::Resource(std::string name, std::string assetsPath, std::string fullPath, ResourceType type) :
	name(name), assetsPath(assetsPath), fullPath(fullPath), resType(type), usedCount(0)
{
	UID = App->fileSystemModule->CreateUID();
	//SetLibraryPath(App->fileSystemModule->libraryPath + "/" + UID);
}

Resource::~Resource()
{
}

std::string Resource::GetUID() const
{
	return UID;
}

void Resource::SetUID(std::string uid)
{
	UID = uid;
}

std::string Resource::GetName() const
{
	return name;
}

void Resource::SetName(std::string name)
{
	this->name = name;
}

std::string Resource::GetAssetsPath() const
{
	return assetsPath;
}

void Resource::SetAssetsPath(std::string path)
{
	assetsPath = path;
}

std::string Resource::GetLibraryPath() const
{
	return libraryPath;
}

void Resource::SetLibraryPath(std::string path)
{
	libraryPath = path;
}

Resource::ResourceType Resource::GetType() const
{
	return resType;
}

void Resource::SetType(ResourceType type)
{
	resType = type;
}

void Resource::IncreaseUsedCount()
{
	usedCount++;
}

void Resource::DecreaseUsedCount()
{
	if (usedCount > 0)
		usedCount--;
}

int Resource::GetUsedCount() const
{
	return usedCount;
}

void Resource::CreateMeta()
{
	time_t now = time(0);
	char* dt = ctime(&now);

	std::string file_content;
	file_content += "UID: " + UID + "\n";
	file_content += std::to_string(now) + "\n";
	file_content += "Library Path: " + libraryPath;

	std::ofstream output_file(assetsPath + ".meta");
	output_file << file_content;
	output_file.close();
}
