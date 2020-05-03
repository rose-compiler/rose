#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryStackDelta.h>

#include <BaseSemantics2.h>
#include <boost/foreach.hpp>
#include <CommandLine.h>
#include <Partitioner2/DataFlow.h>
#include <Partitioner2/Partitioner.h>
#include <RegisterStateGeneric.h>
#include <Sawyer/ProgressBar.h>
#include <integerOps.h>

namespace Rose {
namespace BinaryAnalysis {
namespace StackDelta {

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;


Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::StackDelta");
        mlog.comment("analyzing stack pointer behavior");
    }
}

void
Analysis::init(Disassembler *disassembler) {
    if (disassembler) {
        const RegisterDictionary *regdict = disassembler->registerDictionary();
        ASSERT_not_null(regdict);
        size_t addrWidth = disassembler->instructionPointerRegister().nBits();

        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        BaseSemantics::SValuePtr protoval = SymbolicSemantics::SValue::instance();
        BaseSemantics::RegisterStatePtr registers = SymbolicSemantics::RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = NullSemantics::MemoryState::instance(protoval, protoval);
        BaseSemantics::StatePtr state = SymbolicSemantics::State::instance(registers, memory);
        BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(state, solver);

        cpu_ = disassembler->dispatcher()->create(ops, addrWidth, regdict);
    }
}

void
Analysis::clearResults() {
    hasResults_ = didConverge_ = false;
    clearStackDeltas();
    clearStackPointers();
}

void
Analysis::clearStackDeltas() {
    functionDelta_ = BaseSemantics::SValuePtr();
    bblockDeltas_.clear();
    insnDeltas_.clear();
}

void
Analysis::clearStackPointers() {
    functionStackPtrs_ = SValuePair();
    bblockStackPtrs_.clear();
    insnStackPtrs_.clear();
}

void
Analysis::clearNonResults() {
    cpu_ = BaseSemantics::DispatcherPtr();
}


// Augment the base data-flow transfer function because we need to keep track of the stack for every instruction and basic
// block.
class TransferFunction: public P2::DataFlow::TransferFunction {
    Analysis *analysis_;
public:
    explicit TransferFunction(Analysis *analysis)
        : P2::DataFlow::TransferFunction(analysis->cpu()), analysis_(analysis) {}

    // Override the base class by initializing only the stack pointer register.
    BaseSemantics::StatePtr initialState() const {
        BaseSemantics::RiscOperatorsPtr ops = cpu()->get_operators();
        BaseSemantics::StatePtr newState = ops->currentState()->clone();
        newState->clear();
        BaseSemantics::RegisterStateGenericPtr regState =
            BaseSemantics::RegisterStateGeneric::promote(newState->registerState());

        const RegisterDescriptor SP = cpu()->stackPointerRegister();
        rose_addr_t initialSp = 0;
        if (analysis_->initialConcreteStackPointer().assignTo(initialSp)) {
            newState->writeRegister(SP, ops->number_(SP.nBits(), initialSp), ops.get());
        } else {
            newState->writeRegister(SP, ops->undefined_(SP.nBits()), ops.get());
        }
        return newState;
    }

    // Required by data-flow engine: compute next state from current state and dfCfg vertex
    BaseSemantics::StatePtr
    operator()(const P2::DataFlow::DfCfg &dfCfg, size_t vertexId, const BaseSemantics::StatePtr &incomingState) const {
        P2::DataFlow::DfCfg::ConstVertexIterator vertex = dfCfg.findVertex(vertexId);
        ASSERT_require(dfCfg.isValidVertex(vertex));
        if (P2::DataFlow::DfCfgVertex::BBLOCK == vertex->value().type()) {
            BaseSemantics::StatePtr retval = incomingState->clone();
            BaseSemantics::RiscOperatorsPtr ops = analysis_->cpu()->get_operators();
            ops->currentState(retval);
            ASSERT_not_null(vertex->value().bblock());
            RegisterDescriptor SP = cpu()->stackPointerRegister();
            BaseSemantics::SValuePtr oldSp = retval->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
            BOOST_FOREACH (SgAsmInstruction *insn, vertex->value().bblock()->instructions()) {
                cpu()->processInstruction(insn);
                BaseSemantics::SValuePtr newSp = retval->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
                BaseSemantics::SValuePtr delta = ops->subtract(newSp, oldSp);
                analysis_->adjustInstruction(insn, oldSp, newSp, delta);
                oldSp = newSp;
            }
            return retval;
        }

        return P2::DataFlow::TransferFunction::operator()(dfCfg, vertexId, incomingState);
    }
};

void
Analysis::analyzeFunction(const P2::Partitioner &partitioner, const P2::FunctionPtr &function,
                          Partitioner2::DataFlow::InterproceduralPredicate &ipPredicate) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"analyzeFunction(" <<function->printableName() <<")\n";
    clearResults();

    // Build the CFG used by the data-flow: dfCfg. The dfCfg includes only those vertices that are reachable from the entry
    // point for the function we're analyzing and which belong to that function.  All return points in the function will flow
    // into a special CALLRET vertex (which is absent if there are no returns).
    typedef P2::DataFlow::DfCfg DfCfg;
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner.cfg(), partitioner.findPlaceholder(function->address()),
                                           ipPredicate);
    size_t startVertexId = 0;
    DfCfg::ConstVertexIterator returnVertex = dfCfg.vertices().end();
    BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
        if (vertex.value().type() == P2::DataFlow::DfCfgVertex::FUNCRET) {
            returnVertex = dfCfg.findVertex(vertex.id());
            break;
        }
    }
    if (returnVertex == dfCfg.vertices().end()) {
        SAWYER_MESG(debug) <<"  function CFG has no return vertex\n";
        // continue anyway, to get stack delta info for blocks and instructions...
    }

    // Build the dataflow engine. If an instruction dispatcher is already provided then use it, otherwise create one and store
    // it in this analysis object.
    typedef DataFlow::Engine<DfCfg, BaseSemantics::StatePtr, TransferFunction, DataFlow::SemanticsMerge> DfEngine;
    if (!cpu_ && NULL==(cpu_ = partitioner.newDispatcher(partitioner.newOperators()))) {
        SAWYER_MESG(debug) <<"  no instruction semantics\n";
        return;
    }
    const CallingConvention::Dictionary &ccDefs = partitioner.instructionProvider().callingConventions();
    P2::DataFlow::MergeFunction merge(cpu_);
    TransferFunction xfer(this);
    xfer.defaultCallingConvention(ccDefs.empty() ? CallingConvention::Definition::Ptr() : ccDefs.front());
    DfEngine dfEngine(dfCfg, xfer, merge);
    size_t maxIterations = dfCfg.nVertices() * 5;       // arbitrary
    dfEngine.maxIterations(maxIterations);
    BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();

    // Build the initial state
    BaseSemantics::StatePtr initialState = xfer.initialState();
    BaseSemantics::RegisterStateGenericPtr initialRegState =
        BaseSemantics::RegisterStateGeneric::promote(initialState->registerState());

    // Run data flow analysis
    bool converged = true;
    try {
        // Use this rather than runToFixedPoint because it lets us show a progress report
        Sawyer::ProgressBar<size_t> progress(maxIterations, mlog[MARCH], function->printableName());
        progress.suffix(" iterations");
        dfEngine.reset(BaseSemantics::StatePtr());
        dfEngine.insertStartingVertex(startVertexId, initialState);
        while (dfEngine.runOneIteration())
            ++progress;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    } catch (const BaseSemantics::Exception &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;
    } catch (const SmtSolver::Exception &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;
    }

    // Get the final dataflow state
    BaseSemantics::StatePtr finalState;
    BaseSemantics::RegisterStateGenericPtr finalRegState;
    if (dfCfg.isValidVertex(returnVertex)) {
        finalState = dfEngine.getInitialState(returnVertex->id());
        if (finalState == NULL) {
            SAWYER_MESG(debug) <<"  data flow analysis did not reach final state\n";
            // continue anyway for stack delta info for blocks and instructions
        }
        if (debug) {
            if (!converged) {
                debug <<"  data flow analysis did not converge to a solution (using partial solution)\n";
            } else if (finalState) {
                SymbolicSemantics::Formatter fmt;
                fmt.set_line_prefix("    ");
                fmt.expr_formatter.max_depth = 10;          // prevent really long output
                debug <<"  final state:\n" <<(*finalState+fmt);
            }
        }
        if (finalState)
            finalRegState = BaseSemantics::RegisterStateGeneric::promote(finalState->registerState());
    }

    // Get stack pointers for each basic block
    BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
        if (vertex.value().type() == P2::DataFlow::DfCfgVertex::BBLOCK) {
            P2::BasicBlock::Ptr bblock = vertex.value().bblock();
            ASSERT_not_null(bblock);
            BaseSemantics::SValuePtr sp0, sp1;
            RegisterDescriptor SP = cpu_->stackPointerRegister();
            if (BaseSemantics::StatePtr state = dfEngine.getInitialState(vertex.id()))
                sp0 = state->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
            if (BaseSemantics::StatePtr state = dfEngine.getFinalState(vertex.id()))
                sp1 = state->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
            bblockStackPtrs_.insert(bblock->address(), SValuePair(sp0, sp1));

            if (sp0 && sp1) {
                BaseSemantics::SValuePtr delta = ops->subtract(sp1, sp0);
                bblockDeltas_.insert(bblock->address(), delta);
            }
        }
    }

    // Functon stack delta is final stack pointer minus initial stack pointer.  This includes popping the return address from
    // the stack (if the function did that) and popping arguments (if the function did that).
    const RegisterDescriptor REG_SP = cpu_->stackPointerRegister();
    functionStackPtrs_.first = initialRegState->peekRegister(REG_SP, ops->undefined_(REG_SP.nBits()), ops.get());
    SAWYER_MESG(debug) <<"  function initial stack pointer is " <<*functionStackPtrs_.first <<"\n";
    if (finalRegState) {
        functionStackPtrs_.second = finalRegState->peekRegister(REG_SP, ops->undefined_(REG_SP.nBits()), ops.get());
        functionDelta_ = ops->subtract(functionStackPtrs_.second, functionStackPtrs_.first);
        SAWYER_MESG(debug) <<"  function final stack pointer is " <<*functionStackPtrs_.second <<"\n";
        SAWYER_MESG(debug) <<"  function stack delta is " <<*functionDelta_ <<"\n";
    } else {
        SAWYER_MESG(debug) <<"  no final state, thus no stack delta\n";
    }

    hasResults_ = true;
    didConverge_ = converged;
}

int64_t
Analysis::functionStackDeltaConcrete() const {
    return toInt(functionStackDelta());
}

Analysis::SValuePair
Analysis::basicBlockStackPointers(rose_addr_t basicBlockAddress) const {
    return bblockStackPtrs_.getOrDefault(basicBlockAddress);
}

BaseSemantics::SValuePtr
Analysis::basicBlockStackDelta(rose_addr_t basicBlockAddress) const {
    return bblockDeltas_.getOrDefault(basicBlockAddress);
}

int64_t
Analysis::basicBlockStackDeltaConcrete(rose_addr_t basicBlockAddress) const {
    return toInt(basicBlockStackDelta(basicBlockAddress));
}

BaseSemantics::SValuePtr
Analysis::basicBlockInputStackDeltaWrtFunction(rose_addr_t basicBlockAddress) const {
    BaseSemantics::SValuePtr initialSp = functionStackPtrs_.first;
    BaseSemantics::SValuePtr finalSp = bblockStackPtrs_.getOrDefault(basicBlockAddress).first;
    if (NULL == initialSp || NULL == finalSp || NULL == cpu_)
        return BaseSemantics::SValuePtr();
    return cpu_->get_operators()->subtract(finalSp, initialSp);
}

BaseSemantics::SValuePtr
Analysis::basicBlockOutputStackDeltaWrtFunction(rose_addr_t basicBlockAddress) const {
    BaseSemantics::SValuePtr initialSp = functionStackPtrs_.first;
    BaseSemantics::SValuePtr finalSp = bblockStackPtrs_.getOrDefault(basicBlockAddress).second;
    if (NULL == initialSp || NULL == finalSp || NULL == cpu_)
        return BaseSemantics::SValuePtr();
    return cpu_->get_operators()->subtract(finalSp, initialSp);
}

Analysis::SValuePair
Analysis::instructionStackPointers(SgAsmInstruction *insn) const {
    if (NULL == insn)
        return SValuePair();
    return insnStackPtrs_.getOrDefault(insn->get_address());
}

BaseSemantics::SValuePtr
Analysis::instructionStackDelta(SgAsmInstruction *insn) const {
    if (NULL == insn)
        return BaseSemantics::SValuePtr();
    return insnDeltas_.getOrDefault(insn->get_address());
}

int64_t
Analysis::instructionStackDeltaConcrete(SgAsmInstruction *insn) const {
    return toInt(instructionStackDelta(insn));
}

BaseSemantics::SValuePtr
Analysis::instructionInputStackDeltaWrtFunction(SgAsmInstruction *insn) const {
    BaseSemantics::SValuePtr initialSp = functionStackPtrs_.first;
    BaseSemantics::SValuePtr finalSp = insnStackPtrs_.getOrDefault(insn->get_address()).first;
    if (NULL == initialSp || NULL == finalSp || NULL == cpu_)
        return BaseSemantics::SValuePtr();
    return cpu_->get_operators()->subtract(finalSp, initialSp);
}

BaseSemantics::SValuePtr
Analysis::instructionOutputStackDeltaWrtFunction(SgAsmInstruction*insn) const {
    BaseSemantics::SValuePtr initialSp = functionStackPtrs_.first;
    BaseSemantics::SValuePtr finalSp = insnStackPtrs_.getOrDefault(insn->get_address()).second;
    if (NULL == initialSp || NULL == finalSp || NULL == cpu_)
        return BaseSemantics::SValuePtr();
    return cpu_->get_operators()->subtract(finalSp, initialSp);
}

void
Analysis::saveAnalysisResults(SgAsmFunction *function) const {
    if (function) {
        clearAstStackDeltas(function);
        if (hasResults_) {
            function->set_stackDelta(functionStackDeltaConcrete());
            BaseSemantics::RiscOperatorsPtr ops = cpu_ ? cpu_->get_operators() : BaseSemantics::RiscOperatorsPtr();
            BaseSemantics::SValuePtr sp0 = functionStackPtrs_.first;
            if (sp0 && ops) {
                BOOST_FOREACH (SgAsmBlock *block, SageInterface::querySubTree<SgAsmBlock>(function)) {
                    if (BaseSemantics::SValuePtr blkAbs = basicBlockStackPointers(block->get_address()).second) {
                        block->set_stackDeltaOut(toInt(ops->subtract(blkAbs, sp0)));

                        BOOST_FOREACH (SgAsmInstruction *insn, SageInterface::querySubTree<SgAsmInstruction>(block)) {
                            if (BaseSemantics::SValuePtr insnAbs = instructionStackPointers(insn).first)
                                insn->set_stackDeltaIn(toInt(ops->subtract(insnAbs, sp0)));
                        }
                    }
                }
            }
        }
    }
}

void
Analysis::print(std::ostream &out) const {
    out <<"StackDelta analysis results:\n";
    if (!hasResults()) {
        out <<"  Analysis has not been run yet, or has been reset.\n";
        return;
    }
    if (!didConverge())
        out <<"  WARNING: Analysis did not converge; following results may be incorrect.\n";

    out <<"  Function information:\n";
    if (functionStackPtrs_.first) {
        out <<"    Initial stack pointer: " <<*functionStackPtrs_.first <<"\n";
    } else {
        out <<"    Initial stack pointer: none\n";
    }
    if (functionStackPtrs_.second) {
        out <<"    Final stack pointer:   " <<*functionStackPtrs_.second <<"\n";
    } else {
        out <<"    Final stack pointer:   none\n";
    }
    if (functionDelta_) {
        out <<"    Stack delta:           " <<*functionDelta_ <<"\n";
    } else {
        out <<"    Stack delta:           none\n";
    }

    out <<"  Basic block information:\n";
    std::set<rose_addr_t> bblockVas;
    BOOST_FOREACH (rose_addr_t va, bblockStackPtrs_.keys())
        bblockVas.insert(va);
    BOOST_FOREACH (rose_addr_t va, bblockDeltas_.keys())
        bblockVas.insert(va);
    BOOST_FOREACH (rose_addr_t va, bblockVas) {
        out <<"    Basic block " <<StringUtility::addrToString(va) <<":\n";
        if (BaseSemantics::SValuePtr v = basicBlockStackPointers(va).first) {
            out <<"      Initial stack pointer: " <<*v <<"\n";
        } else {
            out <<"      Initial stack pointer: none\n";
        }
        if (BaseSemantics::SValuePtr v = basicBlockStackPointers(va).second) {
            out <<"      Final stack pointer:   " <<*v <<"\n";
        } else {
            out <<"      Final stack pointer:   none\n";
        }
        if (BaseSemantics::SValuePtr v = basicBlockStackDelta(va)) {
            out <<"      Stack delta:           " <<*v <<"\n";
        } else {
            out <<"      Stack delta:           none\n";
        }
    }

    out <<"  Instruction information:\n";
    std::set<rose_addr_t> insnVas;
    BOOST_FOREACH (rose_addr_t va, insnStackPtrs_.keys())
        insnVas.insert(va);
    BOOST_FOREACH (rose_addr_t va, insnDeltas_.keys())
        insnVas.insert(va);
    BOOST_FOREACH (rose_addr_t va, insnVas) {
        out <<"    Instruction " <<StringUtility::addrToString(va) <<":\n";
        if (BaseSemantics::SValuePtr v = insnStackPtrs_.getOrDefault(va).first) {
            out <<"      Initial stack pointer: " <<*v <<"\n";
        } else {
            out <<"      Initial stack pointer: none\n";
        }
        if (BaseSemantics::SValuePtr v = insnStackPtrs_.getOrDefault(va).second) {
            out <<"      Final stack pointer:   " <<*v <<"\n";
        } else {
            out <<"      Final stack pointer:   none\n";
        }
        if (BaseSemantics::SValuePtr v = insnDeltas_.getOrDefault(va)) {
            out <<"      Stack delta:           " <<*v <<"\n";
        } else {
            out <<"      Stack delta:           none\n";
        }
    }
}

// class method
int64_t
Analysis::toInt(const BaseSemantics::SValuePtr &v) {
    if (v && v->is_number() && v->get_width() <= 64)
        return IntegerOps::signExtend2<uint64_t>(v->get_number(), v->get_width(), 64);
    return SgAsmInstruction::INVALID_STACK_DELTA;
}

// class method
void
Analysis::clearAstStackDeltas(SgNode *ast) {
    struct T1: AstSimpleProcessing {
        void visit(SgNode *node) {
            if (SgAsmFunction *func = isSgAsmFunction(node)) {
                func->set_stackDelta(SgAsmInstruction::INVALID_STACK_DELTA);
            } else if (SgAsmBlock *blk = isSgAsmBlock(node)) {
                blk->set_stackDeltaOut(SgAsmInstruction::INVALID_STACK_DELTA);
            } else if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
                insn->set_stackDeltaIn(SgAsmInstruction::INVALID_STACK_DELTA);
            }
        }
    };
    T1().traverse(ast, preorder);
}

// internal
void
Analysis::adjustInstruction(SgAsmInstruction *insn, const BaseSemantics::SValuePtr &spIn,
                            const BaseSemantics::SValuePtr &spOut, const BaseSemantics::SValuePtr &delta) {
    if (insn) {
        insnStackPtrs_.insert(insn->get_address(), SValuePair(spIn, spOut));
        insnDeltas_.insert(insn->get_address(), delta);
    }
}

std::ostream& operator<<(std::ostream &out, const Analysis &x) {
    x.print(out);
    return out;
}

} // namespace
} // namespace
} // namespace

#endif
