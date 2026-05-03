#pragma once

#include "../Interfaces/IStressTest.h"

// 실제 메모리를 일정량 할당하고 페이지를 터치해 메모리 부하를 만드는 테스트입니다.
class MemoryStressTest final : public IStressTest {
public:
    std::string Id() const override;
    std::string FileName() const override;
    std::string Title() const override;
    TestResult Run(const PresetConfig& config) override;
};
