#pragma once

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
//
// 장점:
// - 새 테스트를 추가할 때 Runner/CSV/측정 코드를 수정하지 않아도 됩니다.
// - 각 테스트 클래스는 "부하를 어떻게 만들지"에만 집중합니다.
// - Runner는 "언제 측정하고, 언제 로그를 남기고, 언제 대기할지"만 책임집니다.
class IStressTest {
public:
    // 인터페이스 포인터로 파생 테스트 객체를 안전하게 삭제할 수 있도록 virtual 소멸자를 둡니다.
    virtual ~IStressTest() = default;

    // CSV 파일명으로 사용됩니다. 예: SinglePrime -> SinglePrime.csv
    virtual std::string FileName() const = 0;

    // 콘솔 출력과 CSV 첫 줄 제목으로 사용됩니다.
    virtual std::string Title() const = 0;

    // 실제 스트레스 작업을 수행하고, 사람이 읽을 수 있는 결과 요약을 반환합니다.
    // 실행 시간 측정과 CSV 기록은 Runner가 담당하므로 여기서는 부하 로직만 구현합니다.
    virtual std::string Run() = 0;
};
