#pragma once

#include "../Interfaces/IStressTest.h"

// 대용량 랜덤 배열 정렬로 CPU와 메모리 접근 부하를 만드는 테스트입니다.
class SortStressTest final : public IStressTest {
public:
    explicit SortStressTest(ExecutionMode mode);

    std::string Id() const override;
    std::string FileName() const override;
    std::string Title() const override;
    TestResult Run(const PresetConfig& config) override;

private:
    ExecutionMode mode_;
};
