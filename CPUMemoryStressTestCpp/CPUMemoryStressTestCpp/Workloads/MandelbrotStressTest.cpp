#include "MandelbrotStressTest.h"

#include "../Utils/StressAlgorithms.h"

#include <algorithm>
#include <format>

MandelbrotStressTest::MandelbrotStressTest(ExecutionMode mode)
    : mode_(mode)
{
}

std::string MandelbrotStressTest::Id() const
{
    return mode_ == ExecutionMode::Single ? "cpu.mandelbrot.single" : "cpu.mandelbrot.parallel";
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

TestResult MandelbrotStressTest::Run(const PresetConfig& config)
{
    const int width = std::max(1, config.mandelbrotWidth);
    const int height = std::max(1, config.mandelbrotHeight);
    const int iterations = std::max(1, config.mandelbrotIterations);
    const bool parallel = mode_ == ExecutionMode::Parallel;
    const int workerCount = parallel ? std::max(1, config.workerCount) : 1;

    StressAlgorithms::CalculateMandelbrot(width, height, iterations, parallel, workerCount);

    TestResult result;
    result.testId = Id();
    result.title = Title();
    result.summary = std::format("{}x{}, Iterations: {}", width, height, iterations);
    result.metrics["width"] = std::to_string(width);
    result.metrics["height"] = std::to_string(height);
    result.metrics["iterations"] = std::to_string(iterations);
    result.metrics["workerCount"] = std::to_string(workerCount);
    return result;
}
