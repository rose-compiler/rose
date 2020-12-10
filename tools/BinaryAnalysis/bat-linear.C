static const char *purpose = "linear sweep disassembly";
static const char *description =
    "Given a BAT state for a binary specimen, generate an assembly listing by performing a linear sweep of the memory. "
    "In other words, starting at the lowest mapped address, disassemble an instruction and then proceed to the address "
    "the follows the encoding for that instruction.  This method of disassembly does not use instruction semantics to "
    "follow control flow graph edges (for that, use @man{bat-dis}{1} or @man{bat-dis-simple}{1}).";

#include <rose.h>
#include <BinaryUnparserBase.h>                         // rose
#include <CommandLine.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose
#include <TraceSemantics2.h>                            // rose

#include <batSupport.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;

Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format stateFormat;
    rose_addr_t alignment;
    AddressInterval where;
    bool showSemanticTrace;
    bool showSideEffects;

    Settings()
        : stateFormat(SerialIo::BINARY), alignment(1), where(AddressInterval::whole()),
          showSemanticTrace(false), showSideEffects(false) {}
};

// Parse the command-line and return the name of the input file if any (the ROSE binary state).
static boost::filesystem::path
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");

    tool.insert(Switch("alignment")
                .argument("bytes", nonNegativeIntegerParser(settings.alignment))
                .doc("Alignment for instructions. The linear sweep will silently skip over addresses that are not a multiple of"
                     "the alignment.  This is useful on architectures with fixed-size instructions that are always aligned "
                     "because invalid opcode bytes normally return \"instructions\" that occupy one byte of memory. The "
                     "default alignment is " + StringUtility::plural(settings.alignment, "bytes") + "."));

    tool.insert(Switch("where")
                .argument("interval", P2::addressIntervalParser(settings.where))
                .doc("Specifies the range of addresses that should be disassembled.  Only instructions that exist entirely "
                     "within this range are disassembled. The default is the entire address space. " +
                     P2::AddressIntervalParser::docString() + "."));

    CommandLine::insertBooleanSwitch(tool, "trace", settings.showSemanticTrace,
                                     "Shows the steps taken if the instruction were to be executed on a clean machine state.");

    CommandLine::insertBooleanSwitch(tool, "side-effects", settings.showSideEffects,
                                     "Shows the semantics of the instruction by showing it's side effects on a clean "
                                     "machine state.");

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{rba-state}]");
    parser.with(gen).with(tool);
    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
    settings.alignment = std::max(settings.alignment, rose_addr_t(1));
    if (args.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return args.empty() ? boost::filesystem::path("-") : args[0];
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("generating a by-address assembly listing");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    P2::Engine engine;
    Settings settings;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv, engine, settings);
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, settings.stateFormat);
    MemoryMap::Ptr map = partitioner.memoryMap();
    ASSERT_not_null(map);

    BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner.unparser();
    ASSERT_not_null(unparser);
    unparser->settings().function.cg.showing = false;
    unparser->settings().insn.stackDelta.showing = false;

    Facility semanticIo("semantics-trace",
                        Sawyer::Message::StreamSink::instance(std::cout, Sawyer::Message::Prefix::silentInstance()));
    IS::BaseSemantics::DispatcherPtr cpu;
    if (settings.showSideEffects) {
        if (IS::BaseSemantics::RiscOperatorsPtr ops = partitioner.newOperators()) {
            if (settings.showSemanticTrace) {
                IS::TraceSemantics::RiscOperatorsPtr tops = IS::TraceSemantics::RiscOperators::instance(ops);
                ASSERT_not_null(tops);
                tops->showingSubdomain(false);
                tops->showingInstructionVa(false);
                tops->indentation("            |t| ");
                tops->stream(semanticIo[INFO]);
                ops = tops;
            }

            cpu = partitioner.newDispatcher(ops);
            if (!cpu)
                mlog[WARN] <<"no semantics available for this architecture\n";
        }
    }
    IS::BaseSemantics::Formatter fmt;
    fmt.set_line_prefix("            |e| ");
    fmt.set_show_latest_writers(false);

    rose_addr_t va = settings.where.least();
    Sawyer::Optional<rose_addr_t> lastSeenVa;
    while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
        va = alignUp(va, settings.alignment);
        if (!settings.where.isContaining(va))
            break;
        if (lastSeenVa && lastSeenVa.get()+1 != va)
            std::cout <<"\n";

        if (SgAsmInstruction *insn = partitioner.instructionProvider()[va]) {
            unparser->unparse(std::cout, partitioner, insn);
            std::cout <<"\n";

            lastSeenVa = insn->get_address() + insn->get_size() - 1;
            if (lastSeenVa.get() >= settings.where.greatest())
                break;                                  // avoid possible overflow

            va += insn->get_size();

            if (cpu) {
                try {
                    cpu->currentState()->clear();
                    cpu->processInstruction(insn);
                    std::cout <<(*cpu->currentState() + fmt);
                } catch (...) {
                }
            }

        } else {
            // memory is not mapped or is not executable after we aligned upward
            if (va >= settings.where.greatest())
                break;
            ++va;
        }
    }
}
