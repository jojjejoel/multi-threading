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
#include "ThreadPool.h"

std::mutex mtx;

bool RunApplication(std::vector<Vector3>& positions, Sphere& sphere, ThreadPool& pool);
void NonThreadedCalculation(std::vector<Vector3>& positions, Sphere& sphere, std::vector<Vector3>& overlappingPositions);
void ThreadedCalculation(std::vector<Vector3>& positions, Sphere& sphere, std::vector<Vector3>& overlappingPositions, ThreadPool& pool);
void workerThreadTask(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& localOverlappingPositions, size_t start, size_t end);
void IsPositionOverlappingSphere(const Vector3& position, const Sphere& sphere, std::vector<Vector3>& overlappingPositions);
void ReadPositionsData(const char* fileName, std::vector<Vector3>& positions);

int main() {
    std::vector<Vector3> positions;
    for (size_t i = 0; i < 100; i++) {
        ReadPositionsData("positionsData.txt", positions);
    }

    Sphere sphere{ 5, 5, 5, 10 };
    bool isRunning = true;
    ThreadPool pool(12);  // Initialize thread pool with 20 threads

    while (isRunning) {
        isRunning = RunApplication(positions, sphere, pool);
    }
    pool.stop();
    return 0;
}

bool RunApplication(std::vector<Vector3>& positions, Sphere& sphere, ThreadPool& pool) {
    std::cout << "1 = Non-Threaded " << std::endl;
    std::cout << "2 = Threaded " << std::endl;
    std::cout << "3 = EXIT " << std::endl;
    char inputChar;
    std::cin >> inputChar;
    if (inputChar == '3') {
        return false;
    }

    std::vector<Vector3> overlappingPositions;
    auto start = std::chrono::high_resolution_clock::now();

    if (inputChar == '1') {
        NonThreadedCalculation(positions, sphere, overlappingPositions);
    }
    else if (inputChar == '2') {
        ThreadedCalculation(positions, sphere, overlappingPositions, pool);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Calculation time: " << elapsed.count() << " ms" << std::endl;
    std::cout << "Number of overlapping positions: " << overlappingPositions.size() << std::endl;

    return true;
}

void NonThreadedCalculation(std::vector<Vector3>& positions, Sphere& sphere, std::vector<Vector3>& overlappingPositions) {
    for (auto& position : positions) {
        IsPositionOverlappingSphere(position, sphere, overlappingPositions);
    }
}

void ThreadedCalculation(std::vector<Vector3>& positions, Sphere& sphere, std::vector<Vector3>& overlappingPositions, ThreadPool& pool) {
    size_t numThreads = 20;
    size_t chunkSize = positions.size() / numThreads;
    std::vector<std::vector<Vector3>> localOverlappingPositions(numThreads);

    std::atomic<size_t> remainingTasks(numThreads);

    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? positions.size() : (i + 1) * chunkSize;
        pool.enqueueTask([&, start, end, i] {
            workerThreadTask(positions, sphere, localOverlappingPositions[i], start, end);
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

void workerThreadTask(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& localOverlappingPositions, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        if (positions[i].squaredDistanceTo(sphere.pos) <= std::pow(sphere.radius, 2)) {
            localOverlappingPositions.push_back(positions[i]);
        }
    }
}

void IsPositionOverlappingSphere(const Vector3& position, const Sphere& sphere, std::vector<Vector3>& overlappingPositions) {
    if (position.squaredDistanceTo(sphere.pos) <= pow(sphere.radius, 2)) {
        std::lock_guard<std::mutex> lock(mtx);
        overlappingPositions.push_back(position);
    }
}

void ReadPositionsData(const char* fileName, std::vector<Vector3>& positions) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
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