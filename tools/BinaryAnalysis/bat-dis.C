static const char *purpose = "create an assembly listing";
static const char *description =
    "Given a BAT state for a binary specimen, generate a pseudo-assembly listing on standard output. The listing is not "
    "intended to be input to an assembler, but rather for human consumption.\n\n"

    "This tool reads the binary analysis state file provided as a command-line positional argument, or standard input if "
    "the name is \"-\" (a single hyphen) no file name is specified. The standard input mode works only on those operating "
    "systems whose standard input is opened in binary mode, such as Unix-like systems.";

#include <rose.h>

#include <Rose/BinaryAnalysis/NoOperation.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/CommandLine.h>

#include <batSupport.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <Sawyer/Stopwatch.h>

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
    std::string fileNamePrefix;

    Settings()
        : discardNops(false), stateFormat(SerialIo::BINARY) {}
};

// Create a command-line switch parser
Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");

    tool.insert(Switch("function", 'f')
                .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
                .explosiveLists(true)
                .whichValue(SAVE_ALL)
                .doc("Restricts output to the specified functions. The @v{name_or_address} can be the name of a function as "
                     "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                     "If a value is ambiguous, it's first treated as a name and if no function has that name it's then "
                     "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                     "be specified per occurrence."));

    tool.insert(Switch("containing", 'a')
                .argument("addresses", listParser(nonNegativeIntegerParser(settings.addresses), ","))
                .explosiveLists(true)
                .whichValue(SAVE_ALL)
                .doc("Restricts output to functions that contain one of the specified addresses. This switch may occur "
                     "multiple times and multiple comma-separate addresses may be specified per occurrence."));

    Rose::CommandLine::insertBooleanSwitch(tool, "discard-nops", settings.discardNops,
                                           "Omit instructions that are part of a sequence that has no effect.");
    tool.insert(Switch("output", 'o')
                .longName("prefix")
                .argument("name", anyParser(settings.fileNamePrefix))
                .doc("Instead of sending all listings to standard output, create a separate file for each function. "
                     "The file names will be constructed from the string specified here, followed by the hexadecimal "
                     "function entry address, followed by the extension \".lst\"."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    parser.with(tool);
    parser.with(BinaryAnalysis::Unparser::commandLineSwitches(settings.unparser));
    parser.with(Rose::CommandLine::genericSwitches());
    return parser;
}

// Parses the command-line and returns the positional arguments that describe the binary specimen
std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");                        // read RBA file from standard input

    return specimen;
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
    virtual BinaryAnalysis::Unparser::Base::Ptr copy() const override { return Ptr(new MyUnparser(nextUnparser()->copy())); }
    virtual const BinaryAnalysis::Unparser::Settings& settings() const override { return nextUnparser()->settings(); }
    virtual BinaryAnalysis::Unparser::Settings& settings() override { return nextUnparser()->settings(); }

    void emitBasicBlockBody(std::ostream &out, const P2::BasicBlockPtr &bb, BinaryAnalysis::Unparser::State &state) const override {
        ASSERT_not_null(bb);
        InstructionSemantics::BaseSemantics::RiscOperators::Ptr ops = state.partitioner()->newOperators();
        if (InstructionSemantics::BaseSemantics::Dispatcher::Ptr cpu = state.partitioner()->newDispatcher(ops)) {
            // Find the largest non-overlapping instruction sequences that are effectively no-ops
            NoOperation nopAnalyzer(cpu);
            nopAnalyzer.initialStackPointer(0xceed0000); // arbitrary
            NoOperation::IndexIntervals nopIndexes = nopAnalyzer.findNoopSubsequences(bb->instructions());
            nopIndexes = nopAnalyzer.largestEarliestNonOverlapping(nopIndexes);

            // Select only those instructions that are not no-ops
            std::vector<SgAsmInstruction*> instructions;
            for (size_t i=0; i<bb->nInstructions(); ++i) {
                bool isGoodInstruction = true;
                for (const NoOperation::IndexInterval &interval: nopIndexes) {
                    if (interval.contains(i)) {
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
                for (SgAsmInstruction *insn: instructions) {
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

boost::filesystem::path
functionFileName(const Settings &settings, const P2::Function::Ptr &function) {
    ASSERT_forbid(settings.fileNamePrefix.empty());
    ASSERT_not_null(function);
    return settings.fileNamePrefix + StringUtility::addrToString(function->address()).substr(2) + ".lst";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("generating assembly listings");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    if (boost::ends_with(argv[0], "-simple"))
        settings.unparser = BinaryAnalysis::Unparser::Settings::minimal();
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    const std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    P2::Partitioner::Ptr partitioner;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        try {
            partitioner = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.stateFormat);
        } catch (const std::exception &e) {
            mlog[FATAL] <<"cannot load partitioner from \"" <<StringUtility::cEscape(specimen[0]) <<"\": " <<e.what() <<"\n";
            exit(1);
        }
    } else {
        partitioner = engine->partition(specimen);
    }
    ASSERT_not_null(partitioner);

    // Make sure output directories exit
    boost::filesystem::path dir = boost::filesystem::path(settings.fileNamePrefix).parent_path();
    if (!dir.empty()) {
        boost::system::error_code ec; // because create_directories(".") and probably others doesn't work
        boost::filesystem::create_directories(dir, ec);
    }

    // Obtain an unparser
    BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner->unparser();
    unparser->settings() = settings.unparser;
    if (settings.discardNops)
        unparser = MyUnparser::instance(unparser);

    if (!settings.functionNames.empty() || !settings.addresses.empty()) {
        // Output only selected functions (all to stdout or each to its own file)
        unparser->settings().function.cg.showing = false; // slow
        std::vector<P2::Function::Ptr> selectedFunctions =
            Bat::selectFunctionsByNameOrAddress(partitioner->functions(), settings.functionNames, mlog[WARN]);
        for (const P2::Function::Ptr &f: Bat::selectFunctionsContainingInstruction(partitioner, settings.addresses))
            P2::insertUnique(selectedFunctions, f, P2::sortFunctionsByAddress);
        if (selectedFunctions.empty())
            mlog[WARN] <<"no matching functions found\n";

        Sawyer::ProgressBar<size_t> progress(selectedFunctions.size(), mlog[MARCH], "unparsing");
        progress.suffix(" functions");
        for (const P2::Function::Ptr &function: selectedFunctions) {
            if (settings.fileNamePrefix.empty()) {
                unparser->unparse(std::cout, partitioner, function);
            } else {
                std::ofstream file(functionFileName(settings, function).c_str());
                unparser->unparse(file, partitioner, function);
            }
            ++progress;
        }

    } else if (!settings.fileNamePrefix.empty()) {
        // Output all functions, each to its own file
        Sawyer::ProgressBar<size_t> progress(partitioner->nFunctions(), mlog[MARCH], "unparsing");
        progress.suffix(" functions");
        for (const P2::Function::Ptr &function: partitioner->functions()) {
            std::ofstream file(functionFileName(settings, function).c_str());
            unparser->unparse(file, partitioner, function);
            ++progress;
        }

    } else {
        // Output all functions to standard output
        unparser->unparse(std::cout, partitioner, Progress::instance());
    }
}
