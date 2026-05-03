#pragma once

#include "../Interfaces/IStressTest.h"

// sqrt/sin 기반 배열 계산으로 CPU와 메모리 대역폭 부하를 만드는 테스트입니다.
class ArrayMathStressTest final : public IStressTest {
public:
    explicit ArrayMathStressTest(ExecutionMode mode);

    std::string Id() const override;
    std::string FileName() const override;
    std::string Title() const override;
    TestResult Run(const PresetConfig& config) override;

private:
    ExecutionMode mode_;
};
