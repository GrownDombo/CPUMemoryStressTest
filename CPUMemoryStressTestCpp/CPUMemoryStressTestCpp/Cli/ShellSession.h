#pragma once

#include "CliParser.h"

#include <string>
#include <vector>

class CliCommandExecutor;
class JsonResultWriter;

class ShellSession {
public:
    ShellSession(const CliParser& parser, CliCommandExecutor& executor, JsonResultWriter& json);

    int Run() const;

private:
    static std::vector<std::string> TokenizeLine(const std::string& line, std::string& errorMessage);
    CliParseResult ParseTokens(const std::vector<std::string>& tokens) const;
    bool ConfirmExtreme() const;

    const CliParser& parser_;
    CliCommandExecutor& executor_;
    JsonResultWriter& json_;
};
