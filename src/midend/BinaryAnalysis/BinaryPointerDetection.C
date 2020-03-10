#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryPointerDetection.h>

#include <AsmUnparser_compat.h>
#include <boost/foreach.hpp>
#include <CommandLine.h>
#include <Diagnostics.h>
#include <Disassembler.h>
#include <MemoryCellList.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/ProgressBar.h>
#include <SymbolicSemantics2.h>

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace PointerDetection {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::PointerDetection");
        mlog.comment("finding pointers to code and data");
    }
}

bool
PointerDescriptorLessp::operator()(const PointerDescriptor &a, const PointerDescriptor &b) {
    if (a.lvalue == NULL || b.lvalue == NULL)
        return a.lvalue == NULL && b.lvalue != NULL;
    return a.lvalue->hash() < b.lvalue->hash();
}

void
Analysis::init(Disassembler *disassembler) {
    if (disassembler) {
        const RegisterDictionary *registerDictionary = disassembler->registerDictionary();
        ASSERT_not_null(registerDictionary);
        size_t addrWidth = disassembler->instructionPointerRegister().nBits();

        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        SymbolicSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(registerDictionary, solver);

        cpu_ = disassembler->dispatcher()->create(ops, addrWidth, registerDictionary);
    }
}

void
Analysis::clearResults() {
    codePointers_.clear();
    dataPointers_.clear();
    hasResults_ = didConverge_ = false;
}

void
Analysis::clearNonResults() {
    initialState_ = BaseSemantics::StatePtr();
    finalState_ = BaseSemantics::StatePtr();
    cpu_ = BaseSemantics::DispatcherPtr();
}

typedef Sawyer::Container::Map<uint64_t /*value_hash*/, SymbolicExpr::ExpressionSet /*addresses*/> MemoryTransfers;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantics for pointer detection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef SymbolicSemantics::SValue SValue;
typedef SymbolicSemantics::SValuePtr SValuePtr;

typedef SymbolicSemantics::RegisterState RegisterState;
typedef SymbolicSemantics::RegisterStatePtr RegisterStatePtr;

typedef SymbolicSemantics::MemoryState MemoryState;
typedef SymbolicSemantics::MemoryStatePtr MemoryStatePtr;

// State extended to keep track of multi-byte memory accesses
typedef boost::shared_ptr<class State> StatePtr;

class State: public SymbolicSemantics::State {
public:
    typedef SymbolicSemantics::State Super;

private:
    MemoryTransfers memoryReads_;                       // memory addresses per (hash of) value read.

protected:
    State(const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::MemoryStatePtr &memory)
        : Super(registers, memory) {}

    State(const State &other)
        : Super(other), memoryReads_(other.memoryReads_) {}

public:
    static StatePtr instance(const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }

    static StatePtr instance(const StatePtr &other) {
        return StatePtr(new State(*other));
    }

public:
    virtual BaseSemantics::StatePtr create(const BaseSemantics::RegisterStatePtr &registers,
                                           const BaseSemantics::MemoryStatePtr &memory) const ROSE_OVERRIDE {
        return instance(registers, memory);
    }

    virtual BaseSemantics::StatePtr clone() const ROSE_OVERRIDE {
        return StatePtr(new State(*this));
    }

    static StatePtr promote(const BaseSemantics::StatePtr &x) {
        StatePtr retval = boost::dynamic_pointer_cast<State>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    virtual bool merge(const BaseSemantics::StatePtr &other_, BaseSemantics::RiscOperators *ops) ROSE_OVERRIDE {
        bool changed = false;
        StatePtr other = State::promote(other_);
        BOOST_FOREACH (const MemoryTransfers::Node &otherNode, other->memoryReads_.nodes()) {
            SymbolicExpr::ExpressionSet &addresses = memoryReads_.insertMaybeDefault(otherNode.key());
            BOOST_FOREACH (const SymbolicExpr::Ptr address, otherNode.value().values()) {
                if (addresses.insert(address))
                    changed = true;
            }
        }
        if (Super::merge(other, ops))
            changed = true;
        return changed;
    }

    void saveRead(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value) {
        SymbolicExpr::Ptr addrExpr = SValue::promote(addr)->get_expression();
        SymbolicExpr::Ptr valueExpr = SValue::promote(value)->get_expression();
        memoryReads_.insertMaybeDefault(valueExpr->hash()).insert(addrExpr);
    }

    const MemoryTransfers& memoryReads() const {
        return memoryReads_;
    }
};

// RiscOperators extended to keep track of multi-byte memory accesses
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

class RiscOperators: public SymbolicSemantics::RiscOperators {
public:
    typedef SymbolicSemantics::RiscOperators Super;
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : Super(protoval, solver) {}

    explicit RiscOperators(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr())
        : Super(state, solver) {}

public:
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver = SmtSolverPtr()) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }
    
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        BaseSemantics::SValuePtr retval = Super::readMemory(segreg, addr, dflt, cond);
        StatePtr state = State::promote(currentState());
        state->saveRead(addr, retval);
        return retval;
    }
};


BaseSemantics::RiscOperatorsPtr
Analysis::makeRiscOperators(const P2::Partitioner &partitioner) const {
    const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
    return RiscOperators::instance(regdict, partitioner.smtSolver());
}

void
Analysis::printInstructionsForDebugging(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  function instructions:\n";
        BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(bbVa)) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                    mlog[DEBUG] <<"    " <<partitioner.unparse(insn) <<"\n";
                }
            }
        }
    }
}

struct ExprVisitor: public SymbolicExpr::Visitor {
    Sawyer::Message::Facility &mlog;
    const MemoryTransfers &memoryReads;
    size_t nBits;
    PointerDescriptors &result;

    ExprVisitor(const MemoryTransfers &memoryReads, size_t nBits, PointerDescriptors &result, Sawyer::Message::Facility &mlog)
        : mlog(mlog), memoryReads(memoryReads), nBits(nBits), result(result) {}

    virtual SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
        SymbolicExpr::VisitAction retval = SymbolicExpr::CONTINUE;
        BOOST_FOREACH (SymbolicExpr::Ptr address, memoryReads.getOrDefault(node->hash()).values()) {
            if (result.insert(PointerDescriptor(address, nBits)).second)
                mlog[DEBUG] <<"            l-value = " <<*address <<"\n";
            retval = SymbolicExpr::TRUNCATE;
        }
        return retval;
    }

    virtual SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
        return SymbolicExpr::CONTINUE;
    }
};

void
Analysis::conditionallySavePointer(const BaseSemantics::SValuePtr &ptrRValue_,
                                   Sawyer::Container::Set<uint64_t> &ptrRValuesSeen,
                                   size_t wordSize,     // word size in bits
                                   PointerDescriptors &result) {
    Sawyer::Message::Stream debug = mlog[DEBUG];
    SymbolicExpr::Ptr ptrRValue = SymbolicSemantics::SValue::promote(ptrRValue_)->get_expression();
    if (!ptrRValuesSeen.insert(ptrRValue->hash()))
        return;
    SAWYER_MESG(debug) <<"    pointer r-value = " <<*ptrRValue <<"\n";
    StatePtr finalState = State::promote(finalState_);
    ExprVisitor visitor(finalState->memoryReads(), ptrRValue->nBits(), result /*out*/, mlog);
    ptrRValue->depthFirstTraversal(visitor);
}

void
Analysis::analyzeFunction(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    mlog[DEBUG] <<"analyzeFunction(" <<function->printableName() <<")\n";
    printInstructionsForDebugging(partitioner, function);

    clearResults();
    initialState_ = BaseSemantics::StatePtr();
    finalState_ = BaseSemantics::StatePtr();
            
    // Build the CFG used by the dataflow: dfCfg.  The dfCfg includes only those vertices that are reachable from the entry
    // point for the function we're analyzing and which belong to that function.  All return points in the function will flow
    // into a special CALLRET vertex (which is absent if there are no returns).
    typedef P2::DataFlow::DfCfg DfCfg;
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner.cfg(), partitioner.findPlaceholder(function->address()));
    size_t startVertexId = 0;
    DfCfg::ConstVertexIterator returnVertex = dfCfg.vertices().end();
    BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
        if (vertex.value().type() == P2::DataFlow::DfCfgVertex::FUNCRET) {
            returnVertex = dfCfg.findVertex(vertex.id());
            break;
        }
    }
    if (returnVertex == dfCfg.vertices().end()) {
        mlog[DEBUG] <<"  function CFG has no return vertex\n";
        return;
    }

    // Build the dataflow engine.
    typedef DataFlow::Engine<DfCfg, BaseSemantics::StatePtr, P2::DataFlow::TransferFunction, DataFlow::SemanticsMerge> DfEngine;
    BaseSemantics::RiscOperatorsPtr ops = makeRiscOperators(partitioner);
    BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(ops);
    P2::DataFlow::MergeFunction merge(cpu);
    P2::DataFlow::TransferFunction xfer(cpu);
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.maxIterations(dfCfg.nVertices() * 5);      // arbitrary

    // Build the initial state
    initialState_ = xfer.initialState();
    BaseSemantics::RegisterStateGenericPtr initialRegState =
        BaseSemantics::RegisterStateGeneric::promote(initialState_->registerState());
    initialRegState->initialize_large();

    // Allow data-flow merge operations to create sets of values up to a certain cardinality. This is optional.
    SymbolicSemantics::MergerPtr merger = SymbolicSemantics::Merger::instance(10 /*arbitrary*/);
    initialState_->registerState()->merger(merger);
    initialState_->memoryState()->merger(merger);

    // Run the data-flow analysis
    bool converged = true;
    try {
        // Use this rather than runToFixedPoint because it lets us show a progress report
        Sawyer::ProgressBar<size_t> progress(mlog[MARCH], function->printableName());
        progress.suffix(" iterations");
        dfEngine.reset(BaseSemantics::StatePtr());
        dfEngine.insertStartingVertex(startVertexId, initialState_);
        while (dfEngine.runOneIteration())
            ++progress;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    }
    finalState_ = dfEngine.getInitialState(returnVertex->id());
    StatePtr finalState = State::promote(finalState_);
    SAWYER_MESG(mlog[DEBUG]) <<"  " <<(converged ? "data-flow converged" : "DATA-FLOW DID NOT CONVERGE") <<"\n";

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  memory reads:\n";
        BOOST_FOREACH (const MemoryTransfers::Node &node, finalState->memoryReads().nodes()) {
            mlog[DEBUG] <<"    value-hash = " <<StringUtility::addrToString(node.key()).substr(2) <<"\n";
            BOOST_FOREACH (const SymbolicExpr::Ptr &address, node.value().values()) {
                mlog[DEBUG] <<"      address = " <<*address <<"\n";
            }
        }
    }

    // Find data pointers
    SAWYER_MESG(mlog[DEBUG]) <<"  potential data pointers:\n";
    size_t dataWordSize = partitioner.instructionProvider().stackPointerRegister().nBits();
    Sawyer::Container::Set<SymbolicExpr::Hash> addrSeen;
    BOOST_FOREACH (const BaseSemantics::StatePtr &state, dfEngine.getFinalStates()) {
        BaseSemantics::MemoryCellStatePtr memState = BaseSemantics::MemoryCellState::promote(state->memoryState());
        BOOST_FOREACH (const BaseSemantics::MemoryCellPtr &cell, memState->allCells())
            conditionallySavePointer(cell->get_address(), addrSeen, dataWordSize, dataPointers_);
    }

    // Find code pointers
    SAWYER_MESG(mlog[DEBUG]) <<"  potential code pointers:\n";
    size_t codeWordSize = partitioner.instructionProvider().instructionPointerRegister().nBits();
    addrSeen.clear();
    const RegisterDescriptor IP = partitioner.instructionProvider().instructionPointerRegister();
    BOOST_FOREACH (const BaseSemantics::StatePtr &state, dfEngine.getFinalStates()) {
        SymbolicSemantics::SValuePtr ip =
            SymbolicSemantics::SValue::promote(state->peekRegister(IP, ops->undefined_(IP.nBits()), ops.get()));
        SymbolicExpr::Ptr ipExpr = ip->get_expression();
        if (!addrSeen.exists(ipExpr->hash())) {
            bool isSignificant = false;
            if (!settings_.ignoreConstIp) {
                isSignificant = true;
            } else if (ipExpr->isInteriorNode() && ipExpr->isInteriorNode()->getOperator() == SymbolicExpr::OP_ITE) {
                isSignificant = !ipExpr->isInteriorNode()->child(1)->isIntegerConstant() ||
                                !ipExpr->isInteriorNode()->child(2)->isIntegerConstant();
            } else if (!ipExpr->isIntegerConstant()) {
                isSignificant = true;
            }
            if (isSignificant)
                conditionallySavePointer(ip, addrSeen, codeWordSize, codePointers_);
        }
    }

    hasResults_ = true;
    didConverge_ = converged;
}
    
} // namespace
} // namespace
} // namespace

#endif
