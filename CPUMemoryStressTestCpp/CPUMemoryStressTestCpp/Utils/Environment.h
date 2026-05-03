#pragma once

#include <string>

namespace Environment {
    // 환경 변수 값을 문자열로 읽습니다.
    // 변수가 없거나 읽기에 실패하면 빈 문자열을 반환합니다.
    std::string GetEnv(const char* name);

    // 바탕화면 아래 StressTestResult/yyyyMMdd_HHmmss 폴더를 만들고 전체 경로를 반환합니다.
    // CSV 출력 폴더를 실행마다 분리하기 위해 CsvResultWriter 생성 시 사용됩니다.
    std::string CreateTimestampedResultDirectory();
}
