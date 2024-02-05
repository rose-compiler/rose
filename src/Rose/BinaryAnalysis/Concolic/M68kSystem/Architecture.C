#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/Architecture.h>

#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Debugger/Gdb.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

// Temporary during testing
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryExit.h>
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryInput.h>

#include <boost/process/search_path.hpp>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

Architecture::Architecture(const std::string &name)
    : Concolic::Architecture(name) {}

Architecture::Architecture(const Database::Ptr &db, TestCaseId tcid)
    : Concolic::Architecture(db, tcid) {}

Architecture::~Architecture() {}

Architecture::Ptr
Architecture::factory() {
    return Ptr(new Architecture("M68kSystem"));
}

Architecture::Ptr
Architecture::instance(const Database::Ptr &db, TestCaseId tcid, const Yaml::Node &config) {
    ASSERT_not_null(db);
    ASSERT_require(tcid);
    auto retval = Ptr(new Architecture(db, tcid));
    retval->config_ = config;
    retval->configureSystemCalls();
    retval->configureSharedMemory(config);
    return retval;
}

Architecture::Ptr
Architecture::instance(const Database::Ptr &db, const TestCase::Ptr &tc, const Yaml::Node &config) {
    return instance(db, db->id(tc), config);
}

Concolic::Architecture::Ptr
Architecture::instanceFromFactory(const Database::Ptr &db, TestCaseId tcid, const Yaml::Node &config) const {
    ASSERT_require(isFactory());
    auto retval = instance(db, tcid, config);
    retval->name(name());
    return retval;
}

bool
Architecture::matchFactory(const Yaml::Node &config) const {
    return config["architecture"].as<std::string>()  == name();
}

P2::Partitioner::Ptr
Architecture::partition(const P2::Engine::Ptr &engine, const std::string &specimenName) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    debug <<"partitioning " <<specimenName;
    return engine->partition(specimenName);
}

void
Architecture::configureSystemCalls() {
    // No system calls on bare metal
}

BS::Dispatcher::Ptr
Architecture::makeDispatcher(const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    auto arch = BinaryAnalysis::Architecture::findByName("nxp-coldfire").orThrow();
    return IS::DispatcherM68k::instance(arch, ops);
}

Sawyer::Optional<rose_addr_t>
Architecture::entryAddress() {
    ASSERT_not_null(partitioner());

    // First, try to get the entry address from the file header.
    if (SgAsmInterpretation *interp = partitioner()->interpretation()) {
        for (SgAsmGenericHeader *header: interp->get_headers()->get_headers()) {
            ASSERT_not_null(header);
            for (const RelativeVirtualAddress &rva: header->get_entryRvas())
                return header->get_baseVa() + rva.rva();
        }
    }

    // Since file headers are not always present (e.g., firmware is often raw memory dumps without an ELF or PE container), we then
    // try to look at the interrupt vector for the m68k.
    if (const auto va = partitioner()->memoryMap()->readUnsigned<uint32_t>(4))
        return ByteOrder::leToHost(*va);

    mlog[WARN] <<"M68kSystem::Architecture::entryAddress: unable to find a starting address\n";
    return Sawyer::Nothing();
}

void
Architecture::load(const boost::filesystem::path &tempDirectory) {
    ASSERT_forbid(isFactory());
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Extract the executable into the target temporary directory
    const auto exeName = tempDirectory / [this]() {
        auto base = boost::filesystem::path(testCase()->specimen()->name()).filename();
        return base.empty() ? "a.out" : base;
    }();
    {
        std::ofstream executable(exeName.c_str(), std::ios_base::binary | std::ios_base::trunc);
        if (!executable) {
            mlog[ERROR] <<"cannot write to " <<exeName <<"\n";
        } else if (testCase()->specimen()->content().empty()) {
            mlog[ERROR] <<"specimen content is empty\n";
        } else {
            executable.write(reinterpret_cast<const char*>(&testCase()->specimen()->content()[0]),
                             testCase()->specimen()->content().size());
        }
    }
    boost::filesystem::permissions(exeName, boost::filesystem::owner_all);

    // Load the executable into QEMU.
    const auto qemuExe = boost::process::search_path("qemu-system-m68k");
    if (qemuExe.empty())
        mlog[ERROR] <<"cannot find qemu-system-m68k in your executable search path ($PATH)\n";

    std::vector<std::string> args;
    args.push_back(qemuExe.string());

    // QEMU args specific to the CPU
    args.push_back("-cpu");
    args.push_back("cfv4e");
    args.push_back("-machine");
    args.push_back("an5206");
    args.push_back("-m");
    args.push_back("4096");

    // General QEMU args
    args.push_back("-display");
    args.push_back("none");
    args.push_back("-s");
    args.push_back("-S");
    args.push_back("-no-reboot");

    // Args to load the specimen
    args.push_back("-kernel");
    args.push_back(exeName.string());
    if (const auto entryVa = entryAddress()) {
        args.push_back("-device");
        args.push_back("loader,cpu-num=0,addr=" + StringUtility::addrToString(*entryVa));
    }

    if (debug) {
        debug <<"executing QEMU emulator for m68k firmware\n"
              <<"  command:";
        for (const std::string &arg: args)
            debug <<" " <<StringUtility::bourneEscape(arg);
        debug <<"\n";
    }
    qemu_ = boost::process::child(args);

    // Start the debugger and attach it to the GDB server in QEMU
    debugger(Debugger::Gdb::instance(Debugger::Gdb::Specimen(exeName, "localhost", 1234)));
    ASSERT_forbid(debugger()->isTerminated());
}

ByteOrder::Endianness
Architecture::memoryByteOrder() {
    ASSERT_forbid(isFactory());
    return ByteOrder::Endianness::ORDER_MSB;
}

std::vector<ExecutionEvent::Ptr>
Architecture::createMemoryRestoreEvents() {
    ASSERT_forbid(isFactory());
    SAWYER_MESG(mlog[WARN]) <<"M68kSystem::Architecture::createMemoryRestoreEvents not implemented\n";
    return {};
}

std::vector<ExecutionEvent::Ptr>
Architecture::createMemoryHashEvents() {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

std::vector<ExecutionEvent::Ptr>
Architecture::createMemoryAdjustEvents(const MemoryMap::Ptr&, rose_addr_t /*insnVa*/) {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

void
Architecture::mapMemory(const AddressInterval&, unsigned /*permissions*/) {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

void
Architecture::unmapMemory(const AddressInterval&) {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

void
Architecture::createInputVariables(const P2::PartitionerConstPtr&, const Emulation::RiscOperators::Ptr&, const SmtSolver::Ptr&) {
    ASSERT_forbid(isFactory());
    SAWYER_MESG(mlog[WARN]) <<"M68kSystem::Architecture::createInputVariables not implemented\n";
}

void
Architecture::systemCall(const P2::PartitionerConstPtr&, const InstructionSemantics::BaseSemantics::RiscOperators::Ptr&) {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
