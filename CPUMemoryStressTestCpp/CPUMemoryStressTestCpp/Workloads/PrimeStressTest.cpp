#include "PrimeStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <algorithm>
#include <format>
#include <mutex>
#include <thread>
#include <vector>

PrimeStressTest::PrimeStressTest(ExecutionMode mode)
    : mode_(mode)
{
}

std::string PrimeStressTest::Id() const
{
    return mode_ == ExecutionMode::Single ? "cpu.prime.single" : "cpu.prime.parallel";
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

TestResult PrimeStressTest::Run(const PresetConfig& config)
{
    const int primeRange = std::max(3, config.primeRange);

    if (mode_ == ExecutionMode::Single) {
        int count = 0;
        for (int i = 2; i < primeRange; ++i) {
            if (StressAlgorithms::IsPrime(i)) {
                ++count;
            }
        }

        TestResult result;
        result.testId = Id();
        result.title = Title();
        result.summary = std::format("소수점 검색 범위 : {}, 소수점 개수 : {}", primeRange, count);
        result.metrics["primeRange"] = std::to_string(primeRange);
        result.metrics["primeCount"] = std::to_string(count);
        result.metrics["workerCount"] = "1";
        return result;
    }

    const int processorCount = std::min(std::max(1, config.workerCount), std::max(1, primeRange - 2));
    int totalCount = 0;
    std::mutex countMutex;
    std::vector<std::thread> threads;
    threads.reserve(processorCount);

    const int chunkSize = std::max(1, (primeRange - 2) / processorCount);
    for (int i = 0; i < processorCount; ++i) {
        const int start = 2 + i * chunkSize;
        const int end = (i == processorCount - 1) ? primeRange : std::min(primeRange, start + chunkSize);

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

    TestResult result;
    result.testId = Id();
    result.title = Title();
    result.summary = std::format("소수점 검색 범위 : {}, 소수점 개수 : {}", primeRange, totalCount);
    result.metrics["primeRange"] = std::to_string(primeRange);
    result.metrics["primeCount"] = std::to_string(totalCount);
    result.metrics["workerCount"] = std::to_string(processorCount);
    return result;
}
