#include <sage3basic.h>
#include <AsmUnparser_compat.h>
#include <BaseSemantics2.h>
#include <BinaryFeasiblePath.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphAlgorithm.h>
#include <SymbolicMemory2.h>
#include <YicesSolver.h>

#include <boost/logic/tribool.hpp>

using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace Sawyer::Message::Common;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {

namespace {

SymbolicSemantics::Formatter symbolicFormat(const std::string &prefix="") {
    SymbolicSemantics::Formatter retval;
    retval.set_line_prefix(prefix);
    //retval.expr_formatter.max_depth = settings.maxExprDepth;
    //retval.expr_formatter.show_width = settings.showExprWidth;
    return retval;
}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef SymbolicSemantics::SValue SValue;
typedef SymbolicSemantics::SValuePtr SValuePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RegisterState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef SymbolicSemantics::RegisterState RegisterState;
typedef SymbolicSemantics::RegisterStatePtr RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// varies with path-finding mode


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<class State> StatePtr;

// Semantic state holds mapping from symbolic variable names to comments about where the variable came from.
class State: public BaseSemantics::State {
public:
    typedef BaseSemantics::State Super;

private:
    // Maps symbolic variable names to comments, e.g., "v801" => "eax read at path position #6 by 0x08040000 cmp eax, [esp]"
    typedef Sawyer::Container::Map<std::string /*name*/, std::string /*comment*/> VarComments;
    VarComments varComments_;

protected:
    State(const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::MemoryStatePtr &memory)
        : Super(registers, memory) {}

    State(const State &other)
        : Super(other), varComments_(other.varComments_) {}

public:
    static BaseSemantics::StatePtr instance(const BaseSemantics::RegisterStatePtr &registers,
                                            const BaseSemantics::MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }

    static BaseSemantics::StatePtr
    instance(const StatePtr &other) {
        return StatePtr(new State(*other));
    }

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
    
    /** Set comment for variable name if none exists. */
    void varComment(const std::string &varName, const std::string &comment) {
        varComments_.insertMaybe(varName, comment);
    }

    /** Comment for variable name. */
    std::string varComment(const std::string &varName) const {
        return varComments_.getOrElse(varName, "");
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

// RiscOperators that add some additional tracking information for memory values.
class RiscOperators: public SymbolicSemantics::RiscOperators {
    typedef SymbolicSemantics::RiscOperators Super;
public:
    size_t pathInsnIndex_;                              // current location in path, or -1
    const P2::Partitioner *partitioner_;

protected:
    RiscOperators(const P2::Partitioner *partitioner, const BaseSemantics::SValuePtr &protoval,
                  rose::BinaryAnalysis::SMTSolver *solver)
        : Super(protoval, solver), pathInsnIndex_(-1), partitioner_(partitioner) {
        name("FindPath");
    }

    RiscOperators(const P2::Partitioner *partitioner, const BaseSemantics::StatePtr &state,
                  rose::BinaryAnalysis::SMTSolver *solver)
        : Super(state, solver), pathInsnIndex_(-1), partitioner_(partitioner) {
        name("FindPath");
    }

public:
    static RiscOperatorsPtr instance(const P2::Partitioner *partitioner, const RegisterDictionary *regdict,
                                     FeasiblePath::SearchMode searchMode, rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory;
        switch (searchMode) {
            case FeasiblePath::SEARCH_MULTI:
                // If we're sending multiple paths at a time to the SMT solver then we need to provide the SMT solver with
                // detailed information about how memory is affected on those different paths.
                memory = BaseSemantics::SymbolicMemory::instance(protoval, protoval);
                break;
            case FeasiblePath::SEARCH_SINGLE_DFS:
            case FeasiblePath::SEARCH_SINGLE_BFS:
                // We can perform memory-related operations and simplifications inside ROSE, which results in more but smaller
                // expressions being sent to the SMT solver.
                memory = SymbolicSemantics::MemoryState::instance(protoval, protoval);
                break;
        }
        ASSERT_not_null(memory);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(partitioner, state, solver));
    }

    static RiscOperatorsPtr instance(const P2::Partitioner *partitioner, const BaseSemantics::SValuePtr &protoval,
                                     rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, protoval, solver));
    }

    static RiscOperatorsPtr instance(const P2::Partitioner *partitioner, const BaseSemantics::StatePtr &state,
                                     rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, state, solver));
    }

public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   rose::BinaryAnalysis::SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(NULL, protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   rose::BinaryAnalysis::SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(NULL, state, solver);
    }

public:
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    std::string varComment(const std::string &varName) const {
        return State::promote(currentState())->varComment(varName);
    }

    void varComment(const std::string &varName, const std::string &comment) {
        State::promote(currentState())->varComment(varName, comment);
    }

    size_t pathInsnIndex() const {
        return pathInsnIndex_;
    }

    void pathInsnIndex(size_t n) {
        pathInsnIndex_ = n;
    }

    void partitioner(const P2::Partitioner *p) {
        partitioner_ = p;
    }

private:
    /** Create a comment to describe a variable stored in a register. */
    std::string commentForVariable(const RegisterDescriptor &reg, const std::string &accessMode) const {
        const RegisterDictionary *regs = currentState()->registerState()->get_register_dictionary();
        std::string varComment = RegisterNames(regs)(reg) + " first " + accessMode;
        if (pathInsnIndex_ == (size_t)(-1) && currentInstruction() == NULL) {
            varComment += " by initialization";
        } else {
            if (pathInsnIndex_ != (size_t)(-1))
                varComment += " at path position #" + StringUtility::numberToString(pathInsnIndex_);
            if (SgAsmInstruction *insn = currentInstruction())
                varComment += " by " + unparseInstructionWithAddress(insn);
        }
        return varComment;
    }

    /** Create a comment to describe a memory address if possible. The nBytes will be non-zero when we're describing
     *  an address as opposed to a value stored across some addresses. */
    std::string commentForVariable(const BaseSemantics::SValuePtr &addr,
                                   const std::string &accessMode, size_t byteNumber=0, size_t nBytes=0) const {
        std::string varComment = "first " + accessMode + " at ";
        if (pathInsnIndex_ != (size_t)(-1))
            varComment += "path position #" + StringUtility::numberToString(pathInsnIndex_) + ", ";
        varComment += "instruction " + unparseInstructionWithAddress(currentInstruction());

        // Sometimes we can save useful information about the address.
        if (nBytes != 1) {
            SymbolicExpr::Ptr addrExpr = SValue::promote(addr)->get_expression();
            if (SymbolicExpr::LeafPtr addrLeaf = addrExpr->isLeafNode()) {
                if (addrLeaf->isNumber()) {
                    varComment += "\n";
                    if (nBytes > 1) {
                        varComment += StringUtility::numberToString(byteNumber) + " of " +
                                      StringUtility::numberToString(nBytes) + " bytes starting ";
                    }
                    varComment += "at address " + addrLeaf->toString();
                }
            } else if (SymbolicExpr::InteriorPtr addrINode = addrExpr->isInteriorNode()) {
                if (addrINode->getOperator() == SymbolicExpr::OP_ADD && addrINode->nChildren() == 2 &&
                    addrINode->child(0)->isLeafNode() && addrINode->child(0)->isLeafNode()->isVariable() &&
                    addrINode->child(1)->isLeafNode() && addrINode->child(1)->isLeafNode()->isNumber()) {
                    SymbolicExpr::LeafPtr base = addrINode->child(0)->isLeafNode();
                    SymbolicExpr::LeafPtr offset = addrINode->child(1)->isLeafNode();
                    varComment += "\n";
                    if (nBytes > 1) {
                        varComment += StringUtility::numberToString(byteNumber) + " of " +
                                      StringUtility::numberToString(nBytes) + " bytes starting ";
                    }
                    varComment += "at address ";
                    if (base->comment().empty()) {
                        varComment = base->toString();
                    } else {
                        varComment += base->comment();
                    }
                    Sawyer::Container::BitVector tmp = offset->bits();
                    if (tmp.get(tmp.size()-1)) {
                        varComment += " - 0x" + tmp.negate().toHex();
                    } else {
                        varComment += " + 0x" + tmp.toHex();
                    }
                }
            }
        }
        return varComment;
    }

public:
    virtual void startInstruction(SgAsmInstruction *insn) {
        ASSERT_not_null(partitioner_);
        Super::startInstruction(insn);
        if (mlog[DEBUG]) {
            SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
            mlog[DEBUG] <<"  +-------------------------------------------------\n"
                        <<"  | " <<unparseInstructionWithAddress(insn) <<"\n"
                        <<"  +-------------------------------------------------\n"
                        <<"    state before instruction:\n"
                        <<(*currentState() + fmt);
        }
    }

    virtual void finishInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE {
        if (mlog[DEBUG]) {
            SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
            mlog[DEBUG] <<"    state after instruction:\n" <<(*currentState()+fmt);
        }
        Super::finishInstruction(insn);
    }

    virtual BaseSemantics::SValuePtr readRegister(const RegisterDescriptor &reg,
                 const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE {
        SValuePtr retval = SValue::promote(Super::readRegister(reg, dflt));
        SymbolicExpr::Ptr expr = retval->get_expression();
        if (expr->isLeafNode()) {
            std::string comment = commentForVariable(reg, "read");
            State::promote(currentState())->varComment(expr->isLeafNode()->toString(), comment);
        }
        return retval;
    }
        
    virtual void writeRegister(const RegisterDescriptor &reg,
                  const BaseSemantics::SValuePtr &value) ROSE_OVERRIDE {
        SymbolicExpr::Ptr expr = SValue::promote(value)->get_expression();
        if (expr->isLeafNode()) {
            std::string comment = commentForVariable(reg, "write");
            State::promote(currentState())->varComment(expr->isLeafNode()->toString(), comment);
        }
        Super::writeRegister(reg, value);
    }
        
    // If multi-path is enabled, then return a new memory expression that describes the process of reading a value from the
    // specified address; otherwise, actually read the value and return it.  In any case, record some information about the
    // address that's being read if we've never seen it before.
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt_,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        BaseSemantics::SValuePtr dflt = dflt_;
        const size_t nBytes = dflt->get_width() / 8;
        if (cond->is_number() && !cond->get_number())
            return dflt_;

        // If we know the address and that memory exists, then read the memory to obtain the default value.
        uint8_t buf[8];
        if (addr->is_number() && nBytes < sizeof(buf) &&
            nBytes == partitioner_->memoryMap().at(addr->get_number()).limit(nBytes).read(buf).size()) {
            // FIXME[Robb P. Matzke 2015-05-25]: assuming little endian
            uint64_t value = 0;
            for (size_t i=0; i<nBytes; ++i)
                value |= (uint64_t)buf[i] << (8*i);
            dflt = number_(dflt->get_width(), value);
        }

        // Read from the symbolic state, and update the state with the default from real memory if known.
        BaseSemantics::SValuePtr retval = Super::readMemory(segreg, addr, dflt, cond);

        if (!currentInstruction())
            return retval;                              // not called from dispatcher on behalf of an instruction

        // Save a description of the variable
        SymbolicExpr::Ptr valExpr = SValue::promote(retval)->get_expression();
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->isVariable()) {
            std::string comment = commentForVariable(addr, "read");
            State::promote(currentState())->varComment(valExpr->isLeafNode()->toString(), comment);
        }

        // Save a description for its addresses
        for (size_t i=0; i<nBytes; ++i) {
            SValuePtr va = SValue::promote(add(addr, number_(addr->get_width(), i)));
            if (va->get_expression()->isLeafNode()) {
                std::string comment = commentForVariable(addr, "read", i, nBytes);
                State::promote(currentState())->varComment(va->get_expression()->isLeafNode()->toString(), comment);
            }
        }
        return retval;
    }
        
    // If multi-path is enabled, then return a new memory expression that updates memory with a new address/value pair;
    // otherwise update the memory directly.  In any case, record some information about the address that was written if we've
    // never seen it before.
    virtual void writeMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &value, const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        if (cond->is_number() && !cond->get_number())
            return;
        Super::writeMemory(segreg, addr, value, cond);

        // Save a description of the variable
        SymbolicExpr::Ptr valExpr = SValue::promote(value)->get_expression();
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->isVariable()) {
            std::string comment = commentForVariable(addr, "write");
            State::promote(currentState())->varComment(valExpr->isLeafNode()->toString(), comment);
        }

        // Save a description for its addresses
        size_t nBytes = value->get_width() / 8;
        for (size_t i=0; i<nBytes; ++i) {
            SValuePtr va = SValue::promote(add(addr, number_(addr->get_width(), i)));
            if (va->get_expression()->isLeafNode()) {
                std::string comment = commentForVariable(addr, "read", i, nBytes);
                State::promote(currentState())->varComment(va->get_expression()->isLeafNode()->toString(), comment);
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
hasVirtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    return vertex->value().type() == P2::V_BASIC_BLOCK || vertex->value().type() == P2::V_USER_DEFINED;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

Sawyer::Message::Facility FeasiblePath::mlog;

FeasiblePath::FunctionSummary::FunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &cfgFuncVertex,
                                               uint64_t stackDelta)
    : address(cfgFuncVertex->value().address()), stackDelta(stackDelta) {
    if (cfgFuncVertex->value().type() == P2::V_BASIC_BLOCK) {
        if (P2::Function::Ptr function = cfgFuncVertex->value().isEntryBlock()) {
            name = function->printableName();
            return;
        }
    }
    name = P2::Partitioner::vertexName(*cfgFuncVertex);
}

// class method
void
FeasiblePath::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(mlog, "rose::BinaryAnalysis::FeasiblePath");
    }
}


rose_addr_t
FeasiblePath::virtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    if (vertex->value().type() == P2::V_BASIC_BLOCK || vertex->value().type() == P2::V_USER_DEFINED)
        return vertex->value().address();
    ASSERT_not_reachable("invalid vertex type");
}

BaseSemantics::DispatcherPtr
FeasiblePath::buildVirtualCpu(const P2::Partitioner &partitioner) {
    // Augment the register dictionary with a "path" register that holds the expression describing how the location is
    // reachable along some path.
    if (NULL == registers_) {
        registers_ = new RegisterDictionary("rose::BinaryAnalysis::FeasiblePath");
        registers_->insert(partitioner.instructionProvider().registerDictionary());
        ASSERT_forbid(REG_PATH.is_valid());
        REG_PATH = RegisterDescriptor(registers_->firstUnusedMajor(), 0, 0, 1);
        registers_->insert("path", REG_PATH);

        // Where are return values stored?  FIXME[Robb Matzke 2015-12-01]: We need to support returning multiple values. We
        // should be using the new calling convention analysis to detect these.
        ASSERT_forbid(REG_RETURN_.is_valid());
        const RegisterDescriptor *r = NULL;
        if ((r = registers_->lookup("rax")) || (r = registers_->lookup("eax")) || (r = registers_->lookup("ax"))) {
            REG_RETURN_ = *r;
        } else if ((r = registers_->lookup("d0"))) {
            REG_RETURN_ = *r;                           // m68k also typically has other return registers
        } else {
            ASSERT_not_implemented("function return value register is not implemented for this ISA/ABI");
        }
    }

    // Create the RiscOperators and Dispatcher. We could use an SMT solver here also, but it seems to slow things down more
    // than speed them up.
    SMTSolver *solver = NULL;
    RiscOperatorsPtr ops = RiscOperators::instance(&partitioner, registers_, settings_.searchMode, solver);
    ASSERT_not_null(partitioner.instructionProvider().dispatcher());
    BaseSemantics::DispatcherPtr cpu = partitioner.instructionProvider().dispatcher()->create(ops);
    ASSERT_not_null(cpu);
    return cpu;
}

void
FeasiblePath::setInitialState(const BaseSemantics::DispatcherPtr &cpu,
                              const P2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex) {
    ASSERT_not_null(cpu);
    ASSERT_require(REG_PATH.is_valid());

    // Create the new state from an existing state and make the new state current.
    BaseSemantics::StatePtr state = cpu->currentState()->clone();
    state->clear();
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    ops->currentState(state);

    // Start of path is always feasible.
    ops->writeRegister(REG_PATH, ops->boolean_(true));

    // Initialize instruction pointer register
    if (pathsBeginVertex->value().type() == P2::V_INDETERMINATE) {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->undefined_(cpu->instructionPointerRegister().get_nbits()));
    } else {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->number_(cpu->instructionPointerRegister().get_nbits(), pathsBeginVertex->value().address()));
    }

    // Initialize stack pointer register
    if (settings_.initialStackPtr) {
        const RegisterDescriptor REG_SP = cpu->stackPointerRegister();
        ops->writeRegister(REG_SP, ops->number_(REG_SP.get_nbits(), *settings_.initialStackPtr));
    }

    // Direction flag (DF) is always set
    if (const RegisterDescriptor *REG_DF = cpu->get_register_dictionary()->lookup("df"))
        ops->writeRegister(*REG_DF, ops->boolean_(true));
}

/** Process instructions for one basic block on the specified virtual CPU. */
void
FeasiblePath::processBasicBlock(const P2::BasicBlock::Ptr &bblock, const BaseSemantics::DispatcherPtr &cpu,
                                size_t pathInsnIndex) {
    ASSERT_not_null(bblock);
    
    // Update the path constraint "register"
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    RegisterDescriptor IP = cpu->instructionPointerRegister();
    BaseSemantics::SValuePtr ip = ops->readRegister(IP, ops->undefined_(IP.get_nbits()));
    BaseSemantics::SValuePtr va = ops->number_(ip->get_width(), bblock->address());
    BaseSemantics::SValuePtr pathConstraint = ops->isEqual(ip, va);
    ops->writeRegister(REG_PATH, pathConstraint);

    // Process each instruction in the basic block
    try {
        BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
            if (pathInsnIndex != size_t(-1))
                ops->pathInsnIndex(pathInsnIndex++);
            cpu->processInstruction(insn);
        }
    } catch (const BaseSemantics::Exception &e) {
        mlog[ERROR] <<"semantics failed: " <<e <<"\n";
        return;
    }
}

/** Process an indeterminate block. This represents flow of control through an unknown address. */
void
FeasiblePath::processIndeterminateBlock(const P2::ControlFlowGraph::ConstVertexIterator &vertex,
                                        const BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex) {
    mlog[WARN] <<"control flow passes through an indeterminate address at path position #" <<pathInsnIndex <<"\n";
}

/** Process a function summary vertex. */
void
FeasiblePath::processFunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                                     const BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex) {
    ASSERT_require(functionSummaries_.exists(pathsVertex->value().address()));
    const FunctionSummary &summary = functionSummaries_[pathsVertex->value().address()];

    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    if (pathInsnIndex != size_t(-1))
        ops->pathInsnIndex(pathInsnIndex);

    // Make the function return an unknown value
    SymbolicSemantics::SValuePtr retval = SymbolicSemantics::SValue::promote(ops->undefined_(REG_RETURN_.get_nbits()));
    std::string comment = "return value from " + summary.name + "\n" +
                          "at path position #" + StringUtility::numberToString(ops->pathInsnIndex());
    ops->varComment(retval->get_expression()->isLeafNode()->toString(), comment);
    ops->writeRegister(REG_RETURN_, retval);

    // Cause the function to return to the address stored at the top of the stack.
    RegisterDescriptor SP = cpu->stackPointerRegister();
    BaseSemantics::SValuePtr stackPointer = ops->readRegister(SP, ops->undefined_(SP.get_nbits()));
    BaseSemantics::SValuePtr returnTarget = ops->readMemory(RegisterDescriptor(), stackPointer,
                                                            ops->undefined_(stackPointer->get_width()), ops->boolean_(true));
    ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);

    // Pop some things from the stack.
    int64_t sd = summary.stackDelta != SgAsmInstruction::INVALID_STACK_DELTA ?
                 summary.stackDelta :
                 returnTarget->get_width() / 8;
    stackPointer = ops->add(stackPointer, ops->number_(stackPointer->get_width(), sd));
    ops->writeRegister(cpu->stackPointerRegister(), stackPointer);
}

void
FeasiblePath::processVertex(const BaseSemantics::DispatcherPtr &cpu,
                            const P2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                            size_t &pathInsnIndex /*in,out*/) {
    switch (pathsVertex->value().type()) {
        case P2::V_BASIC_BLOCK:
            processBasicBlock(pathsVertex->value().bblock(), cpu, pathInsnIndex);
            pathInsnIndex += pathsVertex->value().bblock()->instructions().size();
            break;
        case P2::V_INDETERMINATE:
            processIndeterminateBlock(pathsVertex, cpu, pathInsnIndex);
            ++pathInsnIndex;
            break;
        case P2::V_USER_DEFINED:
            processFunctionSummary(pathsVertex, cpu, pathInsnIndex);
            ++pathInsnIndex;
            break;
        default:
            mlog[ERROR] <<"cannot comput path feasibility; invalid vertex type at "
                        <<P2::Partitioner::vertexName(*pathsVertex) <<"\n";
            cpu->get_operators()->writeRegister(cpu->instructionPointerRegister(),
                                                cpu->get_operators()->number_(cpu->instructionPointerRegister().get_nbits(),
                                                                              0x911 /*arbitrary, unlikely to be satisfied*/));
            ++pathInsnIndex;
    }
}

boost::logic::tribool
FeasiblePath::isPathFeasible(const P2::CfgPath &path, const std::vector<SymbolicExpr::Ptr> &endConstraints,
                             std::vector<SymbolicExpr::Ptr> &pathConstraints /*in,out*/,
                             BaseSemantics::DispatcherPtr &cpu /*out*/) {
    ASSERT_not_null2(partitioner_, "analysis is not initialized");
    YicesSolver solver;
    cpu = buildVirtualCpu(partitioner());
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    setInitialState(cpu, path.frontVertex());

    size_t pathInsnIndex = 0;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &pathEdge, path.edges()) {
        processVertex(cpu, pathEdge->source(), pathInsnIndex /*in,out*/);
        RegisterDescriptor IP = partitioner().instructionProvider().instructionPointerRegister();
        BaseSemantics::SValuePtr ip = ops->readRegister(IP, ops->undefined_(IP.get_nbits()));
        if (ip->is_number()) {
            ASSERT_require(hasVirtualAddress(pathEdge->target()));
            if (ip->get_number() != virtualAddress(pathEdge->target())) {
                // Executing the path forces us to go a different direction than where the path indicates we should go. We
                // don't need an SMT solver to tell us that when the values are just integers.
                return false;
            }
        } else if (hasVirtualAddress(pathEdge->target())) {
            SymbolicExpr::Ptr targetVa = SymbolicExpr::makeInteger(ip->get_width(), virtualAddress(pathEdge->target()));
            SymbolicExpr::Ptr constraint = SymbolicExpr::makeEq(targetVa,
                                                                SymbolicSemantics::SValue::promote(ip)->get_expression());
            pathConstraints.push_back(constraint);
        }
    }

    // Are the constraints satisfiable.  Empty constraints are tivially satisfiable.
    pathConstraints.insert(pathConstraints.end(), endConstraints.begin(), endConstraints.end());
    switch (solver.satisfiable(pathConstraints)) {
        case SMTSolver::SAT_YES: return true;
        case SMTSolver::SAT_NO: return false;
        default: return boost::logic::indeterminate;
    }
}

void
FeasiblePath::setSearchBoundary(const P2::Partitioner &partitioner,
                                const P2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                                const P2::ControlFlowGraph::ConstVertexIterator &cfgEndVertex,
                                const P2::CfgConstVertexSet &cfgAvoidVertices,
                                const P2::CfgConstEdgeSet &cfgAvoidEdges) {
    P2::CfgConstVertexSet cfgBeginVertices;
    cfgBeginVertices.insert(cfgBeginVertex);
    P2::CfgConstVertexSet cfgEndVertices;
    cfgEndVertices.insert(cfgEndVertex);
    setSearchBoundary(partitioner, cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges);
}

P2::ControlFlowGraph::ConstVertexIterator
FeasiblePath::pathToCfg(const P2::ControlFlowGraph::ConstVertexIterator &pathVertex) const {
    if (hasVirtualAddress(pathVertex))
        return partitioner().findPlaceholder(virtualAddress(pathVertex));
    if (pathVertex->value().type() == P2::V_INDETERMINATE)
        return partitioner().indeterminateVertex();
    ASSERT_not_implemented("cannot convert path vertex to CFG vertex");
}

P2::CfgConstVertexSet
FeasiblePath::cfgToPaths(const P2::CfgConstVertexSet &vertices) const {
    P2::CfgConstVertexSet retval;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, vertices) {
        if (vmap_.forward().exists(vertex))
            retval.insert(vmap_.forward()[vertex]);
    }
    return retval;
}

void
FeasiblePath::setSearchBoundary(const P2::Partitioner &partitioner,
                                const P2::CfgConstVertexSet &cfgBeginVertices,
                                const P2::CfgConstVertexSet &cfgEndVertices,
                                const P2::CfgConstVertexSet &cfgAvoidVertices,
                                const P2::CfgConstEdgeSet &cfgAvoidEdges) {
    reset();
    partitioner_ = &partitioner;

    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    findInterFunctionPaths(partitioner.cfg(), paths_/*out*/, vmap_/*out*/,
                           cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges);
    if (paths_.isEmpty())
        return;

    // Paths graph equivalents of CFG arguments.
    pathsBeginVertices_ = cfgToPaths(cfgBeginVertices);
    pathsEndVertices_ = cfgToPaths(cfgEndVertices);

    // When finding paths through a called function, avoid the usual vertices and edges, but also avoid those vertices that
    // mark the end of paths. We want paths that go all the way from the entry block of the called function to its returning
    // blocks.
    cfgEndAvoidVertices_ = cfgAvoidVertices;
    cfgEndAvoidVertices_.insert(cfgEndVertices.begin(), cfgEndVertices.end());
    cfgAvoidEdges_ = cfgAvoidEdges;
}

const P2::Partitioner&
FeasiblePath::partitioner() const {
    ASSERT_not_null2(partitioner_, "no search boundary set yet");
    return *partitioner_;
}

// True if vertex is a function call
bool
FeasiblePath::isFunctionCall(const P2::ControlFlowGraph::ConstVertexIterator &pathVertex) const {
    P2::ControlFlowGraph::ConstVertexIterator cfgVertex = pathToCfg(pathVertex);
    ASSERT_require(partitioner().cfg().isValidVertex(cfgVertex));
    BOOST_FOREACH (P2::ControlFlowGraph::Edge edge, cfgVertex->outEdges()) {
        if (edge.value().type() == P2::E_FUNCTION_CALL)
            return true;
    }
    return false;
}

// True if path ends with a function call.
bool
FeasiblePath::pathEndsWithFunctionCall(const P2::CfgPath &path) const {
    if (path.isEmpty())
        return false;
    P2::ControlFlowGraph::ConstVertexIterator pathVertex = path.backVertex();
    return isFunctionCall(pathVertex);
}

bool
FeasiblePath::shouldSummarizeCall(const P2::ControlFlowGraph::ConstVertexIterator &pathVertex,
                                  const P2::ControlFlowGraph &cfg,
                                  const P2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget) {
    if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
        return false;
    P2::Function::Ptr callee = cfgCallTarget->value().isEntryBlock();
    if (!callee)
        return false;
    if (boost::ends_with(callee->name(), "@plt") || boost::ends_with(callee->name(), ".dll"))
        return true;

    // Summarize if the user desires it.
    if (std::find(settings_.summarizeFunctions.begin(), settings_.summarizeFunctions.end(), callee->address()) !=
        settings_.summarizeFunctions.end())
        return true;

    // If the called function calls too many more functions then summarize instead of inlining.  This helps avoid problems with
    // the Partitioner2 not being able to find reasonable function boundaries, especially for GNU libc.
    static const size_t maxCallsAllowed = 10;           // arbitrary
    size_t nCalls = 0;
    using namespace Sawyer::Container::Algorithm;
    typedef DepthFirstForwardEdgeTraversal<const P2::ControlFlowGraph> Traversal;
    for (Traversal t(cfg, cfgCallTarget); t; ++t) {
        if (t.edge()->value().type() == P2::E_FUNCTION_CALL) {
            if (++nCalls > maxCallsAllowed)
                return true;
            t.skipChildren();
        } else if (t.edge()->value().type() == P2::E_FUNCTION_RETURN) {
            t.skipChildren();
        }
    }

    return false;
}

bool
FeasiblePath::shouldInline(const P2::CfgPath &path, const P2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget) {
    // We must inline indeterminte functions or else we'll end up removing the call and return edge, which is another way
    // of saying "we know there's no path through here" when in fact there could be.
    if (cfgCallTarget->value().type() == P2::V_INDETERMINATE)
        return true;

    // Don't let the call depth get too deep
    if (settings_.maxCallDepth > 0) {
        ssize_t callDepth = path.callDepth();
        ASSERT_require(callDepth >= 0);
        if ((size_t)callDepth >= settings_.maxCallDepth)
            return false;
    }

    // Don't let recursion get too deep
    if (settings_.maxRecursionDepth > 0) {
        if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
            return false;
        P2::Function::Ptr callee = cfgCallTarget->value().isEntryBlock();
        if (!callee)
            return false;
        ssize_t callDepth = path.callDepth(callee);
        ASSERT_require(callDepth >= 0);
        if ((size_t)callDepth >= settings_.maxRecursionDepth)
            return false;
    }

    return true;
}

void
FeasiblePath::insertCallSummary(const P2::ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                                const P2::ControlFlowGraph &cfg, const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge) {
    ASSERT_require(cfg.isValidEdge(cfgCallEdge));
    P2::ControlFlowGraph::ConstVertexIterator cfgCallTarget = cfgCallEdge->target();
    P2::Function::Ptr function = cfgCallTarget->value().isEntryBlock();

    P2::ControlFlowGraph::VertexIterator summaryVertex = paths_.insertVertex(P2::CfgVertex(P2::V_USER_DEFINED));
    paths_.insertEdge(pathsCallSite, summaryVertex, P2::CfgEdge(P2::E_FUNCTION_CALL));
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &callret, P2::findCallReturnEdges(pathsCallSite))
        paths_.insertEdge(summaryVertex, callret->target(), P2::CfgEdge(P2::E_FUNCTION_RETURN));

    int64_t stackDelta = function ? function->stackDeltaConcrete() : SgAsmInstruction::INVALID_STACK_DELTA;

    FunctionSummary summary(cfgCallTarget, stackDelta);
    functionSummaries_.insert(summary.address, summary);
    summaryVertex->value().address(summary.address);
}

void 
FeasiblePath::depthFirstSearch(PathProcessor &pathProcessor) {
    Stream info(mlog[INFO]);
    if (paths_.isEmpty())
        return;

    BOOST_FOREACH (P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex, pathsBeginVertices_) {
        P2::CfgPath path(pathsBeginVertex);
        while (!path.isEmpty()) {
            // If backVertex is a function summary, then there is no corresponding cfgBackVertex.
            P2::ControlFlowGraph::ConstVertexIterator backVertex = path.backVertex();
            P2::ControlFlowGraph::ConstVertexIterator cfgBackVertex = pathToCfg(backVertex);

            bool doBacktrack = false;
            bool atEndOfPath = pathsEndVertices_.find(backVertex) != pathsEndVertices_.end();

            // Test path feasibility
            std::vector<SymbolicExpr::Ptr> postConditions, pathConditions;
            if (atEndOfPath)
                postConditions = settings_.postConditions;
            BaseSemantics::DispatcherPtr cpu;
            boost::logic::tribool isFeasible = isPathFeasible(path, postConditions, pathConditions /*in,out*/, cpu /*out*/);

            // Invoke callback if path is found
            if (atEndOfPath) {
                if (isFeasible) {
                    switch (pathProcessor.found(*this, path, pathConditions, cpu)) {
                        case PathProcessor::BREAK: return;
                        case PathProcessor::CONTINUE: break;
                        default: ASSERT_not_reachable("invalid path processor action");
                    }
                }
                doBacktrack = true;
            }

            // If we've visited a vertex too many times (e.g., because of a loop or recursion), then don't go any further.
            if (settings_.vertexVisitLimit > 0 && path.nVisits(backVertex) > settings_.vertexVisitLimit) {
                mlog[WARN] <<"max visits (" <<settings_.vertexVisitLimit <<") reached for vertex " <<backVertex->id() <<"\n";
                doBacktrack = true;
            }

            // Limit path length (in terms of number of instructions)
            if (settings_.maxPathLength > 0 && !doBacktrack) {
                size_t pathNInsns = 0;
                BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, path.vertices()) {
                    switch (vertex->value().type()) {
                        case P2::V_BASIC_BLOCK:
                            pathNInsns += vertex->value().bblock()->instructions().size();
                            break;
                        case P2::V_INDETERMINATE:
                        case P2::V_USER_DEFINED:
                            ++pathNInsns;
                            break;
                        default:
                            ASSERT_not_reachable("invalid path vertex type");
                    }
                }
                if (pathNInsns > settings_.maxPathLength) {
                    mlog[WARN] <<"maximum path length exceeded (" <<settings_.maxPathLength <<" instructions)\n";
                    doBacktrack = true;
                }
            }

            // If we're visiting a function call site, then inline callee paths into the paths graph, but continue to avoid any
            // paths that go through user-specified avoidance vertices and edges. We can modify the paths graph during the
            // traversal because we're modifying parts of the graph that aren't part of the current path.  This is where having
            // insert- and erase-stable graph iterators is a huge help!
            if (!doBacktrack && pathEndsWithFunctionCall(path) && !P2::findCallReturnEdges(backVertex).empty()) {
                ASSERT_require(partitioner().cfg().isValidVertex(cfgBackVertex));
                BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, P2::findCallEdges(cfgBackVertex)) {
                    if (shouldSummarizeCall(path.backVertex(), partitioner().cfg(), cfgCallEdge->target())) {
                        insertCallSummary(backVertex, partitioner().cfg(), cfgCallEdge);
                    } else if (shouldInline(path, cfgCallEdge->target())) {
                        info <<"inlining function call paths at vertex " <<partitioner().vertexName(backVertex) <<"\n";
                        P2::insertCalleePaths(paths_, backVertex, partitioner().cfg(),
                                              cfgBackVertex, cfgEndAvoidVertices_, cfgAvoidEdges_);
                    } else {
                        insertCallSummary(backVertex, partitioner().cfg(), cfgCallEdge);
                    }
                }

                // Remove all call-return edges. This is necessary so we don't re-enter this case with infinite recursion. No
                // need to worry about adjusting the path because these edges aren't on the current path.
                P2::eraseEdges(paths_, P2::findCallReturnEdges(backVertex));

                // If the inlined function had no return sites but the call site had a call-return edge, then part of the paths
                // graph might now be unreachable. In fact, there might now be no paths from the begin vertex to any end
                // vertex.  Erase those parts of the paths graph that are now unreachable.
                P2::eraseUnreachablePaths(paths_, pathsBeginVertices_, pathsEndVertices_, vmap_, path); // all args modified
                ASSERT_require2(!paths_.isEmpty() || path.isEmpty(), "path is empty only if paths graph is empty");
                if (path.isEmpty())
                    break;
                ASSERT_require(!pathsBeginVertices_.empty());
                ASSERT_require(!pathsEndVertices_.empty());
                backVertex = path.backVertex();
                cfgBackVertex = pathToCfg(backVertex);

                info <<"paths graph has " <<StringUtility::plural(paths_.nVertices(), "vertices", "vertex")
                     <<" and " <<StringUtility::plural(paths_.nEdges(), "edges") <<"\n";
            }

            // We've reached a dead end that isn't a final vertex.  This shouldn't ever happen.
            ASSERT_require2(doBacktrack || backVertex->nOutEdges()!=0,
                            "non-final vertex " + partitioner().vertexName(backVertex) + " has no out edges");

            // Advance to next path.
            if (doBacktrack) {
                // Backtrack and follow a different path.  The backtrack not only pops edges off the path, but then also appends
                // the next edge.  We must adjust visit counts for the vertices we backtracked.
                path.backtrack();
            } else {
                // Push next edge onto path.
                ASSERT_require(paths_.isValidEdge(backVertex->outEdges().begin()));
                path.pushBack(backVertex->outEdges().begin());
            }
        }
    }
}

const FeasiblePath::FunctionSummary&
FeasiblePath::functionSummary(rose_addr_t entryVa) const {
    return functionSummaries_.getOrDefault(entryVa);
}

} // namespace
} // namespace
