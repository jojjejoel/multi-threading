#pragma once
#include "Vector3.h"
#include "Sphere.h"
#include "ThreadPool.h"
#include <vector>

class PositionFilterer
{
	public:
	void RunApplication();

	private:
	bool DoCalculations(const std::vector<Vector3>& positions, const Sphere& sphere, ThreadPool& pool);
	void NonThreadedCalculation(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions);
	void ThreadedCalculation(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions, ThreadPool& pool);
	void WorkerThreadTask(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& localOverlappingPositions, size_t start, size_t end);
	void IsPositionOverlappingSphere(const Vector3& position, const Sphere& sphere, std::vector<Vector3>& overlappingPositions);
	void ReadPositionsData(const char* fileName, std::vector<Vector3>& positions);

	bool OpenFile(std::ifstream& file, const char* fileName);

	std::mutex vectorAddMutex;
	std::mutex logMutex;
};

