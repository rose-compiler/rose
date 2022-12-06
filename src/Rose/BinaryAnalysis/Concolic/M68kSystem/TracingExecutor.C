#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingExecutor.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingResult.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Debugger/Gdb.h>
#include <Rose/FileSystem.h>

#include <Sawyer/FileSystem.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

TracingExecutor::TracingExecutor(const Database::Ptr &db)
    : Concolic::ConcreteExecutor(db) {}

TracingExecutor::~TracingExecutor() {}

TracingExecutor::Ptr
TracingExecutor::instance(const Database::Ptr &db) {
    return Ptr(new TracingExecutor(db));
}

boost::process::child
TracingExecutor::startQemu(const boost::filesystem::path &firmwareName) {
    const boost::filesystem::path qemuBaseName = "qemu-m68k-system";
    const boost::filesystem::path qemuName = boost::process::search_path(qemuBaseName);
    if (qemuName.empty())
        throw Exception("cannot find " + qemuBaseName.string() + " in search path");

    boost::process::child qemu(qemuName,
                               "-m", "4096",
                               "-display", "none",
                               "-kernel", firmwareName.string(),
                               "-S",                    // freeze CPU at startup
                               "-gdb", "tcp::1234",
                               "-no-reboot", "-no-shutdown");
    return qemu;
}

Concolic::ConcreteResult::Ptr
TracingExecutor::execute(const TestCase::Ptr &tc) {
    ASSERT_not_null(tc);

    // Start QEMU asynchronously. QEMU will load the test case into memory and stop for debugging using the GDB server at the
    // first instruction.
    const Specimen::Ptr specimen = tc->specimen();
    Sawyer::FileSystem::TemporaryDirectory tempDir;
    const FileSystem::Path firmwareName = tempDir.name() / "firmware";
    Rose::FileSystem::writeFile(firmwareName, specimen->content());
    boost::process::child qemu = startQemu(firmwareName);

    // Attach to the GDB server provided by QEMU
    auto debugger = Debugger::Gdb::instance(Debugger::Gdb::Specimen(firmwareName, "localhost", 1234));
    while (!debugger->isTerminated()) {
        rose_addr_t va = debugger->executionAddress(Debugger::ThreadId::unspecified());
        std::cerr <<"m68k firmware executing at " <<StringUtility::addrToString(va) <<"\n";
        debugger->singleStep(Debugger::ThreadId::unspecified());
    }
    std::cerr <<"m68k firmware " <<debugger->howTerminated() <<"\n";
    debugger->detach();

    qemu.wait();

    ASSERT_not_implemented("[Robb Matzke 2022-12-02]");
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
