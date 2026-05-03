#include "Interfaces/IStressTest.h"
#include "Logging/CsvResultWriter.h"
#include "Runner/StressTestRunner.h"
#include "Utils/ConsoleEncoding.h"
#include "Workloads/ArrayMathStressTest.h"
#include "Workloads/FibonacciStressTest.h"
#include "Workloads/MandelbrotStressTest.h"
#include "Workloads/MemoryStressTest.h"
#include "Workloads/PrimeStressTest.h"
#include "Workloads/SortStressTest.h"

#include <iostream>
#include <memory>
#include <vector>

namespace {
    int PromptRepeatCount()
    {
        int count = 0;
        while (true) {
            std::cout << "스트레스 테스트 반복 횟수 설정 (1~10) : ";
            std::cin >> count;
            if (count >= 1 && count <= 10) {
                return count;
            }

            std::cout << "[오류] 잘못된 입력입니다. 1~10 사이 숫자를 입력하세요.\n" << std::endl;
        }
    }

    std::vector<std::unique_ptr<IStressTest>> CreateStressTests()
    {
        // Composition Root 역할을 하는 등록 지점입니다.
        //
        // Concrete Strategy 객체들을 여기서 만들고, Runner에는 IStressTest 목록으로 넘깁니다.
        // Runner가 구체 클래스를 직접 생성하지 않게 하면 실행 흐름과 테스트 종류가 분리됩니다.
        //
        // 실행 순서는 push_back 순서와 같습니다.
        // 새 테스트 추가 시에는 IStressTest 구현 클래스를 만든 뒤 이 목록에만 추가하면 됩니다.
        std::vector<std::unique_ptr<IStressTest>> tests;
        tests.push_back(std::make_unique<ArrayMathStressTest>(ExecutionMode::Single));
        tests.push_back(std::make_unique<ArrayMathStressTest>(ExecutionMode::Parallel));
        tests.push_back(std::make_unique<FibonacciStressTest>(ExecutionMode::Single));
        tests.push_back(std::make_unique<FibonacciStressTest>(ExecutionMode::Parallel));
        tests.push_back(std::make_unique<PrimeStressTest>(ExecutionMode::Single));
        tests.push_back(std::make_unique<PrimeStressTest>(ExecutionMode::Parallel));
        tests.push_back(std::make_unique<SortStressTest>(ExecutionMode::Single));
        tests.push_back(std::make_unique<SortStressTest>(ExecutionMode::Parallel));
        tests.push_back(std::make_unique<MandelbrotStressTest>(ExecutionMode::Single));
        tests.push_back(std::make_unique<MandelbrotStressTest>(ExecutionMode::Parallel));
        tests.push_back(std::make_unique<MemoryStressTest>());
        return tests;
    }
}

int main()
{
    ConsoleEncoding::UseUtf8();

    std::cout << "============================================" << std::endl;
    std::cout << "       CPU & Memory Stress Test Tool        " << std::endl;
    std::cout << "============================================" << std::endl;

    const int repeat = PromptRepeatCount();
    const CsvResultWriter writer;
    const StressTestRunner runner(writer);
    std::vector<std::unique_ptr<IStressTest>> tests = CreateStressTests();

    // main은 입력/객체 조립/반복 실행만 담당합니다.
    // 실제 테스트 실행 정책은 StressTestRunner, 실제 부하 로직은 각 IStressTest 구현체가 담당합니다.
    std::cout << "결과 파일 경로 설정 : " << writer.OutputDirectory() << std::endl << std::endl;
    for (int i = 0; i < repeat; ++i) {
        std::cout << "===== [테스트 반복] " << (i + 1) << "/" << repeat << " =====" << std::endl;
        runner.RunCycle(tests);
    }

    std::cout << "[완료] 모든 스트레스 테스트가 종료되었습니다." << std::endl;
    return 0;
}
