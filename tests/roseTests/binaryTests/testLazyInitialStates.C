// Test SymbolicSemantics with lazily instantiated initial memory state.

static const char *description =
    "Disassembles and partitions a specimen and then runs a symbolic data-flow analysis on each function, showing the "
    "lazily instantiated initial state plus the state at each control flow vertex of each function.";

#include <rose.h>
#include <Diagnostics.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/Engine.h>
#include <SymbolicSemantics2.h>

using namespace rose;
using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

static Diagnostics::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Basic test
//
// A basic test that uses the same memory state type for the initial state as for the current state. This is expected to be the
// usual situation.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
basicReadTest(const P2::Partitioner &partitioner) {
    std::cout <<"\n" <<std::string(40, '=') <<"\nbasicReadTest\n" <<std::string(40, '=') <<"\n";
    SymbolicSemantics::Formatter fmt;
    fmt.set_line_prefix("  ");

    // Create the RiscOperators and the initial state.
    const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
    BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(regdict);
    ops->currentState()->memoryState()->set_byteOrder(partitioner.instructionProvider().defaultByteOrder());
    BaseSemantics::StatePtr initialState = ops->currentState()->clone();
    ops->initialState(initialState);                    // lazily evaluated initial state
    std::cout <<"Initial state before reading:\n" <<(*initialState+fmt);

    // Read some memory, which should cause it to spring into existence in both the current state and the initial state.
    BaseSemantics::SValuePtr addr1 = ops->number_(32, 0);
    BaseSemantics::SValuePtr dflt1 = ops->number_(32, 0x11223344);
    BaseSemantics::SValuePtr read1 = ops->readMemory(RegisterDescriptor(), addr1, dflt1, ops->boolean_(true));
    std::cout <<"Initial state after reading " <<*read1 <<" from address " <<*addr1 <<"\n" <<(*initialState+fmt);
    ASSERT_always_require(read1->must_equal(dflt1));

    // Create a new current state and read again. We should get the same value even though the current state is empty.
    BaseSemantics::StatePtr curState = ops->currentState()->clone();
    curState->clear();
    ops->currentState(curState);
    BaseSemantics::SValuePtr dflt2 = ops->number_(32, 0x55667788);
    BaseSemantics::SValuePtr read2 = ops->readMemory(RegisterDescriptor(), addr1, dflt2, ops->boolean_(true));
    std::cout <<"Initial state after reading " <<*read2 <<" from address " <<*addr1 <<"\n" <<(*initialState+fmt);
    ASSERT_always_require(read1->must_equal(read2));

    // Disable the initial state. If we re-read the same address we'll still get the same result because it's now present in
    // the current state also.
    ops->initialState(BaseSemantics::StatePtr());
    BaseSemantics::SValuePtr dflt3 = ops->number_(32, 0x99aabbcc);
    BaseSemantics::SValuePtr read3 = ops->readMemory(RegisterDescriptor(), addr1, dflt3, ops->boolean_(true));
    ASSERT_always_require(read1->must_equal(read3));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Advanced test
//
// This test uses a user-defined symbolic state. We could use a different initial state than we do for the current state,
// but advanced users are already likely to be deriving their own memory state, in which case we want to show in this demo that
// it's not much work to specialize the initial state. We do so by taking the user's presumed existing specialization and
// adding a data member that says whether this is an initial state, and modifying readMemory to do something special when this
// data member is set.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const unsigned MY_FLAG = 0x40000000;             // arbitrary symbolic expression bit flag

typedef boost::shared_ptr<class MyMemoryState> MyMemoryStatePtr;

class MyMemoryState: public SymbolicSemantics::MemoryListState {
    typedef SymbolicSemantics::MemoryListState Super;
    bool isInitialState_;                               // true for lazily updated initial states

protected:                                              // typical boilerplate
    explicit MyMemoryState(const BaseSemantics::MemoryCellPtr &protocell)
        : Super(protocell), isInitialState_(false) {}

    MyMemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval)
        : Super(addrProtoval, valProtoval), isInitialState_(false) {}

    MyMemoryState(const MyMemoryState &other)
        : Super(other), isInitialState_(other.isInitialState_) {}

public:                                                 // typical boilerplate
    static MyMemoryStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell) {
        return MyMemoryStatePtr(new MyMemoryState(protocell));
    }

    static MyMemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval,
                                     const BaseSemantics::SValuePtr &valProtoval) {
        return MyMemoryStatePtr(new MyMemoryState(addrProtoval, valProtoval));
    }

    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }

    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell) const {
        return instance(protocell);
    }

    virtual BaseSemantics::MemoryStatePtr clone() const ROSE_OVERRIDE {
        return BaseSemantics::MemoryStatePtr(new MyMemoryState(*this));
    }
    
    static MyMemoryStatePtr promote(const BaseSemantics::MemoryStatePtr &x) {
        MyMemoryStatePtr retval = boost::dynamic_pointer_cast<MyMemoryState>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:                                                 // new methods
    bool isInitialState() const {
        return isInitialState_;
    }
    void isInitialState(bool b) {
        isInitialState_ = b;
    }

public:                                                 // overrides
    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        BaseSemantics::SValuePtr retval = Super::readMemory(addr, dflt, addrOps, valOps);
        if (isInitialState_) {
            SymbolicSemantics::SValuePtr symval = SymbolicSemantics::SValue::promote(retval);
            symval->set_expression(symval->get_expression()->newFlags(MY_FLAG));
        }
        return retval;
    }
};

    
static void
advancedReadTest(const P2::Partitioner &partitioner) {
    std::cout <<"\n" <<std::string(40, '=') <<"\nadvancedReadTest\n" <<std::string(40, '=') <<"\n";
    SymbolicSemantics::Formatter fmt;
    fmt.set_line_prefix("  ");

    // Build the semantics framework. We use SymbolicSemantics, but with our own memory state.
    BaseSemantics::RiscOperatorsPtr ops;
    {
        const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
        BaseSemantics::SValuePtr protoval = SymbolicSemantics::SValue::instance();
        BaseSemantics::RegisterStatePtr registers = SymbolicSemantics::RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MyMemoryState::instance(protoval, protoval);
        memory->set_byteOrder(partitioner.instructionProvider().defaultByteOrder());
        BaseSemantics::StatePtr state = SymbolicSemantics::State::instance(registers, memory);
        ops = SymbolicSemantics::RiscOperators::instance(state);
    }

    // Create the lazily-updated initial state
    BaseSemantics::StatePtr initialState = ops->currentState()->clone();
    MyMemoryState::promote(initialState->memoryState())->isInitialState(true);
    ops->initialState(initialState);                    // lazily evaluated initial state
    std::cout <<"Initial state before reading:\n" <<(*initialState+fmt);

    // Read some memory, which should cause it to spring into existence in both the current state and the initial state.
    BaseSemantics::SValuePtr addr1 = ops->number_(32, 0);
    BaseSemantics::SValuePtr dflt1 = ops->number_(32, 0x11223344);
    BaseSemantics::SValuePtr read1 = ops->readMemory(RegisterDescriptor(), addr1, dflt1, ops->boolean_(true));
    std::cout <<"Initial state after reading " <<*read1 <<" from address " <<*addr1 <<"\n" <<(*initialState+fmt);
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read1)->get_expression()->flags() & MY_FLAG) != 0);

    // Create a new current state and read again. We should get the same value even though the current state is empty.
    BaseSemantics::StatePtr curState = ops->currentState()->clone();
    curState->clear();
    ops->currentState(curState);
    BaseSemantics::SValuePtr dflt2 = ops->number_(32, 0x55667788);
    BaseSemantics::SValuePtr read2 = ops->readMemory(RegisterDescriptor(), addr1, dflt2, ops->boolean_(true));
    std::cout <<"Initial state after reading " <<*read2 <<" from address " <<*addr1 <<"\n" <<(*initialState+fmt);
    ASSERT_always_require(read1->must_equal(read2));
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read2)->get_expression()->flags() & MY_FLAG) != 0);

    // If turn off the initial memory state and read from some other address we should get a different value, one without our
    // special bit set.
    ops->initialState(BaseSemantics::StatePtr());
    BaseSemantics::SValuePtr addr3 = ops->number_(32, 4);
    BaseSemantics::SValuePtr dflt3 = ops->number_(32, 0x99aabbcc);
    BaseSemantics::SValuePtr read3 = ops->readMemory(RegisterDescriptor(), addr3, dflt3, ops->boolean_(true));
    ASSERT_always_forbid(read1->must_equal(read3));
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read3)->get_expression()->flags() & MY_FLAG) == 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Data-flow test
//
// Use a lazily-updated initial state in a data-flow analysis.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
analyzeFunction(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    std::cout <<"\n" <<std::string(40, '=') <<"\n" <<function->printableName() <<"\n" <<std::string(40, '=') <<"\n";
    typedef P2::DataFlow::DfCfg DfCfg;

    // Obtain the control flow graph that we'll use for the data-flow of this function. By convention, the function entry
    // address is vertex #0 of this graph.
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner.cfg(), partitioner.findPlaceholder(function->address()));
    size_t startVertexId = 0;

    // Find the unique return vertex if there is one. All function return blocks flow into this vertex, but this vertex will
    // not exist if the function has no return blocks.
    DfCfg::ConstVertexIterator returnVertex = dfCfg.vertices().end();
    BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
        if (vertex.value().type() == P2::DataFlow::DfCfgVertex::FUNCRET) {
            returnVertex = dfCfg.findVertex(vertex.id());
            break;
        }
    }

    // Build the data-flow engine. We'll use parts from Partitioner2 for convenience, which uses symbolic semantics.
    BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(partitioner.newOperators());
    P2::DataFlow::TransferFunction xfer(cpu);
    P2::DataFlow::MergeFunction merge(cpu);
    typedef DataFlow::Engine<DfCfg, SymbolicSemantics::StatePtr, P2::DataFlow::TransferFunction,
                             DataFlow::SemanticsMerge> DfEngine;
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.maxIterations(dfCfg.nVertices() * 5);      // arbitrary limit

    // Build the initial state. This will serve as the state whose values are lazily instantiated by the symbolic RiscOperators.
    SymbolicSemantics::StatePtr initialState = xfer.initialState();
#if 1 // [Robb Matzke 2016-01-28]
    cpu->get_operators()->initialState(initialState);
#endif

    // Run the data-flow
    try {
        dfEngine.runToFixedPoint(startVertexId, initialState->clone());
    } catch (const DataFlow::NotConverging &e) {
        ::mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        return;
    } catch (const BaseSemantics::Exception &e) {
        ::mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        return;
    }

    // Show the results
    SymbolicSemantics::Formatter fmt;
    fmt.set_line_prefix("  ");
    std::cout <<"Lazily updated initial state:\n" <<(*initialState+fmt);
    if (dfCfg.isValidVertex(returnVertex))
        std::cout <<"Final state at function return:\n" <<(*dfEngine.getFinalState(returnVertex->id())+fmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[]) {
    Diagnostics::initialize();
    ::mlog = Diagnostics::Facility("tool");
    Diagnostics::mfacilities.insertAndAdjust(::mlog);

    P2::Engine engine;
    std::vector<std::string> specimen = engine.parseCommandLine(argc, argv, "tests semantics initial states", description)
                                        .unreachedArgs();
    P2::Partitioner partitioner = engine.partition(specimen);

    basicReadTest(partitioner);
    advancedReadTest(partitioner);
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions())
        analyzeFunction(partitioner, function);
}
