#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/PointerDetection.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <ROSE_UNUSED.h>

#include <SgAsmInstruction.h>

#include <Sawyer/ProgressBar.h>

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
        Architecture::Base::ConstPtr arch = disassembler->architecture();
        RegisterDictionary::Ptr registerDictionary = arch->registerDictionary();
        ASSERT_not_null(registerDictionary);

        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        auto ops = SymbolicSemantics::RiscOperators::instanceFromRegisters(registerDictionary, solver);

        cpu_ = arch->newInstructionDispatcher(ops);
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
    PointerDescriptor::Direction direction;             // read or write

    MemoryTransfer(rose_addr_t insnVa, const SymbolicExpression::Ptr &memoryVa, PointerDescriptor::Direction direction)
        : insnVa(insnVa), memoryVa(memoryVa), direction(direction) {}
};

struct SymbolicLessp {
    bool operator()(const SymbolicExpression::Ptr &a, const SymbolicExpression::Ptr &b) const {
        ASSERT_not_null(a);
        ASSERT_not_null(b);
        return a->hash() < b->hash();
    }
};

using MemoryTransferList = std::list<MemoryTransfer>;

using MemoryTransferMap = Sawyer::Container::Map<SymbolicExpression::Ptr /*value*/, MemoryTransferList, SymbolicLessp>;

// Insert a new MemoryTranfer into a list of transfers. Return true if inserted.
bool
insertMemoryTransfer(MemoryTransferList &list /*in,out*/, const MemoryTransfer &xfer) {
    ASSERT_not_null(xfer.memoryVa);
    uint64_t hash = xfer.memoryVa->hash();

    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        ASSERT_not_null(iter->memoryVa);
        if (iter->insnVa == xfer.insnVa && iter->direction == xfer.direction && iter->memoryVa->hash() == hash)
            return false;                               // already in the list
    }
    list.push_back(xfer);
    return true;
}

// Insert a new MemoryTransfer into the list of transfers for a particular value expression. Return true if inserted.
bool
insertMemoryTransfer(MemoryTransferMap &map /*in,out*/, const SymbolicExpression::Ptr &value, const MemoryTransfer &xfer) {
    ASSERT_not_null(value);
    ASSERT_not_null(xfer.memoryVa);
    return insertMemoryTransfer(map.insertMaybeDefault(value), xfer);
}

// Insert a new result into the list of final results and returns true if anything was inserted.
bool
insertPointerDescriptor(PointerDescriptors &list /*in,out*/, const PointerDescriptor &next) {
    bool inserted = false;
    for (auto cur = list.begin(); cur != list.end(); ++cur) {
        if (cur->nBits == next.nBits && cur->pointerVa->hash() == next.pointerVa->hash()) {
            for (auto nextAccess: next.pointerAccesses) {
                if (cur->pointerAccesses.insert(nextAccess).second)
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
    MemoryTransferMap memoryTransfers_;                     // memory addresses per (hash of) value read.

protected:
    State(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory)
        : Super(registers, memory) {}

    State(const State &other)
        : Super(other), memoryTransfers_(other.memoryTransfers_) {}

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
        Ptr retval = as<State>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    virtual bool merge(const BaseSemantics::State::Ptr &other_, BaseSemantics::RiscOperators *addrOps,
                       BaseSemantics::RiscOperators *valOps) override {
        bool changed = false;
        Ptr other = State::promote(other_);
        for (const MemoryTransferMap::Node &otherNode: other->memoryTransfers_.nodes()) {
            for (const MemoryTransfer &otherTransfer: otherNode.value()) {
                if (insertMemoryTransfer(memoryTransfers_, otherNode.key(), otherTransfer))
                    changed = true;
            }
        }
        if (Super::merge(other, addrOps, valOps))
            changed = true;
        return changed;
    }

    void saveMemoryTransfer(const rose_addr_t insnVa, const BaseSemantics::SValue::Ptr &addr,
                            const BaseSemantics::SValue::Ptr &value, PointerDescriptor::Direction direction) {
        SymbolicExpression::Ptr addrExpr = SValue::promote(addr)->get_expression();
        SymbolicExpression::Ptr valueExpr = SValue::promote(value)->get_expression();
        insertMemoryTransfer(memoryTransfers_, valueExpr, MemoryTransfer(insnVa, addrExpr, direction));
    }

    const MemoryTransferMap& memoryTransfers() const {
        return memoryTransfers_;
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
        RiscOperators::Ptr retval = as<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    virtual BaseSemantics::SValue::Ptr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValue::Ptr &addr,
                                                const BaseSemantics::SValue::Ptr &dflt,
                                                const BaseSemantics::SValue::Ptr &cond) override {
        BaseSemantics::SValue::Ptr retval = Super::readMemory(segreg, addr, dflt, cond);

        if (currentInstruction()) {
            BaseSemantics::SValue::Ptr adjustedVa;
            if (segreg.isEmpty()) {
                adjustedVa = addr;
            } else {
                BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
                adjustedVa = add(addr, signExtend(segregValue, addr->nBits()));
            }

            State::Ptr state = State::promote(currentState());
            state->saveMemoryTransfer(currentInstruction()->get_address(), adjustedVa, retval, PointerDescriptor::READ);
        }

        return retval;
    }

    virtual void writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr,
                             const BaseSemantics::SValue::Ptr &data, const BaseSemantics::SValue::Ptr &cond) override {
        Super::writeMemory(segreg, addr, data, cond);

        if (currentInstruction()) {
            BaseSemantics::SValue::Ptr adjustedVa;
            if (segreg.isEmpty()) {
                adjustedVa = addr;
            } else {
                BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
                adjustedVa = add(addr, signExtend(segregValue, addr->nBits()));
            }

            State::Ptr state = State::promote(currentState());
            state->saveMemoryTransfer(currentInstruction()->get_address(), adjustedVa, data, PointerDescriptor::WRITE);
        }
    }
};


BaseSemantics::RiscOperators::Ptr
Analysis::makeRiscOperators(const P2::Partitioner::ConstPtr &partitioner) const {
    ASSERT_not_null(partitioner);
    RegisterDictionary::Ptr regdict = partitioner->instructionProvider().registerDictionary();
    auto retval = RiscOperators::instance(regdict, partitioner->smtSolver());
    retval->trimThreshold(settings_.symbolicTrimThreshold);
    return retval;
}

void
Analysis::printInstructionsForDebugging(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  function instructions:\n";
        for (rose_addr_t bbVa: function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bbVa)) {
                for (SgAsmInstruction *insn: bb->instructions()) {
                    mlog[DEBUG] <<"    " <<partitioner->unparse(insn) <<"\n";
                }
            }
        }
    }
}

struct ExprVisitor: public SymbolicExpression::Visitor {
    Sawyer::Message::Facility &mlog;
    const MemoryTransferMap &memoryTransfers;
    SymbolicExpression::Ptr pointerValue;
    PointerDescriptors &result;

    ExprVisitor(const MemoryTransferMap &memoryTransfers, const SymbolicExpression::Ptr &pointerValue, PointerDescriptors &result,
                Sawyer::Message::Facility &mlog)
        : mlog(mlog), memoryTransfers(memoryTransfers), pointerValue(pointerValue), result(result) {}

    virtual SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) {
        SymbolicExpression::VisitAction retval = SymbolicExpression::CONTINUE;
        for (const MemoryTransfer &xfer: memoryTransfers.getOrDefault(node)) {
            if (insertPointerDescriptor(result, PointerDescriptor(xfer.memoryVa, pointerValue->nBits(), xfer.insnVa, xfer.direction,
                                                                  pointerValue))) {
                mlog[DEBUG] <<"      insn " <<StringUtility::addrToString(xfer.insnVa)
                            <<(PointerDescriptor::READ == xfer.direction ? "reads from" : "writes to") <<" pointer\n";
                mlog[DEBUG] <<"        pointer stored at address: " <<*xfer.memoryVa <<"\n";
                mlog[DEBUG] <<"        pointed-to address (pointer value): " <<*pointerValue <<"\n";
            }
            retval = SymbolicExpression::TRUNCATE;
        }
        return retval;
    }

    virtual SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) {
        return SymbolicExpression::CONTINUE;
    }
};

void
Analysis::conditionallySavePointer(const BaseSemantics::SValue::Ptr &ptrRValue_, Sawyer::Container::Set<uint64_t> &ptrRValuesSeen,
                                   PointerDescriptors &result) {
    Sawyer::Message::Stream debug = mlog[DEBUG];
    SymbolicExpression::Ptr ptrRValue = SymbolicSemantics::SValue::promote(ptrRValue_)->get_expression();
#if 0 // [Robb Matzke 2022-09-23]
    if (!ptrRValuesSeen.insert(ptrRValue->hash()))
        return;
#else
    ROSE_UNUSED(ptrRValuesSeen);
#endif
    SAWYER_MESG(debug) <<"    pointed-to address " <<*ptrRValue <<"\n";
    State::Ptr finalState = State::promote(finalState_);
    ExprVisitor visitor(finalState->memoryTransfers(), ptrRValue, result /*out*/, mlog);
    ptrRValue->depthFirstTraversal(visitor);
}

static bool
contains(const SymbolicExpression::Ptr &haystack, const SymbolicExpression::Ptr &needle) {
    struct Visitor: SymbolicExpression::Visitor {
        bool found = false;
        SymbolicExpression::Ptr needle;

        Visitor(const SymbolicExpression::Ptr &needle)
            : needle(needle) {}

        virtual SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) {
            if (node->hash() == needle->hash()) {
                found = true;
                return SymbolicExpression::TRUNCATE;
            } else {
                return SymbolicExpression::CONTINUE;
            }
        }

        virtual SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) {
            return SymbolicExpression::CONTINUE;
        }
    };

    Visitor visitor(needle);
    haystack->depthFirstTraversal(visitor);
    return visitor.found;
}

static void
discoverDereferences(PointerDescriptors &pointers, const MemoryTransferMap &transferMap) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"finding pointer dereferences\n";
    for (PointerDescriptor &pointer: pointers) {
        SAWYER_MESG(debug) <<"  finding dereferences for pointer stored at " <<*pointer.pointerVa <<"\n";

        // Find all the values this pointer can have; i.e., all the addresses to which this pointer can point.
        std::set<SymbolicExpression::Ptr, SymbolicLessp> pointerValues;
        for (const PointerDescriptor::Access &access: pointer.pointerAccesses) {
            SAWYER_MESG(debug) <<"    at " <<StringUtility::addrToString(access.insnVa)
                               <<" pointer value = " <<*access.value <<"\n";
            pointerValues.insert(access.value);
        }

        // Now find all the memory accesses that have the pointer value as an address.
        for (const SymbolicExpression::Ptr &pointerValue: pointerValues) {
            for (const MemoryTransferMap::Node &node: transferMap.nodes()) {
                for (const MemoryTransfer &xfer: node.value()) {
                    if (contains(xfer.memoryVa, pointerValue)) {
                        SAWYER_MESG(debug) <<"    at " <<StringUtility::addrToString(xfer.insnVa)
                                           <<" there is a " <<(PointerDescriptor::READ==xfer.direction?"read":"write") <<"-dereference "
                                           <<" using pointer value " <<*pointerValue
                                           <<" for memory address " <<*xfer.memoryVa
                                           <<" having value " <<*node.key() <<"\n";
                        pointer.dereferences.insert(PointerDescriptor::Access(xfer.insnVa, xfer.direction, node.key()));
                    }
                }
            }
        }
    }
}

void
Analysis::pruneResults(PointerDescriptors &descriptors) {
    // Discard things we don't want to save.
    for (PointerDescriptor &desc: descriptors) {
        if (!settings_.savePointerVas)
            desc.pointerVa = SymbolicExpression::Ptr();
        if (!settings_.savePointerAccesses) {
            desc.pointerAccesses.clear();
        } else if (!settings_.savePointerAccessValues) {
            std::set<PointerDescriptor::Access> set;
            for (const PointerDescriptor::Access &access: desc.pointerAccesses)
                set.insert(PointerDescriptor::Access(access.insnVa, access.direction, SymbolicExpression::Ptr()));
            desc.pointerAccesses = set;
        }
        if (!settings_.savePointerDereferences) {
            desc.dereferences.clear();
        } else if (!settings_.savePointerDereferenceValues) {
            std::set<PointerDescriptor::Access> set;
            for (const PointerDescriptor::Access &access: desc.dereferences)
                set.insert(PointerDescriptor::Access(access.insnVa, access.direction, SymbolicExpression::Ptr()));
            desc.dereferences = set;
        }
    }
}

void
Analysis::analyzeFunction(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    mlog[DEBUG] <<"analyzeFunction(" <<function->printableName() <<")\n";
    printInstructionsForDebugging(partitioner, function);

    clearResults();
    initialState_ = BaseSemantics::State::Ptr();
    finalState_ = BaseSemantics::State::Ptr();
            
    // Build the CFG used by the dataflow: dfCfg.  The dfCfg includes only those vertices that are reachable from the entry
    // point for the function we're analyzing and which belong to that function.  All return points in the function will flow
    // into a special CALLRET vertex (which is absent if there are no returns).
    typedef P2::DataFlow::DfCfg DfCfg;
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner->cfg(), partitioner->findPlaceholder(function->address()));
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
    ASSERT_always_not_null(ops);
    BaseSemantics::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
    ASSERT_always_not_null(cpu);
    P2::DataFlow::MergeFunction merge(cpu);
    P2::DataFlow::TransferFunction xfer(cpu);
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.name("pointer-detection");
    dfEngine.maxIterations(dfCfg.nVertices() * settings_.maximumDataFlowIterationFactor);

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
    } catch (const InstructionSemantics::BaseSemantics::NotImplemented &e) {
        mlog[WHERE] <<e.what() <<"\n";
        converged = false;
    } catch (const InstructionSemantics::BaseSemantics::Exception &e) {
        mlog[WARN] <<e.what() <<"\n";
        converged = false;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    }
    finalState_ = dfEngine.getInitialState(returnVertex->id());
    if (!finalState_)
        return;

    State::Ptr finalState = State::promote(finalState_);
    SAWYER_MESG(mlog[DEBUG]) <<"  " <<(converged ? "data-flow converged" : "DATA-FLOW DID NOT CONVERGE") <<"\n";

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  memory accesses:\n";
        for (const MemoryTransferMap::Node &node: finalState->memoryTransfers().nodes()) {
            mlog[DEBUG] <<"    value " <<*node.key() <<"\n";
            for (const MemoryTransfer &xfer: node.value()) {
                mlog[DEBUG] <<"      " <<(PointerDescriptor::READ == xfer.direction ? "read" : "write")
                            <<" at insn " <<StringUtility::addrToString(xfer.insnVa)
                            <<" for pointer at " <<*xfer.memoryVa <<"\n";
            }
        }
    }

    // Find data pointers
    if (settings_.saveDataPointers) {
        SAWYER_MESG(mlog[DEBUG]) <<"  potential data pointers:\n";
        Sawyer::Container::Set<SymbolicExpression::Hash> addrSeen;
        for (size_t vertexId = 0; vertexId < dfCfg.nVertices(); ++vertexId) {
            if (BaseSemantics::State::Ptr state = dfEngine.getFinalState(vertexId)) {
                auto memState = BaseSemantics::MemoryCellState::promote(state->memoryState());
                for (const BaseSemantics::MemoryCell::Ptr &cell: memState->allCells())
                    conditionallySavePointer(cell->address(), addrSeen, dataPointers_);
            }
        }
        discoverDereferences(dataPointers_, finalState->memoryTransfers());
        pruneResults(dataPointers_);
    }

    // Find code pointers
    if (settings_.saveCodePointers) {
        SAWYER_MESG(mlog[DEBUG]) <<"  potential code pointers:\n";
        Sawyer::Container::Set<SymbolicExpression::Hash> addrSeen;
        const RegisterDescriptor IP = partitioner->instructionProvider().instructionPointerRegister();
        for (size_t vertexId = 0; vertexId < dfCfg.nVertices(); ++vertexId) {
            if (BaseSemantics::State::Ptr state = dfEngine.getFinalState(vertexId)) {
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
                        conditionallySavePointer(ip, addrSeen, codePointers_);
                }
            }
        }
        pruneResults(codePointers_);
    }

    hasResults_ = true;
    didConverge_ = converged;
}
    
} // namespace
} // namespace
} // namespace

#endif
