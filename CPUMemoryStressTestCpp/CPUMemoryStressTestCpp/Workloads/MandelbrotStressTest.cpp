#include "MandelbrotStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <format>

MandelbrotStressTest::MandelbrotStressTest(ExecutionMode mode)
    : mode_(mode)
{
}

std::string MandelbrotStressTest::FileName() const
{
    return mode_ == ExecutionMode::Single ? "SingleMandelbrot" : "ParallelMandelbrot";
}

std::string MandelbrotStressTest::Title() const
{
    return mode_ == ExecutionMode::Single
        ? "[단일 처리] Mandelbrot 계산"
        : "[병렬 처리] [작업 분담형] Mandelbrot 계산";
}

std::string MandelbrotStressTest::Run()
{
    if (mode_ == ExecutionMode::Single) {
        StressAlgorithms::CalculateMandelbrot(
            StressAlgorithms::MandelbrotWidthSingle,
            StressAlgorithms::MandelbrotHeightSingle,
            StressAlgorithms::MandelbrotMaxIterationsSingle,
            false);

        return std::format(
            "{}x{}, Iterations: {}",
            StressAlgorithms::MandelbrotWidthSingle,
            StressAlgorithms::MandelbrotHeightSingle,
            StressAlgorithms::MandelbrotMaxIterationsSingle);
    }

    StressAlgorithms::CalculateMandelbrot(
        StressAlgorithms::MandelbrotWidthParallel,
        StressAlgorithms::MandelbrotHeightParallel,
        StressAlgorithms::MandelbrotMaxIterationsParallel,
        true);

    return std::format(
        "{}x{}, Iterations: {}",
        StressAlgorithms::MandelbrotWidthParallel,
        StressAlgorithms::MandelbrotHeightParallel,
        StressAlgorithms::MandelbrotMaxIterationsParallel);
}
