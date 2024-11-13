#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/I386Linux/TracingExecutor.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Concolic.h>
#include <Rose/BinaryAnalysis/Debugger/Linux.h>

#include <boost/filesystem.hpp>
#include <boost/serialization/export.hpp>
#include <memory.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

TracingExecutor::TracingExecutor(const std::string &name)
    : Super(name) {}

TracingExecutor::TracingExecutor(const Database::Ptr &db)
    : Super(db) {}

TracingExecutor::~TracingExecutor() {}

TracingExecutor::Ptr
TracingExecutor::instance(const Database::Ptr &db) {
    return Ptr(new TracingExecutor(db));
}

TracingExecutor::Ptr
TracingExecutor::factory() {
    return Ptr(new TracingExecutor("I386Linux::Tracing"));
}

Concolic::ConcreteExecutor::Ptr
TracingExecutor::instanceFromFactory(const Database::Ptr &db) {
    ASSERT_require(isFactory());
    auto retval = instance(db);
    retval->name(name());
    return retval;
}

bool
TracingExecutor::matchFactory(const std::string &name) const {
    return name == this->name();
}

Concolic::ConcreteResult::Ptr
TracingExecutor::execute(const TestCase::Ptr &testCase) {
    ASSERT_forbid(isFactory());

    // FIXME[Robb Matzke 2020-07-15]: This temp dir should be automatically removed.

    // Copy the specimen to a temporary directory
    boost::filesystem::path tempdir = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::path exeName = tempdir / "a.out";
    {
        boost::filesystem::create_directories(tempdir);
        std::ofstream exe(exeName.c_str(), std::ios::binary);
        const uint8_t *data = testCase->specimen()->content().data();
        exe.write(reinterpret_cast<const char*>(data), testCase->specimen()->content().size());
    }
    boost::filesystem::permissions(exeName, boost::filesystem::add_perms | boost::filesystem::owner_exe);

    // Prepare to run the test case in a debugger
    Debugger::Ptr debugger;
    {
        Debugger::Linux::Specimen specimen(exeName);
        specimen.arguments(testCase->args());
        specimen.eraseAllEnvironmentVariables();
        specimen.flags().clear();
        specimen.flags().set(Debugger::Linux::Flag::REDIRECT_INPUT);
        specimen.flags().set(Debugger::Linux::Flag::REDIRECT_OUTPUT);
        specimen.flags().set(Debugger::Linux::Flag::REDIRECT_ERROR);
        specimen.flags().set(Debugger::Linux::Flag::CLOSE_FILES);
        specimen.randomizedAddresses(false);
        for (const EnvValue &env: testCase->env())
            specimen.insertEnvironmentVariable(env.first, env.second);
        debugger = Debugger::Linux::instance(specimen);
    }

    // Run the specimen by single stepping to get the instruction addresses that were executed
    AddressSet executedVas;
    while (!debugger->isTerminated()) {
        executedVas.insert(debugger->executionAddress(Debugger::ThreadId::unspecified()));
        debugger->singleStep(Debugger::ThreadId::unspecified());
    }

    const double rank = -static_cast<double>(executedVas.size()); // more instructions are better (lower rank)
    auto result = TracingResult::instance(rank, executedVas);

    // If the concrete results are a duplicate of other concrete results then mark this test case as not interesting.
    for (TestCaseId otherId: database()->testCases()) {
        TestCase::Ptr other = database()->object(otherId);
        if (other != testCase) {
            if (auto otherResult = as<TracingResult>(database()->readConcreteResult(otherId))) {
                if (result->executedVas() == otherResult->executedVas()) {
                    result->isInteresting(false);
                    break;
                }
            }
        }
    }

    database()->saveConcreteResult(testCase, result);
    return result;
}

int
TracingExecutor::exitStatus(const ConcreteResult::Ptr &result_) {
    auto result = as<TracingResult>(result_);
    ASSERT_not_null(result);
    return result->exitStatus();
}

const AddressSet&
TracingExecutor::executedVas(const ConcreteResult::Ptr &result_) {
    auto result = as<TracingResult>(result_);
    ASSERT_not_null(result);
    return result->executedVas();
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
