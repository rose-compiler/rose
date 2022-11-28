#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/LinuxTraceConcrete.h>

#include <Rose/BinaryAnalysis/Concolic.h>
#include <boost/filesystem.hpp>
#include <boost/serialization/export.hpp>
#include <memory.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

LinuxTraceConcrete::LinuxTraceConcrete(const Database::Ptr &db)
    : ConcreteExecutor(db) {}

LinuxTraceConcrete::~LinuxTraceConcrete() {}

// class method
LinuxTraceConcrete::Ptr
LinuxTraceConcrete::instance(const Database::Ptr &db) {
    return Ptr(new LinuxTraceConcrete(db));
}

ConcreteExecutorResult*
LinuxTraceConcrete::execute(const TestCase::Ptr &testCase) {

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
    auto result = std::make_unique<Result>();
    std::set<rose_addr_t> executedVas;
    while (!debugger->isTerminated()) {
        result->executedVas.insert(debugger->executionAddress(Debugger::ThreadId::unspecified()));
        debugger->singleStep(Debugger::ThreadId::unspecified());
    }

    result->rank(-static_cast<double>(result->executedVas.size())); // neg because lowest ranks execute firstdatabase()->saveConcreteResult(testCase, result.get());

    // If the concrete results are a duplicate of other concrete results then mark this test case as not interesting.
    for (TestCaseId otherId: database()->testCases()) {
        TestCase::Ptr other = database()->object(otherId);
        if (other != testCase) {
            std::unique_ptr<ConcreteExecutorResult> otherResult_ = database()->readConcreteResult(otherId);
            if (auto otherResult = dynamic_cast<const LinuxTraceConcrete::Result*>(otherResult_.get())) {
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
LinuxTraceConcrete::exitStatus(const ConcreteExecutorResult *result_) {
    auto result = dynamic_cast<const Result*>(result_);
    ASSERT_not_null(result);
    return result->exitStatus;
}

const AddressSet&
LinuxTraceConcrete::executedVas(const ConcreteExecutorResult *result_) {
    auto result = dynamic_cast<const Result*>(result_);
    ASSERT_not_null(result);
    return result->executedVas;
}

} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Concolic::LinuxTraceConcrete::Result);

#endif
