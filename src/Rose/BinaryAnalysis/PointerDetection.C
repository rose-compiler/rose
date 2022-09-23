#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/PointerDetection.h>

#include <AsmUnparser_compat.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Sawyer/ProgressBar.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics;
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

void
Analysis::init(const Disassembler::Base::Ptr &disassembler) {
    if (disassembler) {
        RegisterDictionary::Ptr registerDictionary = disassembler->registerDictionary();
        ASSERT_not_null(registerDictionary);
        size_t addrWidth = disassembler->instructionPointerRegister().nBits();

        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        SymbolicSemantics::RiscOperators::Ptr ops = SymbolicSemantics::RiscOperators::instanceFromRegisters(registerDictionary, solver);

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
    initialState_ = BaseSemantics::State::Ptr();
    finalState_ = BaseSemantics::State::Ptr();
    cpu_ = BaseSemantics::Dispatcher::Ptr();
}

struct MemoryTransfer {
    rose_addr_t insnVa;                                 // address of instruction that accessed memory
    SymbolicExpression::Ptr memoryVa;                   // memory address being accessed

    MemoryTransfer(rose_addr_t insnVa, const SymbolicExpression::Ptr &memoryVa)
        : insnVa(insnVa), memoryVa(memoryVa) {}
};

using MemoryTransferList = std::list<MemoryTransfer>;

using MemoryTransferMap = Sawyer::Container::Map<uint64_t /*value_hash*/, MemoryTransferList>;

typedef Sawyer::Container::Map<uint64_t /*value_hash*/, SymbolicExpression::ExpressionSet /*addresses*/> MemoryTransfers;

// Insert a new MemoryTranfer into a list of transfers. Return true if inserted.
bool
insertMemoryTransfer(MemoryTransferList &list /*in,out*/, const MemoryTransfer &xfer) {
    ASSERT_not_null(xfer.memoryVa);
    uint64_t hash = xfer.memoryVa->hash();

    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        ASSERT_not_null(iter->memoryVa);
        if (iter->insnVa == xfer.insnVa && iter->memoryVa->hash() == hash)
            return false;                               // already in the list
    }
    list.push_back(xfer);
    return true;
}

// Insert a new MemoryTransfer into the list of transfers for a particular value expression. Return true if inserted.
bool
insertMemoryTransfer(MemoryTransferMap &map /*in,out*/, const uint64_t valueHash, const MemoryTransfer &xfer) {
    ASSERT_not_null(xfer.memoryVa);
    return insertMemoryTransfer(map.insertMaybeDefault(valueHash), xfer);
}

// Insert a new result into the list of final results and returns true if anything was inserted.
bool
insertPointerDescriptor(PointerDescriptors &list /*in,out*/, const PointerDescriptor &next) {
    bool inserted = false;
    for (auto cur = list.begin(); cur != list.end(); ++cur) {
        if (cur->nBits == next.nBits && cur->lvalue->hash() == next.lvalue->hash()) {
            for (auto insnVa: next.insnVas) {
                if (cur->insnVas.insert(insnVa).second)
                    inserted = true;
            }
            return inserted;
        }
    }

    list.push_back(next);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantics for pointer detection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef SymbolicSemantics::SValue SValue;
typedef SymbolicSemantics::RegisterState RegisterState;
typedef SymbolicSemantics::MemoryState MemoryState;

// State extended to keep track of multi-byte memory accesses
typedef boost::shared_ptr<class State> StatePtr;

class State: public SymbolicSemantics::State {
public:
    using Super = SymbolicSemantics::State;
    using Ptr = StatePtr;

private:
    MemoryTransferMap memoryReads_;                     // memory addresses per (hash of) value read.

protected:
    State(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory)
        : Super(registers, memory) {}

    State(const State &other)
        : Super(other), memoryReads_(other.memoryReads_) {}

public:
    static Ptr instance(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory) {
        return Ptr(new State(registers, memory));
    }

    static Ptr instance(const Ptr &other) {
        return Ptr(new State(*other));
    }

public:
    virtual BaseSemantics::State::Ptr create(const BaseSemantics::RegisterState::Ptr &registers,
                                           const BaseSemantics::MemoryState::Ptr &memory) const override {
        return instance(registers, memory);
    }

    virtual BaseSemantics::State::Ptr clone() const override {
        return Ptr(new State(*this));
    }

    static Ptr promote(const BaseSemantics::State::Ptr &x) {
        Ptr retval = boost::dynamic_pointer_cast<State>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    virtual bool merge(const BaseSemantics::State::Ptr &other_, BaseSemantics::RiscOperators *ops) override {
        bool changed = false;
        Ptr other = State::promote(other_);
        for (const MemoryTransferMap::Node &otherNode: other->memoryReads_.nodes()) {
            for (const MemoryTransfer &otherTransfer: otherNode.value()) {
                uint64_t valueHash = otherNode.key();
                if (insertMemoryTransfer(memoryReads_, valueHash, otherTransfer))
                    changed = true;
            }
        }
        if (Super::merge(other, ops))
            changed = true;
        return changed;
    }

    void saveRead(const rose_addr_t insnVa, const BaseSemantics::SValue::Ptr &addr, const BaseSemantics::SValue::Ptr &value) {
        SymbolicExpression::Ptr addrExpr = SValue::promote(addr)->get_expression();
        SymbolicExpression::Ptr valueExpr = SValue::promote(value)->get_expression();
        insertMemoryTransfer(memoryReads_, valueExpr->hash(), MemoryTransfer(insnVa, addrExpr));
    }

    const MemoryTransferMap& memoryReads() const {
        return memoryReads_;
    }
};

// RiscOperators extended to keep track of multi-byte memory accesses
using RiscOperatorsPtr = boost::shared_ptr<class RiscOperators>;

class RiscOperators: public SymbolicSemantics::RiscOperators {
public:
    using Super = SymbolicSemantics::RiscOperators;
    using Ptr = RiscOperatorsPtr;

protected:
    explicit RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : Super(protoval, solver) {}

    explicit RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolverPtr &solver = SmtSolverPtr())
        : Super(state, solver) {}

public:
    static RiscOperators::Ptr instance(const RegisterDictionary::Ptr &regdict, const SmtSolverPtr &solver = SmtSolverPtr()) {
        BaseSemantics::SValue::Ptr protoval = SValue::instance();
        BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
        BaseSemantics::State::Ptr state = State::instance(registers, memory);
        return RiscOperators::Ptr(new RiscOperators(state, solver));
    }

    static RiscOperators::Ptr instance(const BaseSemantics::SValue::Ptr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperators::Ptr(new RiscOperators(protoval, solver));
    }

    static RiscOperators::Ptr instance(const BaseSemantics::State::Ptr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperators::Ptr(new RiscOperators(state, solver));
    }
    
public:
    virtual BaseSemantics::RiscOperators::Ptr create(const BaseSemantics::SValue::Ptr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperators::Ptr create(const BaseSemantics::State::Ptr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override {
        return instance(state, solver);
    }

    static RiscOperators::Ptr promote(const BaseSemantics::RiscOperators::Ptr &x) {
        RiscOperators::Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    virtual BaseSemantics::SValue::Ptr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValue::Ptr &addr,
                                                const BaseSemantics::SValue::Ptr &dflt,
                                                const BaseSemantics::SValue::Ptr &cond) override {
        // Offset the address by the value of the segment register.
        BaseSemantics::SValue::Ptr adjustedVa;
        if (segreg.isEmpty()) {
            adjustedVa = addr;
        } else {
            BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
            adjustedVa = add(addr, signExtend(segregValue, addr->nBits()));
        }

        BaseSemantics::SValue::Ptr retval = Super::readMemory(segreg, addr, dflt, cond);

        if (currentInstruction()) {
            State::Ptr state = State::promote(currentState());
            state->saveRead(currentInstruction()->get_address(), adjustedVa, retval);
        }
        return retval;
    }
};


BaseSemantics::RiscOperators::Ptr
Analysis::makeRiscOperators(const P2::Partitioner &partitioner) const {
    RegisterDictionary::Ptr regdict = partitioner.instructionProvider().registerDictionary();
    return RiscOperators::instance(regdict, partitioner.smtSolver());
}

void
Analysis::printInstructionsForDebugging(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  function instructions:\n";
        for (rose_addr_t bbVa: function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(bbVa)) {
                for (SgAsmInstruction *insn: bb->instructions()) {
                    mlog[DEBUG] <<"    " <<partitioner.unparse(insn) <<"\n";
                }
            }
        }
    }
}

struct ExprVisitor: public SymbolicExpression::Visitor {
    Sawyer::Message::Facility &mlog;
    const MemoryTransferMap &memoryReads;
    size_t nBits;
    PointerDescriptors &result;

    ExprVisitor(const MemoryTransferMap &memoryReads, size_t nBits, PointerDescriptors &result, Sawyer::Message::Facility &mlog)
        : mlog(mlog), memoryReads(memoryReads), nBits(nBits), result(result) {}

    virtual SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) {
        SymbolicExpression::VisitAction retval = SymbolicExpression::CONTINUE;
        for (const MemoryTransfer &xfer: memoryReads.getOrDefault(node->hash())) {
            if (insertPointerDescriptor(result, PointerDescriptor(xfer.memoryVa, nBits, xfer.insnVa)))
                mlog[DEBUG] <<"            insn = " <<StringUtility::addrToString(xfer.insnVa) <<", l-value = " <<*xfer.memoryVa <<"\n";
            retval = SymbolicExpression::TRUNCATE;
        }
        return retval;
    }

    virtual SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) {
        return SymbolicExpression::CONTINUE;
    }
};

void
Analysis::conditionallySavePointer(const Sawyer::Optional<rose_addr_t> &vertexVa, const BaseSemantics::SValue::Ptr &ptrRValue_,
                                   Sawyer::Container::Set<uint64_t> &ptrRValuesSeen, PointerDescriptors &result) {
    Sawyer::Message::Stream debug = mlog[DEBUG];
    SymbolicExpression::Ptr ptrRValue = SymbolicSemantics::SValue::promote(ptrRValue_)->get_expression();
    if (!ptrRValuesSeen.insert(ptrRValue->hash()))
        return;
    SAWYER_MESG(debug) <<"    pointer r-value = " <<*ptrRValue <<"\n";
    State::Ptr finalState = State::promote(finalState_);
    ExprVisitor visitor(finalState->memoryReads(), ptrRValue->nBits(), result /*out*/, mlog);
    ptrRValue->depthFirstTraversal(visitor);
}

void
Analysis::analyzeFunction(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    mlog[DEBUG] <<"analyzeFunction(" <<function->printableName() <<")\n";
    printInstructionsForDebugging(partitioner, function);

    clearResults();
    initialState_ = BaseSemantics::State::Ptr();
    finalState_ = BaseSemantics::State::Ptr();
            
    // Build the CFG used by the dataflow: dfCfg.  The dfCfg includes only those vertices that are reachable from the entry
    // point for the function we're analyzing and which belong to that function.  All return points in the function will flow
    // into a special CALLRET vertex (which is absent if there are no returns).
    typedef P2::DataFlow::DfCfg DfCfg;
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner.cfg(), partitioner.findPlaceholder(function->address()));
    size_t startVertexId = 0;
    DfCfg::ConstVertexIterator returnVertex = dfCfg.vertices().end();
    for (const DfCfg::Vertex &vertex: dfCfg.vertices()) {
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
    typedef DataFlow::Engine<DfCfg, BaseSemantics::State::Ptr, P2::DataFlow::TransferFunction, DataFlow::SemanticsMerge> DfEngine;
    BaseSemantics::RiscOperators::Ptr ops = makeRiscOperators(partitioner);
    BaseSemantics::Dispatcher::Ptr cpu = partitioner.newDispatcher(ops);
    P2::DataFlow::MergeFunction merge(cpu);
    P2::DataFlow::TransferFunction xfer(cpu);
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.name("pointer-detection");
    dfEngine.maxIterations(dfCfg.nVertices() * 5);      // arbitrary

    // Build the initial state
    initialState_ = xfer.initialState();
    BaseSemantics::RegisterStateGeneric::Ptr initialRegState =
        BaseSemantics::RegisterStateGeneric::promote(initialState_->registerState());
    initialRegState->initialize_large();
    cpu->initializeState(initialState_);

    // Allow data-flow merge operations to create sets of values up to a certain cardinality. This is optional.
    SymbolicSemantics::Merger::Ptr merger = SymbolicSemantics::Merger::instance(10 /*arbitrary*/);
    initialState_->registerState()->merger(merger);
    initialState_->memoryState()->merger(merger);

    // Run the data-flow analysis
    bool converged = true;
    try {
        // Use this rather than runToFixedPoint because it lets us show a progress report
        Sawyer::ProgressBar<size_t> progress(mlog[MARCH], function->printableName());
        progress.suffix(" iterations");
        dfEngine.reset(BaseSemantics::State::Ptr());
        dfEngine.insertStartingVertex(startVertexId, initialState_);
        while (dfEngine.runOneIteration())
            ++progress;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    }
    finalState_ = dfEngine.getInitialState(returnVertex->id());
    State::Ptr finalState = State::promote(finalState_);
    SAWYER_MESG(mlog[DEBUG]) <<"  " <<(converged ? "data-flow converged" : "DATA-FLOW DID NOT CONVERGE") <<"\n";

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  memory reads:\n";
        for (const MemoryTransferMap::Node &node: finalState->memoryReads().nodes()) {
            mlog[DEBUG] <<"    value-hash = " <<StringUtility::addrToString(node.key()).substr(2) <<"\n";
            for (const MemoryTransfer &xfer: node.value()) {
                mlog[DEBUG] <<"      address = " <<xfer.memoryVa <<"\n";
            }
        }
    }

    // Find data pointers
    SAWYER_MESG(mlog[DEBUG]) <<"  potential data pointers:\n";
    Sawyer::Container::Set<SymbolicExpression::Hash> addrSeen;

    for (size_t vertexId = 0; vertexId < dfCfg.nVertices(); ++vertexId) {
        Sawyer::Optional<rose_addr_t> vertexVa = dfCfg.findVertex(vertexId)->value().address();
        BaseSemantics::State::Ptr state = dfEngine.getFinalState(vertexId);
        auto memState = BaseSemantics::MemoryCellState::promote(state->memoryState());
        for (const BaseSemantics::MemoryCell::Ptr &cell: memState->allCells())
            conditionallySavePointer(vertexVa, cell->address(), addrSeen, dataPointers_);
    }

    // Find code pointers
    SAWYER_MESG(mlog[DEBUG]) <<"  potential code pointers:\n";
    addrSeen.clear();
    const RegisterDescriptor IP = partitioner.instructionProvider().instructionPointerRegister();
    for (size_t vertexId = 0; vertexId < dfCfg.nVertices(); ++vertexId) {
        Sawyer::Optional<rose_addr_t> vertexVa = dfCfg.findVertex(vertexId)->value().address();
        BaseSemantics::State::Ptr state = dfEngine.getFinalState(vertexId);
        SymbolicSemantics::SValue::Ptr ip =
            SymbolicSemantics::SValue::promote(state->peekRegister(IP, ops->undefined_(IP.nBits()), ops.get()));
        SymbolicExpression::Ptr ipExpr = ip->get_expression();
        if (!addrSeen.exists(ipExpr->hash())) {
            bool isSignificant = false;
            if (!settings_.ignoreConstIp) {
                isSignificant = true;
            } else if (ipExpr->isInteriorNode() && ipExpr->isInteriorNode()->getOperator() == SymbolicExpression::OP_ITE) {
                isSignificant = !ipExpr->isInteriorNode()->child(1)->isIntegerConstant() ||
                                !ipExpr->isInteriorNode()->child(2)->isIntegerConstant();
            } else if (!ipExpr->isIntegerConstant()) {
                isSignificant = true;
            }
            if (isSignificant)
                conditionallySavePointer(vertexVa, ip, addrSeen, codePointers_);
        }
    }

    hasResults_ = true;
    didConverge_ = converged;
}
    
} // namespace
} // namespace
} // namespace

#endif
