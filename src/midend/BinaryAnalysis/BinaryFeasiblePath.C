#include <sage3basic.h>
#include <AsmUnparser_compat.h>
#include <BaseSemantics2.h>
#include <BinaryFeasiblePath.h>
#include <BinarySymbolicExprParser.h>
#include <BinaryYicesSolver.h>
#include <Combinatorics.h>
#include <CommandLine.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/GraphAlgorithm.h>
#include <SymbolicMemory2.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
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
    // Maps symbolic variable names to additional information about where the variable appears in the path.
    FeasiblePath::VarDetails varDetails_;

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
    size_t pathInsnIndex_;                              // current location in path, or -1
    const P2::Partitioner *partitioner_;
    FeasiblePath *fpAnalyzer_;
    FeasiblePath::PathProcessor *pathProcessor_;
    SmtSolver::Ptr nullPtrSolver_;
    const P2::CfgPath *path_;

protected:
    RiscOperators(const P2::Partitioner *partitioner, const BaseSemantics::SValuePtr &protoval,
                  const Rose::BinaryAnalysis::SmtSolverPtr &solver)
        : Super(protoval, solver), pathInsnIndex_(-1), partitioner_(partitioner), fpAnalyzer_(NULL),
          pathProcessor_(NULL), path_(NULL){
        name("FindPath");
    }

    RiscOperators(const P2::Partitioner *partitioner, const BaseSemantics::StatePtr &state,
                  const Rose::BinaryAnalysis::SmtSolverPtr &solver)
        : Super(state, solver), pathInsnIndex_(-1), partitioner_(partitioner), fpAnalyzer_(NULL), pathProcessor_(NULL),
          path_(NULL) {
        name("FindPath");
    }

public:
    static RiscOperatorsPtr instance(const P2::Partitioner *partitioner, const RegisterDictionary *regdict,
                                     FeasiblePath *fpAnalyzer, const P2::CfgPath *path, FeasiblePath::PathProcessor *pathProcessor,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) {
        ASSERT_not_null(fpAnalyzer);
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory;
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
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        RiscOperatorsPtr ops = RiscOperatorsPtr(new RiscOperators(partitioner, state, solver));
        ops->fpAnalyzer_ = fpAnalyzer;
        ops->pathProcessor_ = pathProcessor;
        ops->path_ = path;
        return ops;
    }

    static RiscOperatorsPtr instance(const P2::Partitioner *partitioner, const BaseSemantics::SValuePtr &protoval,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, protoval, solver));
    }

    static RiscOperatorsPtr instance(const P2::Partitioner *partitioner, const BaseSemantics::StatePtr &state,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, state, solver));
    }

public:
    virtual BaseSemantics::RiscOperatorsPtr
    create(const BaseSemantics::SValuePtr &protoval,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(NULL, protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr
    create(const BaseSemantics::StatePtr &state,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(NULL, state, solver);
    }

public:
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
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

    void partitioner(const P2::Partitioner *p) {
        partitioner_ = p;
    }

private:
    /** Description a variable stored in a register. */
    FeasiblePath::VarDetail detailForVariable(RegisterDescriptor reg, const std::string &accessMode) const {
        const RegisterDictionary *regs = currentState()->registerState()->get_register_dictionary();
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
    FeasiblePath::VarDetail detailForVariable(const BaseSemantics::SValuePtr &addr,
                                              const std::string &accessMode, size_t byteNumber=0, size_t nBytes=0) const {
        FeasiblePath::VarDetail retval;
        retval.firstAccessMode = accessMode;
        if (pathInsnIndex_ != INVALID_INDEX)
            retval.firstAccessIdx = pathInsnIndex_;
        retval.firstAccessInsn = currentInstruction();

        // Sometimes we can save useful information about the address.
        if (nBytes != 1) {
            SymbolicExpr::Ptr addrExpr = SValue::promote(addr)->get_expression();
            if (SymbolicExpr::LeafPtr addrLeaf = addrExpr->isLeafNode()) {
                if (addrLeaf->isIntegerConstant()) {
                    retval.memSize = nBytes;
                    retval.memByteNumber = byteNumber;
                    retval.memAddress = addrExpr;
                }
            } else if (SymbolicExpr::InteriorPtr addrINode = addrExpr->isInteriorNode()) {
                if (addrINode->getOperator() == SymbolicExpr::OP_ADD && addrINode->nChildren() == 2 &&
                    addrINode->child(0)->isLeafNode() && addrINode->child(0)->isLeafNode()->isIntegerVariable() &&
                    addrINode->child(1)->isLeafNode() && addrINode->child(1)->isLeafNode()->isIntegerConstant()) {
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

    // True if the expression contains only constants.
    bool isConstExpr(const SymbolicExpr::Ptr &expr) {
        ASSERT_not_null(expr);

        struct VarFinder: SymbolicExpr::Visitor {
            bool hasVariable;

            VarFinder(): hasVariable(false) {}

            SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) ROSE_OVERRIDE {
                if (node->isLeafNode() && !node->isLeafNode()->isIntegerConstant()) {
                    hasVariable = true;
                    return SymbolicExpr::TERMINATE;
                } else {
                    return SymbolicExpr::CONTINUE;
                }
            }

            SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr &node) ROSE_OVERRIDE {
                return SymbolicExpr::CONTINUE;
            }
        } varFinder;
        expr->depthFirstTraversal(varFinder);
        return !varFinder.hasVariable;
    }

    // Warning: Caller should create a nullPtrSolver transaction if necessary
    bool isNullDeref(const BaseSemantics::SValuePtr &addr) {
        SymbolicExpr::Ptr expr = SymbolicSemantics::SValue::promote(addr)->get_expression();
        return isNullDeref(expr);
    }

    // Warning: Caller should create a nullPtrSolver transaction if necessary
    bool isNullDeref(const SymbolicExpr::Ptr &expr) {
        ASSERT_not_null(expr);
        Sawyer::Message::Stream debug(fpAnalyzer_->mlog[DEBUG]);
        SAWYER_MESG(debug) <<"          isNullDeref addr = " <<*expr <<"\n";
        SmtSolver::Ptr solver = nullPtrSolver();

        // Instead of using SymbolicExpr::mustEqual and SymbolicExpr::mayEqual, we always call the SMT solver. This is so that
        // if a null pointer is found, the callback will be able to get the evidence from the solver. The problem with
        // mustEqual and mayEqual is threefold: (1) these functions can sometimes make the determination themselves without
        // invoking an SMT solver thus the solver would not be updated with evidence, (2) these functions might invoke a user-defined
        // callback that does almost anything, (3) when these functions invoke an SMT solver they protect it with a transaction so
        // the evidence would be gone by time we get control back.
        bool retval = false;
        switch (fpAnalyzer_->settings().nullDeref.mode) {
            case FeasiblePath::MAY:
                if (solver) {
                    SymbolicExpr::Ptr assertion = SymbolicExpr::makeEq(expr, SymbolicExpr::makeIntegerConstant(expr->nBits(), 0));
                    solver->insert(assertion);
                    retval = SmtSolver::SAT_YES == solver->check();
                } else {
                    retval = expr->mayEqual(SymbolicExpr::makeIntegerConstant(expr->nBits(), 0));
                }
                break;

            case FeasiblePath::MUST:
                // Check for constness of the address expression first since that doesn't involve an SMT solver and we might
                // be able to return quickly.
                if (!fpAnalyzer_->settings().nullDeref.constOnly ||  isConstExpr(expr)) {
                    if (solver) {
                        SymbolicExpr::Ptr assertion = SymbolicExpr::makeNe(expr, SymbolicExpr::makeIntegerConstant(expr->nBits(), 0));
                        solver->insert(assertion);
                        retval = SmtSolver::SAT_NO == solver->check();
                    } else {
                        retval = expr->mustEqual(SymbolicExpr::makeIntegerConstant(expr->nBits(), 0));
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
    virtual void startInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE {
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

    virtual void finishInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE {
        if (mlog[DEBUG]) {
            SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
            mlog[DEBUG] <<"    state after instruction:\n" <<(*currentState()+fmt);
        }
        Super::finishInstruction(insn);
    }

    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg,
                 const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE {
        SValuePtr retval = SValue::promote(Super::readRegister(reg, dflt));
        SymbolicExpr::Ptr expr = retval->get_expression();
        if (expr->isLeafNode())
            State::promote(currentState())->varDetail(expr->isLeafNode()->toString(), detailForVariable(reg, "read"));
        return retval;
    }

    virtual void writeRegister(RegisterDescriptor reg,
                  const BaseSemantics::SValuePtr &value) ROSE_OVERRIDE {
        SymbolicExpr::Ptr expr = SValue::promote(value)->get_expression();
        if (expr->isLeafNode())
            State::promote(currentState())->varDetail(expr->isLeafNode()->toString(), detailForVariable(reg, "write"));
        Super::writeRegister(reg, value);
    }

    // If multi-path is enabled, then return a new memory expression that describes the process of reading a value from the
    // specified address; otherwise, actually read the value and return it.  In any case, record some information about the
    // address that's being read if we've never seen it before.
    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt_,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        BaseSemantics::SValuePtr dflt = dflt_;
        const size_t nBytes = dflt->get_width() / 8;
        if (cond->is_number() && !cond->get_number())
            return dflt_;

        // Check for null pointer dereferences
        if (fpAnalyzer_->settings().nullDeref.check && pathProcessor_) {
            SmtSolver::Transaction transaction(nullPtrSolver());
            if (isNullDeref(addr)) {
                ASSERT_not_null(fpAnalyzer_);
                ASSERT_not_null(path_);
                BaseSemantics::RiscOperatorsPtr cpu = shared_from_this();
                ASSERT_not_null(cpu);
                pathProcessor_->nullDeref(*fpAnalyzer_, *path_, currentInstruction(), cpu, nullPtrSolver(),
                                          FeasiblePath::READ, addr);
            }
        }
        
        // If we know the address and that memory exists, then read the memory to obtain the default value.
        uint8_t buf[8];
        if (addr->is_number() && nBytes < sizeof(buf) &&
            nBytes == partitioner_->memoryMap()->at(addr->get_number()).limit(nBytes).read(buf).size()) {
            switch (partitioner_->memoryMap()->byteOrder()) {
                case ByteOrder::ORDER_UNSPECIFIED:
                case ByteOrder::ORDER_LSB: {
                    uint64_t value = 0;
                    for (size_t i=0; i<nBytes; ++i)
                        value |= (uint64_t)buf[i] << (8*i);
                    dflt = number_(dflt->get_width(), value);
                    break;
                }

                case ByteOrder::ORDER_MSB: {
                    uint64_t value = 0;
                    for (size_t i=0; i<nBytes; ++i)
                        value = (value << 8) | (uint64_t)buf[i];
                    dflt = number_(dflt->get_width(), value);
                    break;
                }
            }
        }

        // Read from the symbolic state, and update the state with the default from real memory if known.
        BaseSemantics::SValuePtr retval = Super::readMemory(segreg, addr, dflt, cond);

        if (!currentInstruction())
            return retval;                              // not called from dispatcher on behalf of an instruction

        // Save a description of the variable
        SymbolicExpr::Ptr valExpr = SValue::promote(retval)->get_expression();
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->isIntegerVariable())
            State::promote(currentState())->varDetail(valExpr->isLeafNode()->toString(), detailForVariable(addr, "read"));

        // Save a description for its addresses
        for (size_t i=0; i<nBytes; ++i) {
            SValuePtr va = SValue::promote(add(addr, number_(addr->get_width(), i)));
            if (va->get_expression()->isLeafNode()) {
                State::promote(currentState())->varDetail(va->get_expression()->isLeafNode()->toString(),
                                                          detailForVariable(addr, "read", i, nBytes));
            }
        }

        // Callback for the memory access
        if (pathProcessor_) {
            ASSERT_not_null(fpAnalyzer_);
            ASSERT_not_null(path_);
            SmtSolver::Ptr s = nullPtrSolver();
            SmtSolver::Transaction tx(s);
            BaseSemantics::RiscOperatorsPtr cpu = shared_from_this();
            ASSERT_not_null(cpu);
            pathProcessor_->memoryIo(*fpAnalyzer_, *path_, currentInstruction(), cpu, s, FeasiblePath::READ, addr, retval);
        }

        return retval;
    }

    // If multi-path is enabled, then return a new memory expression that updates memory with a new address/value pair;
    // otherwise update the memory directly.  In any case, record some information about the address that was written if we've
    // never seen it before.
    virtual void writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &value, const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        if (cond->is_number() && !cond->get_number())
            return;
        Super::writeMemory(segreg, addr, value, cond);

        // Check for null pointer dereferences
        if (fpAnalyzer_->settings().nullDeref.check && pathProcessor_) {
            SmtSolver::Transaction transaction(nullPtrSolver());
            if (isNullDeref(addr)) {
                ASSERT_not_null(fpAnalyzer_);
                ASSERT_not_null(path_);
                BaseSemantics::RiscOperatorsPtr cpu = shared_from_this();
                ASSERT_not_null(cpu);
                pathProcessor_->nullDeref(*fpAnalyzer_, *path_, currentInstruction(), cpu, nullPtrSolver(),
                                          FeasiblePath::WRITE, addr);
            }
        }

        // Save a description of the variable
        SymbolicExpr::Ptr valExpr = SValue::promote(value)->get_expression();
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->isIntegerVariable())
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

        // Callback for the memory access
        if (pathProcessor_) {
            ASSERT_not_null(fpAnalyzer_);
            ASSERT_not_null(path_);
            SmtSolver::Ptr s = nullPtrSolver();
            SmtSolver::Transaction tx(s);
            BaseSemantics::RiscOperatorsPtr cpu = shared_from_this();
            ASSERT_not_null(cpu);
            pathProcessor_->memoryIo(*fpAnalyzer_, *path_, currentInstruction(), cpu, s, FeasiblePath::WRITE, addr, value);
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
Sawyer::Attribute::Id FeasiblePath::POST_STATE(-1);
Sawyer::Attribute::Id FeasiblePath::POST_INSN_LENGTH(-1);
Sawyer::Attribute::Id FeasiblePath::EFFECTIVE_K(-1);

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
        mlog.comment("model checking and path feasibility");
        POST_STATE = Sawyer::Attribute::declare("post-execution semantic state");
        POST_INSN_LENGTH = Sawyer::Attribute::declare("post-execution path length (size_t)");
        EFFECTIVE_K = Sawyer::Attribute::declare("effective maximum path length (double)");
    }
}

// class method
std::string
FeasiblePath::expressionDocumentation() {
    return SymbolicExprParser().docString() +
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

    sg.insert(Switch("cycle-k")
              .argument("coefficent", realNumberParser(settings.kCycleCoefficient))
              .doc("When the algorithm encounters a vertex which has already been visited by the current path, then the "
                   "effective k value is adjusted. The amount of adjustment is the size of the vertex (e.g., number of "
                   "instructions) multiplied by the @v{coefficient} specified here. Both positive and negative coefficients "
                   "are permitted. The default is " + boost::lexical_cast<std::string>(settings.kCycleCoefficient) + "."));

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
                    "global SMT solver. Since an SMT solver is required for model checking, in the absense of any specified "
                    "solver the \"best\" solver is used.  The default solver is \"" + settings.solverName + "\"."));

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

BaseSemantics::DispatcherPtr
FeasiblePath::buildVirtualCpu(const P2::Partitioner &partitioner, const P2::CfgPath *path, PathProcessor *pathProcessor,
                              const SmtSolver::Ptr &solver) {
    // Augment the register dictionary with a "path" register that holds the expression describing how the location is
    // reachable along some path.
    if (NULL == registers_) {
        registers_ = new RegisterDictionary("Rose::BinaryAnalysis::FeasiblePath");
        registers_->insert(partitioner.instructionProvider().registerDictionary());
        ASSERT_require(REG_PATH.isEmpty());
        REG_PATH = RegisterDescriptor(registers_->firstUnusedMajor(), 0, 0, 1);
        registers_->insert("path", REG_PATH);

        // Where are return values stored?  FIXME[Robb Matzke 2015-12-01]: We need to support returning multiple values. We
        // should be using the new calling convention analysis to detect these.
        ASSERT_require(REG_RETURN_.isEmpty());
        const RegisterDescriptor *r = NULL;
        if ((r = registers_->lookup("rax")) || (r = registers_->lookup("eax")) || (r = registers_->lookup("ax"))) {
            REG_RETURN_ = *r;
        } else if ((r = registers_->lookup("d0"))) {
            REG_RETURN_ = *r;                           // m68k also typically has other return registers
        } else if ((r = registers_->lookup("r3"))) {
            REG_RETURN_ = *r;                           // PowerPC also returns via r4
        } else {
            ASSERT_not_implemented("function return value register is not implemented for this ISA/ABI");
        }
    }

    // Create the RiscOperators and Dispatcher.
    RiscOperatorsPtr ops = RiscOperators::instance(&partitioner, registers_, this, path, pathProcessor);
    ops->initialState(ops->currentState()->clone());
    ops->nullPtrSolver(solver);
    for (size_t i = 0; i < settings().ipRewrite.size(); i += 2) {
        const RegisterDescriptor REG_IP = partitioner.instructionProvider().instructionPointerRegister();
        BaseSemantics::SValuePtr oldValue = ops->number_(REG_IP.nBits(), settings().ipRewrite[i+0]);
        BaseSemantics::SValuePtr newValue = ops->number_(REG_IP.nBits(), settings().ipRewrite[i+1]);
        SAWYER_MESG(mlog[DEBUG]) <<"ip-rewrite hot-patch from " <<*oldValue <<" to " <<*newValue <<"\n";
        ops->hotPatch().append(HotPatch::Record(REG_IP, oldValue, newValue, HotPatch::Record::MATCH_BREAK));
    }

    ASSERT_not_null(partitioner.instructionProvider().dispatcher());
    BaseSemantics::DispatcherPtr cpu = partitioner.instructionProvider().dispatcher()->create(ops);
    ASSERT_not_null(cpu);
    return cpu;
}

void
FeasiblePath::setInitialState(const BaseSemantics::DispatcherPtr &cpu,
                              const P2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex) {
    ASSERT_not_null(cpu);
    ASSERT_forbid(REG_PATH.isEmpty());
    checkSettings();

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
    if (const RegisterDescriptor *REG_DF = cpu->get_register_dictionary()->lookup("df"))
        ops->writeRegister(*REG_DF, ops->boolean_(true));
}

/** Process instructions for one basic block on the specified virtual CPU. */
void
FeasiblePath::processBasicBlock(const P2::BasicBlock::Ptr &bblock, const BaseSemantics::DispatcherPtr &cpu,
                                size_t pathInsnIndex) {
    ASSERT_not_null(bblock);
    checkSettings();
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"      processing basic block " <<bblock->printableName() <<"\n";

    // Update the path constraint "register"
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    const RegisterDescriptor IP = cpu->instructionPointerRegister();
    BaseSemantics::SValuePtr ip = ops->readRegister(IP, ops->undefined_(IP.nBits()));
    BaseSemantics::SValuePtr va = ops->number_(ip->get_width(), bblock->address());
    BaseSemantics::SValuePtr pathConstraint = ops->isEqual(ip, va);
    ops->writeRegister(REG_PATH, pathConstraint);

    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("          ");
        debug <<"        state before block:\n" <<(*ops->currentState() + fmt);
    }

    // Process each instruction in the basic block
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
        try {
            if (pathInsnIndex != size_t(-1)) {
                SAWYER_MESG(debug) <<"        processing path insn #" <<pathInsnIndex <<" at " <<insn->toString() <<"\n";
                ops->pathInsnIndex(pathInsnIndex++);
            } else {
                SAWYER_MESG(debug) <<"        processing path insn at " <<insn->toString() <<"\n";
            }
            cpu->processInstruction(insn);
            if (debug) {
                // Show stack pointer
                const RegisterDescriptor SP = cpu->stackPointerRegister();
                BaseSemantics::SValuePtr sp = ops->readRegister(SP, ops->undefined_(SP.nBits()));
                debug <<"          sp = " <<*sp <<"\n";
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
FeasiblePath::processIndeterminateBlock(const P2::ControlFlowGraph::ConstVertexIterator &vertex,
                                        const BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex) {
    SAWYER_MESG(mlog[DEBUG]) <<"      processing indeterminate vertex\n";
    mlog[WARN] <<"control flow passes through an indeterminate address at path position #" <<pathInsnIndex <<"\n";
}

/** Process a function summary vertex. */
void
FeasiblePath::processFunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                                     const BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    ASSERT_require(functionSummaries_.exists(pathsVertex->value().address()));
    const FunctionSummary &summary = functionSummaries_[pathsVertex->value().address()];
    SAWYER_MESG(debug) <<"      processing function summary " <<summary.name <<"\n";

    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    if (pathInsnIndex != size_t(-1))
        ops->pathInsnIndex(pathInsnIndex);

    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
        debug <<"summary semantics for " <<summary.name <<"\n";
        debug <<"  +-------------------------------------------------\n"
              <<"  | " <<summary.name <<"\n"
              <<"  +-------------------------------------------------\n"
              <<"    state before summarized function:\n"
              <<(*ops->currentState() + fmt);
    }

    SymbolicSemantics::SValuePtr retval;
    if (functionSummarizer_ && functionSummarizer_->process(*this, summary, ops)) {
        retval = functionSummarizer_->returnValue(*this, summary, ops);
    } else {
        // Make the function return an unknown value
        retval = SymbolicSemantics::SValue::promote(ops->undefined_(REG_RETURN_.nBits()));
        ops->writeRegister(REG_RETURN_, retval);

        // Simulate function returning to caller
        if (boost::dynamic_pointer_cast<InstructionSemantics2::DispatcherPowerpc>(cpu)) {
            // PowerPC calling convention stores the return address in the link register (LR)
            const RegisterDescriptor LR = cpu->callReturnRegister();
            ASSERT_forbid(LR.isEmpty());
            BaseSemantics::SValuePtr returnTarget = ops->readRegister(LR, ops->undefined_(LR.nBits()));
            ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);

        } else if (boost::dynamic_pointer_cast<InstructionSemantics2::DispatcherX86>(cpu)) {
            // x86 and amd64 store the return address at the top of the stack
            const RegisterDescriptor SP = cpu->stackPointerRegister();
            ASSERT_forbid(SP.isEmpty());
            BaseSemantics::SValuePtr stackPointer = ops->readRegister(SP, ops->undefined_(SP.nBits()));
            BaseSemantics::SValuePtr returnTarget = ops->readMemory(RegisterDescriptor(), stackPointer,
                                                                    ops->undefined_(stackPointer->get_width()),
                                                                    ops->boolean_(true));
            ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);

            // Pop some things from the stack.
            int64_t sd = summary.stackDelta != SgAsmInstruction::INVALID_STACK_DELTA ?
                         summary.stackDelta :
                         returnTarget->get_width() / 8;
            stackPointer = ops->add(stackPointer, ops->number_(stackPointer->get_width(), sd));
            ops->writeRegister(cpu->stackPointerRegister(), stackPointer);
        }
    }

    if (retval) {
        VarDetail varDetail;
        varDetail.returnFrom = summary.address;
        varDetail.firstAccessIdx = ops->pathInsnIndex();
        ops->varDetail(retval->get_expression()->isLeafNode()->toString(), varDetail);
    }

    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
        debug <<"    state after summarized function:\n";
        debug <<(*ops->currentState() + fmt);
    }
}

void
FeasiblePath::processVertex(const BaseSemantics::DispatcherPtr &cpu,
                            const P2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
                            size_t &pathInsnIndex /*in,out*/) {
    checkSettings();
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
                                                cpu->get_operators()->number_(cpu->instructionPointerRegister().nBits(),
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

SymbolicExpr::Ptr
FeasiblePath::pathEdgeConstraint(const P2::ControlFlowGraph::ConstEdgeIterator &pathEdge, BaseSemantics::DispatcherPtr &cpu) {
    ASSERT_not_null(cpu);
    BaseSemantics::RiscOperatorsPtr ops = cpu->get_operators();
    static const char *prefix = "      ";

    const RegisterDescriptor IP = partitioner().instructionProvider().instructionPointerRegister();
    BaseSemantics::SValuePtr ip = ops->peekRegister(IP, ops->undefined_(IP.nBits()));
    if (!settings_.nonAddressIsFeasible && !hasVirtualAddress(pathEdge->target())) {
        SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner().edgeName(pathEdge)
                                 <<" because settings().nonAddressIsFeasible is false\n";
        return SymbolicExpr::Ptr();                     // trivially unfeasible
    } else if (ip->is_number()) {
        if (!hasVirtualAddress(pathEdge->target())) {
            // If the IP register is pointing to an instruction but the path vertex is indeterminate (or undiscovered or
            // nonexisting) then consider this path to be not-feasible. If the CFG is accurate then there's probably
            // a sibling edge that points to the correct vertex.
            SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner().edgeName(pathEdge) <<" because IP = "
                                     <<StringUtility::addrToString(ip->get_number()) <<" and edge target has no address\n";
            return SymbolicExpr::Ptr();                 // trivially unfeasible
        } else if (ip->get_number() != virtualAddress(pathEdge->target())) {
            // Executing the path forces us to go a different direction than where the path indicates we should go. We
            // don't need an SMT solver to tell us that when the values are just integers.
            SAWYER_MESG(mlog[DEBUG]) <<prefix <<"unfeasible at edge " <<partitioner().edgeName(pathEdge) <<" because IP = "
                                     <<StringUtility::addrToString(ip->get_number()) <<" and edge target is "
                                     <<StringUtility::addrToString(virtualAddress(pathEdge->target())) <<"\n";
            return SymbolicExpr::Ptr();                 // trivially unfeasible
        }
    } else if (hasVirtualAddress(pathEdge->target())) {
        SymbolicExpr::Ptr targetVa = SymbolicExpr::makeIntegerConstant(ip->get_width(), virtualAddress(pathEdge->target()));
        SymbolicExpr::Ptr constraint = SymbolicExpr::makeEq(targetVa,
                                                            SymbolicSemantics::SValue::promote(ip)->get_expression());
        constraint->comment("cfg edge " + partitioner().edgeName(pathEdge));
        SAWYER_MESG(mlog[DEBUG]) <<prefix <<"constraint at edge " <<partitioner().edgeName(pathEdge)
                                 <<": " <<*constraint <<"\n";
        return constraint;
    }

    SAWYER_MESG(mlog[DEBUG]) <<prefix <<"trivially feasible at edge " <<partitioner().edgeName(pathEdge) <<"\n";
    return SymbolicExpr::makeBooleanConstant(true);     // trivially feasible
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
FeasiblePath::cfgToPaths(const P2::CfgConstVertexSet &vertexSet) const {
    P2::CfgConstVertexSet retval;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, vertexSet.values()) {
        if (Sawyer::Optional<P2::ControlFlowGraph::ConstVertexIterator> found = vmap_.forward().find(vertex))
            retval.insert(*found);
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
    cfgEndAvoidVertices_.insert(cfgEndVertices);
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
        if ((size_t)callDepth >= settings_.maxRecursionDepth)
            return false;
    }

    // Don't inline imported functions that aren't linked -- we'd just get bogged down deep inside the
    // dynamic linker without ever being able to resolve the actual function's instructions.
    if (P2::ModulesElf::isUnlinkedImport(*partitioner_, callee))
        return false;

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
    P2::CfgConstEdgeSet callReturnEdges = P2::findCallReturnEdges(pathsCallSite);
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &callret, callReturnEdges.values())
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
    sprintf(callIdStr, "invoc-%04zu", callId);
    sprintf(graphIdStr, "cfg-%06zu.dot", graphId);
    boost::filesystem::path debugDir = boost::filesystem::path("rose-debug/BinaryAnalysis/FeasiblePath") / callIdStr;
    boost::filesystem::create_directories(debugDir);
    boost::filesystem::path pathGraphName = debugDir / graphIdStr;

    std::ofstream file(pathGraphName.string().c_str());
    P2::GraphViz::CfgEmitter emitter(partitioner(), paths_);
    emitter.showInstructions(true);
    emitter.selectWholeGraph();

    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, pathsBeginVertices_.values()) {
        emitter.vertexOrganization(v).attributes().insert("style", "filled");
        emitter.vertexOrganization(v).attributes().insert("fillcolor", "#faff7d");
    }
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, pathsEndVertices_.values()) {
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
    if (endVertices.empty())
        return false;
    typedef Sawyer::Container::Algorithm::DepthFirstForwardVertexTraversal<const P2::ControlFlowGraph> Traversal;
    for (Traversal t(cfg, beginVertex); t; ++t) {
        if (endVertices.exists(t.vertex()))
            return true;
    }
    return false;
}

BaseSemantics::StatePtr
FeasiblePath::pathPostState(const P2::CfgPath &path, size_t vertexIdx) {
    return path.vertexAttributes(vertexIdx).attributeOrDefault<BaseSemantics::StatePtr>(POST_STATE);
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
FeasiblePath::parseExpression(Expression expr, const std::string &where, SymbolicExprParser &exprParser) const {
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
            BOOST_FOREACH (const P2::Function::Ptr &function, partitioner_->functions()) {
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

SymbolicExpr::Ptr
FeasiblePath::expandExpression(const Expression &expr, SymbolicExprParser &parser) {
    if (expr.expr) {
        return parser.delayedExpansion(expr.expr);
    } else if (!expr.parsable.empty()) {
        ASSERT_not_reachable("string should have been parsed by now");
    } else {
        return SymbolicExpr::makeBooleanConstant(false);
    }
}

void
FeasiblePath::insertAssertions(const SmtSolver::Ptr &solver, const P2::CfgPath &path,
                               const std::vector<Expression> &assertions, bool atEndOfPath,
                               SymbolicExprParser &parser) {
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
                SymbolicExpr::Ptr assertion = expandExpression(assertions[i], parser);
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
FeasiblePath::solvePathConstraints(SmtSolver::Ptr &solver, const P2::CfgPath &path, const SymbolicExpr::Ptr &edgeAssertion,
                                   const std::vector<Expression> &assertions, bool atEndOfPath, SymbolicExprParser &parser) {
    ASSERT_not_null(solver);
    ASSERT_not_null(edgeAssertion);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    solver->insert(edgeAssertion);
    insertAssertions(solver, path, assertions, atEndOfPath, parser);
    return solver->check();
}

size_t
FeasiblePath::vertexSize(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    switch (vertex->value().type()) {
        case P2::V_BASIC_BLOCK:
            return vertex->value().bblock()->nInstructions();
        case P2::V_INDETERMINATE:
        case P2::V_USER_DEFINED:
            return 1;
        default:
            ASSERT_not_reachable("invalid path vertex type");
    }
}

size_t
FeasiblePath::pathLength(const P2::CfgPath &path) {
    size_t retval = 0;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, path.vertices())
        retval += vertexSize(vertex);
    return retval;
}

const FeasiblePath::AddressSet&
FeasiblePath::reachedBlockVas() const {
    return reachedBlockVas_;
}

void
FeasiblePath::markAsReached(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    if (Sawyer::Optional<rose_addr_t> addr = vertex->value().optionalAddress())
        reachedBlockVas_.insert(*addr);
}

void
FeasiblePath::depthFirstSearch(PathProcessor &pathProcessor) {
    ASSERT_not_null(partitioner_);
    static size_t callId = 0;                           // number of calls to this function
    size_t graphId = 0;                                 // incremented each time the graph is modified
    reachedBlockVas_.clear();
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
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, pathsBeginVertices_.values())
            debug <<"  begin at vertex " <<partitioner().vertexName(v) <<"\n";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, pathsEndVertices_.values())
            debug <<"  end   at vertex " <<partitioner().vertexName(v) <<"\n";
    }

    const RegisterDescriptor IP = partitioner().instructionProvider().instructionPointerRegister();

    // Parse user-supplied assertions and their locations. Register and memory references are replaced by temporary variables
    // that will be expanded at a later time in order to obtain the then-current values of registers and memory.
    std::vector<Expression> assertions;
    SymbolicExprParser exprParser;
    SymbolicExprParser::RegisterSubstituter::Ptr regSubber =
        exprParser.defineRegisters(partitioner().instructionProvider().registerDictionary());
    SymbolicExprParser::MemorySubstituter::Ptr memSubber =
        SymbolicExprParser::MemorySubstituter::instance(SmtSolver::Ptr());
    exprParser.appendOperatorExpansion(memSubber);
    ASSERT_require(settings_.assertions.size() == settings_.assertionLocations.size());
    for (size_t i = 0; i < settings_.assertions.size(); ++i)
        assertions.push_back(parseExpression(settings_.assertions[i], settings_.assertionLocations[i], exprParser));

    // Analyze each of the starting locations individually
    BOOST_FOREACH (P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex, pathsBeginVertices_.values()) {
        // Create the SMT solver.  The solver will have one initial state, plus one additional state pushed for each edge of
        // the current path.
        SmtSolverPtr solver = SmtSolver::instance(settings_.solverName);
        ASSERT_always_not_null(solver);
        solver->errorIfReset(true);
        solver->name("FeasiblePath " + solver->name());
#if 1 // DEBUGGING [Robb Matzke 2018-11-14]
        solver->memoization(false);
#endif

        P2::CfgPath path(pathsBeginVertex);
        BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner(), &path, &pathProcessor, solver);
        ASSERT_not_null(cpu);
        setInitialState(cpu, pathsBeginVertex);
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
        ASSERT_not_null(ops);
        BaseSemantics::StatePtr originalState = ops->currentState();
        ASSERT_not_null(originalState);
        double effectiveMaxPathLength = settings_.maxPathLength;

        // Make sure symbolic expression parsers use the latest state when expanding register and memory references.
        regSubber->riscOperators(ops);
        memSubber->riscOperators(ops);

        while (!path.isEmpty()) {
            size_t pathNInsns = pathLength(path);

            if (debug) {
                debug <<"  path vertices (" <<path.nVertices() <<"):";
                BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &v, path.vertices())
                    debug <<" " <<partitioner().vertexName(v);
                debug <<"\n";
                debug <<"    SMT solver has " <<StringUtility::plural(solver->nLevels(), "transactions") <<"\n";
                debug <<"    path has " <<StringUtility::plural(path.nVertices(), "vertices") <<"\n";
                debug <<"    path length is " <<StringUtility::plural(pathNInsns, "instructions") <<"\n";
                debug <<"    effective k is " <<effectiveMaxPathLength <<" instructions\n";
            }
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

            // If backVertex is a function summary, then there is no corresponding cfgBackVertex.
            P2::ControlFlowGraph::ConstVertexIterator backVertex = path.backVertex();
            P2::ControlFlowGraph::ConstVertexIterator cfgBackVertex = pathToCfg(backVertex);
            if (settings_.trackingCodeCoverage)
                markAsReached(backVertex);

            bool doBacktrack = false;
            bool atEndOfPath = pathsEndVertices_.exists(backVertex);

            // Process the second-to-last vertex of the path to obtain a new virtual machine state, and make that state
            // the RiscOperators current state.
            BaseSemantics::StatePtr penultimateState;
            size_t pathInsnIndex = 0;
            bool pathProcessed = false;                 // true if path semantic processing is successful, false if failed.
            if (path.nEdges() > 0) {
                BaseSemantics::StatePtr state;
                if (path.nVertices() >= 3) {
                    state = pathPostState(path, path.nVertices()-3);
                    pathInsnIndex = path.vertexAttributes(path.nVertices()-3).getAttribute<size_t>(POST_INSN_LENGTH);
                } else {
                    state = originalState;
                    pathInsnIndex = 0;
                }
                penultimateState = state->clone();
                ops->currentState(penultimateState);
                try {
                    processVertex(cpu, path.edges().back()->source(), pathInsnIndex /*in,out*/);
                    pathProcessed = true;
                } catch (...) {
                    SAWYER_MESG(debug) <<"    path semantics failed\n";
                }
                path.vertexAttributes(path.nVertices()-2).setAttribute(POST_STATE, penultimateState);
                path.vertexAttributes(path.nVertices()-2).setAttribute(POST_INSN_LENGTH, pathInsnIndex);
            } else {
                ops->currentState(originalState);
                pathProcessed = true;
            }

            // Check whether this path is feasible. We've already validated the path up to but not including its final edge,
            // and we've processed instructions semantically up to the beginning of the final edge's target vertex (the
            // CPU points to this state).  Furthermore, the SMT solver knows all the path conditions up to but not including
            // the final edge. Therefore, we just need to push this final edge's condition into the SMT solver and check. We
            // also add any user-defined conditions that apply at the beginning of the last path vertex.
            SAWYER_MESG(debug) <<"    checking path feasibility";
            boost::logic::tribool pathIsFeasible = false;
            if (!pathProcessed) {
                pathIsFeasible = false;                 // encountered unhandled error during semantic processing
                SAWYER_MESG(debug) <<" = not feasible (semantic failure)\n";
            } else if (path.nEdges() == 0) {
                ASSERT_require(path.nVertices() == 1);
                ASSERT_require(solver->nLevels() == 1);
                switch (solvePathConstraints(solver, path, SymbolicExpr::makeBooleanConstant(true), assertions, atEndOfPath,
                                             exprParser)) {
                    case SmtSolver::SAT_YES:
                        SAWYER_MESG(debug) <<" = is feasible\n";
                        pathIsFeasible = true;
                        break;
                    case SmtSolver::SAT_NO:
                        SAWYER_MESG(debug) <<" = not feasible\n";
                        pathIsFeasible = false;
                        doBacktrack = true;
                        break;
                    case SmtSolver::SAT_UNKNOWN:
                        SAWYER_MESG(debug) <<" = unknown\n";
                        pathIsFeasible = boost::logic::indeterminate;
                        doBacktrack = true;
                        break;
                }
            } else {
                ASSERT_require(solver->nLevels() == 1 + path.nEdges());
                if (solver->nAssertions(solver->nLevels()-1) > 0) {
                    SAWYER_MESG(debug) <<" = is feasible (previously computed)\n";
                    pathIsFeasible = true;
                } else if (SymbolicExpr::Ptr edgeConstraint = pathEdgeConstraint(path.edges().back(), cpu)) {
                    switch (solvePathConstraints(solver, path, edgeConstraint, assertions, atEndOfPath, exprParser)) {
                        case SmtSolver::SAT_YES:
                            SAWYER_MESG(debug) <<" = is feasible\n";
                            pathIsFeasible = true;
                            break;
                        case SmtSolver::SAT_NO:
                            SAWYER_MESG(debug) <<" = not feasible\n";
                            pathIsFeasible = false;
                            doBacktrack = true;
                            break;
                        case SmtSolver::SAT_UNKNOWN:
                            SAWYER_MESG(debug) <<" = unknown\n";
                            pathIsFeasible = boost::logic::indeterminate;
                            doBacktrack = true;
                            break;
                    }
                } else {
                    SAWYER_MESG(debug) <<" = not feasible (trivial)\n";
                    pathIsFeasible = false;
                    doBacktrack = true;
                }
            }

            // Call user-supplied path processor when appropriate
            if (atEndOfPath && pathIsFeasible) {
                // Process final vertex semantics before invoking user callback?
                if (settings().processFinalVertex) {
                    SAWYER_MESG(debug) <<"    reached end of path; processing final path vertex\n";
                    BaseSemantics::StatePtr saved = cpu->currentState();
                    cpu->get_operators()->currentState(saved->clone());
                    processVertex(cpu, path.backVertex(), pathInsnIndex /*in,out*/);
                }

                SAWYER_MESG(debug) <<"    feasible end of path found; calling user-defined processor\n";
                switch (pathProcessor.found(*this, path, cpu, solver)) {
                    case PathProcessor::BREAK:
                        return;
                    case PathProcessor::CONTINUE:
                        break;
                    default:
                        ASSERT_not_reachable("invalid user-defined path processor action");
                }
            }

            // If we've visited a vertex too many times (e.g., because of a loop or recursion), then don't go any further.
            size_t nVertexVisits = path.nVisits(backVertex);
            if (nVertexVisits > settings_.maxVertexVisit) {
                SAWYER_MESG(mlog[WARN]) <<indent <<"max visits (" <<settings_.maxVertexVisit <<") reached"
                                        <<" for vertex " <<partitioner().vertexName(backVertex) <<"\n";
                doBacktrack = true;
            } else if (nVertexVisits > 1 && !rose_isnan(settings_.kCycleCoefficient)) {
                size_t n = vertexSize(backVertex);
                double increment = n * settings_.kCycleCoefficient;
                if (increment != 0.0) {
                    effectiveMaxPathLength += increment;
                    SAWYER_MESG(debug) <<"    revisting prior vertex; k += " <<increment
                                       <<", effective k = " <<effectiveMaxPathLength <<"\n";
                    path.vertexAttributes(path.nVertices()-1).setAttribute(EFFECTIVE_K, effectiveMaxPathLength);
                }
            }

            // Limit path length (in terms of number of instructions)
            if (!doBacktrack) {
                if ((double)pathNInsns > effectiveMaxPathLength) {
                    SAWYER_MESG(mlog[WARN]) <<indent <<"maximum path length exceeded:"
                                            <<" path length is " <<StringUtility::plural(pathNInsns, "instructions")
                                            <<", effective limit is " <<effectiveMaxPathLength
                                            <<" at vertex " <<partitioner().vertexName(backVertex) <<"\n";
                    doBacktrack = true;
                }
            }

            // If we're visiting a function call site, then inline callee paths into the paths graph, but continue to avoid any
            // paths that go through user-specified avoidance vertices and edges. We can modify the paths graph during the
            // traversal because we're modifying parts of the graph that aren't part of the current path.  This is where having
            // insert- and erase-stable graph iterators is a huge help!
            if (!doBacktrack && pathEndsWithFunctionCall(path) && !P2::findCallReturnEdges(backVertex).empty()) {
                ASSERT_require(partitioner().cfg().isValidVertex(cfgBackVertex));
                P2::CfgConstEdgeSet callEdges = P2::findCallEdges(cfgBackVertex);
                BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, callEdges.values()) {
                    if (shouldSummarizeCall(path.backVertex(), partitioner().cfg(), cfgCallEdge->target())) {
                        info <<indent <<"summarizing function for edge " <<partitioner().edgeName(cfgCallEdge) <<"\n";
                        insertCallSummary(backVertex, partitioner().cfg(), cfgCallEdge);
                    } else if (shouldInline(path, cfgCallEdge->target())) {
                        info <<indent <<"inlining function call paths at vertex " <<partitioner().vertexName(backVertex) <<"\n";
                        if (cfgCallEdge->target()->value().type() == P2::V_INDETERMINATE &&
                            cfgBackVertex->value().type() == P2::V_BASIC_BLOCK) {
                            // If the CFG has a vertex to an indeterminate function (e.g., from "call eax"), then instead of
                            // inlining the indeterminate vertex, see if we can inline an actual function by using the
                            // instruction pointer register. The cpu's currentState is the one at the beginning of the final
                            // vertex of the path; we need the state at the end of the final vertex.
                            BaseSemantics::StatePtr savedState = cpu->get_operators()->currentState()->clone();
                            BaseSemantics::SValuePtr ip;
                            try {
                                BOOST_FOREACH (SgAsmInstruction *insn, cfgBackVertex->value().bblock()->instructions())
                                    cpu->processInstruction(insn);
                                ip = cpu->currentState()->peekRegister(IP, cpu->undefined_(IP.nBits()),
                                                                       cpu->get_operators().get());
                            } catch (const BaseSemantics::Exception &e) {
                                mlog[ERROR] <<"semantics failed when trying to determine call target address: " <<e <<"\n";
                            }
                            cpu->get_operators()->currentState(savedState);
                            if (ip && ip->is_number() && ip->get_width() <= 64) {
                                rose_addr_t targetVa = ip->get_number();
                                P2::ControlFlowGraph::ConstVertexIterator targetVertex = partitioner().findPlaceholder(targetVa);
                                if (partitioner().cfg().isValidVertex(targetVertex))
                                    P2::inlineOneCallee(paths_, backVertex, partitioner().cfg(),
                                                        targetVertex, cfgEndAvoidVertices_, cfgAvoidEdges_);
                            }
                        } else {
                            P2::inlineMultipleCallees(paths_, backVertex, partitioner().cfg(),
                                                      cfgBackVertex, cfgEndAvoidVertices_, cfgAvoidEdges_);
                        }
                    } else {
                        info <<indent <<"summarizing function for edge " <<partitioner().edgeName(cfgCallEdge) <<"\n";
                        insertCallSummary(backVertex, partitioner().cfg(), cfgCallEdge);
                    }
                }

                // Remove all call-return edges. This is necessary so we don't re-enter this case with infinite recursion. No
                // need to worry about adjusting the path because these edges aren't on the current path.
                P2::eraseEdges(paths_, P2::findCallReturnEdges(backVertex));

                // If the inlined function had no "return" instructions but the call site had a call-return edge and that edge
                // was the only possible way to get from the starting vertex to an ending vertex, then that ending vertex is no
                // longer reachable.  A previous version of this code called P2::eraseUnreachablePaths, but it turned out that
                // doing so was unsafe--it might remove a vertex that is pointed to by some iterator in some variable, perhaps
                // the current path. Instead, we call isAnyEndpointReachable here and above.
                if (!isAnyEndpointReachable(paths_, pathsBeginVertex, pathsEndVertices_)) {
                    SAWYER_MESG(debug) <<"    none of the end vertices are reachable after inlining\n";
                    break;
                }

                backVertex = path.backVertex();
                cfgBackVertex = pathToCfg(backVertex);

                info <<indent <<"paths graph has " <<StringUtility::plural(paths_.nVertices(), "vertices", "vertex")
                     <<" and " <<StringUtility::plural(paths_.nEdges(), "edges") <<"\n";
                SAWYER_MESG(debug) <<"    paths graph saved in " <<emitPathGraph(callId, ++graphId) <<"\n";
            }


            // Advance to next path.
            if (doBacktrack || backVertex->nOutEdges() == 0) {
                // Backtrack and follow a different path.  The backtrack not only pops edges off the path, but then also appends
                // the next edge.  We must adjust visit counts for the vertices we backtracked.
                SAWYER_MESG(debug) <<"    backtrack\n";
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
                SAWYER_MESG(debug) <<"    advance along cfg edge " <<partitioner().edgeName(backVertex->outEdges().begin()) <<"\n";
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

const FeasiblePath::VarDetails&
FeasiblePath::varDetails(const BaseSemantics::StatePtr &state) const {
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
