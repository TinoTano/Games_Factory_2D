#pragma once
#include <map>
#include <string>

class Resource;

class ResourcesManager
{
public:
	ResourcesManager();
	~ResourcesManager();

	void AddResource(Resource& resource);
	Resource* GetResource(std::string UID) const;

private:
	std::map<std::string, Resource*> resources;
};

