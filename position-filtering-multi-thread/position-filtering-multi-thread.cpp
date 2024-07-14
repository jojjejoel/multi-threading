#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <cmath>
#include <chrono>
#include "Sphere.h"
#include "position-filtering-multi-thread.h"

int main()
{
	std::vector<Vector3> positions;

	for (size_t i = 0; i < 1000000; i++)
	{
		ReadPositionsData("positionsData.txt", positions);
	}

	Sphere sphere{ 5, 5, 5, 10 };
	bool isRunning = true;

	while (isRunning)
	{
		isRunning = RunApplication(positions, sphere);
	}
	return 0;
}

bool RunApplication(std::vector<Vector3>& positions, Sphere& sphere)
{

	std::cout << "1 = Non-Threaded " << std::endl;
	std::cout << "2 = Threaded " << std::endl;
	std::cout << "3 = EXIT " << std::endl;
	char inputChar;
	std::cin >> inputChar;
	if (inputChar == '3')
	{
		return false;
	}

	std::vector<Vector3> overlappingPositions;
	auto start = std::chrono::high_resolution_clock::now();

	if (inputChar == '1')
	{
		NonThreadedCalculation(positions, sphere, overlappingPositions);
	}
	else if (inputChar == '2')
	{
		ThreadedCalculation(positions, sphere, overlappingPositions);
	}


	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::milli> elapsed = end - start;


	std::cout << "Calculation time: " << elapsed << std::endl;

	std::cout << "Number of overlapping positions: " << overlappingPositions.size() << std::endl;

	return true;
}

void NonThreadedCalculation(std::vector<Vector3>& positions, Sphere& sphere, std::vector<Vector3>& overlappingPositions)
{
	for (auto& position : positions) {
		IsPostionOverLappingSphere(position, sphere, overlappingPositions);
	}
}

void ThreadedCalculation(std::vector<Vector3>& positions, Sphere& sphere, std::vector<Vector3>& overlappingPositions)
{
	size_t numThreads = 12;
	size_t chunkSize = positions.size() / numThreads;
	std::vector<std::thread> threads;


	auto startTime = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < numThreads; ++i) {
		size_t start = i * chunkSize;
		size_t end = (i == numThreads - 1) ? positions.size() : (i + 1) * chunkSize;
		threads.emplace_back(workerThread, std::ref(positions), std::ref(sphere), std::ref(overlappingPositions), start, end);
	}

	auto endTime = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::milli> elapsed = endTime - startTime;


	std::cout << "Creating threads time: " << elapsed << std::endl;

	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}



void workerThread(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions, size_t start, size_t end) {

	for (size_t i = start; i < end; ++i) {
		IsPostionOverLappingSphere(positions[i], sphere, overlappingPositions);
	}

}

void IsPostionOverLappingSphere(const Vector3& position, const Sphere& sphere, std::vector<Vector3>& overlappingPositions)
{
	if (position.squaredDistanceTo(sphere.pos) <= pow(sphere.radius, 2))
	{
		bool mutexIsOccupied = !vectorAddMutex.try_lock();
		while (mutexIsOccupied)
		{
			mutexIsOccupied = !vectorAddMutex.try_lock();
		}
		overlappingPositions.push_back(position);
		vectorAddMutex.unlock();
	}
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

