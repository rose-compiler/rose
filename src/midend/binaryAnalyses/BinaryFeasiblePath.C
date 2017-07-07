#include <sage3basic.h>
#include <AsmUnparser_compat.h>
#include <BaseSemantics2.h>
#include <BinaryFeasiblePath.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphAlgorithm.h>
#include <SymbolicMemory2.h>
#include <YicesSolver.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/logic/tribool.hpp>

using namespace Rose::BinaryAnalysis::InstructionSemantics2;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
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
    // Maps symbolic variable names to additional information about where the variable appears in the path. */
    typedef Sawyer::Container::Map<std::string /*name*/, FeasiblePath::VarDetail> VarDetails;
    VarDetails varDetails_;

protected:
    State(const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::MemoryStatePtr &memory)
        : Super(registers, memory) {}

    State(const State &other)
        : Super(other), varDetails_(other.varDetails_) {}

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

    /** Set detail for variable name if none exists. */
    void varDetail(const std::string &varName, const FeasiblePath::VarDetail &vdetail) {
        varDetails_.insertMaybe(varName, vdetail);
    }

    /** Detail for variable name. */
    const FeasiblePath::VarDetail& varDetail(const std::string &varName) const {
        return varDetails_.getOrDefault(varName);
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
                  Rose::BinaryAnalysis::SMTSolver *solver)
        : Super(protoval, solver), pathInsnIndex_(-1), partitioner_(partitioner) {
        name("FindPath");
    }

    RiscOperators(const P2::Partitioner *partitioner, const BaseSemantics::StatePtr &state,
                  Rose::BinaryAnalysis::SMTSolver *solver)
        : Super(state, solver), pathInsnIndex_(-1), partitioner_(partitioner) {
        name("FindPath");
    }

public:
    static RiscOperatorsPtr instance(const P2::Partitioner *partitioner, const RegisterDictionary *regdict,
                                     FeasiblePath::SearchMode searchMode, Rose::BinaryAnalysis::SMTSolver *solver=NULL) {
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
                                     Rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, protoval, solver));
    }

    static RiscOperatorsPtr instance(const P2::Partitioner *partitioner, const BaseSemantics::StatePtr &state,
                                     Rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, state, solver));
    }

public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   Rose::BinaryAnalysis::SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(NULL, protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   Rose::BinaryAnalysis::SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(NULL, state, solver);
    }

public:
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    const FeasiblePath::VarDetail& varDetail(const std::string &varName) const {
        return State::promote(currentState())->varDetail(varName);
    }

    void varDetail(const std::string &varName, const FeasiblePath::VarDetail &varDetail) {
        State::promote(currentState())->varDetail(varName, varDetail);
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
    /** Description a variable stored in a register. */
    FeasiblePath::VarDetail detailForVariable(const RegisterDescriptor &reg, const std::string &accessMode) const {
        const RegisterDictionary *regs = currentState()->registerState()->get_register_dictionary();
        FeasiblePath::VarDetail retval;
        retval.registerName = RegisterNames(regs)(reg);
        retval.firstAccessMode = accessMode;
        if (pathInsnIndex_ == (size_t)(-1) && currentInstruction() == NULL) {
            // no path position (i.e., present in initial state)
        } else {
            if (pathInsnIndex_ != (size_t)(-1))
                retval.firstAccessIdx = pathInsnIndex_;
            retval.firstAccessInsn = currentInstruction();
        }
        return retval;
    }

    /** Describe a memory address if possible. The nBytes will be non-zero when we're describing an address as opposed to a
     *  value stored across some addresses. */
    FeasiblePath::VarDetail detailForVariable(const BaseSemantics::SValuePtr &addr,
                                              const std::string &accessMode, size_t byteNumber=0, size_t nBytes=0) const {
        FeasiblePath::VarDetail retval;
        retval.firstAccessMode = accessMode;
        if (pathInsnIndex_ != (size_t)(-1))
            retval.firstAccessIdx = pathInsnIndex_;
        retval.firstAccessInsn = currentInstruction();

        // Sometimes we can save useful information about the address.
        if (nBytes != 1) {
            SymbolicExpr::Ptr addrExpr = SValue::promote(addr)->get_expression();
            if (SymbolicExpr::LeafPtr addrLeaf = addrExpr->isLeafNode()) {
                if (addrLeaf->isNumber()) {
                    retval.memSize = nBytes;
                    retval.memByteNumber = byteNumber;
                    retval.memAddress = addrExpr;
                }
            } else if (SymbolicExpr::InteriorPtr addrINode = addrExpr->isInteriorNode()) {
                if (addrINode->getOperator() == SymbolicExpr::OP_ADD && addrINode->nChildren() == 2 &&
                    addrINode->child(0)->isLeafNode() && addrINode->child(0)->isLeafNode()->isVariable() &&
                    addrINode->child(1)->isLeafNode() && addrINode->child(1)->isLeafNode()->isNumber()) {
                    SymbolicExpr::LeafPtr base = addrINode->child(0)->isLeafNode();
                    SymbolicExpr::LeafPtr offset = addrINode->child(1)->isLeafNode();
                    retval.memSize = nBytes;
                    retval.memByteNumber = byteNumber;
                    retval.memAddress = addrExpr;
                }
            }
        }
        return retval;
    }

public:
    virtual void startInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE {
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
        if (expr->isLeafNode())
            State::promote(currentState())->varDetail(expr->isLeafNode()->toString(), detailForVariable(reg, "read"));
        return retval;
    }

    virtual void writeRegister(const RegisterDescriptor &reg,
                  const BaseSemantics::SValuePtr &value) ROSE_OVERRIDE {
        SymbolicExpr::Ptr expr = SValue::promote(value)->get_expression();
        if (expr->isLeafNode())
            State::promote(currentState())->varDetail(expr->isLeafNode()->toString(), detailForVariable(reg, "write"));
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
            nBytes == partitioner_->memoryMap()->at(addr->get_number()).limit(nBytes).read(buf).size()) {
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
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->isVariable())
            State::promote(currentState())->varDetail(valExpr->isLeafNode()->toString(), detailForVariable(addr, "read"));

        // Save a description for its addresses
        for (size_t i=0; i<nBytes; ++i) {
            SValuePtr va = SValue::promote(add(addr, number_(addr->get_width(), i)));
            if (va->get_expression()->isLeafNode()) {
                State::promote(currentState())->varDetail(va->get_expression()->isLeafNode()->toString(),
                                                          detailForVariable(addr, "read", i, nBytes));
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
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->isVariable())
            State::promote(currentState())->varDetail(valExpr->isLeafNode()->toString(), detailForVariable(addr, "write"));

        // Save a description for its addresses
        size_t nBytes = value->get_width() / 8;
        for (size_t i=0; i<nBytes; ++i) {
            SValuePtr va = SValue::promote(add(addr, number_(addr->get_width(), i)));
            if (va->get_expression()->isLeafNode()) {
                State::promote(currentState())->varDetail(va->get_expression()->isLeafNode()->toString(),
                                                          detailForVariable(addr, "read", i, nBytes));
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
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::FeasiblePath");
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
        registers_ = new RegisterDictionary("Rose::BinaryAnalysis::FeasiblePath");
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
    VarDetail varDetail;
    varDetail.returnFrom = summary.address;
    varDetail.firstAccessIdx = ops->pathInsnIndex();
    ops->varDetail(retval->get_expression()->isLeafNode()->toString(), varDetail);
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

void
FeasiblePath::printPathVertex(std::ostream &out, const P2::ControlFlowGraph::Vertex &pathVertex, size_t &insnIdx) const {
    switch (pathVertex.value().type()) {
        case P2::V_BASIC_BLOCK: {
            BOOST_FOREACH (SgAsmInstruction *insn, pathVertex.value().bblock()->instructions()) {
                out <<"    #" <<std::setw(5) <<std::left <<insnIdx++
                    <<" " <<unparseInstructionWithAddress(insn) <<"\n";
            }
            break;
        }

        case P2::V_USER_DEFINED: {
            ASSERT_require(functionSummaries().exists(pathVertex.value().address()));
            const FeasiblePath::FunctionSummary &summary = functionSummary(pathVertex.value().address());
            out <<"    #" <<std::setw(5) <<std::left <<insnIdx++ <<" summary for " <<summary.name <<"\n";
            break;
        }

        case P2::V_INDETERMINATE: {
            out <<"     " <<std::setw(5) <<std::left <<"n.a." <<" indeterminate\n";
            break;
        }

        case P2::V_NONEXISTING: {
            out <<"     " <<std::setw(5) <<std::left <<"n.a." <<" nonexisting\n";
            break;
        }

        case P2::V_UNDISCOVERED: {
            out <<"     " <<std::setw(5) <<std::left <<"n.a." <<" undiscovered\n";
            break;
        }
    }
}

void
FeasiblePath::printPath(std::ostream &out, const P2::CfgPath &path) const {
    size_t pathIdx = 0, insnIdx = 0;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &pathVertex, path.vertices()) {
        if (0==pathIdx) {
            out <<"  at path vertex " <<partitioner().vertexName(pathVertex) <<"\n";
        } else {
            out <<"  via path edge " <<partitioner().edgeName(path.edges()[pathIdx-1]) <<"\n";
        }
        printPathVertex(out, *pathVertex, insnIdx /*in,out*/);
        ++pathIdx;
    }
}

boost::logic::tribool
FeasiblePath::isPathFeasible(const P2::CfgPath &path, SMTSolver &solver, const std::vector<SymbolicExpr::Ptr> &endConstraints,
                             std::vector<SymbolicExpr::Ptr> &pathConstraints /*in,out*/,
                             BaseSemantics::DispatcherPtr &cpu /*out*/) {
    static const char *prefix = "      ";
    ASSERT_not_null2(partitioner_, "analysis is not initialized");
    cpu = buildVirtualCpu(partitioner());
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    setInitialState(cpu, path.frontVertex());

    size_t pathInsnIndex = 0;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &pathEdge, path.edges()) {
        processVertex(cpu, pathEdge->source(), pathInsnIndex /*in,out*/);
        RegisterDescriptor IP = partitioner().instructionProvider().instructionPointerRegister();
        BaseSemantics::SValuePtr ip = ops->readRegister(IP, ops->undefined_(IP.get_nbits()));
        if (!settings_.nonAddressIsFeasible && !hasVirtualAddress(pathEdge->target())) {
            SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner_->edgeName(pathEdge)
                                     <<" because settings().nonAddressIsFeasible is false\n";
            return false;
        } else if (ip->is_number()) {
            if (!hasVirtualAddress(pathEdge->target())) {
                // If the IP register is pointing to an instruction but the path vertex is indeterminate (or undiscovered or
                // nonexisting) then consider this path to be not-feasible. If the CFG is accurate then there's probably
                // a sibling edge that points to the correct vertex.
                SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner_->edgeName(pathEdge) <<" because IP = "
                                         <<StringUtility::addrToString(ip->get_number()) <<" and edge target has no address\n";
                return false;
            } else if (ip->get_number() != virtualAddress(pathEdge->target())) {
                // Executing the path forces us to go a different direction than where the path indicates we should go. We
                // don't need an SMT solver to tell us that when the values are just integers.
                SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner_->edgeName(pathEdge) <<" because IP = "
                                         <<StringUtility::addrToString(ip->get_number()) <<" and edge target is "
                                         <<StringUtility::addrToString(virtualAddress(pathEdge->target())) <<"\n";
                return false;
            }
        } else if (hasVirtualAddress(pathEdge->target())) {
            SymbolicExpr::Ptr targetVa = SymbolicExpr::makeInteger(ip->get_width(), virtualAddress(pathEdge->target()));
            SymbolicExpr::Ptr constraint = SymbolicExpr::makeEq(targetVa,
                                                                SymbolicSemantics::SValue::promote(ip)->get_expression());
            constraint->comment("cfg edge " + partitioner_->edgeName(pathEdge));
            SAWYER_MESG(mlog[DEBUG]) <<prefix <<"constraint at edge " <<partitioner_->edgeName(pathEdge) <<": " <<*constraint <<"\n";
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
    ssize_t callDepth = path.callDepth();
    ASSERT_require(callDepth >= 0);
    if ((size_t)callDepth >= settings_.maxCallDepth)
        return false;

    // Don't let recursion get too deep
    if (settings_.maxRecursionDepth < (size_t)(-1)) {
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

boost::filesystem::path
FeasiblePath::emitPathGraph(size_t callId, size_t graphId) {
    char callIdStr[32], graphIdStr[32];
    sprintf(callIdStr, "invoc-%04zu", callId);
    sprintf(graphIdStr, "cfg-%06zu.dot", graphId);
    boost::filesystem::path debugDir = boost::filesystem::path("rose-debug/BinaryAnalysis/FeasiblePath") / callIdStr;
    boost::filesystem::create_directories(debugDir);
    boost::filesystem::path pathGraphName = debugDir / graphIdStr;

    std::ofstream file(pathGraphName.string().c_str());
    P2::GraphViz::CfgEmitter emitter(*partitioner_, paths_);
    emitter.showInstructions(true);
    emitter.selectWholeGraph();

    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, pathsBeginVertices_) {
        emitter.vertexOrganization(v).attributes().insert("style", "filled");
        emitter.vertexOrganization(v).attributes().insert("fillcolor", "#faff7d");
    }
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, pathsEndVertices_) {
        emitter.vertexOrganization(v).attributes().insert("style", "filled");
        emitter.vertexOrganization(v).attributes().insert("fillcolor", "#faff7d");
    }

    emitter.emit(file);
    return pathGraphName;
}

void
FeasiblePath::depthFirstSearch(PathProcessor &pathProcessor) {
    ASSERT_not_null(partitioner_);
    static size_t callId = 0;                           // number of calls to this function
    size_t graphId = 0;                                 // incremented each time the graph is modified
    {
        static SAWYER_THREAD_TRAITS::Mutex mutex;
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
        ++callId;
    }

    Stream debug(mlog[DEBUG]);
    Stream info(mlog[INFO]);
    std::string indent = debug ? "    " : "";
    if (paths_.isEmpty())
        return;

    // Debugging
    if (debug) {
        debug <<"depthFirstSearch call #" <<callId <<":\n";
        debug <<"  paths graph saved in " <<emitPathGraph(callId, graphId) <<"\n";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, pathsBeginVertices_)
            debug <<"  begin at vertex " <<partitioner_->vertexName(v) <<"\n";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, pathsEndVertices_)
            debug <<"  end   at vertex " <<partitioner_->vertexName(v) <<"\n";
    }

    // Analyze each of the starting locations individually
    BOOST_FOREACH (P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex, pathsBeginVertices_) {
        P2::CfgPath path(pathsBeginVertex);
        while (!path.isEmpty()) {
            if (debug) {
                debug <<"  path vertices (" <<path.nVertices() <<"):";
                BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, path.vertices())
                    debug <<" " <<partitioner_->vertexName(v);
                debug <<"\n";
            }
            
            // If backVertex is a function summary, then there is no corresponding cfgBackVertex.
            P2::ControlFlowGraph::ConstVertexIterator backVertex = path.backVertex();
            P2::ControlFlowGraph::ConstVertexIterator cfgBackVertex = pathToCfg(backVertex);

            bool doBacktrack = false;
            bool atEndOfPath = pathsEndVertices_.find(backVertex) != pathsEndVertices_.end();

            // Test path feasibility
            SAWYER_MESG(debug) <<"    checking feasibility";
            std::vector<SymbolicExpr::Ptr> postConditions, pathConditions;
            if (atEndOfPath)
                postConditions = settings_.postConditions;
            BaseSemantics::DispatcherPtr cpu;
            YicesSolver solver;
            boost::logic::tribool isFeasible = isPathFeasible(path, solver, postConditions,
                                                              pathConditions /*in,out*/, cpu /*out*/);
            if (debug) {
                if (isFeasible) {
                    debug <<" = is feasible\n";
                } else if (!isFeasible) {
                    debug <<" = not feasible\n";
                } else {
                    debug <<" = unknown\n";
                }
            }
            
            // Invoke callback if path is found
            if (atEndOfPath) {
                SAWYER_MESG(debug) <<"    reached path end vertex\n";
                if (isFeasible) {
                    SAWYER_MESG(debug) <<"    feasible path found; calling processor\n";
                    switch (pathProcessor.found(*this, path, pathConditions, cpu, solver)) {
                        case PathProcessor::BREAK: return;
                        case PathProcessor::CONTINUE: break;
                        default: ASSERT_not_reachable("invalid path processor action");
                    }
                }
                doBacktrack = true;
#if 1 // Adding this because I think it should be here. [Robb P Matzke 2017-03-28]
            } else if (!isFeasible) {
                doBacktrack = true;
#endif
            }

            // If we've visited a vertex too many times (e.g., because of a loop or recursion), then don't go any further.
            if (path.nVisits(backVertex) > settings_.vertexVisitLimit) {
                mlog[WARN] <<indent <<"max visits (" <<settings_.vertexVisitLimit <<") reached for vertex " <<backVertex->id() <<"\n";
                doBacktrack = true;
            }

            // Limit path length (in terms of number of instructions)
            if (settings_.maxPathLength < (size_t)(-1) && !doBacktrack) {
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
                    mlog[WARN] <<indent <<"maximum path length exceeded (" <<settings_.maxPathLength <<" instructions)\n";
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
                        info <<indent <<"summarizing function for edge " <<partitioner().edgeName(cfgCallEdge) <<"\n";
                        insertCallSummary(backVertex, partitioner().cfg(), cfgCallEdge);
                    } else if (shouldInline(path, cfgCallEdge->target())) {
                        info <<indent <<"inlining function call paths at vertex " <<partitioner().vertexName(backVertex) <<"\n";
                        P2::insertCalleePaths(paths_, backVertex, partitioner().cfg(),
                                              cfgBackVertex, cfgEndAvoidVertices_, cfgAvoidEdges_);
                    } else {
                        info <<indent <<"summarizing function for edge " <<partitioner().edgeName(cfgCallEdge) <<"\n";
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

                info <<indent <<"paths graph has " <<StringUtility::plural(paths_.nVertices(), "vertices", "vertex")
                     <<" and " <<StringUtility::plural(paths_.nEdges(), "edges") <<"\n";
                SAWYER_MESG(debug) <<"    paths graph saved in " <<emitPathGraph(callId, ++graphId) <<"\n";
            }

            // We've reached a dead end that isn't a final vertex.  This shouldn't ever happen.
            ASSERT_require2(doBacktrack || backVertex->nOutEdges()!=0,
                            "non-final vertex " + partitioner().vertexName(backVertex) + " has no out edges");

            // Advance to next path.
            if (doBacktrack) {
                // Backtrack and follow a different path.  The backtrack not only pops edges off the path, but then also appends
                // the next edge.  We must adjust visit counts for the vertices we backtracked.
                SAWYER_MESG(debug) <<"    backtrack\n";
                path.backtrack();
            } else {
                // Push next edge onto path.
                SAWYER_MESG(debug) <<"    advance along cfg edge " <<partitioner_->edgeName(backVertex->outEdges().begin()) <<"\n";
                ASSERT_require(paths_.isValidEdge(backVertex->outEdges().begin()));
                path.pushBack(backVertex->outEdges().begin());
            }
        }
    }
    SAWYER_MESG(debug) <<"  path search completed\n";
}

const FeasiblePath::FunctionSummary&
FeasiblePath::functionSummary(rose_addr_t entryVa) const {
    return functionSummaries_.getOrDefault(entryVa);
}

const FeasiblePath::VarDetail&
FeasiblePath::varDetail(const BaseSemantics::StatePtr &state, const std::string &varName) const {
    return State::promote(state)->varDetail(varName);
}

std::string
FeasiblePath::VarDetail::toString() const {
    std::ostringstream ss;
    if (!registerName.empty())
        ss <<" register \"" <<StringUtility::cEscape(registerName) <<"\"";
    if (!firstAccessMode.empty())
        ss <<" first " <<firstAccessMode;
    if (firstAccessIdx || firstAccessInsn) {
        ss <<" at instruction";
        if (firstAccessIdx)
            ss <<" #" <<*firstAccessIdx;
        if (firstAccessInsn)
            ss <<" " <<unparseInstructionWithAddress(firstAccessInsn);
    }
    if (memAddress)
        ss <<" mem[" <<*memAddress <<"]";
    if (memSize > 0)
        ss <<" byte " <<memByteNumber <<" of " <<memSize;
    if (returnFrom)
        ss <<" return from function " <<StringUtility::addrToString(*returnFrom);

    return boost::trim_copy(ss.str());
}

} // namespace
} // namespace
