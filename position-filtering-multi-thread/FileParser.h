#pragma once
#include "Vector3.h"
#include <vector>
class FileParser
{
	public:
	void ReadPositionsData(const char* fileName, std::vector<Vector3>& positions);
};

