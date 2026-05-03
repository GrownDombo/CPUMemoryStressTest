#include "Environment.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <shlobj.h>

namespace Environment {
    std::string GetEnv(const char* name)
    {
        char* buffer = nullptr;
        size_t size = 0;
        if (_dupenv_s(&buffer, &size, name) == 0 && buffer != nullptr) {
            std::string result(buffer);
            free(buffer);
            return result;
        }

        return "";
    }

    std::string CreateTimestampedResultDirectory()
    {
        char desktopPath[MAX_PATH];
        std::string baseDir;
        if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_DESKTOP, nullptr, 0, desktopPath))) {
            baseDir = std::format("{}\\StressTestResult", desktopPath);
        }
        else {
            baseDir = std::format("{}\\Desktop\\StressTestResult", GetEnv("USERPROFILE"));
        }

        const auto now = std::chrono::system_clock::now();
        const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        tm localTime;
        localtime_s(&localTime, &currentTime);

        std::stringstream ss;
        ss << std::put_time(&localTime, "%Y%m%d_%H%M%S");

        const std::string fullPath = std::format("{}\\{}", baseDir, ss.str());
        std::filesystem::create_directories(fullPath);
        return fullPath;
    }
}
