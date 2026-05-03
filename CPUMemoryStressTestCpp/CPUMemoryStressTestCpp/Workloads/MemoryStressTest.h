#pragma once

#include "../Interfaces/IStressTest.h"

// 실제 메모리를 일정 비율까지 할당하고 페이지를 터치해 메모리 부하를 만드는 테스트입니다.
class MemoryStressTest final : public IStressTest {
public:
    // 메모리 테스트 CSV 파일명을 반환합니다.
    std::string FileName() const override;

    // 메모리 테스트의 콘솔/CSV 표시 제목을 반환합니다.
    std::string Title() const override;

    // 물리 메모리 정보를 조회하고 목표량까지 할당/접근한 뒤 결과 요약 문자열을 반환합니다.
    std::string Run() override;
};
