#include "SortStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <algorithm>
#include <format>
#include <thread>
#include <vector>

SortStressTest::SortStressTest(ExecutionMode mode)
    : mode_(mode)
{
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

std::string SortStressTest::Run()
{
    const int length = std::min(StressAlgorithms::MaxArray<int>(), 10000000);
    std::vector<int> data = StressAlgorithms::GenerateRandomVector(length);

    if (mode_ == ExecutionMode::Single) {
        std::sort(data.begin(), data.end());
        return std::format("배열 길이 : {}", length);
    }

    const int processorCount = StressAlgorithms::WorkerCount();
    const int chunkSize = length / processorCount;
    std::vector<std::vector<int>> sortedChunks(processorCount);
    std::vector<std::thread> threads;
    threads.reserve(processorCount);

    for (int i = 0; i < processorCount; ++i) {
        threads.emplace_back([&, i]() {
            const int start = i * chunkSize;
            const int end = (i == processorCount - 1) ? length : start + chunkSize;
            auto first = data.begin() + start;
            auto last = data.begin() + end;

            sortedChunks[i].assign(first, last);
            std::sort(sortedChunks[i].begin(), sortedChunks[i].end());
        });
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    std::vector<int> merged = StressAlgorithms::MergeSortedChunks(sortedChunks);
    return std::format("배열 길이 : {}", static_cast<int>(merged.size()));
}
