#pragma once

#include <string>

// CSV 저장 책임만 가진 클래스입니다.
//
// Runner가 직접 파일 경로 생성, 디렉터리 생성, 파일 append 규칙을 알게 되면
// 실행 흐름과 저장 방식이 다시 섞입니다. 이 클래스로 분리해서 Runner는
// "기록한다"는 사실만 알고, 실제 저장 방식은 CsvResultWriter가 담당하게 합니다.
class CsvResultWriter {
public:
    // 실행 결과를 저장할 timestamp 기반 출력 폴더를 생성하고 보관합니다.
    CsvResultWriter();

    // 실행 시작 시 생성된 timestamp 기반 결과 폴더 경로를 반환합니다.
    const std::string& OutputDirectory() const;

    // 테스트 결과를 해당 테스트의 CSV 파일에 append합니다.
    // 파일이 비어 있으면 제목 줄과 UTF-8 BOM을 먼저 기록합니다.
    void Write(const std::string& fileName, const std::string& title, const std::string& result, double durationMs) const;

private:
    std::string outputDirectory_;
};
