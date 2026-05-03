#include "CliParser.h"

#include <charconv>
#include <string>
#include <system_error>
#include <utility>

namespace {
    bool TryParseRepeat(const std::string& value, int& repeat)
    {
        const char* first = value.data();
        const char* last = value.data() + value.size();
        int parsed = 0;
        const auto result = std::from_chars(first, last, parsed);
        if (result.ec != std::errc() || result.ptr != last || parsed < 1 || parsed > 5) {
            return false;
        }

        repeat = parsed;
        return true;
    }

    CliParseResult Error(std::string code, std::string message)
    {
        CliParseResult result;
        result.success = false;
        result.errorCode = std::move(code);
        result.errorMessage = std::move(message);
        return result;
    }
}

CliParseResult CliParser::Parse(int argc, char* argv[]) const
{
    CliParseResult result;
    if (argc < 2) {
        result.success = true;
        result.command.type = CliCommandType::Help;
        return result;
    }

    const std::string command = ToLowerAscii(argv[1]);
    if (command == "help" || command == "--help" || command == "-h") {
        result.success = true;
        result.command.type = CliCommandType::Help;
        return result;
    }

    if (command == "list") {
        if (argc != 2) {
            return Error("invalid_arguments", "list does not accept additional arguments.");
        }

        result.success = true;
        result.command.type = CliCommandType::List;
        return result;
    }

    if (command == "shell") {
        if (argc != 2) {
            return Error("invalid_arguments", "shell does not accept additional arguments.");
        }

        result.success = true;
        result.command.type = CliCommandType::Shell;
        return result;
    }

    if (command != "run") {
        return Error("unknown_command", "Unknown command: " + std::string(argv[1]));
    }

    if (argc < 3) {
        return Error("missing_test_id", "run requires a test id or all.");
    }

    result.success = true;
    result.command.type = CliCommandType::Run;
    result.command.testId = ToLowerAscii(argv[2]);

    for (int i = 3; i < argc; ++i) {
        const std::string option = argv[i];

        if (option == "--preset") {
            if (i + 1 >= argc) {
                return Error("missing_option_value", "--preset requires quick, normal, heavy, or extreme.");
            }

            Preset preset = Preset::Quick;
            if (!TryParsePreset(argv[++i], preset)) {
                return Error("invalid_preset", "Invalid preset: " + std::string(argv[i]));
            }

            result.command.preset = preset;
            continue;
        }

        if (option == "--repeat") {
            if (i + 1 >= argc) {
                return Error("missing_option_value", "--repeat requires a value from 1 to 5.");
            }

            int repeat = 1;
            if (!TryParseRepeat(argv[++i], repeat)) {
                return Error("invalid_repeat", "Invalid repeat value: " + std::string(argv[i]) + ". Allowed range is 1..5.");
            }

            result.command.repeat = repeat;
            continue;
        }

        if (option == "--save-csv") {
            result.command.saveCsv = true;
            continue;
        }

        if (option == "--csv-dir") {
            if (i + 1 >= argc) {
                return Error("missing_option_value", "--csv-dir requires a directory path.");
            }

            result.command.csvDirectory = argv[++i];
            result.command.saveCsv = true;
            continue;
        }

        return Error("unknown_option", "Unknown option: " + option);
    }

    return result;
}
