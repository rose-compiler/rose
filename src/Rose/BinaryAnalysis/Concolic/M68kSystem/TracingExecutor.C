#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingExecutor.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingResult.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Debugger/Gdb.h>
#include <Rose/FileSystem.h>

#include <Sawyer/FileSystem.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

TracingExecutor::TracingExecutor(const std::string &name)
    : Super(name) {}

TracingExecutor::TracingExecutor(const Database::Ptr &db)
    : Super(db) {}

TracingExecutor::~TracingExecutor() {}

TracingExecutor::Ptr
TracingExecutor::factory() {
    return Ptr(new TracingExecutor("M68kSystem::Tracing"));
}

TracingExecutor::Ptr
TracingExecutor::instance(const Database::Ptr &db) {
    return Ptr(new TracingExecutor(db));
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

boost::process::child
TracingExecutor::startQemu(const boost::filesystem::path &firmwareName) {
    ASSERT_forbid(isFactory());
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    const boost::filesystem::path qemuBaseName = "qemu-system-m68k";
    const boost::filesystem::path qemuName = boost::process::search_path(qemuBaseName);
    if (qemuName.empty())
        throw Exception("cannot find " + qemuBaseName.string() + " in search path");

    std::vector<std::string> command{
        qemuName.string(),
        "-m", "4096",
        "-display", "none",
        "-kernel", firmwareName.string(),
        "-S",                                           // freeze CPU at startup
        "-gdb", "tcp::1234",
        "-no-reboot", "-no-shutdown"};
    if (debug) {
        debug <<"starting QEMU with this command-line:";
        for (const std::string &arg: command)
            debug <<" " <<StringUtility::bourneEscape(arg);
        debug <<"\n";
    }
    boost::process::child qemu(command);
    return qemu;
}

Concolic::ConcreteResult::Ptr
TracingExecutor::execute(const TestCase::Ptr &testCase) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(testCase);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"executing " <<testCase->printableName(database()) <<" in " <<name() <<" concrete executor\n";

    // Start QEMU asynchronously. QEMU will load the test case into memory and stop for debugging using the GDB server at the
    // first instruction.
    const Specimen::Ptr specimen = testCase->specimen();
    Sawyer::FileSystem::TemporaryDirectory tempDir;
    const FileSystem::Path firmwareName = tempDir.name() / "firmware";
    Rose::FileSystem::writeFile(firmwareName, specimen->content());
    boost::process::child qemu = startQemu(firmwareName);

    // Results, incrementally constructed.
    auto result = TracingResult::instance(0.0);

    // Attach to the GDB server provided by QEMU
    static const size_t maxInsnsExecuted = 100;         // arbitrary, make this a setting
    size_t nInsnsExecuted = 0;
    auto debugger = Debugger::Gdb::instance(Debugger::Gdb::Specimen(firmwareName, "localhost", 1234));
    while (!debugger->isTerminated()) {
        Address va = debugger->executionAddress(Debugger::ThreadId::unspecified());
        result->trace().append(va);
        if (++nInsnsExecuted >= maxInsnsExecuted) {
            SAWYER_MESG(debug) <<"maximum number of allowed instructions reached (" <<maxInsnsExecuted <<");"
                               <<" terminating executor\n";
            debugger->terminate();
        } else {
            debugger->singleStep(Debugger::ThreadId::unspecified());
        }
    }
    SAWYER_MESG(debug) <<"firmware emulation " <<debugger->howTerminated() <<"\n";
    if (debugger->isAttached())
        debugger->detach();

    qemu.wait();

    // The more distinct instructions executed, the better
    double rank = -(double)result->trace().nLabels();
    SAWYER_MESG(debug) <<"concrete execution rank = " <<rank <<"\n";
    result->rank(rank);

    database()->saveConcreteResult(testCase, result);
    return result;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
