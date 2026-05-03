#pragma once

#include "../Interfaces/IStressTest.h"

#include <memory>
#include <vector>

class CsvResultWriter;

// Strategy들을 실행하는 Context/Runner 역할입니다.
//
// IStressTest 구현체는 서로 다른 알고리즘을 갖지만,
// "시작 출력 -> 시간 측정 -> 테스트 실행 -> CSV 기록 -> 종료 출력 -> 대기/메모리 정리"
// 흐름은 모든 테스트에서 같습니다. 이 반복되는 실행 절차를 Runner 한 곳에 모아
// 각 테스트 클래스가 공통 코드 없이 자신의 부하 작업만 구현하게 합니다.
//
// 새 테스트를 추가하는 절차:
// 1. IStressTest를 구현하는 Concrete Strategy 클래스를 만듭니다.
// 2. main.cpp의 CreateStressTests()에 등록합니다.
// 3. Runner는 수정하지 않습니다.
class StressTestRunner {
public:
    // CSV 기록기를 주입받아 Runner가 저장 방식에 직접 의존하지 않게 합니다.
    explicit StressTestRunner(const CsvResultWriter& writer);

    // 테스트 하나를 공통 실행 파이프라인으로 실행합니다.
    void Run(IStressTest& test) const;

    // 등록된 테스트 목록을 순서대로 실행합니다. vector에 넣은 순서가 실행 순서입니다.
    void RunCycle(const std::vector<std::unique_ptr<IStressTest>>& tests) const;

private:
    // 테스트 간 CPU/메모리 상태를 조금 가라앉히기 위한 공통 후처리입니다.
    void PauseAndCollectMemory() const;

    const CsvResultWriter& writer_;
};
