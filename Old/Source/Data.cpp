#include "Data.h"
#include <fstream>

Data::Data()
{
}

Data::~Data()
{
	dataNames.clear();
	dataValues.clear();
}

void Data::AddInt(std::string name, int value)
{
	dataNames.emplace_back(name);
	dataValues.emplace_back(std::to_string(value));
}

void Data::AddFloat(std::string name, float value)
{
	dataNames.emplace_back(name);
	dataValues.emplace_back(std::to_string(value));
}

void Data::AddBool(std::string name, bool value)
{
	dataNames.emplace_back(name);
	dataValues.emplace_back(std::to_string(value));
}

void Data::AddString(std::string name, std::string value)
{
	dataNames.emplace_back(name);
	dataValues.emplace_back(value);
}

int Data::GetInt(std::string name)
{
	int ret = -1;
	std::vector<std::string>::iterator it = std::find(dataNames.begin(), dataNames.end(), name);
	if (it != dataNames.end())
	{
		ret = stoi(dataValues[it - dataNames.begin()]);
	}
	return ret;
}

float Data::GetFloat(std::string name)
{
	float ret = -1.0f;
	std::vector<std::string>::iterator it = std::find(dataNames.begin(), dataNames.end(), name);
	if (it != dataNames.end())
	{
		ret = stof(dataValues[it - dataNames.begin()]);
	}
	return ret;
}

bool Data::GetBool(std::string name)
{
	bool ret = false;
	std::vector<std::string>::iterator it = std::find(dataNames.begin(), dataNames.end(), name);
	if (it != dataNames.end())
	{
		if (dataValues[it - dataNames.begin()] == "1") ret = true;
	}
	return ret;
}

std::string Data::GetString(std::string name)
{
	std::string ret = "";
	std::vector<std::string>::iterator it = std::find(dataNames.begin(), dataNames.end(), name);
	if (it != dataNames.end())
	{
		ret = dataValues[it - dataNames.begin()];
	}
	return ret;
}

void Data::CreateSection(std::string sectionName)
{
	AddString("Section", sectionName);
}

void Data::CloseSection(std::string sectionName)
{
	AddString("CloseSection", sectionName);
}

bool Data::GetSectionData(std::string sectionName, Data& data)
{
	bool ret = false;

	std::vector<std::string>::iterator it = std::find(dataValues.begin(), dataValues.end(), sectionName);
	if (it != dataValues.end())
	{
		ret = true;
		while (dataNames[it - dataValues.begin()] != "CloseSection" || *it != sectionName)
		{
			*it++;
			data.AddString(dataNames[it - dataValues.begin()], *it);
		}
	}

	return ret;
}

void Data::SaveData(std::string path)
{
	FILE* file = fopen(path.c_str(), "wb");
	if (file != NULL)
	{
		int vsize = int(dataNames.size());
		fwrite(&vsize, sizeof(int), 1, file);

		for (int i = 0; i < dataNames.size(); i++)
		{
			vsize = int(dataNames[i].size());
			fwrite(&vsize, sizeof(int), 1, file);
			fwrite(dataNames[i].c_str(), sizeof(char), dataNames[i].length(), file);
		}

		for (int i = 0; i < dataValues.size(); i++)
		{
			vsize = int(dataValues[i].size());
			fwrite(&vsize, sizeof(int), 1, file);
			fwrite(dataValues[i].c_str(), sizeof(char), dataValues[i].length(), file);
		}
		fclose(file);
	}
}

bool Data::LoadData(std::string path)
{
	bool ret = false;

	FILE* file = fopen(path.c_str(), "rb");

	if (file != NULL)
	{
		int size = 0;
		fread(&size, sizeof(int), 1, file);
		dataNames.resize(size);
		dataValues.resize(size);

		for (int i = 0; i < dataNames.size(); i++)
		{
			int sSize = 0;
			fread(&sSize, sizeof(int), 1, file);
			std::string s;
			for (int j = 0; j < sSize; j++)
			{
				char c;
				fread(&c, sizeof(char), 1, file);
				s += c;
			}
			dataNames[i] = s;
		}

		for (int i = 0; i < dataValues.size(); i++)
		{
			int sSize = 0;
			fread(&sSize, sizeof(int), 1, file);
			std::string s;
			for (int j = 0; j < sSize; j++)
			{
				char c;
				fread(&c, sizeof(char), 1, file);
				s += c;
			}
			dataValues[i] = s;
		}
		ret = true;
		fclose(file);
	}

	return ret;
}
