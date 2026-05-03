#include "CsvResultWriter.h"

#include "../Utils/Environment.h"

#include <filesystem>
#include <format>
#include <fstream>

CsvResultWriter::CsvResultWriter()
    : outputDirectory_(Environment::CreateTimestampedResultDirectory())
{
}

const std::string& CsvResultWriter::OutputDirectory() const
{
    return outputDirectory_;
}

void CsvResultWriter::Write(const std::string& fileName, const std::string& title, const std::string& result, double durationMs) const
{
    std::filesystem::create_directories(outputDirectory_);
    const std::string fullPath = std::format("{}\\{}.csv", outputDirectory_, fileName);

    std::ofstream file(fullPath, std::ios::app);
    if (file.tellp() == std::streampos(0)) {
        // Excel 같은 Windows 도구가 UTF-8 CSV의 한글을 바로 인식하도록 BOM을 기록합니다.
        file << "\xEF\xBB\xBF";
        file << "\"" << title << "\"" << '\n';
    }

    file << std::format("{:.2f} ms,결과 : {}\n", durationMs, result);
}
