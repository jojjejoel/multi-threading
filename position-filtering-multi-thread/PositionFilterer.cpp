#include "PositionFilterer.h"
#include "Sphere.h"
#include "Timer.h"
#include "Logger.h"
#include "FileParser.h"

#include <thread>
#include <mutex>

#include <stdlib.h>
#include <Windows.h>


void PositionFilterer::RunApplication() {
    std::vector<Vector3> positions;
    FileParser fileParser;
    fileParser.ReadPositionsData("positionsData.txt", positions);
    Sphere sphere{ 5, 5, 5, 10 };

    unsigned long const hardwareThreads = std::thread::hardware_concurrency();
    if (!(std::thread::hardware_concurrency() > 0)) {
        throw std::runtime_error("Hardware concurrency is not detected!");
    }

    threadPool.Init(hardwareThreads - 1);
    threadPool.Start();

    while (isRunning) {
        system("CLS");
        Log("Number of positions: " + std::to_string(positions.size()));
        Log("Hardware threads: " + std::to_string(hardwareThreads) + "\n");
        DoCalculations(positions, sphere, threadPool);
        WaitForInput();
    }
    threadPool.Stop();
}

void PositionFilterer::PerformCalculation(const std::string& calculationTypeString, const std::function<void(std::vector<Vector3>&)>& calculationFunc) {
    std::vector<Vector3> overlappingPositions;
    Timer timer;
    timer.Start();
    calculationFunc(overlappingPositions);
    timer.Stop();
    Log(calculationTypeString);
    Log("Calculation time: " + std::to_string(timer.GetElapsedTime()) + " ms");
    Log("Overlapping positions: " + std::to_string(overlappingPositions.size()) + "\n");
}

void PositionFilterer::DoCalculations(const std::vector<Vector3>& positions, const Sphere& sphere, ThreadPool& pool) {
    PerformCalculation("NON-THREADED", [&](std::vector<Vector3>& overlappingPositions) {
        NonThreadedCalculation(positions, sphere, overlappingPositions);
        });

    PerformCalculation("THREADED (" + std::to_string(pool.GetNumThreads()) + " threads)", [&](std::vector<Vector3>& overlappingPositions) {
        ThreadedCalculation(positions, sphere, overlappingPositions, pool);
        });
}

void PositionFilterer::NonThreadedCalculation(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions) {
    for (const auto& position : positions) {
        IsPositionOverlappingSphere(position, sphere, overlappingPositions);
    }
}

void PositionFilterer::ThreadedCalculation(const std::vector<Vector3>& positions, const Sphere& sphere, std::vector<Vector3>& overlappingPositions, ThreadPool& pool) {
    const size_t numThreads = pool.GetNumThreads();
    const size_t chunkSize = (positions.size() + numThreads - 1) / numThreads;  // Ensure we cover all positions
    std::vector<std::vector<Vector3>> localOverlappingPositions(numThreads);

    std::atomic<size_t> remainingTasks(numThreads);

    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? positions.size() : (i + 1) * chunkSize;
        pool.EnqueueTask([&, start, end, i] {
            WorkerThreadTask(positions, sphere, localOverlappingPositions[i], start, end);
            remainingTasks--;
            });
    }

    while (remainingTasks > 0) {
        std::this_thread::yield();  // Yield to allow other threads to progress
    }

    for (const auto& localList : localOverlappingPositions) {
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

void PositionFilterer::Log(const std::string msg)
{
    Logger::GetInstance().Log(msg);
}

void PositionFilterer::IsPositionOverlappingSphere(const Vector3& position, const Sphere& sphere, std::vector<Vector3>& overlappingPositions) {
    if (position.squaredDistanceTo(sphere.pos) <= std::pow(sphere.radius, 2)) {
        overlappingPositions.push_back(position);
    }
}

void PositionFilterer::WaitForInput() {
    Log("Press 'Enter' to run the calculations again...");
    Log("Press 'Esc' to exit the application...");

    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE)) {
            isRunning = false;
            return;
        }
        if (GetAsyncKeyState(VK_RETURN)) {
            return;
        }
    }
}
