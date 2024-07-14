#pragma once
#include "Vector3.h"

void ReadPositionsData(const char* fileName, std::vector<Vector3>& positions);

void workerThread(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions, size_t start, size_t end);

bool RunApplication(std::vector<Vector3>& positions, Sphere& sphere);

void NonThreadedCalculation(std::vector<Vector3>& positions, Sphere& sphere, std::vector<Vector3>& overlappingPositions);

void ThreadedCalculation(std::vector<Vector3>& positions, Sphere& sphere, std::vector<Vector3>& overlappingPositions);

void IsPostionOverLappingSphere(const Vector3 & position, const Sphere & sphere, std::vector<Vector3>& overlappingPositions);

std::mutex vectorAddMutex;
std::mutex logMutex;