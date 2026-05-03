#pragma once

#include "../Interfaces/IStressTest.h"

// 재귀 피보나치 계산으로 CPU 연산 부하를 만드는 테스트입니다.
class FibonacciStressTest final : public IStressTest {
public:
    // 단일 계산 또는 프로세서 수만큼 같은 재귀 계산을 병렬 실행할지 결정합니다.
    explicit FibonacciStressTest(ExecutionMode mode);

    // 실행 모드에 맞는 CSV 파일명을 반환합니다.
    std::string FileName() const override;

    // 실행 모드에 맞는 콘솔/CSV 표시 제목을 반환합니다.
    std::string Title() const override;

    // 지정된 깊이의 재귀 피보나치를 계산하고 결과 요약 문자열을 반환합니다.
    std::string Run() override;

private:
    ExecutionMode mode_;
};
