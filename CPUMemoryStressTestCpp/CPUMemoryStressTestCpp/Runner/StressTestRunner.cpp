#include "StressTestRunner.h"

#include <chrono>
#include <exception>
#include <thread>
#include <windows.h>

TestResult StressTestRunner::Run(IStressTest& test, const PresetConfig& config) const
{
    TestResult result;
    result.testId = test.Id();
    result.title = test.Title();

    const auto start = std::chrono::high_resolution_clock::now();

    try {
        result = test.Run(config);
        result.testId = test.Id();
        result.title = test.Title();
        result.success = true;
    }
    catch (const std::exception& ex) {
        result.success = false;
        result.summary = "test failed";
        result.errorMessage = ex.what();
    }
    catch (...) {
        result.success = false;
        result.summary = "test failed";
        result.errorMessage = "Unknown error.";
    }

    const auto end = std::chrono::high_resolution_clock::now();
    result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
    return result;
}

std::vector<TestResult> StressTestRunner::RunCycle(const std::vector<std::unique_ptr<IStressTest>>& tests, const PresetConfig& config) const
{
    std::vector<TestResult> results;
    results.reserve(tests.size());

    for (const auto& test : tests) {
        results.push_back(Run(*test, config));
    }

    return results;
}

void StressTestRunner::PauseAndCollectMemory() const
{
    std::this_thread::sleep_for(std::chrono::seconds(5));

    HANDLE process = GetCurrentProcess();
    SetProcessWorkingSetSize(process, static_cast<SIZE_T>(-1), static_cast<SIZE_T>(-1));
}
