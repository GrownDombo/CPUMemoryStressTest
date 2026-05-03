#pragma once

#include "CliCommand.h"

class CliParser {
public:
    CliParseResult Parse(int argc, char* argv[]) const;
};
