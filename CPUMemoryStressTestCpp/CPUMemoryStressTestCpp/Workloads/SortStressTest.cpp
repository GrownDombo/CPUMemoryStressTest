#include "SortStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <algorithm>
#include <exception>
#include <format>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

SortStressTest::SortStressTest(ExecutionMode mode)
    : mode_(mode)
{
}

std::string SortStressTest::Id() const
{
    return mode_ == ExecutionMode::Single ? "cpu.sort.single" : "cpu.sort.parallel";
}

std::string SortStressTest::FileName() const
{
    return mode_ == ExecutionMode::Single ? "SingleSort" : "ParallelSort";
}

std::string SortStressTest::Title() const
{
    return mode_ == ExecutionMode::Single
        ? "[단일 처리] 배열 정렬"
        : "[병렬 처리] [작업 분담형] 배열 정렬 및 병합";
}

TestResult SortStressTest::Run(const PresetConfig& config)
{
    const int length = std::max(1, config.arrayLength);
    std::vector<int> data = StressAlgorithms::GenerateRandomVector(length);

    if (mode_ == ExecutionMode::Single) {
        std::sort(data.begin(), data.end());

        TestResult result;
        result.testId = Id();
        result.title = Title();
        result.summary = std::format("배열 길이 : {}", length);
        result.metrics["arrayLength"] = std::to_string(length);
        result.metrics["workerCount"] = "1";
        return result;
    }

    const int processorCount = std::min(std::max(1, config.workerCount), length);
    const int chunkSize = std::max(1, length / processorCount);
    std::vector<std::vector<int>> sortedChunks(processorCount);
    std::vector<std::thread> threads;
    threads.reserve(processorCount);
    std::mutex errorMutex;
    std::string threadError;

    for (int i = 0; i < processorCount; ++i) {
        threads.emplace_back([&, i]() {
            try {
                const int start = i * chunkSize;
                const int end = (i == processorCount - 1) ? length : std::min(length, start + chunkSize);
                auto first = data.begin() + start;
                auto last = data.begin() + end;

                sortedChunks[i].assign(first, last);
                std::sort(sortedChunks[i].begin(), sortedChunks[i].end());
            }
            catch (const std::exception& ex) {
                std::lock_guard<std::mutex> lock(errorMutex);
                if (threadError.empty()) {
                    threadError = ex.what();
                }
            }
            catch (...) {
                std::lock_guard<std::mutex> lock(errorMutex);
                if (threadError.empty()) {
                    threadError = "unknown parallel sort worker failure";
                }
            }
        });
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    if (!threadError.empty()) {
        throw std::runtime_error("parallel sort stress failed: " + threadError);
    }

    std::vector<int> merged = StressAlgorithms::MergeSortedChunks(sortedChunks);

    TestResult result;
    result.testId = Id();
    result.title = Title();
    result.summary = std::format("배열 길이 : {}", static_cast<int>(merged.size()));
    result.metrics["arrayLength"] = std::to_string(static_cast<int>(merged.size()));
    result.metrics["workerCount"] = std::to_string(processorCount);
    return result;
}
