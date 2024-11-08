static const char *purpose = "debug data flow";
static const char *description =
    "Runs a simple data-flow analysis on the specified function and shows the machine state along the way.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>

#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace Symbolic = Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;

static Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;
    std::set<std::string> functionNames;
    size_t maxIterations = 100;
};

static Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");

    tool.insert(Switch("function", 'f')
                .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
                .explosiveLists(true)
                .whichValue(SAVE_ALL)
                .doc("Restricts analysis to the specified functions. The @v{name_or_address} can be the name of a function as "
                    "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                    "If a value is ambiguous, it's first treated as a name and if no function has that name it's then "
                    "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                    "be specified per occurrence."));

    tool.insert(Switch("max-iterations", 'n')
                .argument("n", nonNegativeIntegerParser(settings.maxIterations))
                .doc("Limit data-flow to at most @v{n} iterations. The default is " +
                     boost::lexical_cast<std::string>(settings.maxIterations) + "."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    parser.with(tool).with(gen);
    return parser;
}

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");
    return specimen;
}

static void
processFunction(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function, const Settings &settings) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);

    Sawyer::Message::Stream info(DataFlow::mlog[INFO]);
    info <<std::string(80, '=') <<"\n"
         <<"== Data flow for " <<function->printableName() <<"\n"
         <<std::string(80, '=') <<"\n";

    // Types for data-flow
    using Cfg = P2::DataFlow::DfCfg;
    using State = BS::State::Ptr;
    using Transfer = P2::DataFlow::TransferFunction;
    using Merge = DataFlow::SemanticsMerge;
    using Engine = DataFlow::Engine<Cfg, State, Transfer, Merge>;

    // Build the data-flow CFG for this one function
    const Cfg cfg = P2::DataFlow::buildDfCfg(partitioner, partitioner->cfg(), partitioner->findPlaceholder(function->address()),
                                             P2::DataFlow::NOT_INTERPROCEDURAL);
    const boost::filesystem::path dotFileName = "dataflow-" + StringUtility::addrToString(function->address()).substr(2) + ".dot";
    {
        std::ofstream dotFile(dotFileName.c_str());
        P2::DataFlow::dumpDfCfg(dotFile, cfg);
    }
    info <<"data-flow graph has " <<StringUtility::plural(cfg.nVertices(), "vertices") <<" stored in " <<dotFileName <<"\n";

    // Build and initialize the data-flow engine
    auto ops = Symbolic::RiscOperators::promote(partitioner->newOperators());
    ops->computingDefiners(Symbolic::TRACK_ALL_DEFINERS);
    auto cpu = partitioner->newDispatcher(ops);
    ASSERT_always_require2(cpu, "instruction semantics not available for this instruction set");
    Transfer transfer(cpu);
    Merge merge(cpu);
    Engine engine(cfg, transfer, merge);
    auto initialState = transfer.initialState();
    engine.insertStartingVertex(0, initialState);

    // Run the data-flow
    bool completed = false;
    for (size_t i = 0; i < settings.maxIterations; ++i) {
        DataFlow::mlog[DEBUG] <<std::string(80, '-') <<"\n"
                              <<"Step " <<i <<"\n";
        if (!engine.runOneIteration()) {
            completed = true;
            break;
        }
    }
    info <<"dataflow " <<(completed ? "completed" : "did not complete") <<" for " <<function->printableName() <<"\n";
}


int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("debugging data flow");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();
    DataFlow::mlog[DEBUG].enable();

    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    P2::Partitioner::Ptr partitioner;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        try {
            partitioner = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.stateFormat);
        } catch (const std::exception &e) {
            mlog[FATAL] <<"cannot load partitioner from " <<specimen[0] <<": " <<e.what() <<"\n";
            exit(1);
        }
    } else {
        partitioner = engine->partition(specimen);
    }
    ASSERT_not_null(partitioner);

    const std::vector<P2::Function::Ptr> functions = [&settings, &partitioner]() {
        if (settings.functionNames.empty()) {
            return partitioner->functions();
        } else {
            return Bat::selectFunctionsByNameOrAddress(partitioner->functions(), settings.functionNames, mlog[WARN]);
        }
    }();

    mlog[INFO] <<"Processing " <<StringUtility::plural(functions.size(), "functions") <<"\n";
    for (const P2::Function::Ptr &function: functions)
        processFunction(partitioner, function, settings);
}
