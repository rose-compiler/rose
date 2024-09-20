static const char *purpose = "demonstrate read/write sets";
static const char *description = "To be written.";

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/ReadWriteSets.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <batSupport.h>

#include <stringify.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace Symbolic = Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;

struct Settings {
    std::set<std::string> functionNames;
    size_t maxIterations = 100;
};

static Sawyer::Message::Facility mlog;

static Sawyer::CommandLine::Parser
buildSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;


    SwitchGroup tool("Tool specific switches");
    tool.name("tool");

    tool.insert(Switch("function", 'f')
                .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
                .explosiveLists(true)
                .whichValue(SAVE_ALL)
                .doc("Restricts analysis to the specified functions. The @v{name_or_address} can be the name of a function ax "
                     "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                     "If a value is ambiguous, it's first treated as a name and if no functio has that name it's then "
                     "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                     "be specified per occurrence."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}");
    parser.errorStream(mlog[FATAL]);
    parser.with(tool);
    parser.with(Rose::CommandLine::genericSwitches());
    return parser;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class for read/write sets
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0 // [Robb Matzke 2024-09-24]
class RwSetAnalysis {
public:
    using Cfg = P2::DataFlow::DfCfg;
    using State = BS::State::Ptr;
    using Transfer = P2::DataFlow::TransferFunction;
    using Merge = DataFlow::SemanticsMerge;
    using Engine = DataFlow::Engine<Cfg, State, Transfer, Merge>;
    using SValue = P2::Semantics::SValue;

private:
    Cfg cfg_;

private:
    class InterproceduralPredicate: public P2::DataFlow::InterproceduralPredicate {
    public:
        bool operator()(const P2::ControlFlowGraph&, const P2::ControlFlowGraph::ConstEdgeIterator&, size_t depth) override {
            return depth <= 2;
        }
    };

public:
    void analyze(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function, const size_t maxIterations) {
        ASSERT_not_null(partitioner);
        ASSERT_not_null(function);
        const RegisterDescriptor REG_SP = partitioner->architecture()->registerDictionary()->stackPointerRegister();
        const RegisterDescriptor REG_FP = partitioner->architecture()->registerDictionary()->stackFrameRegister();

        InterproceduralPredicate ipPred;
        cfg_ = P2::DataFlow::buildDfCfg(partitioner, partitioner->cfg(), partitioner->findPlaceholder(function->address()), ipPred);
        dumpCfg();

        // Build and initialize the data-flow engine
        auto ops = Symbolic::RiscOperators::promote(partitioner->newOperators());
        ops->computingDefiners(Symbolic::TRACK_ALL_DEFINERS);
        auto cpu = partitioner->newDispatcher(ops);
        ASSERT_always_require2(cpu, "instruction semantics not available for this instruction set");
        Transfer transfer(cpu);
        Merge merge(cpu);
        Engine engine(cfg_, transfer, merge);
        State initialState = transfer.initialState();
        BS::SValue::Ptr initialSp = ops->undefined_(REG_SP.nBits());
        initialState->registerState()->writeRegister(REG_SP, initialSp, ops.get());
        engine.insertStartingVertex(0, initialState);

        // Run the data-flow
        bool completed = false;
        for (size_t i = 0; i < maxIterations; ++i) {
            DataFlow::mlog[DEBUG] <<std::string(80, '-') <<"\n"
                                  <<"Step " <<i <<"\n";
            if (!engine.runOneIteration()) {
                completed = true;
                break;
            }
        }
        if (!completed)
            mlog[WARN] <<"data-flow did not reach a fixed point for " <<function->printableName() <<"\n";

        auto vf = Variables::VariableFinder::instance();
        auto svars = vf->findStackVariables(partitioner, function);

        class FindByAddress: public BS::MemoryCell::Predicate {
            BS::SValue::Ptr addr_;
        public:
            FindByAddress() = delete;
            explicit FindByAddress(const BS::SValue::Ptr &addr)
                : addr_(addr) {}

            bool operator()(const BS::MemoryCellPtr &cell) override {
                return cell->address()->mayEqual(addr_);
            }
        };

        if (State state = engine.getFinalState(6)) {
            std::cout <<*state;
            for (const Variables::StackVariable &stackVar: svars.values()) {
                std::cout <<stackVar <<" in " <<stackVar.function()->printableName() <<"\n";
                if (auto memory = BS::MemoryCellState::promote(state->memoryState())) {
                    BS::SValue::Ptr fp = state->peekRegister(REG_FP, ops->undefined_(REG_FP.nBits()), ops.get());
                    BS::SValue::Ptr varAddr = ops->add(fp, ops->number_(fp->nBits(), stackVar.frameOffset()));
                    std::cout <<"  fp   = " <<*fp <<"\n";
                    std::cout <<"  addr = " <<*varAddr <<"\n";

                    FindByAddress finder(varAddr);
                    std::vector<P2::BaseSemantics::MemoryCellPtr> cells = memory->matchingCells(finder);
                    for (const P2::BaseSemantics::MemoryCellPtr &cell: cells) {
                        std::cout <<"  cell:\n";
                        for (Address writerAddr: cell->getWriters().values()) {
                            std::cout <<"    writer at " <<StringUtility::addrToString(writerAddr) <<"\n";
                        }

                        for (BS::InputOutputProperty prop: cell->ioProperties().values()) {
                            std::cout <<"    property: " <<stringify::Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::InputOutputProperty(prop, "") <<"\n";
                        }
                    }
                }
            }
        }
    }

    void dumpCfg() const {
        const boost::filesystem::path dotFileName = "x.dot";
        std::ofstream dotFile(dotFileName.c_str());
        P2::DataFlow::dumpDfCfg(dotFile, cfg_);
    }
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char *argv[]) {
    // Initialize
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("read-write sets");

    // Parse command-line
    Settings settings;
    Sawyer::CommandLine::Parser parser = buildSwitchParser(settings);
    P2::Engine::Ptr engine = P2::Engine::forge(argc, argv, parser /*in,out*/);
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    mlog[INFO] <<"using the " <<engine->name() <<" partitioning engine\n";
    if (specimen.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }
    P2::Partitioner::Ptr partitioner = engine->partition(specimen);

    // Choose functions on which to run the analysis.
    const std::vector<P2::Function::Ptr> functions = [&settings, &partitioner]() {
        if (settings.functionNames.empty()) {
            return partitioner->functions();
        } else {
            return Bat::selectFunctionsByNameOrAddress(partitioner->functions(), settings.functionNames, mlog[WARN]);
        }
    }();
    mlog[INFO] <<"processing " <<StringUtility::plural(functions.size(), "functons") <<"\n";

    // Process each function
    for (const P2::Function::Ptr &function: functions) {
#if 0 // [Robb Matzke 2024-09-24]
        RwSetAnalysis().analyze(partitioner, function, settings.maxIterations);
#endif
        auto rw = ReadWriteSets::instance(partitioner);
        rw->analyze(function);
    }
}
