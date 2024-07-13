#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Sphere.h"
#include "position-filtering-multi-thread.h"

int main()
{
	std::vector<Vector3> positions;
	ReadPositionsData("positionsData.txt", positions);


	Sphere sphere{ 5,5,5,10 };

	std::vector<Vector3> overlappingPositions;

	for (auto& position : positions) {
		if (position.squaredDistanceTo(sphere.pos) <= pow(sphere.radius, 2))
		{
			overlappingPositions.push_back(position);
		}
	}
	return 0;
}

void ReadPositionsData(const char* fileName, std::vector<Vector3>& positions)
{
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
    }

    std::string line;
    while (std::getline(file, line)) { 
        std::istringstream iss(line);
        char dummy; 
        Vector3 position;

        if (!(iss >> dummy >> position.x >> dummy >> position.y >> dummy >> position.z >> dummy)) {
            std::cerr << "Error parsing line: " << line << std::endl;
            continue; 
        }

        positions.push_back(position);
    }

    file.close();  
}

