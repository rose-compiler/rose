static const char *purpose = "linear sweep disassembly";
static const char *description =
    "Given a BAT state for a binary specimen, generate an assembly listing by performing a linear sweep of the memory. "
    "In other words, starting at the lowest mapped address, disassemble an instruction and then proceed to the address "
    "the follows the encoding for that instruction.  This method of disassembly does not use instruction semantics to "
    "follow control flow graph edges (for that, use @man{bat-dis}{1} or @man{bat-dis-simple}{1}).";

#include <rose.h>
#include <batSupport.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;

Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;
    Address alignment = 1;
    AddressInterval where = AddressInterval::whole();
    bool showSemanticTrace = false;
    bool showSideEffects = false;
    bool countMnemonics = false;
};

// Create a command-line switch parser
static Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
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

    tool.insert(Switch("where", 'a')
                .argument("interval", P2::addressIntervalParser(settings.where))
                .doc("Specifies the range of addresses that should be disassembled.  Only instructions that exist entirely "
                     "within this range are disassembled. The default is the entire address space. " +
                     P2::AddressIntervalParser::docString() + "."));

    CommandLine::insertBooleanSwitch(tool, "count", settings.countMnemonics,
                                     "Count number of times each instruction mnemonica appears and show the counts in a table "
                                     "printed to standard output.");

    CommandLine::insertBooleanSwitch(tool, "trace", settings.showSemanticTrace,
                                     "Shows the steps taken if the instruction were to be executed on a clean machine state.");

    CommandLine::insertBooleanSwitch(tool, "side-effects", settings.showSideEffects,
                                     "Shows the semantics of the instruction by showing it's side effects on a clean "
                                     "machine state.");

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    parser.with(tool).with(gen);

    return parser;
}

// Parse the command-line and return the positional arguments representing the binary specimen
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser, Settings &settings) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    settings.alignment = std::max(settings.alignment, Address(1));
    if (specimen.empty())
        specimen.push_back("-");
    return specimen;
}

/*static*/ std::string
trim(const std::string &s, const std::string &suffix1, const std::string &suffix2 = "") {
    if (!suffix1.empty() && boost::ends_with(s, suffix1) && s.size() > suffix1.size())
        return s.substr(0, s.size() - suffix1.size());
    if (!suffix2.empty() && boost::ends_with(s, suffix2) && s.size() > suffix2.size())
        return s.substr(0, s.size() - suffix2.size());
    return s;
}

static std::string
baseMnemonic(SgAsmInstruction *insn_) {
    ASSERT_not_null(insn_);
    std::string s = insn_->get_mnemonic();

#ifdef ROSE_ENABLE_ASM_AARCH32
    if (auto insn = isSgAsmAarch32Instruction(insn_)) {
        switch (insn->get_condition()) {
            case ARM_CC_INVALID:
                return s;
            case ARM_CC_EQ:
                return trim(s, "eq");
            case ARM_CC_NE:
                return trim(s, "ne");
            case ARM_CC_HS:
                return trim(s, "hs", "cs");
            case ARM_CC_LO:
                return trim(s, "lo", "cc");
            case ARM_CC_MI:
                return trim(s, "mi");
            case ARM_CC_PL:
                return trim(s, "pl");
            case ARM_CC_VS:
                return trim(s, "vs");
            case ARM_CC_VC:
                return trim(s, "vc");
            case ARM_CC_HI:
                return trim(s, "hi");
            case ARM_CC_LS:
                return trim(s, "ls");
            case ARM_CC_GE:
                return trim(s, "ge");
            case ARM_CC_LT:
                return trim(s, "lt");
            case ARM_CC_GT:
                return trim(s, "gt");
            case ARM_CC_LE:
                return trim(s, "le");
            case ARM_CC_AL:
                return s;
        }
        ASSERT_not_reachable("unhandled insn condition: " + boost::lexical_cast<std::string>(insn->get_condition()));
    }
#endif

    return s;
}

static IS::BaseSemantics::RiscOperators::Ptr
newOperators(const Architecture::Base::ConstPtr &arch, const MemoryMap::Ptr &map) {
    auto ops = P2::Semantics::RiscOperators::instance(arch->registerDictionary());
    IS::BaseSemantics::MemoryState::Ptr mem = ops->currentState()->memoryState();
    if (auto ml = as<P2::Semantics::MemoryListState>(mem)) {
        ml->memoryMap(map);
    } else if (auto mm = as<P2::Semantics::MemoryMapState>(mem)) {
        mm->memoryMap(map);
    }
    return ops;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("generating a by-address assembly listing");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Parse the command-line
    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser, settings /*in,out*/);
    Architecture::Base::ConstPtr architecture = engine->architecture();

    // Map the specimen into virtual memory
    MemoryMap::Ptr map;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        map = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.stateFormat)->memoryMap();
    } else {
        map = engine->loadSpecimens(specimen);
    }
    ASSERT_not_null(map);

    // Obtain instruction parser and unparser
    auto insns = InstructionProvider::instance(architecture, map);
    BinaryAnalysis::Unparser::Base::Ptr unparser = architecture->newUnparser();
    ASSERT_not_null(unparser);
    unparser->settings().function.cg.showing = false;
    unparser->settings().insn.stackDelta.showing = false;

    Facility semanticIo("semantics-trace",
                        Sawyer::Message::StreamSink::instance(std::cout, Sawyer::Message::Prefix::silentInstance()));
    IS::BaseSemantics::Dispatcher::Ptr cpu;
    if (settings.showSideEffects) {
        if (IS::BaseSemantics::RiscOperators::Ptr ops = newOperators(architecture, map)) {
            if (settings.showSemanticTrace) {
                IS::TraceSemantics::RiscOperators::Ptr tops = IS::TraceSemantics::RiscOperators::instance(ops);
                ASSERT_not_null(tops);
                tops->showingSubdomain(false);
                tops->showingInstructionVa(false);
                tops->indentation("            |t| ");
                tops->stream(semanticIo[INFO]);
                ops = tops;
            }

            cpu = architecture->newInstructionDispatcher(ops);
            if (!cpu)
                mlog[WARN] <<"no semantics available for this architecture\n";
        }
    }
    IS::BaseSemantics::Formatter fmt;
    fmt.set_line_prefix("            |e| ");
    fmt.set_show_latest_writers(false);

    std::map<std::string, size_t> histogram;
    Address va = settings.where.least();
    Sawyer::Optional<Address> lastSeenVa;
    while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
        va = alignUp(va, settings.alignment);
        if (!settings.where.contains(va))
            break;
        if (lastSeenVa && lastSeenVa.get()+1 != va)
            std::cout <<"\n";

        if (SgAsmInstruction *insn = (*insns)[va]) {
            unparser->unparse(std::cout, insn);
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

            if (settings.countMnemonics && !architecture->isUnknown(insn))
                ++histogram[baseMnemonic(insn)];

        } else {
            // memory is not mapped or is not executable after we aligned upward
            if (va >= settings.where.greatest())
                break;
            ++va;
        }
    }

    if (settings.countMnemonics) {
        using Pair = std::pair<std::string, size_t>;
        std::vector<Pair> sorted(histogram.begin(), histogram.end());
        std::sort(sorted.begin(), sorted.end(), [](const Pair &a, const Pair &b) {
                return a.second > b.second;
            });
        size_t total = 0;
        for (const auto &pair: sorted)
            total += pair.second;

        size_t runningTotal = 0;
        Rose::FormattedTable table;
        table.columnHeader(0, 0, "Mnemonic");
        table.columnHeader(0, 1, "Count");
        table.columnHeader(0, 2, "Mnemonic\nPercent");
        table.columnHeader(0, 3, "Cumulative\nPercent");
        Rose::FormattedTable::CellProperties right;
        right.alignment(Rose::FormattedTable::Alignment::RIGHT);

        for (size_t i = 0; i < sorted.size(); ++i) {
            runningTotal += sorted[i].second;
            table.insert(i, 0, sorted[i].first);
            table.insert(i, 1, sorted[i].second, right);
            table.insert(i, 2, (boost::format("%1.3f%%") %(100.0 * sorted[i].second / total)).str(), right);
            table.insert(i, 3, (boost::format("%1.3f%%") %(100.0 * runningTotal / total)).str(), right);
        }

        table.insert(sorted.size(), 0, "Total");
        table.insert(sorted.size(), 1, total, right);
        table.insert(sorted.size(), 2, "100.000%", right);
        table.insert(sorted.size(), 3, "100.000%", right);
        std::cout <<table;
    }
}
