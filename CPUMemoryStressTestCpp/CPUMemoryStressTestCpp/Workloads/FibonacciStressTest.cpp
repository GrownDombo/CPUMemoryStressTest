#include "FibonacciStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <algorithm>
#include <format>
#include <thread>
#include <vector>

FibonacciStressTest::FibonacciStressTest(ExecutionMode mode)
    : mode_(mode)
{
}

std::string FibonacciStressTest::Id() const
{
    return mode_ == ExecutionMode::Single ? "cpu.fibonacci.single" : "cpu.fibonacci.parallel";
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

TestResult FibonacciStressTest::Run(const PresetConfig& config)
{
    const int depth = std::max(1, config.fibonacciDepth);

    if (mode_ == ExecutionMode::Single) {
        const volatile double value = StressAlgorithms::FibonacciRecursive(depth);
        (void)value;

        TestResult result;
        result.testId = Id();
        result.title = Title();
        result.summary = std::format("피보나치 수열 깊이 : {}", depth);
        result.metrics["depth"] = std::to_string(depth);
        result.metrics["workerCount"] = "1";
        return result;
    }

    const int processorCount = std::max(1, config.workerCount);
    std::vector<std::thread> threads(processorCount);
    std::vector<double> results(processorCount);

    for (int i = 0; i < processorCount; ++i) {
        threads[i] = std::thread([i, depth, &results]() {
            results[i] = StressAlgorithms::FibonacciRecursive(depth);
        });
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    TestResult result;
    result.testId = Id();
    result.title = Title();
    result.summary = std::format("피보나치 수열 깊이 : {}, 프로세서 개수 : {}", depth, processorCount);
    result.metrics["depth"] = std::to_string(depth);
    result.metrics["workerCount"] = std::to_string(processorCount);
    return result;
}
