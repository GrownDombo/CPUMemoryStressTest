#include "StressTestRunner.h"

#include "../Logging/CsvResultWriter.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <windows.h>

StressTestRunner::StressTestRunner(const CsvResultWriter& writer)
    : writer_(writer)
{
}

void StressTestRunner::Run(IStressTest& test) const
{
    const std::string title = test.Title();

    // Runner는 구체 테스트 타입을 모릅니다. IStressTest 인터페이스만 호출합니다.
    // 이 덕분에 Concrete Strategy가 늘어나도 측정/로그/대기 흐름은 한 곳에서 유지됩니다.
    std::cout << "[시작] " << title << std::endl;
    const auto start = std::chrono::high_resolution_clock::now();
    const std::string result = test.Run();
    const auto end = std::chrono::high_resolution_clock::now();
    const double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    // 테스트가 반환한 결과 요약과 Runner가 측정한 실행 시간을 합쳐 기록합니다.
    writer_.Write(test.FileName(), title, result, elapsed);
    std::cout << "[정보] " << result << std::endl;
    std::cout << "[결과] 소요 시간 : " << std::fixed << std::setprecision(2) << elapsed << " ms" << std::endl;
    std::cout << "[종료] " << title << std::endl << std::endl;

    PauseAndCollectMemory();
}

void StressTestRunner::RunCycle(const std::vector<std::unique_ptr<IStressTest>>& tests) const
{
    // 테스트 등록 순서를 그대로 실행 순서로 사용합니다.
    // 순서를 바꾸고 싶으면 Runner가 아니라 main.cpp의 CreateStressTests()만 수정하면 됩니다.
    for (const auto& test : tests) {
        Run(*test);
    }
}

void StressTestRunner::PauseAndCollectMemory() const
{
    std::cout << "[대기] 5초간 대기 중..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    HANDLE process = GetCurrentProcess();
    SetProcessWorkingSetSize(process, static_cast<SIZE_T>(-1), static_cast<SIZE_T>(-1));

    std::cout << "[완료] 대기 및 메모리 정리 완료" << std::endl << std::endl;
}
