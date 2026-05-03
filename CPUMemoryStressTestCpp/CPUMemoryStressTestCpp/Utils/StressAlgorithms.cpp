#include "StressAlgorithms.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <random>
#include <thread>

namespace {
    volatile std::uint64_t MandelbrotSink = 0;
}

namespace StressAlgorithms {
    int WorkerCount()
    {
        return static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));
    }

    std::vector<int> GenerateRandomVector(int length)
    {
        std::vector<int> data(length);
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, INT32_MAX);

        for (int& value : data) {
            value = dist(rng);
        }

        return data;
    }

    double FibonacciRecursive(int n)
    {
        if (n <= 1) {
            return n;
        }

        return FibonacciRecursive(n - 1) + FibonacciRecursive(n - 2);
    }

    bool IsPrime(int number)
    {
        if (number < 2) {
            return false;
        }

        if (number == 2) {
            return true;
        }

        if (number % 2 == 0) {
            return false;
        }

        const int boundary = static_cast<int>(std::sqrt(number));
        for (int i = 3; i <= boundary; i += 2) {
            if (number % i == 0) {
                return false;
            }
        }

        return true;
    }

    std::vector<int> MergeSortedChunks(const std::vector<std::vector<int>>& chunks)
    {
        if (chunks.empty()) {
            return {};
        }

        std::vector<int> merged = chunks.front();
        for (std::size_t i = 1; i < chunks.size(); ++i) {
            std::vector<int> next;
            next.reserve(merged.size() + chunks[i].size());
            std::merge(merged.begin(), merged.end(), chunks[i].begin(), chunks[i].end(), std::back_inserter(next));
            merged = std::move(next);
        }

        return merged;
    }

    void CalculateMandelbrot(int width, int height, int maxIterations, bool parallel, int requestedWorkerCount)
    {
        const double xmin = -2.1;
        const double xmax = 1.0;
        const double ymin = -1.3;
        const double ymax = 1.3;
        const double xstep = (xmax - xmin) / width;
        const double ystep = (ymax - ymin) / height;

        auto processRange = [&](int startX, int endX) {
            std::uint64_t localIterations = 0;

            for (int px = startX; px < endX; ++px) {
                for (int py = 0; py < height; ++py) {
                    const double x = xmin + px * xstep;
                    const double y = ymin + py * ystep;
                    double zx = 0;
                    double zy = 0;

                    int iterations = 0;
                    for (; zx * zx + zy * zy <= 4.0 && iterations < maxIterations; ++iterations) {
                        const double temp = zx * zx - zy * zy + x;
                        zy = 2.0 * zx * zy + y;
                        zx = temp;
                    }

                    localIterations += static_cast<std::uint64_t>(iterations);
                }
            }

            return localIterations;
        };

        if (!parallel) {
            MandelbrotSink = processRange(0, width);
            return;
        }

        const int workerCount = std::min(std::max(1, requestedWorkerCount), width);
        const int chunkSize = width / workerCount;
        std::vector<std::uint64_t> results(workerCount);
        std::vector<std::thread> threads;
        threads.reserve(workerCount);

        for (int i = 0; i < workerCount; ++i) {
            const int startX = i * chunkSize;
            const int endX = (i == workerCount - 1) ? width : startX + chunkSize;
            threads.emplace_back([&, i, startX, endX]() {
                results[i] = processRange(startX, endX);
            });
        }

        for (std::thread& thread : threads) {
            thread.join();
        }

        std::uint64_t totalIterations = 0;
        for (const std::uint64_t result : results) {
            totalIterations += result;
        }

        MandelbrotSink = totalIterations;
    }
}
