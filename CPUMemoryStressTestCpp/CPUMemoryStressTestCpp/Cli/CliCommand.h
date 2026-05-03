#pragma once

#include "../Results/PresetConfig.h"

#include <string>

enum class CliCommandType {
    Help,
    List,
    Run,
    Shell
};

struct CliCommand {
    CliCommandType type = CliCommandType::Help;
    std::string testId;
    Preset preset = Preset::Quick;
    int repeat = 1;
    bool saveCsv = false;
    std::string csvDirectory;
};

struct CliParseResult {
    bool success = false;
    CliCommand command;
    std::string errorCode;
    std::string errorMessage;
};
