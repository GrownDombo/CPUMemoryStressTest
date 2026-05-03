#include "MemoryStressTest.h"

#include <algorithm>
#include <format>
#include <memory>
#include <new>
#include <vector>
#include <windows.h>

std::string MemoryStressTest::Id() const
{
    return "memory";
}

std::string MemoryStressTest::FileName() const
{
    return "Memory";
}

std::string MemoryStressTest::Title() const
{
    return "[메모리 테스트]";
}

TestResult MemoryStressTest::Run(const PresetConfig& config)
{
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);

    ULONGLONG totalPhysicalMemory = 8ULL * 1024 * 1024 * 1024;
    if (GlobalMemoryStatusEx(&memStatus)) {
        totalPhysicalMemory = memStatus.ullTotalPhys;
    }

    const std::size_t totalPhysicalMB = static_cast<std::size_t>(totalPhysicalMemory / 1024 / 1024);
    const std::size_t defaultSafetyLimitMB = totalPhysicalMB / 2 == 0 ? 1 : totalPhysicalMB / 2;
    const std::size_t extremeSafetyLimitMB = (totalPhysicalMB * 80) / 100 == 0 ? 1 : (totalPhysicalMB * 80) / 100;
    const std::size_t safetyLimitMB = config.preset == Preset::Extreme ? extremeSafetyLimitMB : defaultSafetyLimitMB;
    const std::size_t requestedTargetMB = config.memoryTargetPercent > 0
        ? (totalPhysicalMB * static_cast<std::size_t>(config.memoryTargetPercent)) / 100
        : static_cast<std::size_t>(config.memoryTargetMB < 1 ? 1 : config.memoryTargetMB);
    const std::size_t targetMB = requestedTargetMB < safetyLimitMB ? requestedTargetMB : safetyLimitMB;

    constexpr std::size_t BlockSize = 1024 * 1024;
    std::vector<std::unique_ptr<char[]>> allocations;
    std::size_t totalAllocatedMB = 0;

    try {
        while (totalAllocatedMB < targetMB) {
            std::unique_ptr<char[]> block(new (std::nothrow) char[BlockSize]);
            if (!block) {
                throw std::bad_alloc();
            }

            for (std::size_t i = 0; i < BlockSize; i += 4096) {
                block[i] = static_cast<char>(i % 256);
            }

            allocations.push_back(std::move(block));
            ++totalAllocatedMB;
        }
    }
    catch (const std::bad_alloc&) {
        // 부분 할당 결과도 의미 있는 스트레스 결과로 반환합니다.
    }

    TestResult result;
    result.testId = Id();
    result.title = Title();
    result.summary = std::format("목표 메모리 사용량 : {} MB, 실제 메모리 사용량 : {} MB", targetMB, totalAllocatedMB);
    result.metrics["totalPhysicalMemoryBytes"] = std::to_string(totalPhysicalMemory);
    result.metrics["totalPhysicalMemoryMB"] = std::to_string(totalPhysicalMB);
    result.metrics["targetMB"] = std::to_string(targetMB);
    result.metrics["allocatedMB"] = std::to_string(totalAllocatedMB);
    if (config.memoryTargetPercent > 0) {
        result.metrics["targetPercent"] = std::to_string(config.memoryTargetPercent);
    }
    return result;
}
