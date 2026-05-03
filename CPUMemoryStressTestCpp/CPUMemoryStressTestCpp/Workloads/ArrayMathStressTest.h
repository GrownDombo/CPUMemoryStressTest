#pragma once

#include "../Interfaces/IStressTest.h"

// sqrt/sin 기반 대용량 배열 계산으로 CPU와 메모리 대역폭 부하를 만드는 테스트입니다.
class ArrayMathStressTest final : public IStressTest {
public:
    // 단일 실행 또는 프로세서 수만큼 같은 작업을 병렬 반복할지 결정합니다.
    explicit ArrayMathStressTest(ExecutionMode mode);

    // 실행 모드에 맞는 CSV 파일명을 반환합니다.
    std::string FileName() const override;

    // 실행 모드에 맞는 콘솔/CSV 표시 제목을 반환합니다.
    std::string Title() const override;

    // 배열을 할당하고 sqrt/sin 계산을 채워 넣은 뒤 결과 요약 문자열을 반환합니다.
    std::string Run() override;

private:
    ExecutionMode mode_;
};
