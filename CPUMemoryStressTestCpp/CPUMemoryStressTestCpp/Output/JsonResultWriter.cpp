#include "JsonResultWriter.h"

#include <iomanip>
#include <ostream>
#include <sstream>

JsonResultWriter::JsonResultWriter(std::ostream& output)
    : output_(output)
{
}

void JsonResultWriter::WriteHelp() const
{
    output_
        << "{\n"
        << "  \"ok\": true,\n"
        << "  \"command\": \"help\",\n"
        << "  \"usage\": [\n"
        << "    \"CPUMemoryStressTestCpp.exe list\",\n"
        << "    \"CPUMemoryStressTestCpp.exe run <test-id|all> [--preset quick|normal|heavy] [--repeat 1..5] [--save-csv] [--csv-dir <path>]\",\n"
        << "    \"CPUMemoryStressTestCpp.exe shell\",\n"
        << "    \"extreme preset is available only from shell or interactive mode\"\n"
        << "  ]\n"
        << "}\n";
}

void JsonResultWriter::WriteList(const std::vector<TestDescriptor>& tests) const
{
    output_
        << "{\n"
        << "  \"ok\": true,\n"
        << "  \"command\": \"list\",\n"
        << "  \"tests\": [\n";

    for (std::size_t i = 0; i < tests.size(); ++i) {
        const TestDescriptor& test = tests[i];
        output_
            << "    {\n"
            << "      \"id\": \"" << Escape(test.id) << "\",\n"
            << "      \"title\": \"" << Escape(test.title) << "\",\n"
            << "      \"fileName\": \"" << Escape(test.fileName) << "\"\n"
            << "    }";

        if (i + 1 < tests.size()) {
            output_ << ',';
        }

        output_ << '\n';
    }

    output_
        << "  ]\n"
        << "}\n";
}

void JsonResultWriter::WriteRunResult(const CliCommand& command, const std::vector<TestResult>& results) const
{
    bool ok = true;
    for (const TestResult& result : results) {
        ok = ok && result.success;
    }

    output_
        << "{\n"
        << "  \"ok\": " << (ok ? "true" : "false") << ",\n"
        << "  \"command\": \"run\",\n"
        << "  \"target\": \"" << Escape(command.testId) << "\",\n"
        << "  \"preset\": \"" << PresetName(command.preset) << "\",\n"
        << "  \"repeat\": " << command.repeat << ",\n"
        << "  \"csvSaved\": " << (command.saveCsv ? "true" : "false") << ",\n"
        << "  \"csvDirectory\": \"" << Escape(command.csvDirectory) << "\",\n"
        << "  \"results\": [\n";

    for (std::size_t i = 0; i < results.size(); ++i) {
        WriteTestResult(results[i], 4);
        if (i + 1 < results.size()) {
            output_ << ',';
        }

        output_ << '\n';
    }

    output_
        << "  ]\n"
        << "}\n";
}

void JsonResultWriter::WriteError(const std::string& code, const std::string& message) const
{
    output_
        << "{\n"
        << "  \"ok\": false,\n"
        << "  \"error\": {\n"
        << "    \"code\": \"" << Escape(code) << "\",\n"
        << "    \"message\": \"" << Escape(message) << "\"\n"
        << "  }\n"
        << "}\n";
}

std::string JsonResultWriter::Escape(const std::string& value)
{
    std::ostringstream escaped;
    for (const unsigned char ch : value) {
        switch (ch) {
        case '\"':
            escaped << "\\\"";
            break;
        case '\\':
            escaped << "\\\\";
            break;
        case '\b':
            escaped << "\\b";
            break;
        case '\f':
            escaped << "\\f";
            break;
        case '\n':
            escaped << "\\n";
            break;
        case '\r':
            escaped << "\\r";
            break;
        case '\t':
            escaped << "\\t";
            break;
        default:
            if (ch < 0x20) {
                escaped << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(ch);
            }
            else {
                escaped << static_cast<char>(ch);
            }
            break;
        }
    }

    return escaped.str();
}

void JsonResultWriter::WriteTestResult(const TestResult& result, int indent) const
{
    const std::string spaces(indent, ' ');
    const std::string childSpaces(indent + 2, ' ');

    output_
        << spaces << "{\n"
        << childSpaces << "\"testId\": \"" << Escape(result.testId) << "\",\n"
        << childSpaces << "\"title\": \"" << Escape(result.title) << "\",\n"
        << childSpaces << "\"durationMs\": " << std::fixed << std::setprecision(2) << result.durationMs << ",\n"
        << childSpaces << "\"success\": " << (result.success ? "true" : "false") << ",\n"
        << childSpaces << "\"summary\": \"" << Escape(result.summary) << "\",\n"
        << childSpaces << "\"metrics\": {";

    if (!result.metrics.empty()) {
        output_ << '\n';
        std::size_t index = 0;
        for (const auto& [key, value] : result.metrics) {
            output_ << childSpaces << "  \"" << Escape(key) << "\": \"" << Escape(value) << "\"";
            if (++index < result.metrics.size()) {
                output_ << ',';
            }
            output_ << '\n';
        }
        output_ << childSpaces;
    }

    output_ << "}";

    if (!result.errorMessage.empty()) {
        output_ << ",\n" << childSpaces << "\"errorMessage\": \"" << Escape(result.errorMessage) << "\"\n";
    }
    else {
        output_ << '\n';
    }

    output_ << spaces << "}";
}
