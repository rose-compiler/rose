#include <rose.h>
#include <rosePublicConfig.h>

#include <Disassembler.h>
#include <DispatcherM68k.h>
#include <Partitioner2/Engine.h>
#include <SymbolicSemantics2.h>
#include <TraceSemantics2.h>

#include <map>
#include <Sawyer/Assert.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <string>

using namespace rose;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;

// Round X up to the next multiple of ALIGNMENT
static rose_addr_t
alignUp(rose_addr_t x, rose_addr_t alignment)
{
    return alignment>1 ? ((x+alignment-1)/alignment)*alignment : x;
}

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    rose_addr_t startVa;
    rose_addr_t alignment;
    bool runSemantics;
    Settings(): startVa(0), alignment(1), runSemantics(false) {}
};

// Describe and parse the command-line
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings)
{
    using namespace Sawyer::CommandLine;

    std::string purpose = "disassembles files one address at a time";
    std::string description =
        "This program is a very simple disassembler that tries to disassemble in instruction at each executable "
        "address, one instruction after the next.";

    // The parser is the same as that created by Engine::commandLineParser except we don't need any partitioning switches since
    // this tool doesn't partition.
    Parser parser;
    parser
        .purpose(purpose)
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("Description", description)
        .doc("Specimens", engine.specimenNameDocumentation())
        .with(engine.engineSwitches())
        .with(engine.loaderSwitches())
        .with(engine.disassemblerSwitches());

    SwitchGroup switches("Tool-specific switches");
    switches.insert(Switch("start")
                    .argument("virtual-address", nonNegativeIntegerParser(settings.startVa))
                    .doc("Address at which disassembly will start.  The default is to start at the lowest mapped "
                         "address."));
    switches.insert(Switch("alignment")
                    .argument("align", nonNegativeIntegerParser(settings.alignment))
                    .doc("Alignment for instructions.  The default is 1 (no alignment).  Values larger than one will "
                         "cause each candidate address to be rounded up to the next multiple of @v{align}.  If this "
                         "rounding up causes addresses after a valid instruction to be skipped then a warning is printed. "
                         "No warning is printed if the alignment skips addresses after a disassembly failure."));
    switches.insert(Switch("semantics")
                    .intrinsicValue(true, settings.runSemantics)
                    .doc("Run semantics for each basic block. This is only useful to debug instruction semantics."));
    switches.insert(Switch("no-semantics")
                    .key("semantics")
                    .intrinsicValue(false, settings.runSemantics)
                    .hidden(true));

    return parser.with(switches).parse(argc, argv).apply().unreachedArgs();
}

int main(int argc, char *argv[])
{
    Diagnostics::initialize();
    ::mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(::mlog);

    // Parse the command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, engine, settings);

    // Load the speciem as raw data or an ELF or PE container
    MemoryMap map = engine.loadSpecimens(specimenNames);
    map.dump(::mlog[INFO]);
    map.dump(std::cout);
    Disassembler *disassembler = engine.obtainDisassembler();

    // Obtain an unparser suitable for this disassembler
    AsmUnparser unparser;
    unparser.set_registers(disassembler->get_registers());

    // Build semantics framework; only used when settings.runSemantics is set
    BaseSemantics::DispatcherPtr dispatcher;
    if (settings.runSemantics) {
        BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(disassembler->get_registers());
        ops = TraceSemantics::RiscOperators::instance(ops);
        dispatcher = DispatcherM68k::instance(ops, disassembler->get_wordsize()*8);
        dispatcher->currentState()->memoryState()->set_byteOrder(ByteOrder::ORDER_MSB);
    }

    // Disassemble at each valid address, and show disassembly errors
    rose_addr_t va = settings.startVa;
    while (map.atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
        va = alignUp(va, settings.alignment);
        try {
            SgAsmInstruction *insn = disassembler->disassembleOne(&map, va);
            ASSERT_not_null(insn);
            unparser.unparse(std::cout, insn);

            if (settings.runSemantics) {
                if (isSgAsmM68kInstruction(insn)) {
                    bool skipThisInstruction = false;
#if 0 // [Robb P. Matzke 2014-07-29]
                    switch (isSgAsmM68kInstruction(insn)->get_kind()) {
                        case m68k_cpusha:
                        case m68k_cpushl:
                        case m68k_cpushp:
                            std::cout <<"    No semantics yet for privileged instructions\n";
                            skipThisInstruction = true;
                            break;

                        case m68k_fbeq:
                        case m68k_fbne:
                        case m68k_fboge:
                        case m68k_fbogt:
                        case m68k_fbule:
                        case m68k_fbult:
                        case m68k_fcmp:
                        case m68k_fdabs:
                        case m68k_fdadd:
                        case m68k_fddiv:
                        case m68k_fdiv:
                        case m68k_fdmove:
                        case m68k_fdmul:
                        case m68k_fdneg:
                        case m68k_fdsqrt:
                        case m68k_fdsub:
                        case m68k_fintrz:
                        case m68k_fmove:
                        case m68k_fmovem:
                        case m68k_fsadd:
                        case m68k_fsdiv:
                        case m68k_fsmove:
                        case m68k_fsmul:
                        case m68k_fsneg:
                        case m68k_fssub:
                        case m68k_ftst:
                            std::cout <<"    No semantics yet for floating-point instructions\n";
                            skipThisInstruction = true;
                            break;

                        case m68k_nbcd:
                        case m68k_rtm:
                        case m68k_movep:
                            std::cout <<"    No semantics yet for this odd instruction\n";
                            skipThisInstruction = true;
                            break;

                        default:
                            break;
                    }
#endif

                    if (!skipThisInstruction) {
                        //ops->currentState()->clear();
                        dispatcher->processInstruction(insn);
                        std::ostringstream ss;
                        ss <<*dispatcher->currentState();
                        std::cout <<StringUtility::prefixLines(ss.str(), "    ") <<"\n";
                    }
                }
            }


            va += insn->get_size();
            if (0 != va % settings.alignment)
                std::cerr <<StringUtility::addrToString(va) <<": invalid alignment\n";
#if 0 // [Robb P. Matzke 2014-06-19]: broken
            deleteAST(insn);
#endif
        } catch (const Disassembler::Exception &e) {
            std::cerr <<StringUtility::addrToString(va) <<": " <<e.what() <<"\n";
            ++va;
        }
    }

    exit(0);
}
