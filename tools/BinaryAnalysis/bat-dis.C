static const char *purpose = "create an assembly listing";
static const char *description =
    "Given a BAT state for a binary specimen, generate a pseudo-assembly listing on standard output. The listing is not "
    "intended to be input to an assembler, but rather for human consumption.\n\n"

    "This tool reads the binary analysis state file provided as a command-line positional argument, or standard input if "
    "the name is \"-\" (a single hyphen) no file name is specified. The standard input mode works only on those operating "
    "systems whose standard input is opened in binary mode, such as Unix-like systems.";

#include <rose.h>
#include <BinaryNoOperation.h>                                  // rose
#include <BinaryUnparserBase.h>                                 // rose
#include <CommandLine.h>                                        // rose
#include <Partitioner2/Engine.h>                                // rose
#include <Sawyer/Stopwatch.h>

#include <batSupport.h>
#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;

struct Settings {
    std::set<std::string> functionNames;
    std::set<rose_addr_t> addresses;
    BinaryAnalysis::Unparser::Settings unparser;
    bool discardNops;
    SerialIo::Format stateFormat;

    Settings()
        : discardNops(false), stateFormat(SerialIo::BINARY) {}
};

// Parses the command-line and returns the name of the input file if any (the ROSE binary state).
boost::filesystem::path
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup sel("Selection switches");
    sel.name("sel");

    sel.insert(Switch("function")
               .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to the specified functions. The @v{name_or_address} can be the name of a function as "
                    "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                    "If a value is ambiguous, it's first treated as a name and if no function has that name it's then "
                    "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                    "be specified per occurrence."));

    sel.insert(Switch("containing")
               .argument("addresses", listParser(nonNegativeIntegerParser(settings.addresses), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to functions that contain one of the specified addresses. This switch may occur "
                    "multiple times and multiple comma-separate addresses may be specified per occurrence."));

    SwitchGroup out = BinaryAnalysis::Unparser::commandLineSwitches(settings.unparser);
    Rose::CommandLine::insertBooleanSwitch(out, "discard-nops", settings.discardNops,
                                               "Omit instructions that are part of a sequence that has no effect.");

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{rba-state}]");
    parser.with(gen).with(sel).with(out);
    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();
    if (input.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return input.empty() ? boost::filesystem::path("-") : input[0];
}

// A specialized unparser that omits instructions that are effectively no-ops.
class MyUnparser: public BinaryAnalysis::Unparser::Base {
protected:
    explicit MyUnparser(const BinaryAnalysis::Unparser::Base::Ptr &next)
        : BinaryAnalysis::Unparser::Base(next) {
        ASSERT_not_null(next);
    }

public:
    typedef Sawyer::SharedPointer<MyUnparser> Ptr;

    static Ptr instance(const BinaryAnalysis::Unparser::Base::Ptr &next) { return Ptr(new MyUnparser(next)); }
    virtual BinaryAnalysis::Unparser::Base::Ptr copy() const ROSE_OVERRIDE { return Ptr(new MyUnparser(nextUnparser()->copy())); }
    virtual const BinaryAnalysis::Unparser::Settings& settings() const ROSE_OVERRIDE { return nextUnparser()->settings(); }
    virtual BinaryAnalysis::Unparser::Settings& settings() ROSE_OVERRIDE { return nextUnparser()->settings(); }

    void emitBasicBlockBody(std::ostream &out, const P2::BasicBlockPtr &bb, BinaryAnalysis::Unparser::State &state) const ROSE_OVERRIDE {
        ASSERT_not_null(bb);
        InstructionSemantics2::BaseSemantics::RiscOperatorsPtr ops = state.partitioner().newOperators();
        if (InstructionSemantics2::BaseSemantics::DispatcherPtr cpu = state.partitioner().newDispatcher(ops)) {
            // Find the largest non-overlapping instruction sequences that are effectively no-ops
            NoOperation nopAnalyzer(cpu);
            nopAnalyzer.initialStackPointer(0xceed0000); // arbitrary
            NoOperation::IndexIntervals nopIndexes = nopAnalyzer.findNoopSubsequences(bb->instructions());
            nopIndexes = nopAnalyzer.largestEarliestNonOverlapping(nopIndexes);

            // Select only those instructions that are not no-ops
            std::vector<SgAsmInstruction*> instructions;
            for (size_t i=0; i<bb->nInstructions(); ++i) {
                bool isGoodInstruction = true;
                BOOST_FOREACH (const NoOperation::IndexInterval &interval, nopIndexes) {
                    if (interval.isContaining(i)) {
                        isGoodInstruction = false;
                        break;
                    }
                }
                if (isGoodInstruction)
                    instructions.push_back(bb->instructions()[i]);
            }

            // Output
            if (instructions.empty()) {
                state.thisIsBasicBlockFirstInstruction();
                state.thisIsBasicBlockLastInstruction();
                state.frontUnparser().emitLinePrefix(out, state);
                out <<StringUtility::addrToString(bb->address()) <<": no instructions\n";
            } else {
                state.thisIsBasicBlockFirstInstruction();
                BOOST_FOREACH (SgAsmInstruction *insn, instructions) {
                    if (insn == instructions.back())
                        state.thisIsBasicBlockLastInstruction();
                    state.frontUnparser().emitInstruction(out, insn, state);
                    out <<"\n";
                    state.nextInsnLabel("");
                }
            }
        } else {
            ASSERT_not_null(nextUnparser());
            nextUnparser()->emitBasicBlockBody(out, bb, state);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("generating assembly listings");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    P2::Engine engine;
    Settings settings;
    if (boost::ends_with(argv[0], "-simple"))
        settings.unparser = BinaryAnalysis::Unparser::Settings::minimal();
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv, engine, settings);
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, settings.stateFormat);

    // Show selected functions or all functions
    BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner.unparser();
    unparser->settings() = settings.unparser;
    if (settings.discardNops)
        unparser = MyUnparser::instance(unparser);

    if (!settings.functionNames.empty() || !settings.addresses.empty()) {
        unparser->settings().function.cg.showing = false; // slow
        std::vector<P2::Function::Ptr> selectedFunctions =
            Bat::selectFunctionsByNameOrAddress(partitioner.functions(), settings.functionNames, mlog[WARN]);
        BOOST_FOREACH (const P2::Function::Ptr &f, Bat::selectFunctionsContainingInstruction(partitioner, settings.addresses))
            P2::insertUnique(selectedFunctions, f, P2::sortFunctionsByAddress);
        if (selectedFunctions.empty())
            mlog[WARN] <<"no matching functions found\n";

        Sawyer::ProgressBar<size_t> progress(selectedFunctions.size(), mlog[MARCH], "unparsing");
        progress.suffix(" functions");
        BOOST_FOREACH (const P2::Function::Ptr &function, selectedFunctions) {
            unparser->unparse(std::cout, partitioner, function);
            ++progress;
        }

    } else {
        unparser->unparse(std::cout, partitioner, Progress::instance());
    }
}
