#pragma once

#include "../Interfaces/IStressTest.h"

// Mandelbrot 반복 계산으로 부동소수점 연산 부하를 만드는 테스트입니다.
class MandelbrotStressTest final : public IStressTest {
public:
    explicit MandelbrotStressTest(ExecutionMode mode);

    std::string Id() const override;
    std::string FileName() const override;
    std::string Title() const override;
    TestResult Run(const PresetConfig& config) override;

private:
    ExecutionMode mode_;
};
