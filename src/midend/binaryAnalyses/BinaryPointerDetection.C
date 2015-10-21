#include <sage3basic.h>
#include <BinaryPointerDetection.h>

#include <AsmUnparser_compat.h>
#include <boost/foreach.hpp>
#include <Diagnostics.h>
#include <Disassembler.h>
#include <MemoryCellList.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/ProgressBar.h>
#include <SymbolicSemantics2.h>

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {
namespace PointerDetection {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::PointerDetection", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

void
Analysis::init(Disassembler *disassembler) {
    if (disassembler) {
        const RegisterDictionary *registerDictionary = disassembler->get_registers();
        ASSERT_not_null(registerDictionary);
        size_t addrWidth = disassembler->instructionPointerRegister().get_nbits();

        SMTSolver *solver = NULL;
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

typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

// This memory state tracks which instructions read which addresses.
class MemoryState: public P2::Semantics::MemoryState {
public:
    typedef P2::Semantics::MemoryState Super;
    typedef Sawyer::Container::Map<uint64_t /*expr_hash*/, SymbolicExpr::Ptr> AddressExpressions;
    typedef Sawyer::Container::Map<rose_addr_t /*insn*/, AddressExpressions> AddressesRead;

private:
    AddressesRead addressesRead_;
    
protected:
    explicit MemoryState(const BaseSemantics::MemoryCellPtr &protocell)
        : P2::Semantics::MemoryState(protocell) {}
    MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval)
        : P2::Semantics::MemoryState(addrProtoval, valProtoval) {}
    MemoryState(const MemoryState &other)
        : P2::Semantics::MemoryState(other), addressesRead_(other.addressesRead_) {}

public:
    static MemoryStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell) {
        return MemoryStatePtr(new MemoryState(protocell));
    }

    static  MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval) {
        return MemoryStatePtr(new MemoryState(addrProtoval, valProtoval));
    }

    static MemoryStatePtr instance(const MemoryStatePtr &other) {
        return MemoryStatePtr(new MemoryState(*other));
    }

public:
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }

    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell) const ROSE_OVERRIDE {
        return instance(protocell);
    }

    virtual BaseSemantics::MemoryStatePtr clone() const ROSE_OVERRIDE {
        return MemoryStatePtr(new MemoryState(*this));
    }

public:
    static MemoryStatePtr promote(const BaseSemantics::MemoryStatePtr &x) {
        MemoryStatePtr retval = boost::dynamic_pointer_cast<MemoryState>(x);
        assert(x!=NULL);
        return retval;
    }

public:
    const std::vector<rose_addr_t> instructionsReadingAddresses() const {
        std::vector<rose_addr_t> retval;
        BOOST_FOREACH (rose_addr_t va, addressesRead_.keys())
            retval.push_back(va);
        return retval;
    }
    
    const std::vector<SymbolicExpr::Ptr> addressesRead(rose_addr_t insnVa) const {
        std::vector<SymbolicExpr::Ptr> retval;
        BOOST_FOREACH (const SymbolicExpr::Ptr &expr, addressesRead_.getOrDefault(insnVa).values())
            retval.push_back(expr);
        return retval;
    }

    void printAddressesRead(std::ostream &out, const std::string &indentation = "") const {
        BOOST_FOREACH (const AddressesRead::Node &node, addressesRead_.nodes()) {
            out <<indentation <<"insn " <<StringUtility::addrToString(node.key()) <<" {\n";
            BOOST_FOREACH (const SymbolicExpr::Ptr &addrExpr, node.value().values()) {
                out <<indentation <<"  reads from address " <<*addrExpr <<"\n";
            }
            out <<indentation <<"}\n";
        }
    }

public:
    // Save information about which addresses each instruction reads.  Also, make sure that all readMemory results include the
    // current instruction as one of their definers (SymbolicSemantics::RiscOperators::readMemory intentionally excludes the
    // current instruction).
    virtual BaseSemantics::SValuePtr
    readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
               BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        SgAsmInstruction *insn = valOps->get_insn();
        if (insn) {
            SymbolicExpr::Ptr addrExpr = SymbolicSemantics::SValue::promote(addr)->get_expression();
            addressesRead_.insertMaybeDefault(insn->get_address()).insertMaybe(addrExpr->hash(), addrExpr);
        }
        BaseSemantics::SValuePtr retval = Super::readMemory(addr, dflt, addrOps, valOps);
        if (insn)
            SymbolicSemantics::SValue::promote(retval)->defined_by(insn);
        return retval;
    }

    virtual bool
    merge(const BaseSemantics::MemoryStatePtr &other_, BaseSemantics::RiscOperators *addrOps,
          BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        MemoryStatePtr other = MemoryState::promote(other_);
        bool changed = false;
        BOOST_FOREACH (const AddressesRead::Node &otherNode, other->addressesRead_.nodes()) {
            AddressExpressions &addrs = addressesRead_.insertMaybeDefault(otherNode.key());
            BOOST_FOREACH (const SymbolicExpr::Ptr &addrExpr, otherNode.value().values()) {
                if (!addrs.exists(addrExpr->hash())) {
                    changed = true;
                    addrs.insert(addrExpr->hash(), addrExpr);
                }
            }
        }
        if (Super::merge(other, addrOps, valOps))
            changed = true;
        return changed;
    }

    virtual void
    print(std::ostream &out, BaseSemantics::Formatter &fmt) const ROSE_OVERRIDE {
        out <<fmt.get_line_prefix() <<"instructions writing to memory:\n";
        printAddressesRead(out, fmt.get_line_prefix() + fmt.get_indentation_suffix());
        Super::print(out, fmt);
    }
};

BaseSemantics::RiscOperatorsPtr
Analysis::makeRiscOperators(const P2::Partitioner &partitioner) const {
    const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
    BaseSemantics::SValuePtr protoval = P2::Semantics::SValue::instance();
    BaseSemantics::RegisterStatePtr registers = P2::Semantics::RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
    BaseSemantics::StatePtr state = P2::Semantics::State::instance(registers, memory);
    P2::Semantics::RiscOperatorsPtr ops = P2::Semantics::RiscOperators::instance(state, partitioner.smtSolver());
    ops->computingDefiners(SymbolicSemantics::TRACK_ALL_DEFINERS);
    return ops;
}

void
Analysis::conditionallySavePointer(const BaseSemantics::SValuePtr &ptrValue_, // pointer r-value
                                   Sawyer::Container::Set<uint64_t> &ptrValueSeen,
                                   Sawyer::Container::Set<SgAsmInstruction*> &insnSeen,
                                   size_t wordSize,     // word size in bits
                                   AddressExpressions &result) {
    Sawyer::Message::Stream debug = mlog[DEBUG];
    SymbolicSemantics::SValuePtr ptrValue = SymbolicSemantics::SValue::promote(ptrValue_);
    if (!ptrValueSeen.insert(ptrValue->get_expression()->hash()))
        return;
    SAWYER_MESG(debug) <<"    potential pointer value = " <<*ptrValue <<"\n";
    MemoryStatePtr finalMemory = MemoryState::promote(finalState_->get_memory_state());

    BOOST_FOREACH (SgAsmInstruction *insn, ptrValue->get_defining_instructions()) {
        if (insnSeen.insert(insn)) {

            // Find pointer addresses. For each instruction that helped define the pointer's value, look at what memory was
            // read by that instruction. The addresses that were read could be the location of the pointer variable.
            std::vector<SymbolicExpr::Ptr> foundPtrVas;
            SAWYER_MESG(debug) <<"      defined by " <<unparseInstructionWithAddress(insn) <<"\n";
            BOOST_FOREACH (const SymbolicExpr::Ptr &ptrVa, finalMemory->addressesRead(insn->get_address())) {
                if (!settings_.ignoreStrangeSizes || ptrVa->nBits() == wordSize) {
                    SAWYER_MESG(debug) <<"        reads from " <<*ptrVa <<"\n";
                    foundPtrVas.push_back(ptrVa);
                }
                if (ptrVa->isEquivalentTo(ptrValue->get_expression())) {
                    SAWYER_MESG(debug) <<"        read and defining addresses are equivalent (abort)\n";
                    return;
                }
            }

            // Save found addresses into the result.
            BOOST_FOREACH (const SymbolicExpr::Ptr &ptrVa, foundPtrVas)
                result.insertMaybe(ptrVa->hash(), ptrVa);
        }
    }
}

void
Analysis::analyzeFunction(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    mlog[DEBUG] <<"analyzeFunction(" <<function->printableName() <<")\n";
    clearResults();
    initialState_ = BaseSemantics::StatePtr();
    finalState_ = BaseSemantics::StatePtr();

    // Print function's instructions for easy reference
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  function instructions:\n";
        BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(bbVa)) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                    mlog[DEBUG] <<"    " <<unparseInstructionWithAddress(insn) <<"\n";
                }
            }
        }
    }
            
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
        BaseSemantics::RegisterStateGeneric::promote(initialState_->get_register_state());
    initialRegState->initialize_large();

    // Run the data-flow analysis
    bool converged = true;
    try {
        // Use this rather than runToFixedPoint because it lets us show a progress report
        Sawyer::ProgressBar<size_t> progress(mlog[MARCH], function->printableName());
        dfEngine.reset(startVertexId, initialState_);
        while (dfEngine.runOneIteration())
            ++progress;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    }
    finalState_ = dfEngine.getInitialState(returnVertex->id());
    MemoryStatePtr finalMemory = MemoryState::promote(finalState_->get_memory_state());
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  " <<(converged ? "data-flow converged" : "DATA-FLOW DID NOT CONVERGE") <<"\n";
        mlog[DEBUG] <<"  memory addresses read by each instruction:\n";
        finalMemory->printAddressesRead(mlog[DEBUG], "    ");
    }
    
    // Find instructions that defined addresses, and the addresses read by those instructions
    size_t dataWordSize = partitioner.instructionProvider().stackPointerRegister().get_nbits();
    Sawyer::Container::Set<uint64_t> addrSeen;
    Sawyer::Container::Set<SgAsmInstruction*> insnSeen;
    SAWYER_MESG(mlog[DEBUG]) <<"  instructions that defined memory addresses:\n";
    BOOST_FOREACH (const BaseSemantics::StatePtr &state, dfEngine.getFinalStates()) {
        BaseSemantics::MemoryCellListPtr memState = BaseSemantics::MemoryCellList::promote(state->get_memory_state());
        BOOST_FOREACH (BaseSemantics::MemoryCellPtr &cell, memState->get_cells())
            conditionallySavePointer(cell->get_address(), addrSeen, insnSeen, dataWordSize, dataPointers_);
    }

    // Find instructions that defined instruction pointer values, and the addresses read by those instructions
    size_t codeWordSize = partitioner.instructionProvider().instructionPointerRegister().get_nbits();
    insnSeen.clear();
    addrSeen.clear();
    SAWYER_MESG(mlog[DEBUG]) <<"  instructions that defined the instruction pointer:\n";
    const RegisterDescriptor IP = partitioner.instructionProvider().instructionPointerRegister();
    BOOST_FOREACH (const BaseSemantics::StatePtr &state, dfEngine.getFinalStates()) {
        SymbolicSemantics::SValuePtr ip = SymbolicSemantics::SValue::promote(state->readRegister(IP, ops.get()));
        SymbolicExpr::Ptr ipExpr = ip->get_expression();
        if (!addrSeen.exists(ipExpr->hash())) {
            bool isSignificant = false;
            if (!settings_.ignoreConstIp) {
                isSignificant = true;
            } else if (ipExpr->isInteriorNode() && ipExpr->isInteriorNode()->getOperator() == SymbolicExpr::OP_ITE) {
                isSignificant = !ipExpr->isInteriorNode()->child(1)->isNumber() ||
                                !ipExpr->isInteriorNode()->child(2)->isNumber();
            } else if (!ipExpr->isNumber()) {
                isSignificant = true;
            }
            if (isSignificant)
                conditionallySavePointer(ip, addrSeen, insnSeen, codeWordSize, codePointers_);
        }
    }

    // Show code and data pointers for debugging
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  addresses of data pointers:\n";
        BOOST_FOREACH (const SymbolicExpr::Ptr &ptrVa, dataPointers_.values())
            mlog[DEBUG] <<"    " <<*ptrVa <<"\n";
        mlog[DEBUG] <<"  addresses of code pointers:\n";
        BOOST_FOREACH (const SymbolicExpr::Ptr &ptrVa, codePointers_.values())
            mlog[DEBUG] <<"    " <<*ptrVa <<"\n";
    }

    hasResults_ = true;
    didConverge_ = converged;
}

// Sort exprs in place
static void
sortLexically(std::vector<SymbolicExpr::Ptr> &exprs) {
    Sawyer::Container::Map<std::string, SymbolicExpr::Ptr> sorted;
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs) {
        std::ostringstream ss;
        ss <<*expr;
        sorted.insert(ss.str(), expr);
    }

    exprs.clear();
    exprs.reserve(sorted.size());
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, sorted.values())
        exprs.push_back(expr);
}

std::vector<SymbolicExpr::Ptr>
Analysis::codePointers(bool sort) const {
    std::vector<SymbolicExpr::Ptr> retval;
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, codePointers_.values())
        retval.push_back(expr);
    if (sort)
        sortLexically(retval);
    return retval;
}

std::vector<SymbolicExpr::Ptr>
Analysis::dataPointers(bool sort) const {
    std::vector<SymbolicExpr::Ptr> retval;
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, dataPointers_.values())
        retval.push_back(expr);
    if (sort)
        sortLexically(retval);
    return retval;
}

    
} // namespace
} // namespace
} // namespace
