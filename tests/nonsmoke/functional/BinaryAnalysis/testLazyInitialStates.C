// Test SymbolicSemantics with lazily instantiated initial memory state.
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

static const char *description =
    "Disassembles and partitions a specimen and then runs a symbolic data-flow analysis on each function, showing the "
    "lazily instantiated initial state plus the state at each control flow vertex of each function.";

#include <rose.h>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Diagnostics::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Basic test
//
// A basic test that uses the same state type for the initial state as for the current state. This is expected to be the
// usual situation.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [basicReadTest]
static void
basicReadTest(const P2::Partitioner &partitioner) {
    std::cout <<"\n" <<std::string(40, '=') <<"\nbasicReadTest\n" <<std::string(40, '=') <<"\n";
    SymbolicSemantics::Formatter fmt;
    fmt.set_line_prefix("  ");

    // Create the RiscOperators and the initial state.
    RegisterDictionary::Ptr regdict = partitioner.instructionProvider().registerDictionary();
    const RegisterDescriptor REG = partitioner.instructionProvider().stackPointerRegister();
    const std::string REG_NAME = RegisterNames(regdict)(REG);
    BaseSemantics::RiscOperators::Ptr ops = SymbolicSemantics::RiscOperators::instanceFromRegisters(regdict);
    ASSERT_always_not_null(ops);
    ops->currentState()->memoryState()->set_byteOrder(partitioner.instructionProvider().defaultByteOrder());
    BaseSemantics::State::Ptr initialState = ops->currentState()->clone();
    ops->initialState(initialState);                    // lazily evaluated initial state
    std::cout <<"Initial state before reading:\n" <<(*initialState+fmt);

    // Read some memory and a register, which should cause them to spring into existence in both the current state and the
    // initial state.
    BaseSemantics::SValue::Ptr addr1 = ops->number_(32, 0);
    BaseSemantics::SValue::Ptr dflt1m = ops->number_(32, 0x11223344);
    BaseSemantics::SValue::Ptr read1m = ops->readMemory(RegisterDescriptor(), addr1, dflt1m, ops->boolean_(true));
    BaseSemantics::SValue::Ptr dflt1r = ops->undefined_(REG.nBits());
    BaseSemantics::SValue::Ptr read1r = ops->readRegister(REG, dflt1r);

    std::cout <<"Initial state after reading " <<*read1m <<" from address " <<*addr1 <<"\n"
              <<"and " <<*read1r <<" from " <<REG_NAME <<"\n"
              <<(*initialState+fmt);
    ASSERT_always_require(read1m->mustEqual(dflt1m));
    ASSERT_always_require(read1r->mustEqual(dflt1r));

    // Create a new current state and read again. We should get the same value even though the current state is empty.
    BaseSemantics::State::Ptr curState = ops->currentState()->clone();
    curState->clear();
    ops->currentState(curState);
    BaseSemantics::SValue::Ptr dflt2m = ops->number_(32, 0x55667788);
    BaseSemantics::SValue::Ptr read2m = ops->readMemory(RegisterDescriptor(), addr1, dflt2m, ops->boolean_(true));
    BaseSemantics::SValue::Ptr dflt2r = ops->undefined_(REG.nBits());
    BaseSemantics::SValue::Ptr read2r = ops->readRegister(REG, dflt2r);

    std::cout <<"Initial state after reading " <<*read2m <<" from address " <<*addr1 <<"\n"
              <<"and " <<*read2r <<" from " <<REG_NAME <<"\n"
              <<(*initialState+fmt);
    ASSERT_always_require(read1m->mustEqual(read2m));
    ASSERT_always_require(read1r->mustEqual(read2r));

    // Disable the initial state. If we re-read the same address we'll still get the same result because it's now present in
    // the current state also.
    ops->initialState(BaseSemantics::State::Ptr());
    BaseSemantics::SValue::Ptr dflt3m = ops->number_(32, 0x99aabbcc);
    BaseSemantics::SValue::Ptr read3m = ops->readMemory(RegisterDescriptor(), addr1, dflt3m, ops->boolean_(true));
    BaseSemantics::SValue::Ptr dflt3r = ops->undefined_(REG.nBits());
    BaseSemantics::SValue::Ptr read3r = ops->readRegister(REG, dflt3r);
    ASSERT_always_require(read1m->mustEqual(read3m));
    ASSERT_always_require(read1r->mustEqual(read3r));
}
//! [basicReadTest]


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Advanced test
//
// This test uses a user-defined symbolic state. We could use a different initial state than we do for the current state, but
// advanced users are already likely to be deriving their own memory state, in which case we want to show in this demo that
// it's not much work to specialize the initial state. We do so by taking the user's presumed existing specialization and
// adding a data member that says whether this is an initial state, and modifying readRegister and/or readMemory to do
// something special when this data member is set.
//
// We override SymbolicSemantics::State's readRegister and readMemory methods, but we could just as well have derived two
// classes from SymbolicSemantics::MemoryState and SymbolicSemantics::RegisterState instead.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [advancedReadTest]
// The flag bit to indicate that a value came from an initial value.
static const unsigned MY_FLAG = 0x40000000;

// Shared-ownership pointer.
typedef boost::shared_ptr<class MyState> MyStatePtr;

// User-defined state. Most of these methods are the required boilerplate for subclassing a state. The only two new
// things are the readRegister and readMemory implementations, which use an isInitialState property.
class MyState: public SymbolicSemantics::State {
private:
    using Super = SymbolicSemantics::State;
public:
    using Ptr = MyStatePtr;

private:
    bool isInitialState_;                               // true for lazily updated initial states

protected:                                              // typical boilerplate
    MyState(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory)
        : Super(registers, memory), isInitialState_(false) {}

    MyState(const MyState &other)
        : Super(other), isInitialState_(other.isInitialState_) {}

public:                                                 // typical boilerplate
    static MyState::Ptr instance(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory) {
        return MyState::Ptr(new MyState(registers, memory));
    }

    static MyState::Ptr instance(const MyState::Ptr &other) {
        return MyState::Ptr(new MyState(*other));
    }

    virtual BaseSemantics::State::Ptr create(const BaseSemantics::RegisterState::Ptr &registers,
                                           const BaseSemantics::MemoryState::Ptr &memory) const override {
        return instance(registers, memory);
    }

    virtual BaseSemantics::State::Ptr clone() const override {
        return BaseSemantics::State::Ptr(new MyState(*this));
    }
    
    static MyState::Ptr promote(const BaseSemantics::State::Ptr &x) {
        MyState::Ptr retval = boost::dynamic_pointer_cast<MyState>(x);
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
    virtual BaseSemantics::SValue::Ptr readRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt,
                                                  BaseSemantics::RiscOperators *ops) override {
        BaseSemantics::SValue::Ptr retval = Super::readRegister(reg, dflt, ops);
        if (isInitialState_) {
            SymbolicSemantics::SValue::Ptr symval = SymbolicSemantics::SValue::promote(retval);
            symval->set_expression(symval->get_expression()->newFlags(MY_FLAG));
        }
        return retval;
    }

    virtual BaseSemantics::SValue::Ptr readMemory(const BaseSemantics::SValue::Ptr &addr, const BaseSemantics::SValue::Ptr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) override {
        BaseSemantics::SValue::Ptr retval = Super::readMemory(addr, dflt, addrOps, valOps);
        if (isInitialState_) {
            SymbolicSemantics::SValue::Ptr symval = SymbolicSemantics::SValue::promote(retval);
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
    RegisterDictionary::Ptr regdict = partitioner.instructionProvider().registerDictionary();
    const RegisterDescriptor REG = partitioner.instructionProvider().stackPointerRegister();
    const std::string REG_NAME = RegisterNames(regdict)(REG);
    const RegisterDescriptor REG2(15, 1023, 0, REG.nBits());
    BaseSemantics::RiscOperators::Ptr ops;
    {
        BaseSemantics::SValue::Ptr protoval = SymbolicSemantics::SValue::instance();
        BaseSemantics::RegisterState::Ptr registers = SymbolicSemantics::RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryState::Ptr memory = SymbolicSemantics::MemoryListState::instance(protoval, protoval);
        memory->set_byteOrder(partitioner.instructionProvider().defaultByteOrder());
        BaseSemantics::State::Ptr state = MyState::instance(registers, memory);
        ops = SymbolicSemantics::RiscOperators::instanceFromState(state);
    }

    // Create the lazily-updated initial state
    BaseSemantics::State::Ptr initialState = ops->currentState()->clone();
    MyState::promote(initialState)->isInitialState(true);
    ops->initialState(initialState);                    // lazily evaluated initial state
    std::cout <<"Initial state before reading:\n" <<(*initialState+fmt);

    // Read some memory, which should cause it to spring into existence in both the current state and the initial state.
    BaseSemantics::SValue::Ptr addr1 = ops->number_(32, 0);
    BaseSemantics::SValue::Ptr dflt1m = ops->number_(32, 0x11223344);
    BaseSemantics::SValue::Ptr read1m = ops->readMemory(RegisterDescriptor(), addr1, dflt1m, ops->boolean_(true));
    BaseSemantics::SValue::Ptr dflt1r = ops->undefined_(REG.nBits());
    BaseSemantics::SValue::Ptr read1r = ops->readRegister(REG, dflt1r);

    std::cout <<"Initial state after reading " <<*read1m <<" from address " <<*addr1 <<"\n"
              <<"and reading " <<*read1r <<" from " <<REG_NAME <<"\n"
              <<(*initialState+fmt);
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read1m)->get_expression()->flags() & MY_FLAG) != 0);
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read1r)->get_expression()->flags() & MY_FLAG) != 0);

    // Create a new current state and read again. We should get the same value even though the current state is empty.
    BaseSemantics::State::Ptr curState = ops->currentState()->clone();
    curState->clear();
    ops->currentState(curState);
    BaseSemantics::SValue::Ptr dflt2m = ops->number_(32, 0x55667788);
    BaseSemantics::SValue::Ptr read2m = ops->readMemory(RegisterDescriptor(), addr1, dflt2m, ops->boolean_(true));
    BaseSemantics::SValue::Ptr dflt2r = ops->undefined_(REG.nBits());
    BaseSemantics::SValue::Ptr read2r = ops->readRegister(REG, dflt2r);

    std::cout <<"Initial state after reading " <<*read2m <<" from address " <<*addr1 <<"\n"
              <<"and reading " <<*read2r <<" from " <<REG_NAME <<"\n"
              <<(*initialState+fmt);
    ASSERT_always_require(read1m->mustEqual(read2m));
    ASSERT_always_require(read1r->mustEqual(read2r));
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read2m)->get_expression()->flags() & MY_FLAG) != 0);
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read2r)->get_expression()->flags() & MY_FLAG) != 0);

    // If we turn off the initial state and read from some other address we should get a different value, one without our
    // special bit set.
    ops->initialState(BaseSemantics::State::Ptr());
    BaseSemantics::SValue::Ptr addr3 = ops->number_(32, 4);
    BaseSemantics::SValue::Ptr dflt3m = ops->number_(32, 0x99aabbcc);
    BaseSemantics::SValue::Ptr read3m = ops->readMemory(RegisterDescriptor(), addr3, dflt3m, ops->boolean_(true));
    BaseSemantics::SValue::Ptr dflt3r = ops->undefined_(REG2.nBits());
    BaseSemantics::SValue::Ptr read3r = ops->readRegister(REG2, dflt3r);

    ASSERT_always_forbid(read1m->mustEqual(read3m));
    ASSERT_always_forbid(read1r->mustEqual(read3r));
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read3m)->get_expression()->flags() & MY_FLAG) == 0);
    ASSERT_always_require((SymbolicSemantics::SValue::promote(read3r)->get_expression()->flags() & MY_FLAG) == 0);
}
//! [advancedReadTest]

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
    BaseSemantics::Dispatcher::Ptr cpu = partitioner.newDispatcher(partitioner.newOperators());
    P2::DataFlow::TransferFunction xfer(cpu);
    P2::DataFlow::MergeFunction merge(cpu);
    typedef DataFlow::Engine<DfCfg, SymbolicSemantics::State::Ptr, P2::DataFlow::TransferFunction,
                             DataFlow::SemanticsMerge> DfEngine;
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.name("data-flow-test");
    dfEngine.maxIterations(dfCfg.nVertices() * 5);      // arbitrary limit

    // Build the initial state. This will serve as the state whose values are lazily instantiated by the symbolic RiscOperators.
    SymbolicSemantics::State::Ptr initialState = xfer.initialState();
#if 1 // [Robb Matzke 2016-01-28]
    cpu->operators()->initialState(initialState);
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
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&::mlog, "tool");

    P2::Engine *engine = P2::Engine::instance();
    std::vector<std::string> specimen = engine->parseCommandLine(argc, argv, "tests semantics initial states", description)
                                        .unreachedArgs();
    P2::Partitioner partitioner = engine->partition(specimen);

    basicReadTest(partitioner);
    advancedReadTest(partitioner);
    for (const P2::Function::Ptr &function : partitioner.functions())
         analyzeFunction(partitioner, function);

    delete engine;
}

#endif
