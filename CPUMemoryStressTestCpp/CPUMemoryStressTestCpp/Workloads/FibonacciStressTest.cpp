#include "FibonacciStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <format>
#include <thread>
#include <vector>

FibonacciStressTest::FibonacciStressTest(ExecutionMode mode)
    : mode_(mode)
{
}

std::string FibonacciStressTest::FileName() const
{
    return mode_ == ExecutionMode::Single ? "SingleRecursive" : "ParallelRecursive";
}

std::string FibonacciStressTest::Title() const
{
    return mode_ == ExecutionMode::Single
        ? "[단일 처리] 재귀 피보나치"
        : "[병렬 처리] [같은 작업 동시 실행형] 재귀함수 피보나치 계산";
}

std::string FibonacciStressTest::Run()
{
    if (mode_ == ExecutionMode::Single) {
        const volatile double value = StressAlgorithms::FibonacciRecursive(StressAlgorithms::FibonacciDepth);
        (void)value;
        return std::format("피보나치 수열 깊이 : {}", StressAlgorithms::FibonacciDepth);
    }

    const int processorCount = StressAlgorithms::WorkerCount();
    std::vector<std::thread> threads(processorCount);
    std::vector<double> results(processorCount);

    for (int i = 0; i < processorCount; ++i) {
        threads[i] = std::thread([i, &results]() {
            results[i] = StressAlgorithms::FibonacciRecursive(StressAlgorithms::FibonacciDepth);
        });
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    return std::format("피보나치 수열 깊이 : {}, 프로세서 개수 : {}", StressAlgorithms::FibonacciDepth, processorCount);
}
