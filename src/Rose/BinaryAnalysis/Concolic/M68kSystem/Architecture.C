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

    // If the configuration has an "unpack" list, then run those commands sequentially
    if (config_ && config_.exists("unpack")) {
        if (!config_["unpack"].isSequence()) {
            mlog[ERROR] <<"configuration \"unpack\" must be a sequence\n";
            return {};
        }
        for (const auto &pair: config_["unpack"]) {
            const Yaml::Node &unpack = pair.second;
            if (unpack["command"]) {
                const std::string cmd = (boost::format(unpack["command"].as<std::string>()) % specimenName).str();
                debug <<"  executing shell command: " <<cmd <<"\n";
                if (system(cmd.c_str()) != 0)
                    SAWYER_MESG(mlog[ERROR]) <<"cannot execute: " <<cmd <<"\n";
            } else {
                mlog[ERROR] <<"configuration \"unpack\" element has no \"command\"\n";
            }
        }
    }

    // If the configuration has a "load" list then form the partitioner arguments from this list.
    std::vector<std::string> args;
    if (config_.exists("loaders")) {
        if (!config_["loaders"].isSequence()) {
            mlog[ERROR] <<"configuration \"loaders\" must be a sequence\n";
            return {};
        }
        for (const auto &pair: config_["loaders"]) {
            const Yaml::Node &loader = pair.second;
            if (!loader.isMap()) {
                mlog[ERROR] <<"configuration \"loaders\" element must be a map\n";
                return {};
            }
            if (!loader.exists("driver")) {
                mlog[ERROR] <<"configuration \"loaders\" element must have a \"driver\"\n";
                return {};
            }
            const std::string driver = loader["driver"].as<std::string>();
            if ("map" == driver) {
                std::string memoryOffset, memorySize, fileOffset, fileSize, access, fileName;
                for (const auto &kv: loader) {
                    if (kv.first == "memory-offset") {
                        memoryOffset = kv.second.as<std::string>();
                    } else if (kv.first == "memory-size") {
                        memorySize = "+" + kv.second.as<std::string>();
                    } else if (kv.first == "access") {
                        access = "=" + kv.second.as<std::string>();
                    } else if (kv.first == "file-offset") {
                        fileOffset = kv.second.as<std::string>();
                    } else if (kv.first == "file-size") {
                        fileSize = kv.second.as<std::string>();
                    } else if (kv.first == "file-name") {
                        fileName = kv.second.as<std::string>();
                    } else {
                        mlog[ERROR] <<"configuration \"loaders.driver=map\" has unknown key \"" <<kv.first <<"\"\n";
                        return {};
                    }
                }
                std::string s = "map:" +
                                memoryOffset + memorySize + access + ":" +
                                fileOffset + fileSize + ":" + fileName;
                args.push_back(specimenName);

            } else {
                mlog[ERROR] <<"configuration \"loaders.driver=" <<driver <<"\" is unknown\"\n";
                return {};
            }
        }
    } else {
        args.push_back(specimenName);
    }

    if (debug) {
        for (const std::string &arg: args)
            debug <<"  partitioner arg: " <<arg <<"\n";
    }

    return engine->partition(args);
}

void
Architecture::configureSystemCalls() {
    // No system calls on bare metal
}

BS::Dispatcher::Ptr
Architecture::makeDispatcher(const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    return IS::DispatcherM68k::instance(ops,
                                        Emulation::Dispatcher::unwrapEmulationOperators(ops)->wordSizeBits(),
                                        Emulation::Dispatcher::unwrapEmulationOperators(ops)->registerDictionary());
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

#if 1
    std::vector<std::string> args;
    args.push_back(qemuExe.string());
    args.push_back("-display");
    args.push_back("none");
    args.push_back("-s");
    args.push_back("-S");
    args.push_back("-no-reboot");
    args.push_back("-kernel");
    args.push_back(exeName.string());
    if (debug) {
        debug <<"executing QEMU emulator for m68k firmware\n"
              <<"  command:";
        for (const std::string &arg: args)
            debug <<" " <<StringUtility::bourneEscape(arg);
        debug <<"\n";
    }
    qemu_ = boost::process::child(args);
#else
    qemu_ = boost::process::child(qemuExe,
                                  "-display", "none",
                                  "-s", "-S",
                                  "-no-reboot",
                                  "-kernel", exeName.string());
#endif

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
Architecture::createMemoryAdjustEvents(const MemoryMap::Ptr &map, rose_addr_t insnVa) {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

void
Architecture::mapMemory(const AddressInterval &where, unsigned permissions) {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

void
Architecture::unmapMemory(const AddressInterval &where) {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

void
Architecture::createInputVariables(const P2::PartitionerConstPtr &partitioner, const Emulation::RiscOperators::Ptr &ops,
                               const SmtSolver::Ptr &solver) {
    ASSERT_forbid(isFactory());
    SAWYER_MESG(mlog[WARN]) <<"M68kSystem::Architecture::createInputVariables not implemented\n";
}

void
Architecture::systemCall(const P2::PartitionerConstPtr &partitioner,
                         const InstructionSemantics::BaseSemantics::RiscOperators::Ptr &ops) {
    ASSERT_forbid(isFactory());
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
