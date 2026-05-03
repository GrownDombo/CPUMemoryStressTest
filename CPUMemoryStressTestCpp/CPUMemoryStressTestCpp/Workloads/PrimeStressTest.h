#pragma once

#include "../Interfaces/IStressTest.h"

// 지정된 범위에서 소수를 찾아 CPU 분기/반복 연산 부하를 만드는 테스트입니다.
class PrimeStressTest final : public IStressTest {
public:
    explicit PrimeStressTest(ExecutionMode mode);

    std::string Id() const override;
    std::string FileName() const override;
    std::string Title() const override;
    TestResult Run(const PresetConfig& config) override;

private:
    ExecutionMode mode_;
};
