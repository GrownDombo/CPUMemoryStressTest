#include "ArrayMathStressTest.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <format>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {
    volatile double ArrayMathSink = 0.0;
}

ArrayMathStressTest::ArrayMathStressTest(ExecutionMode mode)
    : mode_(mode)
{
}

std::string ArrayMathStressTest::Id() const
{
    return mode_ == ExecutionMode::Single ? "cpu.array.single" : "cpu.array.parallel";
}

std::string ArrayMathStressTest::FileName() const
{
    return mode_ == ExecutionMode::Single ? "SingleArrayMath" : "ParallelArrayMath";
}

std::string ArrayMathStressTest::Title() const
{
    return mode_ == ExecutionMode::Single
        ? "[단일 처리] 배열 수학 계산 (Math.Pow, Math.Sin)"
        : "[병렬 처리] [같은 작업 동시 실행형] 배열 수학 계산 (Math.Pow, Math.Sin)";
}

TestResult ArrayMathStressTest::Run(const PresetConfig& config)
{
    const int length = std::max(1, config.arrayLength);

    if (mode_ == ExecutionMode::Single) {
        std::vector<double> array(length);
        double checksum = 0.0;
        for (int i = 0; i < length; ++i) {
            array[i] = std::pow(i, 0.5) * std::sin(i);
            checksum += array[i];
        }

        ArrayMathSink = checksum;

        TestResult result;
        result.testId = Id();
        result.title = Title();
        result.summary = std::format("배열 길이 : {}", length);
        result.metrics["arrayLength"] = std::to_string(length);
        result.metrics["workerCount"] = "1";
        return result;
    }

    const int processorCount = std::max(1, config.workerCount);
    std::vector<double> checksums(processorCount);
    std::vector<std::thread> threads;
    threads.reserve(processorCount);
    std::mutex errorMutex;
    std::string threadError;

    for (int i = 0; i < processorCount; ++i) {
        threads.emplace_back([length, i, &checksums, &errorMutex, &threadError]() {
            try {
                std::vector<double> array(length);
                double checksum = 0.0;
                for (int j = 0; j < length; ++j) {
                    array[j] = std::pow(j, 0.5) * std::sin(j);
                    checksum += array[j];
                }

                checksums[i] = checksum;
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
                    threadError = "unknown parallel array worker failure";
                }
            }
        });
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    if (!threadError.empty()) {
        throw std::runtime_error("parallel array stress failed: " + threadError);
    }

    double checksum = 0.0;
    for (const double value : checksums) {
        checksum += value;
    }

    ArrayMathSink = checksum;

    TestResult result;
    result.testId = Id();
    result.title = Title();
    result.summary = std::format("배열 길이 : {}, 프로세서 개수 : {}", length, processorCount);
    result.metrics["arrayLength"] = std::to_string(length);
    result.metrics["workerCount"] = std::to_string(processorCount);
    return result;
}
