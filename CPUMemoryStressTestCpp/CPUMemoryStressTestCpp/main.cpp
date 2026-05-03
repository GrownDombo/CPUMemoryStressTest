#include "Cli/CliCommandExecutor.h"
#include "Cli/CliExitCodes.h"
#include "Cli/CliParser.h"
#include "Cli/InteractiveSession.h"
#include "Cli/ShellSession.h"
#include "Output/JsonResultWriter.h"
#include "Registry/TestRegistry.h"
#include "Utils/ConsoleEncoding.h"

#include <iostream>

namespace {
    int RunCli(int argc, char* argv[])
    {
        JsonResultWriter json(std::cout);
        const CliParser parser;
        const CliParseResult parseResult = parser.Parse(argc, argv);

        if (!parseResult.success) {
            json.WriteError(parseResult.errorCode, parseResult.errorMessage);
            return CliExitCodes::InvalidArguments;
        }

        const TestRegistry registry;

        if (parseResult.command.type == CliCommandType::Shell) {
            CliCommandExecutor shellExecutor(json, registry, true);
            const ShellSession shell(parser, shellExecutor, json);
            return shell.Run();
        }

        CliCommandExecutor executor(json, registry);
        return executor.Execute(parseResult.command);
    }
}

int main(int argc, char* argv[])
{
    ConsoleEncoding::UseUtf8();

    if (argc == 1) {
        const InteractiveSession interactive;
        return interactive.Run();
    }

    return RunCli(argc, argv);
}
