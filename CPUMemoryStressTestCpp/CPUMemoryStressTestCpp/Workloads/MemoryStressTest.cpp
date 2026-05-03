#include "MemoryStressTest.h"

#include <format>
#include <iostream>
#include <memory>
#include <new>
#include <vector>
#include <windows.h>

std::string MemoryStressTest::FileName() const
{
    return "Memory";
}

std::string MemoryStressTest::Title() const
{
    return "[메모리 테스트]";
}

std::string MemoryStressTest::Run()
{
    std::cout << "[시작] 메모리 스트레스 테스트" << std::endl;

    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);

    ULONGLONG totalPhysicalMemory = 0;
    if (GlobalMemoryStatusEx(&memStatus)) {
        totalPhysicalMemory = memStatus.ullTotalPhys;
    }
    else {
        std::cerr << "[경고] 메모리 정보를 가져오지 못했습니다. 기본 8GB 로 진행합니다." << std::endl;
        totalPhysicalMemory = 8ULL * 1024 * 1024 * 1024;
    }

    const double targetMB = (totalPhysicalMemory / 1024.0 / 1024.0) * 0.8;
    std::cout << std::format("[정보] 목표 메모리 사용량: {:.2f} MB", targetMB) << std::endl;

    constexpr std::size_t BlockSize = 1024 * 1024;
    std::vector<std::unique_ptr<char[]>> allocations;
    std::size_t totalAllocatedMB = 0;

    try {
        while (true) {
            std::unique_ptr<char[]> block(new (std::nothrow) char[BlockSize]);
            if (!block) {
                throw std::bad_alloc();
            }

            for (std::size_t i = 0; i < BlockSize; i += 4096) {
                block[i] = static_cast<char>(i % 256);
            }

            allocations.push_back(std::move(block));
            ++totalAllocatedMB;

            if (totalAllocatedMB >= targetMB) {
                break;
            }
        }
    }
    catch (const std::bad_alloc&) {
        std::cout << "[메모리] 메모리 부족" << std::endl;
    }

    std::cout << "[종료] 메모리 스트레스 테스트 완료" << std::endl;

    return std::format(
        "전체 메모리 : {}, 목표 메모리 사용량 : {:.2f} MB, 실제 메모리 사용량 : {} MB",
        totalPhysicalMemory,
        targetMB,
        totalAllocatedMB);
}
