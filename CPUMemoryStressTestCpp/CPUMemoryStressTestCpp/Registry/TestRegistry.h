#pragma once

#include "../Interfaces/IStressTest.h"

#include <memory>
#include <string>
#include <vector>

struct TestDescriptor {
    std::string id;
    std::string title;
    std::string fileName;
};

class TestRegistry {
public:
    TestRegistry();

    const std::vector<std::unique_ptr<IStressTest>>& Tests() const;
    IStressTest* FindById(const std::string& id) const;
    std::vector<TestDescriptor> List() const;

private:
    std::vector<std::unique_ptr<IStressTest>> tests_;
};
