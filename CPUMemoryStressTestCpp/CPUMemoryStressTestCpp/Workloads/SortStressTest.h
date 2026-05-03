#pragma once

#include "../Interfaces/IStressTest.h"

// 대용량 랜덤 배열 정렬로 CPU와 메모리 접근 부하를 만드는 테스트입니다.
class SortStressTest final : public IStressTest {
public:
    // 단일 정렬 또는 청크 분할 병렬 정렬/병합 방식 중 하나를 선택합니다.
    explicit SortStressTest(ExecutionMode mode);

    // 실행 모드에 맞는 CSV 파일명을 반환합니다.
    std::string FileName() const override;

    // 실행 모드에 맞는 콘솔/CSV 표시 제목을 반환합니다.
    std::string Title() const override;

    // 랜덤 데이터를 생성해 정렬하고 배열 길이 요약 문자열을 반환합니다.
    std::string Run() override;

private:
    ExecutionMode mode_;
};
