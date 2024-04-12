#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_LINUX

static const char *purpose = "show instructions executed natively";
static const char *description =
    "Runs the specimen in a debugger and prints each instruction that is executed.";

#include <rose.h>
#include <batSupport.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Debugger/Linux.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionProvider.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <boost/filesystem.hpp>
#include <Sawyer/CommandLine.h>

static const bool WITH_INSTRUCTION_PROVIDER = true;

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;
boost::filesystem::path outputFileName;

namespace {
inline Rose::BinaryAnalysis::InstructionProvider::Ptr
makeInstructionProvider(const P2::Engine::Ptr &engine) {
    using Rose::BinaryAnalysis::Disassembler::Base;
    using Rose::BinaryAnalysis::MemoryMap;
    ASSERT_not_null(engine);

    Disassembler::Base::Ptr disasm = engine->architecture()->newInstructionDecoder();
    MemoryMap::Ptr memmap = engine->memoryMap();

    if (!disasm) {
        ::mlog[FATAL] << "no disassembler for this architecture\n";
        exit(1);
    }

    if (!memmap) {
        ::mlog[FATAL] << "no memory map available\n";
        exit(1);
    }

    return Rose::BinaryAnalysis::InstructionProvider::instance(engine->architecture(), memmap);
}

SgAsmInstruction*
disassembleOne(InstructionProvider::Ptr& instructionProvider, rose_addr_t ip) {
    return instructionProvider->operator[](ip);
}

SgAsmInstruction*
disassembleOne(const Disassembler::Base::Ptr &disassembler, const uint8_t *buf, size_t bufSize, rose_addr_t ip)
    try {
        return disassembler->disassembleOne(buf, ip, bufSize, ip);
    } catch (const Disassembler::Exception &e) {
        mlog[WARN] <<"cannot disassemble instruction at " <<StringUtility::addrToString(ip) <<": " <<e.what() <<"\n";
        return NULL;
    }
}

struct Settings {
    bool listingEachInsn = true;                        // show each instruction as it's executed
};

static Sawyer::CommandLine::Parser
makeSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup out("Output switches");
    out.insert(Switch("output", 'o')
               .argument("file", anyParser(outputFileName))
               .doc("Send the trace to the specified file, which is first truncated if it exists or created if it doesn't exist."));

    Rose::CommandLine::insertBooleanSwitch(out, "trace", settings.listingEachInsn,
                                           "Output one line per executed instruction, containing a hexadecimal address, a colon "
                                           "and space, and the disassembled instruction.");

    return Rose::CommandLine::createEmptyParser(purpose, description)
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen} [@v{args}...]")
        .with(Rose::CommandLine::genericSwitches())
        .with(out);
}

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    return parser.parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&::mlog, "tool");
    mlog.comment("native trace");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Parse command-line
    Settings settings;
    Sawyer::CommandLine::Parser switchParser = makeSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/, P2::Engine::FirstPositionalArguments(1));
    std::vector<std::string> args = parseCommandLine(argc, argv, switchParser);
    if (args.empty()) {
        ::mlog[FATAL] <<"no specimen supplied on command-line; see --help\n";
        exit(1);
    }
    Debugger::Linux::Specimen specimen(args);
    specimen.randomizedAddresses(false);
    specimen.flags().set(Debugger::Linux::Flag::CLOSE_FILES);

    // Trace output goes to either std::cout or some file.
    std::filebuf fb;
    if (!outputFileName.empty())
        fb.open(outputFileName.string().c_str(), std::ios::out);
    std::ostream traceOutput(outputFileName.empty() ? std::cout.rdbuf() : &fb);

    // Load specimen into ROSE's simulated memory
    if (!engine->loadSpecimens(args.front())) {
        ::mlog[FATAL] <<"cannot parse specimen binary container\n";
        exit(1);
    }

    InstructionProvider::Ptr instructionProvider;

    if (WITH_INSTRUCTION_PROVIDER)
        instructionProvider = makeInstructionProvider(engine);

    Architecture::Base::ConstPtr arch = engine->architecture();
    Disassembler::Base::Ptr disassembler = arch->newInstructionDecoder();
    if (!disassembler) {
        ::mlog[FATAL] <<"no disassembler for this architecture\n";
        exit(1);
    }
    const RegisterDescriptor REG_IP = arch->registerDictionary()->instructionPointerRegister();
    ASSERT_forbid2(REG_IP.isEmpty(), "simulation must know what register serves as the instruction pointer");

    // Single-step the specimen natively in a debugger and show each instruction.
    size_t nSteps = 0;                                  // number of instructions executed
    auto debugger = Debugger::Linux::instance(specimen);
    while (!debugger->isTerminated()) {
        ++nSteps;
        uint64_t ip = debugger->readRegister(Debugger::ThreadId::unspecified(), REG_IP).toInteger();

        if (WITH_INSTRUCTION_PROVIDER) {
            if (settings.listingEachInsn) {
                uint8_t buf[16];                        // 16 should be large enough for any instruction
                size_t nBytes = debugger->readMemory(ip, sizeof buf, buf);
                if (0 == nBytes) {
                    ::mlog[ERROR] <<"cannot read memory at " <<StringUtility::addrToString(ip) <<"\n";
                } else if (SgAsmInstruction *insn = disassembleOne(instructionProvider, ip)) {
                    traceOutput <<insn->toString() <<"\n";
                } else {
                    ::mlog[ERROR] <<"cannot disassemble instruction at " <<StringUtility::addrToString(ip) <<"\n";
                }
            }
        } else {
            if (settings.listingEachInsn) {
                uint8_t buf[16];                        // 16 should be large enough for any instruction
                size_t nBytes = debugger->readMemory(ip, sizeof buf, buf);
                if (0 == nBytes) {
                    ::mlog[ERROR] <<"cannot read memory at " <<StringUtility::addrToString(ip) <<"\n";
                } else if (SgAsmInstruction *insn = disassembleOne(disassembler, buf, nBytes, ip)) {
                    traceOutput <<insn->toString() <<"\n";
                } else {
                    ::mlog[ERROR] <<"cannot disassemble instruction at " <<StringUtility::addrToString(ip) <<"\n";
                }
            }
        }

        debugger->singleStep(Debugger::ThreadId::unspecified());
    }

    std::cerr <<debugger->howTerminated() <<"\n";
    std::cerr <<StringUtility::plural(nSteps, "instructions") <<" executed\n";
}

#else

#include <rose.h>
#include <Rose/Diagnostics.h>

#include <iostream>
#include <cstring>

int main(int, char *argv[]) {
    ROSE_INITIALIZE;
    Sawyer::Message::Facility mlog;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog[Rose::Diagnostics::FATAL] <<argv[0] <<": this tool is not available in this ROSE configuration\n";

    for (char **arg = argv+1; *arg; ++arg) {
        if (!strcmp(*arg, "--no-error-if-disabled"))
            return 0;
    }
    return 1;
}

#endif
