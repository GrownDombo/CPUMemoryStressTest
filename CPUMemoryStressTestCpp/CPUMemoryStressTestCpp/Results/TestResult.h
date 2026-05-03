#pragma once

#include <map>
#include <string>

struct TestResult {
    std::string testId;
    std::string title;
    double durationMs = 0.0;
    bool success = true;
    std::string summary;
    std::map<std::string, std::string> metrics;
    std::string errorMessage;
};
