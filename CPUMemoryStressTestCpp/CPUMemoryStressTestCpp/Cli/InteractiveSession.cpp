#include "InteractiveSession.h"

#include "CliExitCodes.h"
#include "../Interfaces/IStressTest.h"
#include "../Logging/CsvResultWriter.h"
#include "../Registry/TestRegistry.h"
#include "../Runner/StressTestRunner.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

namespace {
    void WriteCsvResult(const CsvResultWriter& writer, const IStressTest& test, const TestResult& result)
    {
        const std::string summary = result.success ? result.summary : result.errorMessage;
        writer.Write(test.FileName(), result.title, summary, result.durationMs);
    }
}

int InteractiveSession::Run() const
{
    PrintBanner();

    const int repeat = PromptRepeatCount();
    const Preset preset = PromptPreset();
    const CsvResultWriter writer;
    const TestRegistry registry;
    const StressTestRunner runner;
    const PresetConfig config = CreatePresetConfig(preset);

    std::cout << "Preset: " << PresetName(config.preset) << std::endl;
    std::cout << "Result directory: " << writer.OutputDirectory() << std::endl << std::endl;

    for (int i = 0; i < repeat; ++i) {
        std::cout << "===== [Test Repeat] " << (i + 1) << "/" << repeat << " =====" << std::endl;
        for (const auto& test : registry.Tests()) {
            std::cout << "[Start] " << test->Title() << std::endl;
            const TestResult result = runner.Run(*test, config);
            WriteCsvResult(writer, *test, result);

            if (result.success) {
                std::cout << "[Info] " << result.summary << std::endl;
            }
            else {
                std::cout << "[Error] " << result.errorMessage << std::endl;
            }

            std::cout << "[Result] Duration: " << std::fixed << std::setprecision(2) << result.durationMs << " ms" << std::endl;
            std::cout << "[End] " << test->Title() << std::endl << std::endl;

            std::cout << "[Wait] Waiting 5 seconds..." << std::endl;
            runner.PauseAndCollectMemory();
            std::cout << "[Done] Wait and memory cleanup complete" << std::endl << std::endl;
        }
    }

    std::cout << "[Done] All stress tests completed." << std::endl;
    return CliExitCodes::Success;
}

void InteractiveSession::PrintBanner()
{
    std::cout << "============================================" << std::endl;
    std::cout << "       CPU & Memory Stress Test Tool        " << std::endl;
    std::cout << "============================================" << std::endl;
}

int InteractiveSession::PromptRepeatCount()
{
    int count = 0;
    while (true) {
        std::cout << "Stress test repeat count (1~10): ";
        std::cin >> count;
        if (count >= 1 && count <= 10) {
            return count;
        }

        std::cout << "[Error] Enter a number from 1 to 10.\n" << std::endl;
    }
}

Preset InteractiveSession::PromptPreset()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        std::cout << "Preset (quick/normal/heavy/extreme, default heavy): ";

        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
            return Preset::Heavy;
        }

        Preset preset = Preset::Heavy;
        if (!TryParsePreset(input, preset)) {
            std::cout << "[Error] Enter quick, normal, heavy, or extreme.\n" << std::endl;
            continue;
        }

        if (preset == Preset::Extreme && !ConfirmExtreme()) {
            std::cout << "[Info] Extreme preset cancelled.\n" << std::endl;
            continue;
        }

        return preset;
    }
}

bool InteractiveSession::ConfirmExtreme()
{
    std::cout << "Extreme preset can heavily load this PC. Type EXTREME to continue: ";

    std::string confirmation;
    std::getline(std::cin, confirmation);
    return confirmation == "EXTREME";
}
