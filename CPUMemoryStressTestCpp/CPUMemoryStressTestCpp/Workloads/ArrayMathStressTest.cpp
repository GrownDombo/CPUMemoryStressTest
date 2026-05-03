#include "ArrayMathStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <cmath>
#include <format>
#include <thread>
#include <vector>

namespace {
    volatile double ArrayMathSink = 0.0;
}

ArrayMathStressTest::ArrayMathStressTest(ExecutionMode mode)
    : mode_(mode)
{
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

std::string ArrayMathStressTest::Run()
{
    const int length = StressAlgorithms::MaxArray<double>();

    if (mode_ == ExecutionMode::Single) {
        std::vector<double> array(length);
        double checksum = 0.0;
        for (int i = 0; i < length; ++i) {
            array[i] = std::pow(i, 0.5) * std::sin(i);
            checksum += array[i];
        }

        ArrayMathSink = checksum;

        return std::format("배열 길이 : {}", length);
    }

    const int processorCount = StressAlgorithms::WorkerCount();
    std::vector<double> checksums(processorCount);
    std::vector<std::thread> threads;
    threads.reserve(processorCount);

    for (int i = 0; i < processorCount; ++i) {
        threads.emplace_back([length, i, &checksums]() {
            std::vector<double> array(length);
            double checksum = 0.0;
            for (int j = 0; j < length; ++j) {
                array[j] = std::pow(j, 0.5) * std::sin(j);
                checksum += array[j];
            }

            checksums[i] = checksum;
        });
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    double checksum = 0.0;
    for (const double value : checksums) {
        checksum += value;
    }

    ArrayMathSink = checksum;

    return std::format("배열 길이 : {}, 프로세서 개수 : {}", length, processorCount);
}
