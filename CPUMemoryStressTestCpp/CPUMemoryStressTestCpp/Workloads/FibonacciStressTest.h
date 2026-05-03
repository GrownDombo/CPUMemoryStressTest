#pragma once

#include "../Interfaces/IStressTest.h"

// 재귀 피보나치 계산으로 CPU 연산 부하를 만드는 테스트입니다.
class FibonacciStressTest final : public IStressTest {
public:
    explicit FibonacciStressTest(ExecutionMode mode);

    std::string Id() const override;
    std::string FileName() const override;
    std::string Title() const override;
    TestResult Run(const PresetConfig& config) override;

private:
    ExecutionMode mode_;
};
