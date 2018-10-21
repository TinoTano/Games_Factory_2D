#include "Data.h"
#include <fstream>

Data::Data()
{
}

Data::~Data()
{
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

void Data::SaveData(std::string path)
{
	uint32_t dataNamesSize = sizeof(std::string) * dataNames.size();
	uint32_t dataValuesSize = sizeof(std::string) * dataValues.size();

	uint32_t totalSize = 0;
	totalSize += dataNamesSize;
	totalSize += dataValuesSize;
	totalSize += sizeof(uint32_t);
	totalSize += sizeof(uint32_t);

	data = new char[totalSize];
	char* cursor = data;

	uint32_t bytes;
	bytes = sizeof(uint32_t);
	memcpy(cursor, &dataNamesSize, bytes);

	cursor += bytes;
	bytes = dataNamesSize;
	memcpy(cursor, dataNames.data(), bytes);

	cursor += bytes;
	bytes = sizeof(uint32_t);
	memcpy(cursor, &dataValuesSize, bytes);

	cursor += bytes;
	bytes = dataValuesSize;
	memcpy(cursor, dataValues.data(), bytes);

	std::ofstream outfile(path, std::ofstream::binary);
	outfile.write(data, totalSize);
	outfile.close();

	delete[] data;
	data = nullptr;
}

void Data::LoadData(std::string path)
{
	std::ifstream file(path, std::ifstream::binary);
	if (file.is_open())
	{
		// get length of file:
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		char * buffer = new char[length];
		file.read(buffer, length);

		if (file)
		{
			char* cursor = buffer;
			uint32_t dataNamesSize;
			uint32_t dataValuesSize;

			//Get value names
			uint32_t bytes = sizeof(uint32_t);
			memcpy(&dataNamesSize, cursor, bytes);

			cursor += bytes;
			bytes = dataNamesSize;
			if (!dataNames.empty())
			{
				dataNames.clear();
			}
			dataNames.resize(bytes / sizeof(std::string));
			memcpy(dataNames.data(), cursor, bytes);

			//Get value data
			cursor += bytes;
			bytes = sizeof(uint32_t);
			memcpy(&dataValuesSize, cursor, bytes);

			cursor += bytes;
			bytes = dataValuesSize;
			if (!dataValues.empty())
			{
				dataValues.clear();
			}
			dataValues.resize(bytes / sizeof(std::string));
			memcpy(dataValues.data(), cursor, bytes);

			delete[] buffer;
			buffer = nullptr;
		}
		file.close();
	}
}
