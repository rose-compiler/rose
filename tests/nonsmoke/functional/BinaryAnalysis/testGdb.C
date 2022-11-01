#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB
#include <rose.h>

#include <Rose/BinaryAnalysis/Debugger/Gdb.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

static void
test(const Debugger::Ptr &gdb) {
    // Assume we're debugging M68k, and obtain the program counter and data register zero
    const RegisterDescriptor REG_D0 = gdb->registerDictionary()->findOrThrow("d0");
    const RegisterDescriptor REG_FP = gdb->disassembler()->stackFrameRegister();

    // Read register `d0` that has the count
    const auto d0 = gdb->readRegister(Debugger::ThreadId::unspecified(), REG_D0).toInteger();

    // Also read the memory to which `d0` is spilled
    const auto fp = gdb->readRegister(Debugger::ThreadId::unspecified(), REG_FP).toInteger();
    const rose_addr_t va = fp - 4;
    const auto varN = gdb->readMemory(va, 4, ByteOrder::ORDER_MSB).toInteger();

    // Show and check results
    std::cerr <<(boost::format("d0 = %10u (0x%x); n = %10u (0x%x)\n") % d0 % d0 % varN % varN);
    ASSERT_always_require(d0 == varN);

    // Try writing something
    if (d0 == 103) {
        std::cerr <<"setting bits 8-16\n";

        // Write to part of the `d0` register
        const auto REG_D0_B1 = RegisterDescriptor(REG_D0.majorNumber(), REG_D0.minorNumber(), 8, 8);
        gdb->writeRegister(Debugger::ThreadId::unspecified(), REG_D0_B1, 0xff);
        const auto d0b = gdb->readRegister(Debugger::ThreadId::unspecified(), REG_D0).toInteger();
        ASSERT_always_require(0xff00 == (d0b & 0xff00));

        // Write a byte to the `n` variable
        const uint8_t fillByte = 0xff;
        gdb->writeMemory(va + 2, 1, &fillByte);
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    // Parse command-line and find fies
    if (argc != 2) {
        mlog[FATAL] <<"incorrect usage; see source code\n";
        return 1;
    }
    const std::string exeName = argv[1];
    const std::string gdbName = "gdb-multiarch";
    if (boost::process::search_path(gdbName).empty()) {
        mlog[INFO] <<"test skipped because \"" <<gdbName <<"\" is not found in the executable search path\n";
        return 0;
    }
    const std::string qemuName = "qemu-system-m68k";
    const boost::filesystem::path qemuPath = boost::process::search_path(qemuName);
    if (qemuPath.empty()) {
        mlog[INFO] <<"test skipped because \"" <<qemuName <<"\" is not found in the executable search path\n";
        return 0;
    }

    // Start the m68k emulator
    boost::process::child qemu(qemuPath, "-s", "-S", "--kernel", exeName);
    ASSERT_require(qemu.running());

    // Start the debugger
    Debugger::Gdb::Specimen specimen;
    specimen.gdbName = gdbName;
    specimen.executable = exeName;
    specimen.remote.host = "localhost";
    specimen.remote.port = 1234;
    auto gdb = Debugger::Gdb::instance(specimen);

    // Set a breakpoint at the end of the loop (BRA back to beginning). Since this is a singleton break point, it will be
    // handled by GDB and will be quite fast. After a few hundred iterations through the loop, add a regional breakpoint which
    // will need to be handled by ROSE and will therefore slow down the execution.
    gdb->setBreakPoint(0x800000e6);
    for (unsigned i = 0; gdb->isAttached(); ++i) {
        gdb->runToBreakPoint(Debugger::ThreadId::unspecified());
        ASSERT_require(0x800000e6 == gdb->executionAddress(Debugger::ThreadId::unspecified()));
        test(gdb);
        if (350 == i)
            gdb->setBreakPoint(AddressInterval::baseSize(0, 256));

        if (700 == i)
            gdb->terminate();
    }
    std::cerr <<"terminated: " <<gdb->howTerminated() <<"\n";

    // Terminate the emulator (it's probably already terminated)
    qemu.terminate();
    qemu.wait();
}

#else

#include <iostream>

int
main() {
    std::cerr <<"test not enabled in this configuration of ROSE\n";
}

#endif
