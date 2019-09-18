static const char *purpose = "show instructions executed natively";
static const char *description =
    "Runs the specimen in a debugger and prints each instruction that is executed.";

#include <rose.h>
#include <BinaryDebugger.h>                             // rose
#include <CommandLine.h>                                // rose
#include <Diagnostics.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose

#include <boost/filesystem.hpp>
#include <Sawyer/CommandLine.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;
boost::filesystem::path outputFileName;

struct Settings {
    bool listingEachInsn;                               // show each instruction as it's executed

    Settings()
        : listingEachInsn(true) {}
};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup out("Output switches");
    out.insert(Switch("output", 'o')
               .argument("file", anyParser(outputFileName))
               .doc("Send the trace to the specified file, which is first truncated if it exists or created if it doesn't exist."));

    Rose::CommandLine::insertBooleanSwitch(out, "trace", settings.listingEachInsn,
                                           "Output one line per executed instruction, containing a hexadecimal address, a colon "
                                           "and space, and the disassembled instruction.");

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

static SgAsmInstruction*
disassembleOne(Disassembler *disassembler, const uint8_t *buf, size_t bufSize, rose_addr_t ip) {
    try {
        return disassembler->disassembleOne(buf, ip, bufSize, ip);
    } catch (const Disassembler::Exception &e) {
        mlog[WARN] <<"cannot disassemble instruction at " <<StringUtility::addrToString(ip) <<": " <<e.what() <<"\n";
        return NULL;
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&::mlog, "tool");

    // Parse command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, engine, settings);
    if (args.empty()) {
        ::mlog[FATAL] <<"no specimen supplied on command-line; see --help\n";
        exit(1);
    }
    Debugger::Specimen specimen(args);
    specimen.flags().set(Debugger::CLOSE_FILES);

    // Trace output goes to either std::cout or some file.
    std::filebuf fb;
    if (!outputFileName.empty())
        fb.open(outputFileName.native().c_str(), std::ios::out);
    std::ostream traceOutput(outputFileName.empty() ? std::cout.rdbuf() : &fb);

    // Load specimen into ROSE's simulated memory
    if (!engine.parseContainers(args.front())) {
        ::mlog[FATAL] <<"cannot parse specimen binary container\n";
        exit(1);
    }
    Disassembler *disassembler = engine.obtainDisassembler();
    if (!disassembler) {
        ::mlog[FATAL] <<"no disassembler for this architecture\n";
        exit(1);
    }
    const RegisterDescriptor REG_IP = disassembler->instructionPointerRegister();
    ASSERT_forbid2(REG_IP.isEmpty(), "simulation must know what register serves as the instruction pointer");

    // Single-step the specimen natively in a debugger and show each instruction.
    size_t nSteps = 0;                                  // number of instructions executed
    Debugger::Ptr debugger = Debugger::instance(specimen);
    while (!debugger->isTerminated()) {
        ++nSteps;
        uint64_t ip = debugger->readRegister(REG_IP).toInteger();

        if (settings.listingEachInsn) {
            uint8_t buf[16];                            // 16 should be large enough for any instruction
            size_t nBytes = debugger->readMemory(ip, sizeof buf, buf);
            if (0 == nBytes) {
                ::mlog[ERROR] <<"cannot read memory at " <<StringUtility::addrToString(ip) <<"\n";
            } else if (SgAsmInstruction *insn = disassembleOne(disassembler, buf, nBytes, ip)) {
                traceOutput <<insn->toString() <<"\n";
            } else {
                ::mlog[ERROR] <<"cannot disassemble instruction at " <<StringUtility::addrToString(ip) <<"\n";
            }
        }

        debugger->singleStep();
    }

    std::cerr <<debugger->howTerminated() <<"\n";
    std::cerr <<StringUtility::plural(nSteps, "instructions") <<" executed\n";
}
