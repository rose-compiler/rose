#include <rose.h>
#include <BinaryDebugger.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

namespace P2 = rose::BinaryAnalysis::Partitioner2;
using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::Diagnostics;

Sawyer::Message::Facility mlog;

struct Settings {};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    std::string purpose = "show instructions executed natively";
    std::string description = "Runs the specimen in a debugger and prints each instruction that is executed.";
    Parser parser;
    parser
        .purpose(purpose)
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen} [@v{args}...]")
        .doc("Description", description)
        .with(engine.engineSwitches());

    return parser.parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    ::mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(::mlog);

    // Parse command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, engine, settings);
    if (specimen.empty()) {
        ::mlog[FATAL] <<"no specimen supplied on command-line; see --help\n";
        exit(1);
    }

    // Load specimen into ROSE's simulated memory
    if (!engine.parseContainers(specimen.front())) {
        ::mlog[FATAL] <<"cannot parse specimen binary container\n";
        exit(1);
    }
    Disassembler *disassembler = engine.obtainDisassembler();
    if (!disassembler) {
        ::mlog[FATAL] <<"no disassembler for this architecture\n";
        exit(1);
    }
    const RegisterDescriptor REG_IP = disassembler->instructionPointerRegister();
    ASSERT_require2(REG_IP.is_valid(), "simulation must know what register serves as the instruction pointer");

    // Single-step the specimen natively in a debugger and show each instruction.
    BinaryDebugger debugger(specimen);
    while (!debugger.isTerminated()) {
        uint64_t ip = debugger.readRegister(REG_IP).toInteger();
        uint8_t buf[16];                                // 16 should be large enough for any instruction
        size_t nBytes = debugger.readMemory(ip, sizeof buf, buf);
        if (0 == nBytes) {
            ::mlog[ERROR] <<"cannot read memory at " <<StringUtility::addrToString(ip) <<"\n";
        } else if (SgAsmInstruction *insn = disassembler->disassembleOne(buf, ip, nBytes, ip)) {
            std::cout <<unparseInstructionWithAddress(insn) <<"\n";
        } else {
            ::mlog[ERROR] <<"cannot disassemble instruction at " <<StringUtility::addrToString(ip) <<"\n";
        }
        debugger.singleStep();
    }
    std::cout <<debugger.howTerminated();
}
