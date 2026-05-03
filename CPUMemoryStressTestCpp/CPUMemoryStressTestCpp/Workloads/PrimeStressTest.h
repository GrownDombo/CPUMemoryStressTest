#pragma once

#include "../Interfaces/IStressTest.h"

// 지정된 범위에서 소수를 찾아 CPU 분기/반복 연산 부하를 만드는 테스트입니다.
class PrimeStressTest final : public IStressTest {
public:
    // 단일 범위 순회 또는 범위 분할 병렬 순회 중 어떤 방식으로 실행할지 결정합니다.
    explicit PrimeStressTest(ExecutionMode mode);

    // 실행 모드에 맞는 CSV 파일명을 반환합니다.
    std::string FileName() const override;

    // 실행 모드에 맞는 콘솔/CSV 표시 제목을 반환합니다.
    std::string Title() const override;

    // 소수 개수를 계산하고 검색 범위/개수 요약 문자열을 반환합니다.
    std::string Run() override;

private:
    ExecutionMode mode_;
};
