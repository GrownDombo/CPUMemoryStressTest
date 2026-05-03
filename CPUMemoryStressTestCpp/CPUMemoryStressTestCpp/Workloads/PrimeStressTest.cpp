#include "PrimeStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <format>
#include <mutex>
#include <thread>
#include <vector>

PrimeStressTest::PrimeStressTest(ExecutionMode mode)
    : mode_(mode)
{
}

std::string PrimeStressTest::FileName() const
{
    return mode_ == ExecutionMode::Single ? "SinglePrime" : "ParallelPrime";
}

std::string PrimeStressTest::Title() const
{
    return mode_ == ExecutionMode::Single
        ? "[단일 처리] 소수 찾기"
        : "[병렬 처리] [작업 분담형] 소수 찾기";
}

std::string PrimeStressTest::Run()
{
    if (mode_ == ExecutionMode::Single) {
        int count = 0;
        for (int i = 2; i < StressAlgorithms::PrimeStressRange; ++i) {
            if (StressAlgorithms::IsPrime(i)) {
                ++count;
            }
        }

        return std::format("소수점 검색 범위 : {}, 소수점 개수 : {}", StressAlgorithms::PrimeStressRange, count);
    }

    const int processorCount = StressAlgorithms::WorkerCount();
    int totalCount = 0;
    std::mutex countMutex;
    std::vector<std::thread> threads;
    threads.reserve(processorCount);

    const int chunkSize = StressAlgorithms::PrimeStressRange / processorCount;
    for (int i = 0; i < processorCount; ++i) {
        const int start = 2 + i * chunkSize;
        const int end = (i == processorCount - 1) ? StressAlgorithms::PrimeStressRange : start + chunkSize;

        threads.emplace_back([start, end, &totalCount, &countMutex]() {
            int localCount = 0;
            for (int number = start; number < end; ++number) {
                if (StressAlgorithms::IsPrime(number)) {
                    ++localCount;
                }
            }

            std::lock_guard<std::mutex> lock(countMutex);
            totalCount += localCount;
        });
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    return std::format("소수점 검색 범위 : {}, 소수점 개수 : {}", StressAlgorithms::PrimeStressRange, totalCount);
}
