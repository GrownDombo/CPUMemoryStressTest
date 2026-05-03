#pragma once

#include "CliCommand.h"

class JsonResultWriter;
class TestRegistry;

class CliCommandExecutor {
public:
    CliCommandExecutor(JsonResultWriter& json, const TestRegistry& registry, bool allowExtreme = false);

    int Execute(CliCommand command) const;

private:
    JsonResultWriter& json_;
    const TestRegistry& registry_;
    bool allowExtreme_ = false;
};
