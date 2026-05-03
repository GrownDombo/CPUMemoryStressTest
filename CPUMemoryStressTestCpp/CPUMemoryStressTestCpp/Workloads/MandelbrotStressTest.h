#pragma once

#include "../Interfaces/IStressTest.h"

// Mandelbrot 반복 계산으로 부동소수점 연산 부하를 만드는 테스트입니다.
class MandelbrotStressTest final : public IStressTest {
public:
    // 단일 계산 또는 x축 범위 분할 병렬 계산 중 하나를 선택합니다.
    explicit MandelbrotStressTest(ExecutionMode mode);

    // 실행 모드에 맞는 CSV 파일명을 반환합니다.
    std::string FileName() const override;

    // 실행 모드에 맞는 콘솔/CSV 표시 제목을 반환합니다.
    std::string Title() const override;

    // Mandelbrot 영역을 계산하고 해상도/반복 횟수 요약 문자열을 반환합니다.
    std::string Run() override;

private:
    ExecutionMode mode_;
};
