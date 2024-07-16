#pragma once
#include "Vector3.h"
#include "Sphere.h"
#include "ThreadPool.h"
#include <vector>
#include <string>

enum class CalculationType
{
	NonThreaded,
	Threaded
};

class PositionFilterer
{
	public:
	void RunApplication();


	private:
	void PerformCalculation(const std::string& calculationType, const std::function<void(std::vector<Vector3>&)>& calculationFunc);
	void DoCalculations(const std::vector<Vector3>& positions, const Sphere& sphere, ThreadPool& pool);
	void NonThreadedCalculation(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions);
	void ThreadedCalculation(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions, ThreadPool& pool);
	void WorkerThreadTask(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& localOverlappingPositions, size_t start, size_t end);
	void Log(const std::string msg);
	void IsPositionOverlappingSphere(const Vector3& position, const Sphere& sphere, std::vector<Vector3>& overlappingPositions);

	void WaitForInput();


	// Leave one thread for the main thread
	ThreadPool threadPool;
	std::mutex vectorAddMutex;
	std::mutex logMutex;
	bool isRunning = true;
};

