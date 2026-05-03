#pragma once

#include "../Interfaces/IStressTest.h"
#include "../Results/PresetConfig.h"
#include "../Results/TestResult.h"

#include <memory>
#include <vector>

// Strategy들을 실행하는 Context/Runner 역할입니다.
//
// Runner는 테스트 실행, 시간 측정, 예외 처리를 담당하고 결과 DTO를 반환합니다.
// CSV/JSON/콘솔 출력은 호출자가 선택해서 처리합니다.
class StressTestRunner {
public:
    TestResult Run(IStressTest& test, const PresetConfig& config) const;
    std::vector<TestResult> RunCycle(const std::vector<std::unique_ptr<IStressTest>>& tests, const PresetConfig& config) const;

    // 테스트 간 CPU/메모리 상태를 조금 가라앉히기 위한 공통 후처리입니다.
    void PauseAndCollectMemory() const;
};
