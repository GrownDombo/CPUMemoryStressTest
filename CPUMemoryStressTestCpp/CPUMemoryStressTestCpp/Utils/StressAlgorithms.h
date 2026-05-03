#pragma once

#include <cstddef>
#include <limits>
#include <vector>

// 워크로드 클래스들이 공유하는 순수 계산/보조 함수 모음입니다.
//
// 이 namespace는 Strategy 객체가 아닙니다. 테스트 실행 순서, CSV, 시간 측정 같은
// 애플리케이션 흐름을 모르고, 피보나치/소수/정렬 병합/Mandelbrot 계산처럼
// 재사용 가능한 알고리즘만 제공합니다.
namespace StressAlgorithms {
    inline constexpr int FibonacciDepth = 45;
    inline constexpr int PrimeStressRange = (std::numeric_limits<int>::max() / 20) - 2;
    inline constexpr int MandelbrotWidthSingle = 2000;
    inline constexpr int MandelbrotHeightSingle = 2000;
    inline constexpr int MandelbrotMaxIterationsSingle = 5000;
    inline constexpr int MandelbrotWidthParallel = 4000;
    inline constexpr int MandelbrotHeightParallel = 4000;
    inline constexpr int MandelbrotMaxIterationsParallel = 10000;

    // 타입 T 배열이 약 2GB 제한 안에서 가질 수 있는 최대 원소 개수를 계산합니다.
    // 기존 테스트의 대용량 배열 부하 크기를 유지하기 위한 공통 계산 함수입니다.
    template<typename T>
    int MaxArray()
    {
        constexpr std::size_t MaxByteLength = 0x7FFFFFC7;
        return static_cast<int>(MaxByteLength / sizeof(T));
    }

    // 현재 하드웨어 스레드 수를 반환하되, 조회 실패 시에도 최소 1을 보장합니다.
    int WorkerCount();

    // 고정 seed로 int 랜덤 벡터를 생성합니다.
    // 정렬 테스트가 반복 실행되어도 같은 입력 크기와 분포를 쓰도록 합니다.
    std::vector<int> GenerateRandomVector(int length);

    // 재귀 방식으로 피보나치 값을 계산해 CPU 부하를 만듭니다.
    // 의도적으로 메모이제이션 없이 계산합니다.
    double FibonacciRecursive(int n);

    // 주어진 정수가 소수인지 검사합니다.
    // 소수 찾기 스트레스 테스트의 단일/병렬 모드에서 공통으로 사용됩니다.
    bool IsPrime(int number);

    // 각 스레드가 정렬한 청크들을 하나의 정렬된 벡터로 병합합니다.
    // raw pointer와 _msize에 의존하지 않도록 명시적인 vector 크기 정보를 사용합니다.
    std::vector<int> MergeSortedChunks(const std::vector<std::vector<int>>& chunks);

    // Mandelbrot 계산을 수행해 CPU 부하를 만듭니다.
    // parallel이 true이면 지정된 worker 수로 x축 범위를 나눠 처리합니다.
    void CalculateMandelbrot(int width, int height, int maxIterations, bool parallel, int workerCount);
}
