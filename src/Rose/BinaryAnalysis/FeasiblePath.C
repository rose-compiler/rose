#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/FeasiblePath.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/SymbolicExpressionParser.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch64.h>
#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesElf.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SymbolicMemory.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/CommandLine.h>
#include <Combinatorics.h>                              // rose

#include <Sawyer/GraphAlgorithm.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

using namespace Rose::BinaryAnalysis::InstructionSemantics;
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
typedef SymbolicSemantics::SValue::Ptr SValuePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RegisterState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef SymbolicSemantics::RegisterState RegisterState;
typedef SymbolicSemantics::RegisterState::Ptr RegisterStatePtr;


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
    using Super = BaseSemantics::State;
    using Ptr = StatePtr;

private:
    // Maps symbolic variable names to additional information about where the variable appears in the path.
    FeasiblePath::VarDetails varDetails_;

protected:
    State(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory)
        : Super(registers, memory) {}

    State(const State &other)
        : Super(other), varDetails_(other.varDetails_) {}

public:
    static BaseSemantics::State::Ptr instance(const BaseSemantics::RegisterState::Ptr &registers,
                                            const BaseSemantics::MemoryState::Ptr &memory) {
        return Ptr(new State(registers, memory));
    }

    static BaseSemantics::State::Ptr
    instance(const Ptr &other) {
        return Ptr(new State(*other));
    }

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

    // Set detail for variable name if none exists.
    void varDetail(const std::string &varName, const FeasiblePath::VarDetail &vdetail) {
        varDetails_.insertMaybe(varName, vdetail);
    }

    // All variable details.
    const FeasiblePath::VarDetails& varDetails() const {
        return varDetails_;
    }

    // Detail for variable name.
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
    using Ptr = RiscOperatorsPtr;

public:
    size_t pathInsnIndex_;                              // current location in path, or -1
    P2::Partitioner::ConstPtr partitioner_;
    FeasiblePath *fpAnalyzer_;
    FeasiblePath::PathProcessor *pathProcessor_;
    FeasiblePath::PathProcessor::Action pathProcessorAction_; // how to proceed after a path processor call returns
    SmtSolver::Ptr nullPtrSolver_;
    const P2::CfgPath *path_;

protected:
    RiscOperators(const P2::Partitioner::ConstPtr &partitioner, const BaseSemantics::SValue::Ptr &protoval,
                  const Rose::BinaryAnalysis::SmtSolverPtr &solver)
        : Super(protoval, solver), pathInsnIndex_(-1), partitioner_(partitioner), fpAnalyzer_(NULL),
          pathProcessor_(NULL), pathProcessorAction_(FeasiblePath::PathProcessor::CONTINUE), path_(NULL){
        ASSERT_not_null(partitioner);
        name("FindPath");
    }

    RiscOperators(const P2::Partitioner::ConstPtr &partitioner, const BaseSemantics::State::Ptr &state,
                  const Rose::BinaryAnalysis::SmtSolverPtr &solver)
        : Super(state, solver), pathInsnIndex_(-1), partitioner_(partitioner), fpAnalyzer_(NULL), pathProcessor_(NULL),
          pathProcessorAction_(FeasiblePath::PathProcessor::CONTINUE), path_(NULL) {
        ASSERT_not_null(partitioner);
        name("FindPath");
    }

public:
    static RiscOperators::Ptr instance(const P2::Partitioner::ConstPtr &partitioner, const RegisterDictionary::Ptr &regdict,
                                     FeasiblePath *fpAnalyzer, const P2::CfgPath *path, FeasiblePath::PathProcessor *pathProcessor,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) {
        ASSERT_not_null(fpAnalyzer);
        BaseSemantics::SValue::Ptr protoval = SValue::instance();
        BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryState::Ptr memory;
        switch (fpAnalyzer->settings().searchMode) {
            case FeasiblePath::SEARCH_MULTI:
                // If we're sending multiple paths at a time to the SMT solver then we need to provide the SMT solver with
                // detailed information about how memory is affected on those different paths.
                memory = BaseSemantics::SymbolicMemory::instance(protoval, protoval);
                break;
            case FeasiblePath::SEARCH_SINGLE_DFS:
            case FeasiblePath::SEARCH_SINGLE_BFS:
                // We can perform memory-related operations and simplifications inside ROSE, which results in more but smaller
                // expressions being sent to the SMT solver.
                switch (fpAnalyzer->settings().memoryParadigm) {
                    case FeasiblePath::LIST_BASED_MEMORY:
                        memory = SymbolicSemantics::MemoryListState::instance(protoval, protoval);
                        break;
                    case FeasiblePath::MAP_BASED_MEMORY:
                        memory = SymbolicSemantics::MemoryMapState::instance(protoval, protoval);
                        break;
                    default:
                        ASSERT_not_reachable("invalid memory paradigm");
                        break;
                }
                break;
        }
        ASSERT_not_null(memory);
        memory->set_byteOrder(partitioner->instructionProvider().defaultByteOrder());
        BaseSemantics::State::Ptr state = State::instance(registers, memory);
        RiscOperators::Ptr ops = RiscOperators::Ptr(new RiscOperators(partitioner, state, solver));
        ops->fpAnalyzer_ = fpAnalyzer;
        ops->pathProcessor_ = pathProcessor;
        ops->path_ = path;
        ops->computingDefiners(SymbolicSemantics::TRACK_ALL_DEFINERS);
        return ops;
    }

    static RiscOperators::Ptr instance(const P2::Partitioner::ConstPtr &partitioner, const BaseSemantics::SValue::Ptr &protoval,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) {
        return RiscOperators::Ptr(new RiscOperators(partitioner, protoval, solver));
    }

    static RiscOperators::Ptr instance(const P2::Partitioner::ConstPtr &partitioner, const BaseSemantics::State::Ptr &state,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) {
        return RiscOperators::Ptr(new RiscOperators(partitioner, state, solver));
    }

public:
    virtual BaseSemantics::RiscOperators::Ptr
    create(const BaseSemantics::SValue::Ptr &protoval,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) const override {
        return instance(P2::Partitioner::Ptr(), protoval, solver);
    }

    virtual BaseSemantics::RiscOperators::Ptr
    create(const BaseSemantics::State::Ptr &state,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) const override {
        return instance(P2::Partitioner::Ptr(), state, solver);
    }

public:
    static RiscOperators::Ptr promote(const BaseSemantics::RiscOperators::Ptr &x) {
        RiscOperators::Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    // Solver for doing null pointer analysis. This solver is usually the same one as the main path processing.
    SmtSolver::Ptr nullPtrSolver() const { return nullPtrSolver_; }
    void nullPtrSolver(const SmtSolver::Ptr &solver) { nullPtrSolver_ = solver; }

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

    void partitioner(const P2::Partitioner::ConstPtr &partitioner) {
        partitioner_ = partitioner;
    }

    /** Did any path processor callback request that we stop processing this path? */
    bool breakRequested() const {
        return FeasiblePath::PathProcessor::BREAK == pathProcessorAction_;
    }

private:
    // Merge the return value of a path processor call into this object.
    void merge(FeasiblePath::PathProcessor::Action action) {
        if (pathProcessorAction_ != FeasiblePath::PathProcessor::BREAK)
            pathProcessorAction_ = action;
    }

    /** Description a variable stored in a register. */
    FeasiblePath::VarDetail detailForVariable(RegisterDescriptor reg, const std::string &accessMode) const {
        RegisterDictionary::Ptr regs = currentState()->registerState()->registerDictionary();
        FeasiblePath::VarDetail retval;
        retval.registerName = RegisterNames(regs)(reg);
        retval.firstAccessMode = accessMode;
        if (pathInsnIndex_ == INVALID_INDEX && currentInstruction() == NULL) {
            // no path position (i.e., present in initial state)
        } else {
            if (pathInsnIndex_ != INVALID_INDEX)
                retval.firstAccessIdx = pathInsnIndex_;
            retval.firstAccessInsn = currentInstruction();
        }
        return retval;
    }

    /** Describe a memory address if possible. The nBytes will be non-zero when we're describing an address as opposed to a
     *  value stored across some addresses. */
    FeasiblePath::VarDetail detailForVariable(const BaseSemantics::SValue::Ptr &addr,
                                              const std::string &accessMode, size_t byteNumber=0, size_t nBytes=0) const {
        FeasiblePath::VarDetail retval;
        retval.firstAccessMode = accessMode;
        if (pathInsnIndex_ != INVALID_INDEX)
            retval.firstAccessIdx = pathInsnIndex_;
        retval.firstAccessInsn = currentInstruction();

        // Sometimes we can save useful information about the address.
        if (nBytes != 1) {
            SymbolicExpression::Ptr addrExpr = SValue::promote(addr)->get_expression();
            if (SymbolicExpression::LeafPtr addrLeaf = addrExpr->isLeafNode()) {
                if (addrLeaf->isIntegerConstant()) {
                    retval.memSize = nBytes;
                    retval.memByteNumber = byteNumber;
                    retval.memAddress = addrExpr;
                }
            } else if (SymbolicExpression::InteriorPtr addrINode = addrExpr->isInteriorNode()) {
                if (addrINode->getOperator() == SymbolicExpression::OP_ADD && addrINode->nChildren() == 2 &&
                    addrINode->child(0)->isLeafNode() && addrINode->child(0)->isLeafNode()->isIntegerVariable() &&
                    addrINode->child(1)->isLeafNode() && addrINode->child(1)->isLeafNode()->isIntegerConstant()) {
                    SymbolicExpression::LeafPtr base = addrINode->child(0)->isLeafNode();
                    SymbolicExpression::LeafPtr offset = addrINode->child(1)->isLeafNode();
                    retval.memSize = nBytes;
                    retval.memByteNumber = byteNumber;
                    retval.memAddress = addrExpr;
                }
            }
        }
        return retval;
    }

    // True if the expression contains only constants.
    bool isConstExpr(const SymbolicExpression::Ptr &expr) {
        ASSERT_not_null(expr);

        struct VarFinder: SymbolicExpression::Visitor {
            bool hasVariable;

            VarFinder(): hasVariable(false) {}

            SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) override {
                if (node->isLeafNode() && !node->isLeafNode()->isIntegerConstant()) {
                    hasVariable = true;
                    return SymbolicExpression::TERMINATE;
                } else {
                    return SymbolicExpression::CONTINUE;
                }
            }

            SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) override {
                return SymbolicExpression::CONTINUE;
            }
        } varFinder;
        expr->depthFirstTraversal(varFinder);
        return !varFinder.hasVariable;
    }

    // Warning: Caller should create a nullPtrSolver transaction if necessary
    bool isNullDeref(const BaseSemantics::SValue::Ptr &addr) {
        SymbolicExpression::Ptr expr = SymbolicSemantics::SValue::promote(addr)->get_expression();
        return isNullDeref(expr);
    }

    // Warning: Caller should create a nullPtrSolver transaction if necessary
    bool isNullDeref(const SymbolicExpression::Ptr &expr) {
        ASSERT_not_null(expr);
        Sawyer::Message::Stream debug(fpAnalyzer_->mlog[DEBUG]);
        SAWYER_MESG(debug) <<"          isNullDeref addr = " <<*expr <<"\n";
        SmtSolver::Ptr solver = nullPtrSolver();

        // Instead of using SymbolicExpression::mustEqual and SymbolicExpression::mayEqual, we always call the SMT solver. This
        // is so that if a null pointer is found, the callback will be able to get the evidence from the solver. The problem
        // with mustEqual and mayEqual is threefold: (1) these functions can sometimes make the determination themselves
        // without invoking an SMT solver thus the solver would not be updated with evidence, (2) these functions might invoke
        // a user-defined callback that does almost anything, (3) when these functions invoke an SMT solver they protect it
        // with a transaction so the evidence would be gone by time we get control back.
        bool retval = false;
        switch (fpAnalyzer_->settings().nullDeref.mode) {
            case FeasiblePath::MAY:
                if (solver) {
                    SymbolicExpression::Ptr assertion =
                        SymbolicExpression::makeLt(expr,
                                                   SymbolicExpression::makeIntegerConstant(expr->nBits(),
                                                                                           fpAnalyzer_->settings().nullDeref.minValid));
                    solver->insert(assertion);
                    retval = SmtSolver::SAT_YES == solver->check();
                } else {
                    retval =
                        SymbolicExpression::makeLt(expr,
                                                   SymbolicExpression::makeIntegerConstant(expr->nBits(),
                                                                                           fpAnalyzer_->settings().nullDeref.minValid))->mayEqual(SymbolicExpression::makeIntegerConstant(expr->nBits(), 1));
                }
                break;

            case FeasiblePath::MUST:
                // Check for constness of the address expression first since that doesn't involve an SMT solver and we might
                // be able to return quickly.
                if (!fpAnalyzer_->settings().nullDeref.constOnly ||  isConstExpr(expr)) {
                    if (solver) {
                        SymbolicExpression::Ptr assertion = SymbolicExpression::makeGe(expr, SymbolicExpression::makeIntegerConstant(expr->nBits(), fpAnalyzer_->settings().nullDeref.minValid));
                        solver->insert(assertion);
                        retval = SmtSolver::SAT_NO == solver->check();
                    } else {
                        retval = SymbolicExpression::makeLt(expr, SymbolicExpression::makeIntegerConstant(expr->nBits(), fpAnalyzer_->settings().nullDeref.minValid))->mustEqual(SymbolicExpression::makeIntegerConstant(expr->nBits(), 1));
                    }
                } else {
                    SAWYER_MESG(debug) <<"          isNullDeref address is not constant as required by settings\n";
                }
                break;

            default:
                ASSERT_not_reachable("invalid null-dereference mode");
        }

        if (debug) {
            debug <<"          isNullDeref result = " <<(retval ? "is null" : "not null");
            if (solver)
                debug <<" using SMT solver \"" <<StringUtility::cEscape(solver->name()) <<"\"";
            debug <<"\n";
        }

        return retval;
    }

public:
    virtual void startInstruction(SgAsmInstruction *insn) override {
        ASSERT_not_null(partitioner_);
        Super::startInstruction(insn);
        if (mlog[DEBUG]) {
            SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
            mlog[DEBUG] <<"  +-------------------------------------------------\n"
                        <<"  | " <<insn->toString() <<"\n"
                        <<"  +-------------------------------------------------\n"
                        <<"    state before instruction:\n"
                        <<(*currentState() + fmt);
        }
    }

    virtual void finishInstruction(SgAsmInstruction *insn) override {
        if (mlog[DEBUG]) {
            SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
            mlog[DEBUG] <<"    state after instruction:\n" <<(*currentState()+fmt);
        }
        Super::finishInstruction(insn);
    }

    virtual BaseSemantics::SValue::Ptr readRegister(RegisterDescriptor reg,
                 const BaseSemantics::SValue::Ptr &dflt) override {
        SValue::Ptr retval = SValue::promote(Super::readRegister(reg, dflt));
        SymbolicExpression::Ptr expr = retval->get_expression();
        if (expr->isLeafNode())
            State::promote(currentState())->varDetail(expr->isLeafNode()->toString(), detailForVariable(reg, "read"));
        return retval;
    }

    virtual void writeRegister(RegisterDescriptor reg,
                  const BaseSemantics::SValue::Ptr &value) override {
        SymbolicExpression::Ptr expr = SValue::promote(value)->get_expression();
        if (expr->isLeafNode())
            State::promote(currentState())->varDetail(expr->isLeafNode()->toString(), detailForVariable(reg, "write"));
        Super::writeRegister(reg, value);
    }

    // If multi-path is enabled, then return a new memory expression that describes the process of reading a value from the
    // specified address; otherwise, actually read the value and return it.  In any case, record some information about the
    // address that's being read if we've never seen it before.
    virtual BaseSemantics::SValue::Ptr readMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr_,
                                                const BaseSemantics::SValue::Ptr &dflt_,
                                                const BaseSemantics::SValue::Ptr &cond) override {
        BaseSemantics::SValue::Ptr dflt = dflt_;
        const size_t nBytes = dflt->nBits() / 8;
        if (cond->isFalse())
            return dflt_;

        // Offset the address by the value of the segment register.
        BaseSemantics::SValue::Ptr adjustedVa;
        if (segreg.isEmpty()) {
            adjustedVa = addr_;
        } else {
            BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
            adjustedVa = add(addr_, signExtend(segregValue, addr_->nBits()));
        }

        // Check for null pointer dereferences
        if (fpAnalyzer_->settings().nullDeref.check && pathProcessor_ && !breakRequested()) {
            SmtSolver::Transaction transaction(nullPtrSolver());
            if (isNullDeref(adjustedVa)) {
                ASSERT_not_null(fpAnalyzer_);
                ASSERT_not_null(path_);
                BaseSemantics::RiscOperators::Ptr cpu = shared_from_this();
                ASSERT_not_null(cpu);
                merge(pathProcessor_->nullDeref(*fpAnalyzer_, *path_, currentInstruction(), cpu, nullPtrSolver(),
                                                FeasiblePath::READ, adjustedVa));
            }
        }
        
        // If we know the address and that memory exists, then read the memory to obtain the default value.
        uint8_t buf[8];
        if (adjustedVa->toUnsigned() && nBytes < sizeof(buf) &&
            nBytes == partitioner_->memoryMap()->at(adjustedVa->toUnsigned().get()).limit(nBytes).read(buf).size()) {
            switch (partitioner_->memoryMap()->byteOrder()) {
                case ByteOrder::ORDER_UNSPECIFIED:
                case ByteOrder::ORDER_LSB: {
                    uint64_t value = 0;
                    for (size_t i=0; i<nBytes; ++i)
                        value |= (uint64_t)buf[i] << (8*i);
                    dflt = number_(dflt->nBits(), value);
                    break;
                }

                case ByteOrder::ORDER_MSB: {
                    uint64_t value = 0;
                    for (size_t i=0; i<nBytes; ++i)
                        value = (value << 8) | (uint64_t)buf[i];
                    dflt = number_(dflt->nBits(), value);
                    break;
                }
            }
        }

        // Read from the symbolic state, and update the state with the default from real memory if known.
        BaseSemantics::SValue::Ptr retval = Super::readMemory(segreg, addr_, dflt, cond);

        if (!currentInstruction())
            return retval;                              // not called from dispatcher on behalf of an instruction

        // Save a description of the variable
        SymbolicExpression::Ptr valExpr = SValue::promote(retval)->get_expression();
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->isIntegerVariable())
            State::promote(currentState())->varDetail(valExpr->isLeafNode()->toString(), detailForVariable(adjustedVa, "read"));

        // Save a description for its addresses
        for (size_t i=0; i<nBytes; ++i) {
            SValue::Ptr va = SValue::promote(add(adjustedVa, number_(adjustedVa->nBits(), i)));
            if (va->get_expression()->isLeafNode()) {
                State::promote(currentState())->varDetail(va->get_expression()->isLeafNode()->toString(),
                                                          detailForVariable(adjustedVa, "read", i, nBytes));
            }
        }

        // Callback for the memory access
        if (pathProcessor_ && !breakRequested()) {
            ASSERT_not_null(fpAnalyzer_);
            ASSERT_not_null(path_);
            SmtSolver::Ptr s = nullPtrSolver();
            SmtSolver::Transaction tx(s);
            BaseSemantics::RiscOperators::Ptr cpu = shared_from_this();
            ASSERT_not_null(cpu);
            merge(pathProcessor_->memoryIo(*fpAnalyzer_, *path_, currentInstruction(), cpu, s, FeasiblePath::READ,
                                           adjustedVa, retval));
        }

        return retval;
    }

    // If multi-path is enabled, then return a new memory expression that updates memory with a new address/value pair;
    // otherwise update the memory directly.  In any case, record some information about the address that was written if we've
    // never seen it before.
    virtual void writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr_,
                             const BaseSemantics::SValue::Ptr &value, const BaseSemantics::SValue::Ptr &cond) override {
        if (cond->isFalse())
            return;
        Super::writeMemory(segreg, addr_, value, cond);

        // Offset the address by the value of the segment register.
        BaseSemantics::SValue::Ptr adjustedVa;
        if (segreg.isEmpty()) {
            adjustedVa = addr_;
        } else {
            BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
            adjustedVa = add(addr_, signExtend(segregValue, addr_->nBits()));
        }

        // Check for null pointer dereferences
        if (fpAnalyzer_->settings().nullDeref.check && pathProcessor_ && !breakRequested()) {
            SmtSolver::Transaction transaction(nullPtrSolver());
            if (isNullDeref(adjustedVa)) {
                ASSERT_not_null(fpAnalyzer_);
                ASSERT_not_null(path_);
                BaseSemantics::RiscOperators::Ptr cpu = shared_from_this();
                ASSERT_not_null(cpu);
                merge(pathProcessor_->nullDeref(*fpAnalyzer_, *path_, currentInstruction(), cpu, nullPtrSolver(),
                                                FeasiblePath::WRITE, adjustedVa));
            }
        }

        // Save a description of the variable
        SymbolicExpression::Ptr valExpr = SValue::promote(value)->get_expression();
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->isIntegerVariable())
            State::promote(currentState())->varDetail(valExpr->isLeafNode()->toString(), detailForVariable(adjustedVa, "write"));

        // Save a description for its addresses
        size_t nBytes = value->nBits() / 8;
        for (size_t i=0; i<nBytes; ++i) {
            SValue::Ptr va = SValue::promote(add(adjustedVa, number_(adjustedVa->nBits(), i)));
            if (va->get_expression()->isLeafNode()) {
                State::promote(currentState())->varDetail(va->get_expression()->isLeafNode()->toString(),
                                                          detailForVariable(adjustedVa, "read", i, nBytes));
            }
        }

        // Callback for the memory access
        if (pathProcessor_ && !breakRequested()) {
            ASSERT_not_null(fpAnalyzer_);
            ASSERT_not_null(path_);
            SmtSolver::Ptr s = nullPtrSolver();
            SmtSolver::Transaction tx(s);
            BaseSemantics::RiscOperators::Ptr cpu = shared_from_this();
            ASSERT_not_null(cpu);
            merge(pathProcessor_->memoryIo(*fpAnalyzer_, *path_, currentInstruction(), cpu, s, FeasiblePath::WRITE,
                                           adjustedVa, value));
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      helper functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
hasVirtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    return vertex->value().type() == P2::V_BASIC_BLOCK || vertex->value().type() == P2::V_USER_DEFINED;
}

// Unwrap ops if necessary to return the feasible path operators
static RiscOperators::Ptr
fpOperators(BaseSemantics::RiscOperators::Ptr ops) {
    ASSERT_not_null(ops);
    if (TraceSemantics::RiscOperators::Ptr traceOps = boost::dynamic_pointer_cast<TraceSemantics::RiscOperators>(ops)) {
        ops = traceOps->subdomain();
    }
    ASSERT_not_null(RiscOperators::promote(ops));
    return RiscOperators::promote(ops);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

Sawyer::Message::Facility FeasiblePath::mlog;
Sawyer::Attribute::Id FeasiblePath::POST_STATE(-1);
Sawyer::Attribute::Id FeasiblePath::EFFECTIVE_K(-1);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      FeasiblePath nested classes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
FeasiblePath::Expression::print(std::ostream &out) const {
    if (expr) {
        out <<*expr;
    } else if (!parsable.empty()) {
        out <<parsable;
    } else {
        out <<"empty-expression";
    }
}

FeasiblePath::FunctionSummary::FunctionSummary() {}

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

FeasiblePath::Statistics&
FeasiblePath::Statistics::operator+=(const FeasiblePath::Statistics &other) {
    nPathsExplored += other.nPathsExplored;
    maxVertexVisitHits += other.maxVertexVisitHits;
    maxPathLengthHits += other.maxPathLengthHits;
    maxCallDepthHits += other.maxCallDepthHits;
    maxRecursionDepthHits += other.maxRecursionDepthHits;
    typedef Sawyer::Container::Map<rose_addr_t, size_t> Map;
    for (const Map::Node &node: other.reachedBlockVas.nodes())
        reachedBlockVas.insertMaybe(node.key(), 0) += node.value();
    return *this;
}

FeasiblePath::PathProcessor::Action
FeasiblePath::PathProcessor::found(const FeasiblePath&, const Partitioner2::CfgPath&,
                                   const InstructionSemantics::BaseSemantics::Dispatcher::Ptr&,
                                   const SmtSolver::Ptr&) {
    return CONTINUE;
}

FeasiblePath::PathProcessor::Action
FeasiblePath::PathProcessor::nullDeref(const FeasiblePath&, const Partitioner2::CfgPath&, SgAsmInstruction*,
                                       const InstructionSemantics::BaseSemantics::RiscOperators::Ptr&, const SmtSolver::Ptr&,
                                       IoMode, const InstructionSemantics::BaseSemantics::SValue::Ptr&) {
    return CONTINUE;
}

FeasiblePath::PathProcessor::Action
FeasiblePath::PathProcessor::memoryIo(const FeasiblePath&, const Partitioner2::CfgPath&, SgAsmInstruction*,
                                      const InstructionSemantics::BaseSemantics::RiscOperators::Ptr&, const SmtSolver::Ptr&,
                                      IoMode, const InstructionSemantics::BaseSemantics::SValue::Ptr&,
                                      const InstructionSemantics::BaseSemantics::SValue::Ptr&) {
    return CONTINUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      FeasiblePath
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FeasiblePath::FeasiblePath() {}

FeasiblePath::~FeasiblePath() {}

void
FeasiblePath::reset() {
    partitioner_ = P2::Partitioner::ConstPtr();
    registers_ = RegisterDictionary::Ptr();
    REG_PATH = REG_RETURN_ = RegisterDescriptor();
    functionSummaries_.clear();
    vmap_.clear();
    paths_.clear();
    pathsBeginVertices_.clear();
    pathsEndVertices_.clear();
    isDirectedSearch_ = true;
    cfgAvoidEdges_.clear();
    cfgEndAvoidVertices_.clear();
    resetStatistics();
}

// class method
void
FeasiblePath::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::FeasiblePath");
        mlog.comment("model checking and path feasibility");
        POST_STATE = Sawyer::Attribute::declare("post-execution semantic state");
        EFFECTIVE_K = Sawyer::Attribute::declare("effective maximum path length (double)");
    }
}

// class method
std::string
FeasiblePath::expressionDocumentation() {
    return SymbolicExpressionParser().docString() +
        "@named{Registers}{Register locations are specified by just mentioning the name of the register. "
        "Register names are usually lower case, such as \"eax\", \"rip\", etc.}"

        "@named{Memory}{Memory locations can be read using \"(memory[N] ADDR)\" where \"N\" is the number "
        "of bits to read (a multiple of 8) and \"ADDR\" is the address at which to start reading. The order "
        "that the individual bytes are concatenated (the \"endianness\") depends on the architecture.}";
}

// class method
Sawyer::CommandLine::SwitchGroup
FeasiblePath::commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    std::string exprParserDoc = expressionDocumentation();

    SwitchGroup sg("Feasible path analysis switches");

    sg.insert(Switch("search")
              .argument("mode", enumParser(settings.searchMode)
                        ->with("single-dfs", SEARCH_SINGLE_DFS)
                        ->with("single-bfs", SEARCH_SINGLE_BFS)
                        ->with("multi", SEARCH_MULTI))
              .doc("Method to use when searching for feasible paths. The choices are: "

                   "@named{single-dfs}{Drive the SMT solver along a particular path at a time using a depth first "
                   "search of all possible paths.}"

                   "@named{single-bfs}{Drive the SMT solver along a particular path at a time using a breadth first "
                   "search of all possible paths.}"

                   "@named{multi}{Submit all possible paths to the SMT solver at one time.}"

                   "The default is " +
                   std::string(SEARCH_SINGLE_DFS == settings.searchMode ? "single-dfs" :
                               (SEARCH_SINGLE_BFS == settings.searchMode ? "single-bfs" :
                                (SEARCH_MULTI == settings.searchMode ? "multi" :
                                 "unknown"))) + "."));

    sg.insert(Switch("edge-order")
              .argument("order", enumParser<EdgeVisitOrder>(settings.edgeVisitOrder)
                        ->with("natural", VISIT_NATURAL)
                        ->with("reverse", VISIT_REVERSE)
                        ->with("random", VISIT_RANDOM))
              .doc("Specifies the order in which edges of the control flow graph are visited. The choices are:"

                   "@named{natural}{Edges are visited in the order they occur within the control flow graph.}"

                   "@named{reverse}{Edges are visited in reverse order from which they occur in the control flow graph.}"

                   "@named{random}{Edges are visited in random order. Random order visits attempt to avoid the worst case "
                   "scenario where always following the same edge first out of a vertex leads to a very deep traversal when "
                   "a solution could have been found earlier on a shorter path.}"

                   "The default is " + std::string(VISIT_NATURAL==settings.edgeVisitOrder?"natural":
                                                   VISIT_REVERSE==settings.edgeVisitOrder?"reverse":
                                                   VISIT_RANDOM==settings.edgeVisitOrder?"random":
                                                   "unknown") + "."));

    sg.insert(Switch("initial-stack")
              .argument("value", nonNegativeIntegerParser(settings.initialStackPtr))
              .doc("Specifies an initial value for the stack pointer register for each path. The default is that the stack "
                   "pointer register has an undefined (variable) value at the start of the analysis. Setting the initial stack "
                   "pointer to a concrete value causes most stack reads and writes to have concrete addresses that are non-zero. "
                   "The default is " +
                   (settings.initialStackPtr ? StringUtility::addrToString(*settings.initialStackPtr) :
                    std::string("to use a variable with an unknown value")) + "."));

    sg.insert(Switch("max-vertex-visit")
              .argument("n", nonNegativeIntegerParser(settings.maxVertexVisit))
              .doc("Maximum number of times that a single CFG vertex can be visited during the analysis of some function. "
                   "A function that's called from two (or more) places is considered to have two (or more) distinct sets "
                   "of vertices. The default limit is " + StringUtility::numberToString(settings.maxVertexVisit) + "."));

    sg.insert(Switch("max-path-length", 'k')
              .argument("n", nonNegativeIntegerParser(settings.maxPathLength))
              .doc("Maximum length of a path measured in machine instructions. This is the \"k\" in \"k-bounded model "
                   "checking\". The value specified here is only a starting point--the algorithm itself uses an effective-k "
                   "that's adjusted as the search progresses. Furthermore, other settings such as @s{max-call-depth}, "
                   "@s{max-recursion-depth}, and @s{max-vertex-visit} may place more stringent limits on the path length. "
                   "Exploration along a path stops when any of these limits is met. The default maximum path length is " +
                   StringUtility::plural(settings.maxPathLength, "instructions") + "."));

    sg.insert(Switch("max-expression-size")
              .argument("n", nonNegativeIntegerParser(settings.maxExprSize))
              .doc("Maximum size of symbolic expressions, measured in number of nodes of the expression tree, before the "
                   "expression is replaced with a new free variable. This can make the analysis faster but less precise. "
                   "See also, @s{smt-timeout}.  The default is " +
                   boost::lexical_cast<std::string>(settings.maxExprSize) + "."));

    sg.insert(Switch("cycle-k")
              .argument("coefficent", realNumberParser(settings.kCycleCoefficient))
              .doc("When the algorithm encounters a vertex which has already been visited by the current path, then the "
                   "effective k value is adjusted. The amount of adjustment is the size of the vertex (e.g., number of "
                   "instructions) multiplied by the floating-point @v{coefficient} specified here. Both positive and "
                   "negative coefficients are permitted. The default is " +
                   boost::lexical_cast<std::string>(settings.kCycleCoefficient) + "."));

    sg.insert(Switch("max-call-depth")
              .argument("n", nonNegativeIntegerParser(settings.maxCallDepth))
              .doc("Maximum call depth for inter-procedural analysis. A value of zero makes the analysis intra-procedural. "
                   "The default is " + StringUtility::plural(settings.maxCallDepth, "calls") + "."));

    sg.insert(Switch("max-recursion-depth")
              .argument("n", nonNegativeIntegerParser(settings.maxRecursionDepth))
              .doc("Maximum call depth when analyzing recursive functions. The default is " +
                   StringUtility::plural(settings.maxRecursionDepth, "calls") + "."));

    sg.insert(Switch("assert")
              .argument("where", anyParser(settings.assertionLocations))
              .argument("expression", anyParser(settings.assertions))
              .whichValue(SAVE_ALL)
              .doc("Assertions that must be satisified at certain locations along the path. This switch may appear multiple "
                   "times in order to specify multiple assertions.\n\n"

                   "The first argument, @v{where}, indicates where the assertion applies and can be a basic block address, "
                   "an address interval, the name of a function, or the word \"end\".  An \"end\" assertion applies at any of "
                   "the specified analysis end points. " + P2::AddressIntervalParser::docString() + " Empty locations are "
                   "not allowed in this context.\n\n"

                   "The expression is a symbolic expression that evaluates to a Boolean value and must be true in order for the "
                   "path to be considered feasible.  The assertion is added to the SMT solver's state each time the analysis "
                   "encounters a basic block whose address satisfies the @v{where} argument. " +
                   (settings.exprParserDoc.empty() ? exprParserDoc : settings.exprParserDoc)));

    sg.insert(Switch("summarize-function")
              .argument("addr_or_name", anyParser())
              .doc("Summarize all occurrances of the specified function, replacing the function with a stub that returns "
                   "an unconstrained value. The function can be specified by virtual address or name. "
                   "@b{Not implemented yet.}"));

    CommandLine::insertBooleanSwitch(sg, "assume-feasible", settings.nonAddressIsFeasible,
                                     "Assume that indeterminate and undiscovered instruction addresses are feasible.");

    sg.insert(Switch("smt-solver")
               .argument("name", anyParser(settings.solverName))
               .doc("When analyzing paths for model checking, use this SMT solver.  This switch overrides the general, "
                    "global SMT solver. Since an SMT solver is required for model checking, in the absence of any specified "
                    "solver the \"best\" solver is used.  The default solver is \"" + settings.solverName + "\"."));

    sg.insert(Switch("smt-timeout")
              .argument("seconds", realNumberParser(settings.smtTimeout))
              .doc("Amount of time in seconds (fractional values allowed) that the SMT solver is allowed to work on any "
                   "particular system of equations.  See also, @s{max-expression-size}. The default is " +
                   (settings.smtTimeout && settings.smtTimeout->count() > 0 ?
                    boost::lexical_cast<std::string>(*settings.smtTimeout) + " seconds" :
                    std::string("no limit")) + "."));

    CommandLine::insertBooleanSwitch(sg, "null-derefs", settings.nullDeref.check,
                                     "Check for null dereferences along the paths.");

    sg.insert(Switch("null-comparison")
              .argument("modal", enumParser<FeasiblePath::MayOrMust>(settings.nullDeref.mode)
                        ->with("may", MAY)
                        ->with("must", MUST))
              .doc("Mode of comparison when checking for null addresses, where model is \"may\" or \"must\"."
                   "@named{may}{Report points where a null dereference may occur.}"
                   "@named{must}{Report points where a null dereference must occur.}"
                   "The default is \"" + std::string(MAY==settings.nullDeref.mode?"may":"must") + "\"."));

    CommandLine::insertBooleanSwitch(sg, "null-const", settings.nullDeref.constOnly,
                                     "Check for null dereferences only when a pointer is a constant or set of constants. This "
                                     "setting is ignored when the null-comparison mode is \"may\" since \"may\" implies that "
                                     "you're interested in additional cases where the pointer is not a constant expression.");

    sg.insert(Switch("null-address-min")
              .argument("n", nonNegativeIntegerParser(settings.nullDeref.minValid))
              .doc("What is the min address that should not be teated as a null dereference. All memory reads that are less than n "
                   " are considered a null dereference. This is to find when an offset is used on a null pointer such as accessing "
                   "a member of a structure. The default is " + boost::lexical_cast<std::string>(settings.nullDeref.minValid) + " which is the default "
                   "size of the first page on linux which can not be read from."));

    CommandLine::insertBooleanSwitch(sg, "ignore-semantic-failure", settings.ignoreSemanticFailure,
                                     "If set, then any instruction for which semantics are not implemented or for which the "
                                     "semantic evaluation fails will be ignored as if the instruction was not present in the "
                                     "path (although it will still be shown). If disabled, then a semantic failure will cause "
                                     "all paths on which that instruction occurs to be infeasible. In either case, the "
                                     "instruction's semanticsFailed property is incremented.");

    sg.insert(Switch("semantic-memory")
              .argument("type", enumParser<SemanticMemoryParadigm>(settings.memoryParadigm)
                        ->with("list", LIST_BASED_MEMORY)
                        ->with("map", MAP_BASED_MEMORY))
              .doc("The analysis can switch between storing semantic memory states in a list versus a map.  The @v{type} "
                   "should be one of these words:"

                   "@named{list}{List-based memory stores memory cells (essentially address+value pairs) in a reverse "
                   "chronological list and uses an SMT solver to solve aliasing equations.  The number of symbolic expression "
                   "comparisons (either within ROSE or using an SMT solver) is linear with the size of the memory cell list.}"

                   "@named{map}{Map-based memory stores memory cells in a container hashed by address expression. Aliasing "
                   "equations are not solved even when an SMT solver is available. One cell aliases another only if their "
                   "address expressions are identical. This approach is faster but less precise.}"

                   "The default is to use the " +
                   std::string(LIST_BASED_MEMORY==settings.memoryParadigm?"list-based":
                               MAP_BASED_MEMORY==settings.memoryParadigm?"map-based":
                               "UNKNOWN") + " paradigm."));

    CommandLine::insertBooleanSwitch(sg, "trace-semantics", settings.traceSemantics,
                                     "Trace low-level instruction semantics operations. The instruction semantics \"info\" "
                                     "diagnostic stream must also be enabled in order to see the output. This is intended "
                                     "mostly as an aid to debugging ROSE.");

    sg.insert(Switch("ip-rewrite")
              .argument("from", nonNegativeIntegerParser(settings.ipRewrite))
              .argument("to", nonNegativeIntegerParser(settings.ipRewrite))
              .whichValue(SAVE_ALL)
              .doc("After each instruction, if the instruction pointer has the value @v{from} then change it to be @v{to}. "
                   "This switch may appear multiple times."));

    return sg;
}

rose_addr_t
FeasiblePath::virtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    if (vertex->value().type() == P2::V_BASIC_BLOCK || vertex->value().type() == P2::V_USER_DEFINED)
        return vertex->value().address();
    ASSERT_not_reachable("invalid vertex type");
}

BaseSemantics::Dispatcher::Ptr
FeasiblePath::buildVirtualCpu(const P2::Partitioner::ConstPtr &partitioner, const P2::CfgPath *path, PathProcessor *pathProcessor,
                              const SmtSolver::Ptr &solver) {
    ASSERT_not_null(partitioner);

    // Augment the register dictionary with a "path" register that holds the expression describing how the location is
    // reachable along some path.
    if (NULL == registers_) {
        registers_ = RegisterDictionary::instance("Rose::BinaryAnalysis::FeasiblePath");
        registers_->insert(partitioner->instructionProvider().registerDictionary());
        ASSERT_require(REG_PATH.isEmpty());
        REG_PATH = RegisterDescriptor(registers_->firstUnusedMajor(), 0, 0, 1);
        registers_->insert("path", REG_PATH);

        // Where are return values stored?  See also, end of this function. FIXME[Robb Matzke 2015-12-01]: We need to support
        // returning multiple values. We should be using the new calling convention analysis to detect these.
        ASSERT_require(REG_RETURN_.isEmpty());
        Disassembler::Base::Ptr dis = partitioner->instructionProvider().disassembler();
        ASSERT_not_null(dis);
        RegisterDescriptor r;
        if (dis.dynamicCast<Disassembler::X86>()) {
            if ((r = registers_->find("rax")) || (r = registers_->find("eax")) || (r = registers_->find("ax")))
                REG_RETURN_ = r;
        } else if (dis.dynamicCast<Disassembler::M68k>()) {
            if ((r = registers_->find("d0")))
                REG_RETURN_ = r;                        // m68k also typically has other return registers
        } else if (dis.dynamicCast<Disassembler::Powerpc>()) {
            if ((r = registers_->find("r3")))
                REG_RETURN_ = r;                        // PowerPC also returns via r4
#ifdef ROSE_ENABLE_ASM_AARCH32
        } else if (dis.dynamicCast<Disassembler::Aarch32>()) {
            if ((r = registers_->find("r0")))
                REG_RETURN_ = r;
#endif
#ifdef ROSE_ENABLE_ASM_AARCH64
        } else if (dis.dynamicCast<Disassembler::Aarch64>()) {
            if ((r = registers_->find("r0")))
                REG_RETURN_ = r;
#endif
        } else {
            ASSERT_not_implemented("function return value register is not implemented for this ISA/ABI");
        }
    }

    // Create the RiscOperators and Dispatcher.
    RiscOperators::Ptr ops = RiscOperators::instance(partitioner, registers_, this, path, pathProcessor);
    ops->trimThreshold(settings_.maxExprSize);
    ops->initialState(ops->currentState()->clone());
    ops->nullPtrSolver(solver);
    for (size_t i = 0; i < settings().ipRewrite.size(); i += 2) {
        const RegisterDescriptor REG_IP = partitioner->instructionProvider().instructionPointerRegister();
        BaseSemantics::SValue::Ptr oldValue = ops->number_(REG_IP.nBits(), settings().ipRewrite[i+0]);
        BaseSemantics::SValue::Ptr newValue = ops->number_(REG_IP.nBits(), settings().ipRewrite[i+1]);
        SAWYER_MESG(mlog[DEBUG]) <<"ip-rewrite hot-patch from " <<*oldValue <<" to " <<*newValue <<"\n";
        ops->hotPatch().append(HotPatch::Record(REG_IP, oldValue, newValue, HotPatch::Record::MATCH_BREAK));
    }

    BaseSemantics::Dispatcher::Ptr cpu = partitioner->architecture()->newInstructionDispatcher(ops);
    ASSERT_not_null(cpu);

    // More return value stuff, continued from above
#ifdef ROSE_ENABLE_ASM_AARCH64
    if (boost::dynamic_pointer_cast<InstructionSemantics::DispatcherAarch64>(cpu)) {
        REG_RETURN_ = registers_->find("x0");
    }
#endif

    return cpu;
}

void
FeasiblePath::setInitialState(const BaseSemantics::Dispatcher::Ptr &cpu,
                              const P2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex) {
    ASSERT_not_null(cpu);
    ASSERT_forbid(REG_PATH.isEmpty());
    checkSettings();

    // Create the new state from an existing state and make the new state current.
    BaseSemantics::State::Ptr state = cpu->currentState()->clone();
    state->clear();
    cpu->initializeState(state);
    BaseSemantics::RiscOperators::Ptr ops = cpu->operators();
    ops->currentState(state);

    // Start of path is always feasible.
    ops->writeRegister(REG_PATH, ops->boolean_(true));

    // Initialize instruction pointer register
    if (pathsBeginVertex->value().type() == P2::V_INDETERMINATE) {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->undefined_(cpu->instructionPointerRegister().nBits()));
    } else {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->number_(cpu->instructionPointerRegister().nBits(), pathsBeginVertex->value().address()));
    }

    // Initialize stack pointer register
    if (settings_.initialStackPtr) {
        const RegisterDescriptor REG_SP = cpu->stackPointerRegister();
        ops->writeRegister(REG_SP, ops->number_(REG_SP.nBits(), *settings_.initialStackPtr));
    }

    // Direction flag (DF) is always set
    if (const RegisterDescriptor REG_DF = cpu->registerDictionary()->find("df"))
        ops->writeRegister(REG_DF, ops->boolean_(true));

    initialState_ = ops->currentState()->clone();
}

/** Process instructions for one basic block on the specified virtual CPU. */
void
FeasiblePath::processBasicBlock(const P2::BasicBlock::Ptr &bblock, const BaseSemantics::Dispatcher::Ptr &cpu,
                                size_t pathInsnIndex) {
    ASSERT_not_null(bblock);
    ASSERT_not_null(cpu);
    checkSettings();
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"      processing basic block " <<bblock->printableName() <<"\n";

    // Update the path constraint "register"
    BaseSemantics::RiscOperators::Ptr ops = cpu->operators();
    const RegisterDescriptor IP = cpu->instructionPointerRegister();
    BaseSemantics::SValue::Ptr ip = ops->readRegister(IP, ops->undefined_(IP.nBits()));
    BaseSemantics::SValue::Ptr va = ops->number_(ip->nBits(), bblock->address());
    BaseSemantics::SValue::Ptr pathConstraint = ops->isEqual(ip, va);
    ops->writeRegister(REG_PATH, pathConstraint);

    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("          ");
        debug <<"        state before block:\n" <<(*ops->currentState() + fmt);
    }

    // Process each instruction in the basic block
    for (SgAsmInstruction *insn: bblock->instructions()) {
        try {
            if (pathInsnIndex != size_t(-1)) {
                SAWYER_MESG(debug) <<"        processing path insn #" <<pathInsnIndex <<" at " <<insn->toString() <<"\n";
                fpOperators(ops)->pathInsnIndex(pathInsnIndex++);
            } else {
                SAWYER_MESG(debug) <<"        processing path insn at " <<insn->toString() <<"\n";
            }
            cpu->processInstruction(insn);
            if (debug) {
                // Show stack pointer
                const RegisterDescriptor SP = cpu->stackPointerRegister();
                BaseSemantics::SValue::Ptr sp = ops->readRegister(SP, ops->undefined_(SP.nBits()));
                debug <<"          sp = " <<*sp <<"\n";
            }
        } catch (const BaseSemantics::NotImplemented &e) {
            if (settings_.ignoreSemanticFailure) {
                SAWYER_MESG(mlog[WHERE]) <<"semantics failed (instruction ignored): " <<e <<"\n";
            } else {
                SAWYER_MESG(mlog[WHERE]) <<"semantics failed: " <<e <<"\n";
                throw;
            }
        } catch (const BaseSemantics::Exception &e) {
            if (settings_.ignoreSemanticFailure) {
                SAWYER_MESG(mlog[WARN]) <<"semantics failed (instruction ignored): " <<e <<"\n";
            } else {
                SAWYER_MESG(mlog[WARN]) <<"semantics failed: " <<e <<"\n";
                throw;
            }
        }
    }

    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("          ");
        debug <<"        state after block:\n" <<(*ops->currentState() + fmt);
    }
}

/** Process an indeterminate block. This represents flow of control through an unknown address. */
void
FeasiblePath::processIndeterminateBlock(const P2::ControlFlowGraph::ConstVertexIterator&,
                                        const BaseSemantics::Dispatcher::Ptr&, size_t pathInsnIndex) {
    SAWYER_MESG(mlog[DEBUG]) <<"      processing indeterminate vertex\n";
    mlog[WARN] <<"control flow passes through an indeterminate address at path position #" <<pathInsnIndex <<"\n";
}

/** Process a function summary vertex. */
void
FeasiblePath::processFunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                                     const BaseSemantics::Dispatcher::Ptr &cpu, size_t pathInsnIndex) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    ASSERT_require(functionSummaries_.exists(pathsVertex->value().address()));
    const FunctionSummary &summary = functionSummaries_[pathsVertex->value().address()];
    SAWYER_MESG(debug) <<"      processing function summary " <<summary.name <<"\n";

    BaseSemantics::RiscOperators::Ptr ops = cpu->operators();
    if (pathInsnIndex != size_t(-1))
        fpOperators(ops)->pathInsnIndex(pathInsnIndex);

    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
        debug <<"summary semantics for " <<summary.name <<"\n";
        debug <<"  +-------------------------------------------------\n"
              <<"  | " <<summary.name <<"\n"
              <<"  +-------------------------------------------------\n"
              <<"    state before summarized function:\n"
              <<(*ops->currentState() + fmt);
    }

    SymbolicSemantics::SValue::Ptr retval;
    if (functionSummarizer_ && functionSummarizer_->process(*this, summary, fpOperators(ops))) {
        retval = functionSummarizer_->returnValue(*this, summary, fpOperators(ops));
    } else {
        // Make the function return an unknown value
        retval = SymbolicSemantics::SValue::promote(ops->undefined_(REG_RETURN_.nBits()));
        ops->writeRegister(REG_RETURN_, retval);

        // Simulate function returning to caller
        if (boost::dynamic_pointer_cast<InstructionSemantics::DispatcherPowerpc>(cpu)) {
            // PowerPC calling convention stores the return address in the link register (LR)
            const RegisterDescriptor LR = cpu->callReturnRegister();
            ASSERT_forbid(LR.isEmpty());
            BaseSemantics::SValue::Ptr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
            ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);

        } else if (boost::dynamic_pointer_cast<InstructionSemantics::DispatcherX86>(cpu) ||
                   boost::dynamic_pointer_cast<InstructionSemantics::DispatcherM68k>(cpu)) {
            // x86, amd64, and m68k store the return address at the top of the stack
            const RegisterDescriptor SP = cpu->stackPointerRegister();
            ASSERT_forbid(SP.isEmpty());
            BaseSemantics::SValue::Ptr stackPointer = ops->readRegister(SP, ops->undefined_(SP.nBits()));
            BaseSemantics::SValue::Ptr returnTarget = ops->readMemory(RegisterDescriptor(), stackPointer,
                                                                    ops->undefined_(stackPointer->nBits()),
                                                                    ops->boolean_(true));
            ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);

            // Pop some things from the stack.
            int64_t sd = summary.stackDelta != SgAsmInstruction::INVALID_STACK_DELTA ?
                         summary.stackDelta :
                         returnTarget->nBits() / 8;
            stackPointer = ops->add(stackPointer, ops->number_(stackPointer->nBits(), sd));
            ops->writeRegister(cpu->stackPointerRegister(), stackPointer);
#ifdef ROSE_ENABLE_ASM_AARCH64
        } else if (boost::dynamic_pointer_cast<InstructionSemantics::DispatcherAarch64>(cpu)) {
            // Return address is in the link register, lr
            const RegisterDescriptor LR = cpu->callReturnRegister();
            ASSERT_forbid(LR.isEmpty());
            BaseSemantics::SValue::Ptr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
            ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
#endif
#ifdef ROSE_ENABLE_ASM_AARCH32
        } else if (boost::dynamic_pointer_cast<InstructionSemantics::DispatcherAarch32>(cpu)) {
            // Return address is in the link register, lr
            const RegisterDescriptor LR = cpu->callReturnRegister();
            ASSERT_forbid(LR.isEmpty());
            BaseSemantics::SValue::Ptr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
            ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
#endif
        }
    }

    if (retval) {
        VarDetail varDetail;
        varDetail.returnFrom = summary.address;
        varDetail.firstAccessIdx = fpOperators(ops)->pathInsnIndex();
        fpOperators(ops)->varDetail(retval->get_expression()->isLeafNode()->toString(), varDetail);
    }

    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
        debug <<"    state after summarized function:\n";
        debug <<(*ops->currentState() + fmt);
    }
}

void
FeasiblePath::processVertex(const BaseSemantics::Dispatcher::Ptr &cpu,
                            const P2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                            size_t pathInsnIndex) {
    checkSettings();
    switch (pathsVertex->value().type()) {
        case P2::V_BASIC_BLOCK:
            processBasicBlock(pathsVertex->value().bblock(), cpu, pathInsnIndex);
            break;
        case P2::V_INDETERMINATE:
            processIndeterminateBlock(pathsVertex, cpu, pathInsnIndex);
            break;
        case P2::V_USER_DEFINED:
            processFunctionSummary(pathsVertex, cpu, pathInsnIndex);
            break;
        case P2::V_NONEXISTING:
            break;
        default:
            mlog[ERROR] <<"cannot comput path feasibility; invalid vertex type at "
                        <<P2::Partitioner::vertexName(*pathsVertex) <<"\n";
            cpu->operators()->writeRegister(cpu->instructionPointerRegister(),
                                            cpu->operators()->number_(cpu->instructionPointerRegister().nBits(),
                                                                      0x911 /*arbitrary, unlikely to be satisfied*/));
    }
}

void
FeasiblePath::printPathVertex(std::ostream &out, const P2::ControlFlowGraph::Vertex &pathVertex, size_t &insnIdx) const {
    switch (pathVertex.value().type()) {
        case P2::V_BASIC_BLOCK: {
            for (SgAsmInstruction *insn: pathVertex.value().bblock()->instructions()) {
                out <<"    #" <<std::setw(5) <<std::left <<insnIdx++
                    <<" " <<partitioner_->unparse(insn) <<"\n";
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
    for (const P2::ControlFlowGraph::ConstVertexIterator &pathVertex: path.vertices()) {
        if (0==pathIdx) {
            out <<"  at path vertex " <<partitioner()->vertexName(pathVertex) <<"\n";
        } else {
            out <<"  via path edge " <<partitioner()->edgeName(path.edges()[pathIdx-1]) <<"\n";
        }
        printPathVertex(out, *pathVertex, insnIdx /*in,out*/);
        ++pathIdx;
    }
}

SymbolicExpression::Ptr
FeasiblePath::pathEdgeConstraint(const P2::ControlFlowGraph::ConstEdgeIterator &pathEdge, const BaseSemantics::Dispatcher::Ptr &cpu) {
    ASSERT_not_null(cpu);
    BaseSemantics::RiscOperators::Ptr ops = cpu->operators();
    static const char *prefix = "      ";

    const RegisterDescriptor IP = partitioner()->instructionProvider().instructionPointerRegister();
    BaseSemantics::SValue::Ptr ip = ops->peekRegister(IP, ops->undefined_(IP.nBits()));
    if (!settings_.nonAddressIsFeasible && !hasVirtualAddress(pathEdge->target())) {
        SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner()->edgeName(pathEdge)
                                 <<" because settings().nonAddressIsFeasible is false\n";
        return SymbolicExpression::Ptr();                     // trivially unfeasible
    } else if (auto ipval = ip->toUnsigned()) {
        if (!hasVirtualAddress(pathEdge->target())) {
            // If the IP register is pointing to an instruction but the path vertex is indeterminate (or undiscovered or
            // nonexisting) then consider this path to be not-feasible. If the CFG is accurate then there's probably
            // a sibling edge that points to the correct vertex.
            SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner()->edgeName(pathEdge) <<" because IP = "
                                     <<StringUtility::addrToString(*ipval) <<" and edge target has no address\n";
            return SymbolicExpression::Ptr();                 // trivially unfeasible
        } else if (*ipval != virtualAddress(pathEdge->target())) {
            // Executing the path forces us to go a different direction than where the path indicates we should go. We
            // don't need an SMT solver to tell us that when the values are just integers.
            SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner()->edgeName(pathEdge) <<" because IP = "
                                     <<StringUtility::addrToString(*ipval) <<" and edge target is "
                                     <<StringUtility::addrToString(virtualAddress(pathEdge->target())) <<"\n";
            return SymbolicExpression::Ptr();                 // trivially unfeasible
        }
    } else if (hasVirtualAddress(pathEdge->target())) {
        SymbolicExpression::Ptr targetVa = SymbolicExpression::makeIntegerConstant(ip->nBits(), virtualAddress(pathEdge->target()));
        SymbolicExpression::Ptr constraint = SymbolicExpression::makeEq(targetVa,
                                                                        SymbolicSemantics::SValue::promote(ip)->get_expression());
        constraint->comment("cfg edge " + partitioner()->edgeName(pathEdge));
        SAWYER_MESG(mlog[DEBUG]) <<prefix <<"constraint at edge " <<partitioner()->edgeName(pathEdge)
                                 <<": " <<*constraint <<"\n";
        return constraint;
    }

    SAWYER_MESG(mlog[DEBUG]) <<prefix <<"trivially feasible at edge " <<partitioner()->edgeName(pathEdge) <<"\n";
    return SymbolicExpression::makeBooleanConstant(true);     // trivially feasible
}

void
FeasiblePath::setSearchBoundary(const P2::Partitioner::ConstPtr &partitioner,
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

void
FeasiblePath::setSearchBoundary(const P2::Partitioner::ConstPtr &partitioner,
                                const P2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                                const P2::CfgConstVertexSet &cfgAvoidVertices,
                                const P2::CfgConstEdgeSet &cfgAvoidEdges) {
    P2::CfgConstVertexSet cfgBeginVertices;
    cfgBeginVertices.insert(cfgBeginVertex);
    setSearchBoundary(partitioner, cfgBeginVertices, cfgAvoidVertices, cfgAvoidEdges);
}

P2::ControlFlowGraph::ConstVertexIterator
FeasiblePath::pathToCfg(const P2::ControlFlowGraph::ConstVertexIterator &pathVertex) const {
    if (hasVirtualAddress(pathVertex))
        return partitioner()->findPlaceholder(virtualAddress(pathVertex));
    if (pathVertex->value().type() == P2::V_INDETERMINATE)
        return partitioner()->indeterminateVertex();
    if (pathVertex->value().type() == P2::V_NONEXISTING)
        return partitioner()->nonexistingVertex();
    ASSERT_not_implemented("cannot convert path vertex to CFG vertex");
}

P2::CfgConstVertexSet
FeasiblePath::cfgToPaths(const P2::CfgConstVertexSet &vertexSet) const {
    P2::CfgConstVertexSet retval;
    for (const P2::ControlFlowGraph::ConstVertexIterator &vertex: vertexSet.values()) {
        if (Sawyer::Optional<P2::ControlFlowGraph::ConstVertexIterator> found = vmap_.forward().find(vertex))
            retval.insert(*found);
    }
    return retval;
}

P2::CfgConstEdgeSet
FeasiblePath::cfgToPaths(const P2::CfgConstEdgeSet &cfgEdgeSet) const {
    P2::CfgConstEdgeSet retval;
    for (const P2::ControlFlowGraph::ConstEdgeIterator &cfgEdge: cfgEdgeSet.values()) {
        // We don't have a mapping directly from CFG to Paths for edges, so we use the vertex mapping instead.
        Sawyer::Optional<P2::ControlFlowGraph::ConstVertexIterator> pathSrcVertex = vmap_.forward().find(cfgEdge->source());
        Sawyer::Optional<P2::ControlFlowGraph::ConstVertexIterator> pathTgtVertex = vmap_.forward().find(cfgEdge->target());
        if (pathSrcVertex && pathTgtVertex) {
            boost::iterator_range<P2::ControlFlowGraph::ConstEdgeIterator> pathEdges = pathSrcVertex.get()->outEdges();
            for (P2::ControlFlowGraph::ConstEdgeIterator pathEdge = pathEdges.begin(); pathEdge != pathEdges.end(); ++pathEdge) {
                if (pathEdge->target() == *pathTgtVertex && pathEdge->value().type() == cfgEdge->value().type())
                    retval.insert(pathEdge);
            }
        }
    }
    return retval;
}

void
FeasiblePath::setSearchBoundary(const P2::Partitioner::ConstPtr &partitioner,
                                const P2::CfgConstVertexSet &cfgBeginVertices,
                                const P2::CfgConstVertexSet &cfgEndVertices,
                                const P2::CfgConstVertexSet &cfgAvoidVertices,
                                const P2::CfgConstEdgeSet &cfgAvoidEdges) {
    reset();
    partitioner_ = partitioner;
    isDirectedSearch_ = true;

    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    findInterFunctionPaths(partitioner->cfg(), paths_/*out*/, vmap_/*out*/,
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
    cfgEndAvoidVertices_.insert(cfgEndVertices);
    cfgAvoidEdges_ = cfgAvoidEdges;
}

void
FeasiblePath::setSearchBoundary(const P2::Partitioner::ConstPtr &partitioner,
                                const P2::CfgConstVertexSet &cfgBeginVertices,
                                const P2::CfgConstVertexSet &cfgAvoidVertices,
                                const P2::CfgConstEdgeSet &cfgAvoidEdges) {
    reset();
    partitioner_ = partitioner;
    isDirectedSearch_ = false;

    if (isDirectedSearch_) {
        // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an
        // ending vertex.
        findInterFunctionPaths(partitioner->cfg(), paths_/*out*/, vmap_/*out*/, cfgBeginVertices, cfgAvoidVertices, cfgAvoidEdges);
    } else {
        // Find top-level paths. These paths don't traverse into any function calls since all the calls will be inlined later.
        findFunctionPaths(partitioner->cfg(), paths_/*out*/, vmap_/*out*/, cfgBeginVertices, cfgAvoidVertices, cfgAvoidEdges);
    }
    if (paths_.isEmpty())
        return;

    // Paths graph equivalents of CFG arguments.
    pathsBeginVertices_ = cfgToPaths(cfgBeginVertices);
    pathsEndVertices_.clear();

    // When finding paths through a called function, avoid the usual vertices and edges.
    cfgEndAvoidVertices_ = cfgAvoidVertices;
    cfgAvoidEdges_ = cfgAvoidEdges;
}

P2::Partitioner::ConstPtr
FeasiblePath::partitioner() const {
    ASSERT_not_null2(partitioner_, "no search boundary set yet");
    return partitioner_;
}

// True if vertex is a function call
bool
FeasiblePath::isFunctionCall(const P2::ControlFlowGraph::ConstVertexIterator &pathVertex) const {
    P2::ControlFlowGraph::ConstVertexIterator cfgVertex = pathToCfg(pathVertex);
    ASSERT_require(partitioner()->cfg().isValidVertex(cfgVertex));
    for (P2::ControlFlowGraph::Edge edge: cfgVertex->outEdges()) {
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
FeasiblePath::shouldSummarizeCall(const P2::ControlFlowGraph::ConstVertexIterator &/*pathVertex*/,
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
    if ((size_t)callDepth >= settings_.maxCallDepth) {
        SAWYER_THREAD_TRAITS::LockGuard lock(statsMutex_);
        ++stats_.maxCallDepthHits;
        return false;
    }

    // Don't inline if there's no function
    if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
        return false;
    P2::Function::Ptr callee = cfgCallTarget->value().isEntryBlock();
    if (!callee)
        return false;

    // Don't let recursion get too deep
    if (settings_.maxRecursionDepth < UNLIMITED) {
        ssize_t callDepth = path.callDepth(callee);
        ASSERT_require(callDepth >= 0);
        if ((size_t)callDepth >= settings_.maxRecursionDepth) {
            SAWYER_THREAD_TRAITS::LockGuard lock(statsMutex_);
            ++stats_.maxRecursionDepthHits;
            return false;
        }
    }

    // Don't inline imported functions that aren't linked -- we'd just get bogged down deep inside the
    // dynamic linker without ever being able to resolve the actual function's instructions.
    if (P2::ModulesElf::isUnlinkedImport(partitioner_, callee))
        return false;

    return true;
}

void
FeasiblePath::insertCallSummary(const P2::ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                                const P2::ControlFlowGraph &cfg, const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge) {
    ASSERT_always_require(cfg.isValidEdge(cfgCallEdge));
    P2::ControlFlowGraph::ConstVertexIterator cfgCallTarget = cfgCallEdge->target();
    P2::Function::Ptr function = cfgCallTarget->value().isEntryBlock();

    P2::ControlFlowGraph::VertexIterator summaryVertex = paths_.insertVertex(P2::CfgVertex(P2::V_USER_DEFINED));
    paths_.insertEdge(pathsCallSite, summaryVertex, P2::CfgEdge(P2::E_FUNCTION_CALL));
    P2::CfgConstEdgeSet callReturnEdges = P2::findCallReturnEdges(pathsCallSite);
    for (const P2::ControlFlowGraph::ConstEdgeIterator &callret: callReturnEdges.values())
        paths_.insertEdge(summaryVertex, callret->target(), P2::CfgEdge(P2::E_FUNCTION_RETURN));

    int64_t stackDelta = function ? function->stackDeltaConcrete() : SgAsmInstruction::INVALID_STACK_DELTA;

    FunctionSummary summary(cfgCallTarget, stackDelta);
    if (functionSummarizer_)
        functionSummarizer_->init(*this, summary /*in,out*/, function, cfgCallTarget);
    functionSummaries_.insert(summary.address, summary);
    summaryVertex->value().address(summary.address);
}

boost::filesystem::path
FeasiblePath::emitPathGraph(size_t callId, size_t graphId) {
    char callIdStr[32], graphIdStr[32];
    snprintf(callIdStr, sizeof(callIdStr), "invoc-%04zu", callId);
    snprintf(graphIdStr, sizeof(graphIdStr), "cfg-%06zu.dot", graphId);
    boost::filesystem::path debugDir = boost::filesystem::path("rose-debug/BinaryAnalysis/FeasiblePath") / callIdStr;
    boost::filesystem::create_directories(debugDir);
    boost::filesystem::path pathGraphName = debugDir / graphIdStr;

    std::ofstream file(pathGraphName.string().c_str());
    P2::GraphViz::CfgEmitter emitter(partitioner(), paths_);
    emitter.showInstructions(true);
    emitter.selectWholeGraph();

    for (const P2::ControlFlowGraph::ConstVertexIterator &v: pathsBeginVertices_.values()) {
        emitter.vertexOrganization(v).attributes().insert("style", "filled");
        emitter.vertexOrganization(v).attributes().insert("fillcolor", "#faff7d");
    }
    for (const P2::ControlFlowGraph::ConstVertexIterator &v: pathsEndVertices_.values()) {
        emitter.vertexOrganization(v).attributes().insert("style", "filled");
        emitter.vertexOrganization(v).attributes().insert("fillcolor", "#faff7d");
    }

    emitter.emit(file);
    return pathGraphName;
}

bool
FeasiblePath::isAnyEndpointReachable(const P2::ControlFlowGraph &cfg,
                                     const P2::ControlFlowGraph::ConstVertexIterator &beginVertex,
                                     const P2::CfgConstVertexSet &endVertices) {
    if (!isDirectedSearch_)
        return true;
    if (endVertices.empty())
        return false;
    typedef Sawyer::Container::Algorithm::DepthFirstForwardVertexTraversal<const P2::ControlFlowGraph> Traversal;
    for (Traversal t(cfg, beginVertex); t; ++t) {
        if (endVertices.exists(t.vertex()))
            return true;
    }
    return false;
}

BaseSemantics::State::Ptr
FeasiblePath::initialState() const {
    return initialState_;
}

BaseSemantics::State::Ptr
FeasiblePath::pathPostState(const P2::CfgPath &path, size_t vertexIdx) {
    return path.vertexAttributes(vertexIdx).attributeOrDefault<BaseSemantics::State::Ptr>(POST_STATE);
}

double
FeasiblePath::pathEffectiveK(const P2::CfgPath &path) const {
    for (size_t i = path.nVertices(); i > 0; --i) {
        double d = path.vertexAttributes(i-1).attributeOrElse(EFFECTIVE_K, 0.0);
        if (d != 0.0)
            return d;
    }
    return settings_.maxPathLength;
}

void
FeasiblePath::backtrack(P2::CfgPath &path /*in,out*/, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(solver);
    ASSERT_require(!path.isEmpty());
    ASSERT_require(solver->nLevels() == 1 + path.nEdges());

    size_t nPopped = path.backtrack().size();
    for (size_t i=0; i<nPopped; ++i)
        solver->pop();

    while (solver->nLevels() < 1 + path.nEdges())
        solver->push();

    ASSERT_require(solver->nLevels() == 1 + path.nEdges());
}

FeasiblePath::Expression
FeasiblePath::parseExpression(Expression expr, const std::string &where, SymbolicExpressionParser &exprParser) const {
    if (!expr.parsable.empty() && !expr.expr) {
        expr.expr = exprParser.parse(expr.parsable);
    }

    if (where == "end") {
        expr.location = AddressIntervalSet();
    } else if (!where.empty()) {
        expr.location.clear();
        const char *s = where.c_str();
        const char *rest = s;
        try {
            while (*s) {
                AddressInterval interval = P2::AddressIntervalParser::parse(s, &rest);
                expr.location.insert(interval);
                ASSERT_not_null(rest);
                if (','==*rest)
                    ++rest;
                s = rest;
            }
        } catch (const std::runtime_error &e) {
            // If we get here, then the "where" string isn't a valid address or address interval. Try
            // looking for a function with this name.
            ASSERT_not_null(partitioner_);
            for (const P2::Function::Ptr &function: partitioner_->functions()) {
                if (function->name() == where)
                    expr.location.insert(function->address());
            }
            if (expr.location.isEmpty()) {
                mlog[ERROR] <<"problem parsing assertion \"where\" specification: \"" <<StringUtility::cEscape(where) <<"\"\n"
                            <<"  no function with specified name\n"
                            <<"  invalid address of address interval\n"
                            <<"         " <<s <<"\n"
                            <<"  here---" <<std::string(rest-s, '-') <<"^\n";
            }
        }
    } else {
        mlog[ERROR] <<"problem parsing assertion \"where\" specification: specification is empty\n";
    }

    return expr;
}

SymbolicExpression::Ptr
FeasiblePath::expandExpression(const Expression &expr, const SymbolicExpressionParser &parser) {
    if (expr.expr) {
        return parser.delayedExpansion(expr.expr);
    } else if (!expr.parsable.empty()) {
        ASSERT_not_reachable("string should have been parsed by now");
    } else {
        return SymbolicExpression::makeBooleanConstant(false);
    }
}

void
FeasiblePath::insertAssertions(const SmtSolver::Ptr &solver, const P2::CfgPath &path,
                               const std::vector<Expression> &assertions, bool atEndOfPath,
                               const SymbolicExpressionParser &parser) {
    ASSERT_not_null(solver);
    ASSERT_forbid(path.isEmpty());

    if (assertions.empty())
        return;

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    P2::ControlFlowGraph::ConstVertexIterator vertex = path.backVertex();
    if (Sawyer::Optional<rose_addr_t> blockVa = vertex->value().optionalAddress()) {
        SAWYER_MESG(debug) <<"    assertions for " <<StringUtility::addrToString(*blockVa) <<":\n";
        for (size_t i = 0; i < assertions.size(); ++i) {
            if (assertions[i].location.contains(*blockVa) || (atEndOfPath && assertions[i].location.isEmpty())) {
                SymbolicExpression::Ptr assertion = expandExpression(assertions[i], parser);
                solver->insert(assertion);
                if (debug) {
                    debug <<"      #" <<i <<": " <<*assertion <<"\n";
                    if (assertion != assertions[i].expr) {
                        if (!assertions[i].parsable.empty()) {
                            debug <<"        parsed from:   " <<assertions[i].parsable <<"\n";
                        } else {
                            debug <<"        expanded from: " <<*assertions[i].expr <<"\n";
                        }
                    }
                }
            }
        }
    }
}

SmtSolver::Satisfiable
FeasiblePath::solvePathConstraints(const SmtSolver::Ptr &solver, const P2::CfgPath &path,
                                   const SymbolicExpression::Ptr &edgeAssertion, const Substitutions &subst, bool atEndOfPath) {
    ASSERT_not_null(solver);
    ASSERT_not_null(edgeAssertion);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    solver->insert(edgeAssertion);
    insertAssertions(solver, path, subst.assertions, atEndOfPath, subst.exprParser);
    return solver->check();
}

size_t
FeasiblePath::vertexSize(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    switch (vertex->value().type()) {
        case P2::V_BASIC_BLOCK:
            return vertex->value().bblock()->nInstructions();
        case P2::V_INDETERMINATE:
        case P2::V_USER_DEFINED:
        case P2::V_NONEXISTING:
            return 1;
        default:
            ASSERT_not_reachable("invalid path vertex type");
    }
}

size_t
FeasiblePath::pathLength(const P2::CfgPath &path, int position) {
    size_t idx = 0, retval = 0;
    if (positionToIndex(position, path.nVertices()).assignTo(idx)) {
        for (size_t i = 0; i <= idx; ++i)
            retval += vertexSize(path.vertices()[i]);
    }
    return retval;
}

void
FeasiblePath::markAsReached(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    if (Sawyer::Optional<rose_addr_t> addr = vertex->value().optionalAddress()) {
        SAWYER_THREAD_TRAITS::LockGuard lock(statsMutex_);
        ++stats_.reachedBlockVas.insertMaybe(*addr, 0);
    }
}

void
FeasiblePath::dfsDebugHeader(Sawyer::Message::Stream &trace, Sawyer::Message::Stream &debug, size_t callId, size_t graphId) {
    if (trace || debug) {
        SAWYER_MESG_OR(trace, debug) <<"depthFirstSearch call #" <<callId <<":\n";
        SAWYER_MESG_OR(trace, debug) <<"  paths graph saved in " <<emitPathGraph(callId, graphId) <<"\n";
        for (const P2::ControlFlowGraph::ConstVertexIterator &v: pathsBeginVertices_.values())
            SAWYER_MESG_OR(trace, debug) <<"  begin at vertex " <<partitioner()->vertexName(v) <<"\n";
        if (isDirectedSearch()) {
            for (const P2::ControlFlowGraph::ConstVertexIterator &v: pathsEndVertices_.values())
                SAWYER_MESG_OR(trace, debug) <<"  end   at vertex " <<partitioner()->vertexName(v) <<"\n";
        } else {
            SAWYER_MESG_OR(trace, debug) <<"  undirected search (no particular end vertices)\n";
        }
    }
}

FeasiblePath::Substitutions
FeasiblePath::parseSubstitutions() {
    // Parse user-supplied assertions and their locations. Register and memory references are replaced by temporary variables
    // that will be expanded at a later time in order to obtain the then-current values of registers and memory.
    Substitutions retval;

    retval.regSubber = retval.exprParser.defineRegisters(partitioner()->instructionProvider().registerDictionary());
    retval.memSubber = SymbolicExpressionParser::MemorySubstituter::instance(SmtSolver::Ptr());
    retval.exprParser.appendOperatorExpansion(retval.memSubber);
    ASSERT_require(settings_.assertions.size() == settings_.assertionLocations.size());
    for (size_t i = 0; i < settings_.assertions.size(); ++i)
        retval.assertions.push_back(parseExpression(settings_.assertions[i], settings_.assertionLocations[i], retval.exprParser));
    return retval;
}

void
FeasiblePath::makeSubstitutions(const Substitutions &subst, const BaseSemantics::RiscOperators::Ptr &ops) {
    subst.regSubber->riscOperators(ops);
    subst.memSubber->riscOperators(ops);
}

SmtSolver::Ptr
FeasiblePath::createSmtSolver() {
    SmtSolverPtr solver = SmtSolver::instance(settings_.solverName);
    ASSERT_always_not_null(solver);
    solver->errorIfReset(true);
    solver->name("FeasiblePath " + solver->name());
    if (settings_.smtTimeout)
        solver->timeout(*settings_.smtTimeout);
#if 1 // DEBUGGING [Robb Matzke 2018-11-14]
    solver->memoizer(SmtSolver::Memoizer::Ptr());
#else
    solver->memoizer(...something_not_null...);
#endif
    return solver;
}

FeasiblePath::Semantics
FeasiblePath::createSemantics(const P2::CfgPath &path, PathProcessor &pathProcessor, const SmtSolver::Ptr &solver) {
    Semantics retval;

    retval.cpu = buildVirtualCpu(partitioner(), &path, &pathProcessor, solver);
    ASSERT_not_null(retval.cpu);

    setInitialState(retval.cpu, path.frontVertex());

    retval.ops = retval.cpu->operators();
    ASSERT_not_null(retval.ops);

    retval.originalState = retval.ops->currentState();
    ASSERT_not_null(retval.originalState);

    if (settings_.traceSemantics) {
        BaseSemantics::RiscOperators::Ptr tracerOps = TraceSemantics::RiscOperators::instance(retval.ops);
        retval.cpu->operators(tracerOps);
    }

    return retval;
}

void
FeasiblePath::dfsDebugCurrentPath(Sawyer::Message::Stream &debug, const P2::CfgPath &path, const SmtSolver::Ptr &solver,
                                  size_t effectiveMaxPathLength) {
    if (debug) {
        debug <<"  path vertices (" <<path.nVertices() <<"):";
        for (const P2::ControlFlowGraph::ConstVertexIterator &v: path.vertices())
            debug <<" " <<partitioner()->vertexName(v);
        debug <<"\n";
        debug <<"    SMT solver has " <<StringUtility::plural(solver->nLevels(), "transactions") <<"\n";
        debug <<"    path has " <<StringUtility::plural(path.nVertices(), "vertices") <<"\n";
        debug <<"    path length is " <<StringUtility::plural(pathLength(path), "instructions") <<"\n";
        debug <<"    effective k is " <<effectiveMaxPathLength <<" instructions\n";
    }
}

// class method
Sawyer::Optional<size_t>
FeasiblePath::positionToIndex(int position, size_t nElmts) {
    if (position >= 0 && (size_t)position < nElmts) {
        return (size_t)position;
    } else if (position < 0 && (size_t)(-position) <= nElmts) {
        return nElmts - (size_t)(-position);
    } else {
        return Sawyer::Nothing();
    }
}

BaseSemantics::State::Ptr
FeasiblePath::incomingState(const P2::CfgPath &path, int position, const BaseSemantics::State::Ptr &initialState) {
    ASSERT_require2(positionToIndex(position, path.nVertices()), "position is out of range");
    size_t idx = positionToIndex(position, path.nVertices()).orElse(0);
    return 0 == idx ? initialState : pathPostState(path, idx-1);
}

size_t
FeasiblePath::incomingStepCount(const P2::CfgPath &path, int position) {
    return 0 == position ? size_t(0) : pathLength(path, position-1);
}

size_t
FeasiblePath::outgoingStepCount(const P2::CfgPath &path, int position) {
    return pathLength(path, position);
}

BaseSemantics::State::Ptr
FeasiblePath::evaluate(P2::CfgPath &path, int position, const Semantics &sem) {
    Stream debug(mlog[DEBUG]);
    ASSERT_require2(positionToIndex(position, path.nVertices()), "position is out of range");
    size_t goalIdx = positionToIndex(position, path.nVertices()).orElse(0);

    // Find the most recent path vertex that has been evaluated, and get a copy of its outgoing state.
    size_t idx = goalIdx;
    BaseSemantics::State::Ptr state = incomingState(path, idx, sem.originalState);
    while (!state && idx > 0)
        state = incomingState(path, --idx, sem.originalState);
    ASSERT_not_null(state);

    // Evaluate each vertex up to and including the goal vertex, saving the results per vertex.
    for (/*void*/; idx <= goalIdx; ++idx) {
        state = state->clone();                         // need to make a copy before we modify it
        sem.ops->currentState(state);
        try {
            processVertex(sem.cpu, path.vertices()[idx], incomingStepCount(path, idx));
        } catch (...) {
            SAWYER_MESG(debug) <<"    path semantics failed, path idx = " <<idx <<"\n";
            state = BaseSemantics::State::Ptr();
            break;
        }
        path.vertexAttributes(idx).setAttribute(POST_STATE, state);
    }

    sem.ops->currentState(BaseSemantics::State::Ptr());
    return state;
}

boost::logic::tribool
FeasiblePath::isFeasible(P2::CfgPath &path, const Substitutions &subst, const Semantics &sem,
                         const SmtSolver::Ptr &solver) {
    Stream debug(mlog[DEBUG]);
    ASSERT_forbid(path.isEmpty());
    ASSERT_require(solver->nLevels() == path.nVertices());
    boost::logic::tribool retval = false;

    bool atEndOfPath = pathsEndVertices_.exists(path.backVertex()); // true if path has reached one of the end goals
    BaseSemantics::State::Ptr oldState = sem.ops->currentState(); // probably null

    if (path.nVertices() == 1) {
        switch (solvePathConstraints(solver, path, SymbolicExpression::makeBooleanConstant(true), subst, atEndOfPath)) {
            case SmtSolver::SAT_YES:
                SAWYER_MESG(debug) <<" = is feasible\n";
                retval = true;
                break;
            case SmtSolver::SAT_NO:
                SAWYER_MESG(debug) <<" = not feasible\n";
                retval = false;
                break;
            case SmtSolver::SAT_UNKNOWN:
                SAWYER_MESG(debug) <<" = unknown\n";
                retval = boost::logic::indeterminate;
                break;
        }
    } else {
        // pathEdgeConstraint needs the incoming state of the last vertex, which is the outgoing state of the penultimate
        // vertex. It doesn't modify the state.
        BaseSemantics::State::Ptr curState = evaluate(path, -2, sem);
        sem.ops->currentState(curState);

        if (solver->nAssertions(solver->nLevels()-1) > 0) {
            SAWYER_MESG(debug) <<" = is feasible (previously computed)\n";
            retval = true;
        } else if (SymbolicExpression::Ptr edgeConstraint = pathEdgeConstraint(path.edges().back(), sem.cpu)) {
            switch (solvePathConstraints(solver, path, edgeConstraint, subst, atEndOfPath)) {
                case SmtSolver::SAT_YES:
                    SAWYER_MESG(debug) <<" = is feasible\n";
                    retval = true;
                    break;
                case SmtSolver::SAT_NO:
                    SAWYER_MESG(debug) <<" = not feasible\n";
                    retval = false;
                    break;
                case SmtSolver::SAT_UNKNOWN:
                    SAWYER_MESG(debug) <<" = unknown\n";
                    retval = boost::logic::indeterminate;
                    break;
            }
        } else {
            SAWYER_MESG(debug) <<" = not feasible (trivial)\n";
            retval = false;
        }
    }

    // Restore the previous state
    sem.ops->currentState(oldState);
    return retval;
}

FeasiblePath::PathProcessor::Action
FeasiblePath::callPathProcessorFound(PathProcessor &pathProcessor, P2::CfgPath &path, const Semantics &sem,
                                     const SmtSolver::Ptr &solver) {
    Stream debug(mlog[DEBUG]);
    BaseSemantics::State::Ptr curState;
    if (settings().processFinalVertex)
        curState = evaluate(path, -1, sem);             // might be null for failed semantics

    // Set the current state, cloning if necessary so that the user doesn't modify our copy.
    if (curState) {
        sem.ops->currentState(curState->clone());
    } else {
        sem.ops->currentState(BaseSemantics::State::Ptr());
    }

    // Protect our SMT solver from user modifications
    SmtSolver::Transaction transaction(solver);

    // User callback, then restore CPU state
    PathProcessor::Action retval = pathProcessor.found(*this, path, sem.cpu, solver);
    sem.ops->currentState(BaseSemantics::State::Ptr());   // to fail early if we don't specify a state later
    return retval;
}

double
FeasiblePath::adjustEffectiveK(P2::CfgPath &path, double k) {
    ASSERT_forbid(path.isEmpty());
    P2::ControlFlowGraph::ConstVertexIterator backVertex = path.backVertex();
    size_t nVertexVisits = path.nVisits(backVertex);
    std::string indent = mlog[DEBUG] ? "    " : "";

    if (nVertexVisits > settings_.maxVertexVisit) {
        SAWYER_MESG(mlog[TRACE]) <<indent <<"max visits (" <<settings_.maxVertexVisit <<") reached"
                                 <<" for vertex " <<partitioner()->vertexName(backVertex) <<"\n";
        SAWYER_THREAD_TRAITS::LockGuard lock(statsMutex_);
        ++stats_.maxVertexVisitHits;
        return 0.0;                                   // limit reached
    } else if (nVertexVisits > 1 && !rose_isnan(settings_.kCycleCoefficient)) {
        size_t n = vertexSize(backVertex);
        double increment = n * settings_.kCycleCoefficient;
        if (increment != 0.0) {
            k += increment;
            SAWYER_MESG(mlog[DEBUG]) <<"    revisting prior vertex; k += " <<increment
                                     <<", effective k = " <<k <<"\n";
            path.vertexAttributes(path.nVertices()-1).setAttribute(EFFECTIVE_K, k);
        }
    }

    size_t nSteps = pathLength(path);
    if (nSteps > k) {
        SAWYER_MESG(mlog[TRACE]) <<indent <<"maximum path length exceeded:"
                                 <<" path length is " <<StringUtility::plural(nSteps, "steps")
                                 <<", effective limit is " <<k
                                 <<" at vertex " <<partitioner()->vertexName(backVertex) <<"\n";
        SAWYER_THREAD_TRAITS::LockGuard lock(statsMutex_);
        ++stats_.maxPathLengthHits;
        return 0.0;
    }

    return k;
}

void
FeasiblePath::summarizeOrInline(P2::CfgPath &path, const Semantics &sem) {
    Stream debug(mlog[DEBUG]);
    std::string indent = debug ? "    " : "";

    P2::ControlFlowGraph::ConstVertexIterator backVertex = path.backVertex();
    P2::ControlFlowGraph::ConstVertexIterator cfgBackVertex = pathToCfg(backVertex);
    ASSERT_require(partitioner()->cfg().isValidVertex(cfgBackVertex));
    P2::CfgConstEdgeSet callEdges = P2::findCallEdges(cfgBackVertex);
    P2::CfgConstEdgeSet erasableEdges;

    for (const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge: callEdges.values()) {
        if (shouldSummarizeCall(path.backVertex(), partitioner()->cfg(), cfgCallEdge->target())) {
            SAWYER_MESG(debug) <<indent <<"summarizing function for edge " <<partitioner()->edgeName(cfgCallEdge) <<"\n";
            insertCallSummary(backVertex, partitioner()->cfg(), cfgCallEdge);
            erasableEdges.insert(cfgCallEdge);
        } else if (shouldInline(path, cfgCallEdge->target())) {
            if (cfgCallEdge->target()->value().type() == P2::V_INDETERMINATE &&
                cfgBackVertex->value().type() == P2::V_BASIC_BLOCK) {
                // If the CFG has a vertex to an indeterminate function (e.g., from "call eax"), then instead of inlining the
                // indeterminate vertex, see if we can inline an actual function by using the instruction pointer register. The
                // cpu's currentState is the one at the beginning of the final vertex of the path; we need the state at the end
                // of the final vertex.
                SAWYER_MESG(debug) <<indent <<"inlining indeterminate function call paths at vertex "
                                   <<partitioner()->vertexName(backVertex) <<"\n";

                // Process the basic block containing the indirect function call in order to get the final IP value, the target
                // address of the call, if possible.  Be careful not to mess up the state that's already been saved as the
                // incoming state to this block.
                const RegisterDescriptor IP = partitioner()->instructionProvider().instructionPointerRegister();
                BaseSemantics::SValue::Ptr ip;
                if (BaseSemantics::State::Ptr state = evaluate(path, -1, sem)) {
                    ip = state->peekRegister(IP, sem.ops->undefined_(IP.nBits()), sem.ops.get());
                } else {
                    mlog[ERROR] <<"semantics failed when trying to determine call target address\n";
                }
                sem.ops->currentState(BaseSemantics::State::Ptr()); // set to null for safety

                // If the IP is concrete, then we found the target of the indirect call and can inline it.
                if (ip && ip->toUnsigned()) {
                    rose_addr_t targetVa = ip->toUnsigned().get();
                    P2::ControlFlowGraph::ConstVertexIterator targetVertex = partitioner()->findPlaceholder(targetVa);
                    if (partitioner()->cfg().isValidVertex(targetVertex))
                        P2::inlineOneCallee(paths_, backVertex, partitioner()->cfg(),
                                            targetVertex, cfgEndAvoidVertices_, cfgAvoidEdges_);
                }
            } else {
                SAWYER_MESG(debug) <<indent <<"inlining CFG function call paths at vertex "
                                   <<partitioner()->vertexName(backVertex) <<"\n";
                P2::inlineMultipleCallees(paths_, backVertex, partitioner()->cfg(),
                                          cfgBackVertex, cfgEndAvoidVertices_, cfgAvoidEdges_);
            }
        } else {
            SAWYER_MESG(debug) <<indent <<"summarizing function for edge " <<partitioner()->edgeName(cfgCallEdge) <<"\n";
            insertCallSummary(backVertex, partitioner()->cfg(), cfgCallEdge);
            erasableEdges.insert(cfgCallEdge);
        }

        // Remove all call-return edges. This is necessary so we don't re-enter this case with infinite recursion. No need to
        // worry about adjusting the path because these edges aren't on the current path.
        P2::eraseEdges(paths_, P2::findCallReturnEdges(backVertex));
        P2::eraseEdges(paths_, cfgToPaths(erasableEdges));
    }
}

void
FeasiblePath::depthFirstSearch(PathProcessor &pathProcessor) {
    ASSERT_not_null(partitioner_);

    // Initialization
    static size_t callId = 0;                           // number of calls to this function
    size_t graphId = 0;                                 // incremented each time the graph is modified
    {
        static SAWYER_THREAD_TRAITS::Mutex mutex;
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
        ++callId;
    }
    Stream debug(mlog[DEBUG]);
    Stream trace(mlog[TRACE]);
    std::string indent = debug ? "    " : "";
    if (paths_.isEmpty())
        return;
    if (settings().nullDeref.minValid == 0)
        mlog[WARN] <<"minimum valid address is set to zero; no null derefs are possible\n";
    dfsDebugHeader(trace, debug, callId, graphId);
    Substitutions subst = parseSubstitutions();

    // Analyze each of the starting locations individually
    for (P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex: pathsBeginVertices_.values()) {
        // Initialize this starting point. The solver will have one initial state, plus one additional state pushed for each
        // edge of the current path.
        Sawyer::ProgressBar<size_t> progress(std::min(settings_.maxPathLength, (size_t)5000 /*arbitrary*/), mlog[MARCH], "path");
        progress.suffix(" vertices");
        SmtSolverPtr solver = createSmtSolver();
        P2::CfgPath path(pathsBeginVertex);
        Semantics sem = createSemantics(path, pathProcessor, solver);
        double effectiveMaxPathLength = settings_.maxPathLength;
        makeSubstitutions(subst, sem.ops); // so symbolic expression parsers use the latest state when expanding register and memory references.

        while (!path.isEmpty()) {
            {
                SAWYER_THREAD_TRAITS::LockGuard lock(statsMutex_);
                ++stats_.nPathsExplored;
            }
            size_t pathNInsns = pathLength(path);
            progress.value(pathNInsns);
            dfsDebugCurrentPath(debug, path, solver, effectiveMaxPathLength);
            ASSERT_require(solver->nLevels() == 1 + path.nEdges());

            // Avoid spending huge amounts of time checking a path that includes a vertex that has no
            // possibility of reaching any end point. This can happen if we're not pruning such vertices
            // from the graph after inlining functions. See similar call below.
            if (!isAnyEndpointReachable(paths_, pathsBeginVertex, pathsEndVertices_)) {
                SAWYER_MESG(debug) <<"    none of the end vertices are reachable along this path\n";
                SAWYER_MESG(debug) <<"    backtrack\n";
                backtrack(path /*in,out*/, solver);
                continue;
            }

            P2::ControlFlowGraph::ConstVertexIterator backVertex = path.backVertex();
            P2::ControlFlowGraph::ConstVertexIterator cfgBackVertex = pathToCfg(backVertex); // invalid if backVertex is a function summary
            bool doBacktrack = false;
            bool atEndOfPath = pathsEndVertices_.exists(backVertex);
            if (settings_.trackingCodeCoverage)
                markAsReached(backVertex);

            // Process the second-to-last vertex of the path to obtain a new virtual machine state, and make that state
            // the current state for the RiscOperators.
            bool pathProcessed = true;
            if (path.nVertices() >= 2)
                pathProcessed = evaluate(path, -2, sem) != NULL;

            // If a user-defined path process callback says we should backtrack, then backtrack.
            if (fpOperators(sem.ops)->breakRequested())
                doBacktrack = true;

            // Check whether this path is feasible. We've already validated the path up to but not including its final edge,
            // and we've processed instructions semantically up to the beginning of the final edge's target
            // vertex. Furthermore, the SMT solver knows all the path conditions up to but not including the final
            // edge. Therefore, we just need to push this final edge's condition into the SMT solver and check. We also add any
            // user-defined conditions that apply at the beginning of the last path vertex.
            SAWYER_MESG(debug) <<"    checking path feasibility";
            boost::logic::tribool pathIsFeasible = false;
            if (doBacktrack) {
                pathIsFeasible = false;
                SAWYER_MESG(debug) <<" = not checked (path processor requesting backtrack)\n";
            } else if (!pathProcessed) {
                pathIsFeasible = false;                 // encountered unhandled error during semantic processing
                SAWYER_MESG(debug) <<" = not feasible (semantic failure)\n";
            } else {
                pathIsFeasible = isFeasible(path, subst, sem, solver);
                if (pathIsFeasible != true)
                    doBacktrack = true;
            }

            // Mark the CFG vertex as being reachable by this analysis.
            if (pathIsFeasible && cfgBackVertex != partitioner()->cfg().vertices().end())
                markAsReached(cfgBackVertex);

            // Invoke the user-supplied path processor's "found" callback if appropriate, and after possibly evaluating the
            // final vertex state.
            if ((pathIsFeasible && atEndOfPath) ||
                (pathIsFeasible && !isDirectedSearch() && 0 == backVertex->nOutEdges())) {
                switch (callPathProcessorFound(pathProcessor, path, sem, solver)) {
                    case PathProcessor::BREAK:
                        return;
                    case PathProcessor::CONTINUE:
                        break;
                    default:
                        ASSERT_not_reachable("invalid user-defined path processor action");
                }
            }

            // Adjust the effective K, at the same time checking various length limits. The new K is zero if a limit was hit,
            // in which case we should backtrack.
            effectiveMaxPathLength = adjustEffectiveK(path, effectiveMaxPathLength);
            if (effectiveMaxPathLength <= 0.0)
                doBacktrack = true;

            // If we're visiting a function call site, then inline callee paths into the paths graph, but continue to avoid any
            // paths that go through user-specified avoidance vertices and edges. We can modify the paths graph during the
            // traversal because we're modifying parts of the graph that aren't part of the current path.  This is where having
            // insert- and erase-stable graph iterators is a huge help!
            if (!doBacktrack && pathEndsWithFunctionCall(path) && !P2::findCallReturnEdges(backVertex).empty()) {
                summarizeOrInline(path, sem);

                // If the inlined function had no "return" instructions but the call site had a call-return edge and that edge was the
                // only possible way to get from the starting vertex to an ending vertex, then that ending vertex is no longer
                // reachable.  A previous version of this code called P2::eraseUnreachablePaths, but it turned out that doing so was
                // unsafe--it might remove a vertex that is pointed to by some iterator in some variable, perhaps the current
                // path. Instead, we call isAnyEndpointReachable here and above.
                if (!isAnyEndpointReachable(paths_, pathsBeginVertex, pathsEndVertices_)) {
                    SAWYER_MESG(debug) <<"    none of the end vertices are reachable after inlining\n";
                    break;
                }

                backVertex = path.backVertex();
                cfgBackVertex = pathToCfg(backVertex);

                SAWYER_MESG(debug) <<indent <<"paths graph has " <<StringUtility::plural(paths_.nVertices(), "vertices")
                                   <<" and " <<StringUtility::plural(paths_.nEdges(), "edges") <<"\n";
                SAWYER_MESG(debug) <<"    paths graph saved in " <<emitPathGraph(callId, ++graphId) <<"\n";
            }

            // Advance to next path.
            if (doBacktrack || backVertex->nOutEdges() == 0) {
                // Backtrack and follow a different path.  The backtrack not only pops edges off the path, but then also appends
                // the next edge.  We must adjust visit counts for the vertices we backtracked.
                SAWYER_MESG_OR(trace, debug) <<"    backtrack\n";
                backtrack(path, solver);
                if (!path.isEmpty()) {
                    double d = pathEffectiveK(path);
                    if (d != effectiveMaxPathLength) {
                        SAWYER_MESG(debug) <<"      reset effective k = " <<d <<"\n";
                        effectiveMaxPathLength = d;
                    }
                }
            } else {
                // Push next edge onto path.
                SAWYER_MESG_OR(trace, debug) <<"    advance along cfg edge "
                                             <<partitioner()->edgeName(backVertex->outEdges().begin()) <<"\n";
                ASSERT_require(paths_.isValidEdge(backVertex->outEdges().begin()));
                typedef P2::ControlFlowGraph::ConstEdgeIterator CEI;
                std::vector<CEI> outEdges;
                for (CEI edge = backVertex->outEdges().begin(); edge != backVertex->outEdges().end(); ++edge)
                    outEdges.push_back(edge);
                switch (settings_.edgeVisitOrder) {
                    case VISIT_NATURAL:
                        break;
                    case VISIT_REVERSE:
                        std::reverse(outEdges.begin(), outEdges.end());
                        break;
                    case VISIT_RANDOM:
                        Combinatorics::shuffle(outEdges);
                        break;
                }
                path.pushBack(outEdges);
                solver->push();
            }
        }
    }
    SAWYER_MESG_OR(trace, debug) <<"  path search completed\n";
}

const FeasiblePath::FunctionSummary&
FeasiblePath::functionSummary(rose_addr_t entryVa) const {
    return functionSummaries_.getOrDefault(entryVa);
}

const FeasiblePath::VarDetail&
FeasiblePath::varDetail(const BaseSemantics::State::Ptr &state, const std::string &varName) const {
    return State::promote(state)->varDetail(varName);
}

const FeasiblePath::VarDetails&
FeasiblePath::varDetails(const BaseSemantics::State::Ptr &state) const {
    return State::promote(state)->varDetails();
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
            ss <<" " <<firstAccessInsn->toString();
    }
    if (memAddress)
        ss <<" mem[" <<*memAddress <<"]";
    if (memSize > 0)
        ss <<" byte " <<memByteNumber <<" of " <<memSize;
    if (returnFrom)
        ss <<" return from function " <<StringUtility::addrToString(*returnFrom);

    return boost::trim_copy(ss.str());
}

void
FeasiblePath::checkSettings() const {
    if (settings().maxVertexVisit <= 0)
        throw Exception("setting \"maxVertexVisit\" must be positive");
    if (settings().maxPathLength <= 0)
        throw Exception("setting \"maxPathLength\" must be positive");
    if (settings().maxCallDepth <= 0)
        throw Exception("setting \"maxCallDepth\" must be positive");
    if (settings().maxRecursionDepth <= 0)
        throw Exception("setting \"maxRecursionDepth\" must be positive");
    if (settings().ipRewrite.size() % 2 != 0)
        throw Exception("settingss \"ipRewrite\" must have an even number of members");
}

} // namespace
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream &out, const Rose::BinaryAnalysis::FeasiblePath::Expression &expr) {
    expr.print(out);
    return out;
}

#endif
