#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <thread>

enum class Preset {
    Quick,
    Normal,
    Heavy,
    Extreme
};

struct PresetConfig {
    Preset preset = Preset::Quick;
    int workerCount = 1;
    int arrayLength = 100000;
    int primeRange = 10000;
    int fibonacciDepth = 30;
    int mandelbrotWidth = 200;
    int mandelbrotHeight = 160;
    int mandelbrotIterations = 100;
    int memoryTargetMB = 128;
    int memoryTargetPercent = 0;
};

inline std::string PresetName(Preset preset)
{
    switch (preset) {
    case Preset::Quick:
        return "quick";
    case Preset::Normal:
        return "normal";
    case Preset::Heavy:
        return "heavy";
    case Preset::Extreme:
        return "extreme";
    }

    return "quick";
}

inline std::string ToLowerAscii(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

inline bool TryParsePreset(const std::string& value, Preset& preset)
{
    const std::string normalized = ToLowerAscii(value);
    if (normalized == "quick") {
        preset = Preset::Quick;
        return true;
    }

    if (normalized == "normal") {
        preset = Preset::Normal;
        return true;
    }

    if (normalized == "heavy") {
        preset = Preset::Heavy;
        return true;
    }

    if (normalized == "extreme") {
        preset = Preset::Extreme;
        return true;
    }

    return false;
}

inline PresetConfig CreatePresetConfig(Preset preset)
{
    const int hardwareWorkers = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));

    switch (preset) {
    case Preset::Quick:
        return PresetConfig{
            preset,
            std::min(hardwareWorkers, 2),
            100000,
            10000,
            30,
            200,
            160,
            100,
            128,
            0
        };
    case Preset::Normal:
        return PresetConfig{
            preset,
            std::min(hardwareWorkers, 4),
            1000000,
            200000,
            36,
            600,
            400,
            500,
            512,
            0
        };
    case Preset::Heavy:
        return PresetConfig{
            preset,
            hardwareWorkers,
            5000000,
            2000000,
            42,
            1200,
            800,
            1500,
            2048,
            0
        };
    case Preset::Extreme:
        return PresetConfig{
            preset,
            hardwareWorkers,
            50000000,
            100000000,
            45,
            2000,
            2000,
            5000,
            0,
            80
        };
    }

    return CreatePresetConfig(Preset::Quick);
}
