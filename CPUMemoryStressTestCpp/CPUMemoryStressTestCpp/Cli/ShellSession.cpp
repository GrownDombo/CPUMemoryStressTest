#include "ShellSession.h"

#include "CliCommandExecutor.h"
#include "CliExitCodes.h"
#include "../Output/JsonResultWriter.h"
#include "../Results/PresetConfig.h"

#include <iostream>

ShellSession::ShellSession(const CliParser& parser, CliCommandExecutor& executor, JsonResultWriter& json)
    : parser_(parser),
      executor_(executor),
      json_(json)
{
}

int ShellSession::Run() const
{
    std::cout << "CPUMemoryStressTest shell. Type help, list, run ..., exit, or quit." << std::endl;

    std::string line;
    while (true) {
        std::cout << "stress> ";
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl;
            return CliExitCodes::Success;
        }

        std::string tokenizeError;
        std::vector<std::string> tokens = TokenizeLine(line, tokenizeError);
        if (!tokenizeError.empty()) {
            json_.WriteError("invalid_shell_syntax", tokenizeError);
            continue;
        }

        if (tokens.empty()) {
            continue;
        }

        const std::string shellCommand = ToLowerAscii(tokens.front());
        if (shellCommand == "exit" || shellCommand == "quit") {
            return CliExitCodes::Success;
        }

        const CliParseResult parseResult = ParseTokens(tokens);
        if (!parseResult.success) {
            json_.WriteError(parseResult.errorCode, parseResult.errorMessage);
            continue;
        }

        if (parseResult.command.type == CliCommandType::Run && parseResult.command.preset == Preset::Extreme && !ConfirmExtreme()) {
            json_.WriteError("extreme_cancelled", "Extreme preset was cancelled.");
            continue;
        }

        (void)executor_.Execute(parseResult.command);
    }
}

std::vector<std::string> ShellSession::TokenizeLine(const std::string& line, std::string& errorMessage)
{
    std::vector<std::string> tokens;
    std::string current;
    bool inQuote = false;
    char quoteChar = '\0';
    bool escaping = false;

    auto flushCurrent = [&]() {
        if (!current.empty()) {
            tokens.push_back(current);
            current.clear();
        }
    };

    for (const char ch : line) {
        if (escaping) {
            current.push_back(ch);
            escaping = false;
            continue;
        }

        if (inQuote) {
            if (ch == '\\') {
                escaping = true;
                continue;
            }

            if (ch == quoteChar) {
                inQuote = false;
                continue;
            }

            current.push_back(ch);
            continue;
        }

        if (ch == '"' || ch == '\'') {
            inQuote = true;
            quoteChar = ch;
            continue;
        }

        if (ch == ' ' || ch == '\t') {
            flushCurrent();
            continue;
        }

        current.push_back(ch);
    }

    if (escaping) {
        current.push_back('\\');
    }

    if (inQuote) {
        errorMessage = "Unclosed quote in shell command.";
        return {};
    }

    flushCurrent();
    return tokens;
}

CliParseResult ShellSession::ParseTokens(const std::vector<std::string>& tokens) const
{
    std::vector<std::string> arguments;
    arguments.reserve(tokens.size() + 1);
    arguments.push_back("CPUMemoryStressTestCpp.exe");
    arguments.insert(arguments.end(), tokens.begin(), tokens.end());

    std::vector<char*> argv;
    argv.reserve(arguments.size());
    for (std::string& argument : arguments) {
        argv.push_back(argument.data());
    }

    return parser_.Parse(static_cast<int>(argv.size()), argv.data());
}

bool ShellSession::ConfirmExtreme() const
{
    std::cout << "Extreme preset can heavily load this PC. Type EXTREME to continue: ";

    std::string confirmation;
    if (!std::getline(std::cin, confirmation)) {
        return false;
    }

    return confirmation == "EXTREME";
}
