#pragma once

#include "../Results/PresetConfig.h"
#include "../Results/TestResult.h"

#include <string>

// 단일/병렬처럼 "같은 종류의 부하 작업을 어떤 방식으로 실행할지"를 나타냅니다.
// 테스트 클래스는 이 값만 바꿔서 CSV 이름, 제목, 실행 방식을 다르게 구성할 수 있습니다.
enum class ExecutionMode {
    Single,
    Parallel
};

// Strategy 패턴의 공통 인터페이스입니다.
//
// 이 프로젝트에서 "전략"은 스트레스 테스트 하나입니다.
// 배열 수학, 피보나치, 소수 찾기, 정렬, Mandelbrot, 메모리 테스트가 모두
// IStressTest를 구현하면 Runner는 구체 클래스가 무엇인지 몰라도 동일한 흐름으로 실행할 수 있습니다.
class IStressTest {
public:
    virtual ~IStressTest() = default;

    // CLI에서 테스트를 선택할 때 사용하는 안정적인 영문 ID입니다.
    virtual std::string Id() const = 0;

    // CSV 파일명으로 사용됩니다. 예: SinglePrime -> SinglePrime.csv
    virtual std::string FileName() const = 0;

    // 콘솔 출력과 CSV 첫 줄 제목으로 사용됩니다.
    virtual std::string Title() const = 0;

    // 실제 스트레스 작업을 수행하고 구조화된 결과를 반환합니다.
    // 실행 시간 측정, 예외 처리, CSV/JSON 출력은 외부 계층이 담당합니다.
    virtual TestResult Run(const PresetConfig& config) = 0;
};
