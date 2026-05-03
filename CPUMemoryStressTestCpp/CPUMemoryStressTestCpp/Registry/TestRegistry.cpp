#include "TestRegistry.h"

#include "../Workloads/ArrayMathStressTest.h"
#include "../Workloads/FibonacciStressTest.h"
#include "../Workloads/MandelbrotStressTest.h"
#include "../Workloads/MemoryStressTest.h"
#include "../Workloads/PrimeStressTest.h"
#include "../Workloads/SortStressTest.h"

TestRegistry::TestRegistry()
{
    tests_.push_back(std::make_unique<ArrayMathStressTest>(ExecutionMode::Single));
    tests_.push_back(std::make_unique<ArrayMathStressTest>(ExecutionMode::Parallel));
    tests_.push_back(std::make_unique<FibonacciStressTest>(ExecutionMode::Single));
    tests_.push_back(std::make_unique<FibonacciStressTest>(ExecutionMode::Parallel));
    tests_.push_back(std::make_unique<PrimeStressTest>(ExecutionMode::Single));
    tests_.push_back(std::make_unique<PrimeStressTest>(ExecutionMode::Parallel));
    tests_.push_back(std::make_unique<SortStressTest>(ExecutionMode::Single));
    tests_.push_back(std::make_unique<SortStressTest>(ExecutionMode::Parallel));
    tests_.push_back(std::make_unique<MandelbrotStressTest>(ExecutionMode::Single));
    tests_.push_back(std::make_unique<MandelbrotStressTest>(ExecutionMode::Parallel));
    tests_.push_back(std::make_unique<MemoryStressTest>());
}

const std::vector<std::unique_ptr<IStressTest>>& TestRegistry::Tests() const
{
    return tests_;
}

IStressTest* TestRegistry::FindById(const std::string& id) const
{
    for (const auto& test : tests_) {
        if (test->Id() == id) {
            return test.get();
        }
    }

    return nullptr;
}

std::vector<TestDescriptor> TestRegistry::List() const
{
    std::vector<TestDescriptor> descriptors;
    descriptors.reserve(tests_.size());

    for (const auto& test : tests_) {
        descriptors.push_back(TestDescriptor{
            test->Id(),
            test->Title(),
            test->FileName()
        });
    }

    return descriptors;
}
