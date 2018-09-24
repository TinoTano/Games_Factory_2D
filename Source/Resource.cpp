#include "Resource.h"
#include <fstream>
#include <ctime>
#include "md5.h"

Resource::Resource()
{
}

Resource::Resource(std::string name, std::string assets_path, std::string library_path, ResourceType type) :
	name(name), assets_path(assets_path), library_path(library_path), res_type(type), used_count(0)
{
	unsigned int size = sizeof(this);
	char* data = new char[size];
	memcpy(data, this, size);

	UID = md5(data);

	delete[] data;
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
	return assets_path;
}

void Resource::SetAssetsPath(std::string path)
{
	assets_path = path;
}

std::string Resource::GetLibraryPath() const
{
	return library_path;
}

void Resource::SetLibraryPath(std::string path)
{
	library_path = path;
}

Resource::ResourceType Resource::GetType() const
{
	return res_type;
}

void Resource::SetType(ResourceType type)
{
	res_type = type;
}

void Resource::IncreaseUsedCount()
{
	used_count++;
}

void Resource::DecreaseUsedCount()
{
	if (used_count > 0)
		used_count--;
}

int Resource::GetUsedCount() const
{
	return used_count;
}

void Resource::CreateMeta()
{
	time_t now = time(0);
	char* dt = ctime(&now);

	std::string file_content;
	file_content += "UID: " + UID + "\n";
	file_content += std::to_string(now) + "\n";
	file_content += "Library Path: " + library_path;

	std::ofstream output_file(assets_path + ".meta");
	output_file << file_content;
	output_file.close();
}
