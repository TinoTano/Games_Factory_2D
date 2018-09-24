#pragma once
#include <string>

class Resource
{
public:
	enum ResourceType
	{
		RESOURCE_NONE, RESOURCE_TEXTURE
	};

	Resource();
	Resource(std::string name, std::string assets_path, std::string library_path, ResourceType type);
	~Resource();

	std::string GetUID() const;
	void SetUID(std::string uid);

	std::string GetName() const;
	void SetName(std::string name);

	std::string GetAssetsPath() const;
	void SetAssetsPath(std::string path);

	std::string GetLibraryPath() const;
	void SetLibraryPath(std::string path);

	ResourceType GetType() const;
	void SetType(ResourceType type);

	void IncreaseUsedCount();
	void DecreaseUsedCount();
	int GetUsedCount() const;

	void CreateMeta();

private:
	ResourceType res_type;
	std::string UID;
	std::string name;
	std::string assets_path;
	std::string library_path;
	int used_count;
};

