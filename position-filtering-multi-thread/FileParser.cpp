#include "FileParser.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Logger.h"

void FileParser::ReadPositionsData(const char* fileName, std::vector<Vector3>& positions) {
    std::ifstream file;
    file.open(fileName);
    if (!file.is_open()) {
        Logger::GetInstance().Log("Error opening file: " + std::string(fileName));
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char dummy;
        Vector3 position;
        if (!(iss >> dummy >> position.x >> dummy >> position.y >> dummy >> position.z >> dummy)) {
            Logger::GetInstance().Log("Error parsing line: " + line);
            continue;
        }
        positions.push_back(position);
    }
    file.close();
}