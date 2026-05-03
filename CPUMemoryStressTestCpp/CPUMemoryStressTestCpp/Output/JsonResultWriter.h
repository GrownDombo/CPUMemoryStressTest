#pragma once

#include "../Cli/CliCommand.h"
#include "../Registry/TestRegistry.h"
#include "../Results/TestResult.h"

#include <iosfwd>
#include <string>
#include <vector>

class JsonResultWriter {
public:
    explicit JsonResultWriter(std::ostream& output);

    void WriteHelp() const;
    void WriteList(const std::vector<TestDescriptor>& tests) const;
    void WriteRunResult(const CliCommand& command, const std::vector<TestResult>& results) const;
    void WriteError(const std::string& code, const std::string& message) const;

private:
    static std::string Escape(const std::string& value);
    void WriteTestResult(const TestResult& result, int indent) const;

    std::ostream& output_;
};
