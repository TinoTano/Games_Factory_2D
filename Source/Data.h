#pragma once

#include <string>
#include <vector>

class Data
{
public:
	Data();
	~Data();

	void AddInt(std::string name, int value);
	void AddFloat(std::string name, float value);
	void AddBool(std::string name, bool value);
	void AddString(std::string name, std::string value);

	int GetInt(std::string name);
	float GetFloat(std::string name);
	bool GetBool(std::string name);
	std::string GetString(std::string name);

	void SaveData(std::string path);
	void LoadData(std::string path);

private:
	char* data;
	std::vector<std::string> dataNames;
	std::vector<std::string> dataValues;
};

