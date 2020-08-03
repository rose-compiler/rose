#include <sage3basic.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <BinaryConcolic.h>
#include <boost/filesystem.hpp>
#include <memory.h>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Concolic::LinuxTraceExecutor::Result);
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

LinuxTraceExecutor::LinuxTraceExecutor(const Database::Ptr &db)
    : ConcreteExecutor(db) {}

// class method
LinuxTraceExecutor::Ptr
LinuxTraceExecutor::instance(const Database::Ptr &db) {
    return Ptr(new LinuxTraceExecutor(db));
}

ConcreteExecutor::Result*
LinuxTraceExecutor::execute(const TestCase::Ptr &testCase) {

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
        Debugger::Specimen specimen(exeName);
        specimen.arguments(testCase->args());
        specimen.eraseAllEnvironmentVariables();
        specimen.flags() = Debugger::REDIRECT_INPUT | Debugger::REDIRECT_OUTPUT | Debugger::REDIRECT_ERROR | Debugger::CLOSE_FILES;
        specimen.randomizedAddresses(false);
        for (const EnvValue &env: testCase->env())
            specimen.insertEnvironmentVariable(env.first, env.second);
        debugger = Debugger::instance(specimen);
    }

    // Run the specimen by single stepping to get the instruction addresses that were executed
    auto result = std::make_unique<Result>();
    std::set<rose_addr_t> executedVas;
    while (!debugger->isTerminated()) {
        result->executedVas.insert(debugger->executionAddress());
        debugger->singleStep();
    }
    result->rank(-static_cast<double>(result->executedVas.size())); // neg because lowest ranks execute first
    database()->saveConcreteResult(testCase, result.get());

    // If the concrete results are a duplicate of other concrete results then mark this test case as not interesting.
    for (TestCaseId otherId: database()->testCases()) {
        TestCase::Ptr other = database()->object(otherId);
        if (other != testCase) {
            std::unique_ptr<ConcreteExecutor::Result> otherResult_ = database()->readConcreteResult(otherId);
            if (auto otherResult = dynamic_cast<const LinuxTraceExecutor::Result*>(otherResult_.get())) {
                if (result->executedVas == otherResult->executedVas) {
                    result->isInteresting(false);
                    break;
                }
            }
        }
    }

    database()->saveConcreteResult(testCase, result.get());
    return result.release();
}

int
LinuxTraceExecutor::exitStatus(const ConcreteExecutor::Result *result_) {
    auto result = dynamic_cast<const Result*>(result_);
    ASSERT_not_null(result);
    return result->exitStatus;
}

const AddressSet&
LinuxTraceExecutor::executedVas(const ConcreteExecutor::Result *result_) {
    auto result = dynamic_cast<const Result*>(result_);
    ASSERT_not_null(result);
    return result->executedVas;
}

} // namespace
} // namespace
} // namespace
#endif
