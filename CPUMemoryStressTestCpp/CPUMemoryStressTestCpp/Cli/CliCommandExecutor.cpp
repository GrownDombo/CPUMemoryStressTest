#include "CliCommandExecutor.h"

#include "CliExitCodes.h"
#include "../Interfaces/IStressTest.h"
#include "../Logging/CsvResultWriter.h"
#include "../Output/JsonResultWriter.h"
#include "../Registry/TestRegistry.h"
#include "../Runner/StressTestRunner.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {
    void WriteCsvResult(const CsvResultWriter& writer, const IStressTest& test, const TestResult& result)
    {
        const std::string summary = result.success ? result.summary : result.errorMessage;
        writer.Write(test.FileName(), result.title, summary, result.durationMs);
    }

    std::unique_ptr<CsvResultWriter> CreateCsvWriter(const CliCommand& command)
    {
        if (!command.saveCsv) {
            return nullptr;
        }

        if (!command.csvDirectory.empty()) {
            return std::make_unique<CsvResultWriter>(command.csvDirectory);
        }

        return std::make_unique<CsvResultWriter>();
    }
}

CliCommandExecutor::CliCommandExecutor(JsonResultWriter& json, const TestRegistry& registry, bool allowExtreme)
    : json_(json),
      registry_(registry),
      allowExtreme_(allowExtreme)
{
}

int CliCommandExecutor::Execute(CliCommand command) const
{
    if (command.type == CliCommandType::Help) {
        json_.WriteHelp();
        return CliExitCodes::Success;
    }

    if (command.type == CliCommandType::List) {
        json_.WriteList(registry_.List());
        return CliExitCodes::Success;
    }

    if (command.type == CliCommandType::Shell) {
        json_.WriteError("nested_shell_not_supported", "shell can only be started from the executable command line.");
        return CliExitCodes::InvalidArguments;
    }

    if (command.preset == Preset::Extreme && !allowExtreme_) {
        json_.WriteError(
            "extreme_requires_interactive_mode",
            "The extreme preset is only available from shell or interactive mode.");
        return CliExitCodes::InvalidArguments;
    }

    std::vector<IStressTest*> selectedTests;
    if (command.testId == "all") {
        selectedTests.reserve(registry_.Tests().size());
        for (const auto& test : registry_.Tests()) {
            selectedTests.push_back(test.get());
        }
    }
    else {
        IStressTest* test = registry_.FindById(command.testId);
        if (test == nullptr) {
            json_.WriteError("invalid_test_id", "Unknown test id: " + command.testId);
            return CliExitCodes::InvalidArguments;
        }

        selectedTests.push_back(test);
    }

    const PresetConfig config = CreatePresetConfig(command.preset);
    const StressTestRunner runner;
    std::unique_ptr<CsvResultWriter> csvWriter = CreateCsvWriter(command);
    if (csvWriter != nullptr && command.csvDirectory.empty()) {
        command.csvDirectory = csvWriter->OutputDirectory();
    }

    std::vector<TestResult> results;
    results.reserve(selectedTests.size() * command.repeat);

    for (int repeatIndex = 0; repeatIndex < command.repeat; ++repeatIndex) {
        for (IStressTest* test : selectedTests) {
            TestResult result = runner.Run(*test, config);
            result.metrics["repeatIndex"] = std::to_string(repeatIndex + 1);

            if (csvWriter != nullptr) {
                WriteCsvResult(*csvWriter, *test, result);
            }

            results.push_back(std::move(result));
        }
    }

    json_.WriteRunResult(command, results);

    for (const TestResult& result : results) {
        if (!result.success) {
            return CliExitCodes::ExecutionFailed;
        }
    }

    return CliExitCodes::Success;
}
