#include "ResourcesManager.h"
#include "Resource.h"


ResourcesManager::ResourcesManager()
{
}

ResourcesManager::~ResourcesManager()
{
}

void ResourcesManager::AddResource(Resource & resource)
{
	resources[resource.GetUID()] = &resource;
}

Resource * ResourcesManager::GetResource(std::string UID) const
{
	if (resources.find(UID) != resources.end()) return resources.at(UID);
	return nullptr;
}
