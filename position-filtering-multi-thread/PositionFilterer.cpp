#include "PositionFilterer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include "Sphere.h"
#include <cassert>
#include <stdlib.h>
std::mutex mtx;

void PositionFilterer::RunApplication() {
	std::vector<Vector3> positions;
	ReadPositionsData("positionsData.txt", positions);
	std::cout << "Number of positions: " << positions.size() << std::endl;
	Sphere sphere{ 5, 5, 5, 10 };

	unsigned long const hardwareThreads = std::thread::hardware_concurrency();
	std::cout << "Hardware threads: " << hardwareThreads << std::endl;
	assert(std::thread::hardware_concurrency() > 0, "Hardware concurrency is not detected!");
	ThreadPool pool(hardwareThreads);

	bool isRunning = true;
	while (isRunning) {
		isRunning = DoCalculations(positions, sphere, pool);
	}
	pool.stop();
}

bool PositionFilterer::DoCalculations(const std::vector<Vector3>& positions, const Sphere& sphere, ThreadPool& pool) {
	std::cout << "Number of positions: " << positions.size() << std::endl;
	std::cout << "Press Enter to run calculation:" << std::endl;
	std::cin.get();
	system("CLS");

	std::vector<Vector3> overlappingPositionsNonThreaded;
	auto startTimeNonThreaded = std::chrono::high_resolution_clock::now();
	NonThreadedCalculation(positions, sphere, overlappingPositionsNonThreaded);
	auto endTimeNonThreaded = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsedTimeNonThreaded = endTimeNonThreaded - startTimeNonThreaded;
	std::cout << "NON-THREADED" << std::endl;
	std::cout << "Calculation time: " << elapsedTimeNonThreaded.count() << " ms" << std::endl;
	std::cout << "Number of overlapping positions: " << overlappingPositionsNonThreaded.size() << std::endl << std::endl;



	std::vector<Vector3> overlappingPositionsThreaded;
	auto startTimeThreaded = std::chrono::high_resolution_clock::now();
	ThreadedCalculation(positions, sphere, overlappingPositionsThreaded, pool);
	auto endTimeThreaded = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsedTimeThreaded = endTimeThreaded - startTimeThreaded;
	std::cout << "THREADED (" << pool.GetNumberOfThreads() << " threads)" << std::endl;
	std::cout << "Calculation time threaded: " << elapsedTimeThreaded.count() << " ms" << std::endl;
	std::cout << "Number of overlapping positions: " << overlappingPositionsThreaded.size() << std::endl << std::endl;

	return true;
}

void PositionFilterer::NonThreadedCalculation(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions) {
	for (auto& position : positions) {
		IsPositionOverlappingSphere(position, sphere, overlappingPositions);
	}
}

void PositionFilterer::ThreadedCalculation(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions, ThreadPool& pool) {
	size_t numThreads = 20;
	size_t chunkSize = positions.size() / numThreads;
	std::vector<std::vector<Vector3>> localOverlappingPositions(numThreads);

	std::atomic<size_t> remainingTasks(numThreads);

	for (size_t i = 0; i < numThreads; ++i) {
		size_t start = i * chunkSize;
		size_t end = (i == numThreads - 1) ? positions.size() : (i + 1) * chunkSize;
		pool.enqueueTask([&, start, end, i] {
			WorkerThreadTask(positions, sphere, localOverlappingPositions[i], start, end);
			remainingTasks--;
			});
	}

	// Busy wait for all tasks to complete
	while (remainingTasks > 0) {}

	// Gather results from all threads
	for (const auto& localList : localOverlappingPositions) {
		std::lock_guard<std::mutex> lock(mtx);
		overlappingPositions.insert(overlappingPositions.end(), localList.begin(), localList.end());
	}
}

void PositionFilterer::WorkerThreadTask(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& localOverlappingPositions, size_t start, size_t end) {
	for (size_t i = start; i < end; ++i) {
		if (positions[i].squaredDistanceTo(sphere.pos) <= std::pow(sphere.radius, 2)) {
			localOverlappingPositions.push_back(positions[i]);
		}
	}
}

void PositionFilterer::IsPositionOverlappingSphere(const Vector3& position, const Sphere& sphere, std::vector<Vector3>& overlappingPositions) {
	if (position.squaredDistanceTo(sphere.pos) <= pow(sphere.radius, 2)) {
		std::lock_guard<std::mutex> lock(mtx);
		overlappingPositions.push_back(position);
	}
}

void PositionFilterer::ReadPositionsData(const char* fileName, std::vector<Vector3>& positions) {
	std::ifstream file;

	if (!OpenFile(file, fileName))
	{
		return;
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

bool PositionFilterer::OpenFile(std::ifstream& file, const char* fileName)
{
	file.open(fileName);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << fileName << std::endl;
		return false;
	}
	return true;
}
