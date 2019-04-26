static const char *purpose = "show instructions executed natively";
static const char *description =
    "Runs the specimen in a debugger and prints each instruction that is executed.";

#include <rose.h>

#include <BinaryDebugger.h>
#include <boost/filesystem.hpp>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics;

Sawyer::Message::Facility mlog;
boost::filesystem::path outputFileName;

struct Settings {};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup out("Output switches");
    out.insert(Switch("output", 'o')
               .argument("file", anyParser(outputFileName))
               .doc("Send the trace to the specified file, which is first truncated if it exists or created if it doesn't exist."));

    Parser parser;
    parser
        .purpose(purpose)
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen} [@v{args}...]")
        .doc("Description", description)
        .with(engine.engineSwitches())
        .with(out);

    return parser.parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&::mlog, "tool");

    // Parse command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, engine, settings);
    if (specimen.empty()) {
        ::mlog[FATAL] <<"no specimen supplied on command-line; see --help\n";
        exit(1);
    }

    // Trace output goes to either std::cout or some file.
    std::filebuf fb;
    if (!outputFileName.empty())
        fb.open(outputFileName.native().c_str(), std::ios::out);
    std::ostream traceOutput(outputFileName.empty() ? std::cout.rdbuf() : &fb);

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
    BinaryDebugger debugger(specimen, BinaryDebugger::CLOSE_FILES);
    while (!debugger.isTerminated()) {
        uint64_t ip = debugger.readRegister(REG_IP).toInteger();
        uint8_t buf[16];                                // 16 should be large enough for any instruction
        size_t nBytes = debugger.readMemory(ip, sizeof buf, buf);
        if (0 == nBytes) {
            ::mlog[ERROR] <<"cannot read memory at " <<StringUtility::addrToString(ip) <<"\n";
        } else if (SgAsmInstruction *insn = disassembler->disassembleOne(buf, ip, nBytes, ip)) {
            traceOutput <<insn->toString() <<"\n";
        } else {
            ::mlog[ERROR] <<"cannot disassemble instruction at " <<StringUtility::addrToString(ip) <<"\n";
        }
        debugger.singleStep();
    }
    std::cout <<debugger.howTerminated();
}
