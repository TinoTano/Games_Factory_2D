#pragma once
#include <vector>
#include <string>

class Scene;

class Builder
{
public:
	Builder();
	~Builder();

	bool BuildForWindows(std::string path);

private:
	std::vector<Scene*> scenesToBuild;
};

