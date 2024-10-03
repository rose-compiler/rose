#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/StackDelta.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/CommandLine.h>
#include <integerOps.h>                                 // rose

#include <SgAsmBlock.h>
#include <SgAsmFunction.h>
#include <SgAsmInstruction.h>

#include <Cxx_GrammarDowncast.h>

#include <Sawyer/ProgressBar.h>

namespace Rose {
namespace BinaryAnalysis {
namespace StackDelta {

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;


// These are initialized by calling Rose::initialize
Sawyer::Message::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace-level functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Called from Rose::Diagnostics::initialize (which in turn is called from Rose::initialize).
void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::StackDelta");
        mlog.comment("analyzing stack pointer behavior");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Analysis functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Analysis::init(const Disassembler::Base::Ptr &disassembler) {
    if (disassembler) {
        RegisterDictionary::Ptr regdict = disassembler->architecture()->registerDictionary();
        ASSERT_not_null(regdict);

        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        BaseSemantics::SValue::Ptr protoval = SymbolicSemantics::SValue::instance();
        BaseSemantics::RegisterState::Ptr registers = SymbolicSemantics::RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryState::Ptr memory = NullSemantics::MemoryState::instance(protoval, protoval);
        BaseSemantics::State::Ptr state = SymbolicSemantics::State::instance(registers, memory);
        BaseSemantics::RiscOperators::Ptr ops = SymbolicSemantics::RiscOperators::instanceFromState(state, solver);

        cpu_ = disassembler->architecture()->newInstructionDispatcher(ops);
    }
}

void
Analysis::clearResults() {
    hasResults_ = didConverge_ = false;
    clearStackDeltas();
    clearStackPointers();
    clearFramePointers();
}

void
Analysis::clearStackDeltas() {
    functionDelta_ = BaseSemantics::SValue::Ptr();
    bblockDeltas_.clear();
    insnSpDeltas_.clear();
}

void
Analysis::clearStackPointers() {
    functionStackPtrs_ = SValuePair();
    bblockStackPtrs_.clear();
    insnStackPtrs_.clear();
}

void
Analysis::clearFramePointers() {
    insnFramePtrs_.clear();
}

void
Analysis::clearNonResults() {
    cpu_ = BaseSemantics::Dispatcher::Ptr();
}


// Augment the base data-flow transfer function because we need to keep track of the stack for every instruction and basic
// block.
class TransferFunction: public P2::DataFlow::TransferFunction {
    Analysis *analysis_;
public:
    explicit TransferFunction(Analysis *analysis)
        : P2::DataFlow::TransferFunction(analysis->cpu()), analysis_(analysis) {}

    // Override the base class by initializing only the stack pointer register.
    BaseSemantics::State::Ptr initialState() const {
        BaseSemantics::RiscOperators::Ptr ops = cpu()->operators();
        BaseSemantics::State::Ptr newState = ops->currentState()->clone();
        newState->clear();
        cpu()->initializeState(newState);

        BaseSemantics::RegisterStateGeneric::Ptr regState =
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
    BaseSemantics::State::Ptr
    operator()(const P2::DataFlow::DfCfg &dfCfg, size_t vertexId, const BaseSemantics::State::Ptr &incomingState) const {
        P2::DataFlow::DfCfg::ConstVertexIterator vertex = dfCfg.findVertex(vertexId);
        ASSERT_require(dfCfg.isValidVertex(vertex));
        if (P2::DataFlow::DfCfgVertex::BBLOCK == vertex->value().type()) {
            BaseSemantics::State::Ptr retval = incomingState->clone();
            BaseSemantics::RiscOperators::Ptr ops = analysis_->cpu()->operators();
            ops->currentState(retval);
            ASSERT_not_null(vertex->value().bblock());
            const RegisterDescriptor SP = cpu()->stackPointerRegister();
            const RegisterDescriptor FP = cpu()->stackFrameRegister(); // not all architectures have this
            BaseSemantics::SValue::Ptr oldSp = retval->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
            BaseSemantics::SValue::Ptr oldFp = FP ?
                                               retval->peekRegister(FP, ops->undefined_(FP.nBits()), ops.get()) :
                                               BaseSemantics::SValue::Ptr();
            for (SgAsmInstruction *insn: vertex->value().bblock()->instructions()) {
                cpu()->processInstruction(insn);

                // Stack and frame pointers after the instruction executes
                BaseSemantics::SValue::Ptr newSp = retval->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
                BaseSemantics::SValue::Ptr newFp = FP ?
                                                   retval->peekRegister(FP, ops->undefined_(FP.nBits()), ops.get()) :
                                                   BaseSemantics::SValue::Ptr();

                // Stack delta is the net change to the stack pointer by this instruction.
                BaseSemantics::SValue::Ptr spDelta = ops->subtract(newSp, oldSp);

                // Save and update loop variables
                analysis_->adjustInstruction(insn, oldSp, newSp, spDelta, oldFp, newFp);
                oldSp = newSp;
                oldFp = newFp;
            }
            return retval;
        }

        return P2::DataFlow::TransferFunction::operator()(dfCfg, vertexId, incomingState);
    }
};

void
Analysis::analyzeFunction(const P2::Partitioner::ConstPtr &partitioner, const P2::FunctionPtr &function,
                          Partitioner2::DataFlow::InterproceduralPredicate &ipPredicate) {
    ASSERT_not_null(partitioner);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"analyzeFunction(" <<function->printableName() <<")\n";
    clearResults();
    const RegisterDescriptor SP = cpu_->stackPointerRegister();

    // Build the CFG used by the data-flow: dfCfg. The dfCfg includes only those vertices that are reachable from the entry
    // point for the function we're analyzing and which belong to that function.  All return points in the function will flow
    // into a special CALLRET vertex (which is absent if there are no returns).
    typedef P2::DataFlow::DfCfg DfCfg;
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner->cfg(), partitioner->findPlaceholder(function->address()),
                                           ipPredicate);
    size_t startVertexId = 0;
    DfCfg::ConstVertexIterator returnVertex = dfCfg.vertices().end();
    for (const DfCfg::Vertex &vertex: dfCfg.vertices()) {
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
    typedef DataFlow::Engine<DfCfg, BaseSemantics::State::Ptr, TransferFunction, DataFlow::SemanticsMerge> DfEngine;
    if (!cpu_ && NULL==(cpu_ = partitioner->newDispatcher(partitioner->newOperators()))) {
        SAWYER_MESG(debug) <<"  no instruction semantics\n";
        return;
    }
    const CallingConvention::Dictionary &ccDefs = partitioner->instructionProvider().callingConventions();
    P2::DataFlow::MergeFunction merge(cpu_);
    TransferFunction xfer(this);
    xfer.defaultCallingConvention(ccDefs.empty() ? CallingConvention::Definition::Ptr() : ccDefs.front());
    DfEngine dfEngine(dfCfg, xfer, merge);
    dfEngine.name("stack-delta");
    size_t maxIterations = dfCfg.nVertices() * 5;       // arbitrary
    dfEngine.maxIterations(maxIterations);
    BaseSemantics::RiscOperators::Ptr ops = cpu_->operators();

    // Build the initial state
    BaseSemantics::State::Ptr initialState = xfer.initialState();
    BaseSemantics::RegisterStateGeneric::Ptr initialRegState =
        BaseSemantics::RegisterStateGeneric::promote(initialState->registerState());

    // Run data flow analysis
    bool converged = true;
    try {
        // Use this rather than runToFixedPoint because it lets us show a progress report
        Sawyer::ProgressBar<size_t> progress(maxIterations, mlog[MARCH], function->printableName());
        progress.suffix(" iterations");
        dfEngine.reset(BaseSemantics::State::Ptr());
        dfEngine.insertStartingVertex(startVertexId, initialState);
        while (dfEngine.runOneIteration())
            ++progress;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    } catch (const BaseSemantics::NotImplemented &e) {
        mlog[WHERE] <<e.what() <<" for " <<function->printableName() <<"\n";
    } catch (const BaseSemantics::Exception &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;
    } catch (const SmtSolver::Exception &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;
    }

    // Get the final dataflow state
    BaseSemantics::State::Ptr finalState;
    BaseSemantics::RegisterStateGeneric::Ptr finalRegState;
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
    for (const DfCfg::Vertex &vertex: dfCfg.vertices()) {
        if (vertex.value().type() == P2::DataFlow::DfCfgVertex::BBLOCK) {
            P2::BasicBlock::Ptr bblock = vertex.value().bblock();
            ASSERT_not_null(bblock);
            BaseSemantics::SValue::Ptr sp0, sp1;
            if (BaseSemantics::State::Ptr state = dfEngine.getInitialState(vertex.id()))
                sp0 = state->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
            if (BaseSemantics::State::Ptr state = dfEngine.getFinalState(vertex.id()))
                sp1 = state->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
            bblockStackPtrs_.insert(bblock->address(), SValuePair(sp0, sp1));

            if (sp0 && sp1) {
                BaseSemantics::SValue::Ptr delta = ops->subtract(sp1, sp0);
                bblockDeltas_.insert(bblock->address(), delta);
            }
        }
    }

    // Functon stack delta is final stack pointer minus initial stack pointer.  This includes popping the return address from
    // the stack (if the function did that) and popping arguments (if the function did that).
    functionStackPtrs_.first = initialRegState->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
    SAWYER_MESG(debug) <<"  function initial stack pointer is " <<*functionStackPtrs_.first <<"\n";
    if (finalRegState) {
        functionStackPtrs_.second = finalRegState->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
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
    return toInt(functionStackDelta()).orElse(SgAsmInstruction::INVALID_STACK_DELTA);
}

Analysis::SValuePair
Analysis::basicBlockStackPointers(rose_addr_t basicBlockAddress) const {
    return bblockStackPtrs_.getOrDefault(basicBlockAddress);
}

BaseSemantics::SValue::Ptr
Analysis::basicBlockStackDelta(rose_addr_t basicBlockAddress) const {
    return bblockDeltas_.getOrDefault(basicBlockAddress);
}

int64_t
Analysis::basicBlockStackDeltaConcrete(rose_addr_t basicBlockAddress) const {
    return toInt(basicBlockStackDelta(basicBlockAddress)).orElse(SgAsmInstruction::INVALID_STACK_DELTA);
}

BaseSemantics::SValue::Ptr
Analysis::basicBlockInputStackDeltaWrtFunction(rose_addr_t basicBlockAddress) const {
    BaseSemantics::SValue::Ptr initialSp = functionStackPtrs_.first;
    BaseSemantics::SValue::Ptr finalSp = bblockStackPtrs_.getOrDefault(basicBlockAddress).first;
    if (NULL == initialSp || NULL == finalSp || NULL == cpu_)
        return BaseSemantics::SValue::Ptr();
    return cpu_->operators()->subtract(finalSp, initialSp);
}

BaseSemantics::SValue::Ptr
Analysis::basicBlockOutputStackDeltaWrtFunction(rose_addr_t basicBlockAddress) const {
    BaseSemantics::SValue::Ptr initialSp = functionStackPtrs_.first;
    BaseSemantics::SValue::Ptr finalSp = bblockStackPtrs_.getOrDefault(basicBlockAddress).second;
    if (NULL == initialSp || NULL == finalSp || NULL == cpu_)
        return BaseSemantics::SValue::Ptr();
    return cpu_->operators()->subtract(finalSp, initialSp);
}

Analysis::SValuePair
Analysis::instructionStackPointers(SgAsmInstruction *insn) const {
    if (insn) {
        return insnStackPtrs_.getOrDefault(insn->get_address());
    } else {
        return SValuePair();
    }
}

BaseSemantics::SValue::Ptr
Analysis::instructionStackDelta(SgAsmInstruction *insn) const {
    if (insn) {
        return insnSpDeltas_.getOrDefault(insn->get_address());
    } else {
        return BaseSemantics::SValue::Ptr();
    }
}

BaseSemantics::SValue::Ptr
Analysis::instructionInputFrameDelta(SgAsmInstruction *insn) const {
    if (insn) {
        BaseSemantics::SValue::Ptr initialSp = insnStackPtrs_.getOrDefault(insn->get_address()).first;
        BaseSemantics::SValue::Ptr initialFp = insnFramePtrs_.getOrDefault(insn->get_address()).first;
        if (initialSp && initialFp)
            return cpu_->operators()->subtract(initialFp, initialSp);
    }
    return BaseSemantics::SValue::Ptr();
}

BaseSemantics::SValue::Ptr
Analysis::instructionOutputFrameDelta(SgAsmInstruction *insn) const {
    if (insn) {
        BaseSemantics::SValue::Ptr finalSp = insnStackPtrs_.getOrDefault(insn->get_address()).second;
        BaseSemantics::SValue::Ptr finalFp = insnFramePtrs_.getOrDefault(insn->get_address()).second;
        if (finalSp && finalFp)
            return cpu_->operators()->subtract(finalFp, finalSp);
    }
    return BaseSemantics::SValue::Ptr();
}

int64_t
Analysis::instructionStackDeltaConcrete(SgAsmInstruction *insn) const {
    return toInt(instructionStackDelta(insn)).orElse(SgAsmInstruction::INVALID_STACK_DELTA);
}

BaseSemantics::SValue::Ptr
Analysis::instructionInputStackDeltaWrtFunction(SgAsmInstruction *insn) const {
    BaseSemantics::SValue::Ptr initialSp = functionStackPtrs_.first;
    BaseSemantics::SValue::Ptr finalSp = insnStackPtrs_.getOrDefault(insn->get_address()).first;
    if (NULL == initialSp || NULL == finalSp || NULL == cpu_)
        return BaseSemantics::SValue::Ptr();
    return cpu_->operators()->subtract(finalSp, initialSp);
}

BaseSemantics::SValue::Ptr
Analysis::instructionOutputStackDeltaWrtFunction(SgAsmInstruction*insn) const {
    BaseSemantics::SValue::Ptr initialSp = functionStackPtrs_.first;
    BaseSemantics::SValue::Ptr finalSp = insnStackPtrs_.getOrDefault(insn->get_address()).second;
    if (NULL == initialSp || NULL == finalSp || NULL == cpu_)
        return BaseSemantics::SValue::Ptr();
    return cpu_->operators()->subtract(finalSp, initialSp);
}

void
Analysis::saveAnalysisResults(SgAsmFunction *function) const {
    if (function) {
        clearAstStackDeltas(function);
        if (hasResults_) {
            function->set_stackDelta(functionStackDeltaConcrete());
            BaseSemantics::RiscOperators::Ptr ops = cpu_ ? cpu_->operators() : BaseSemantics::RiscOperators::Ptr();
            BaseSemantics::SValue::Ptr sp0 = functionStackPtrs_.first;
            if (sp0 && ops) {
                for (SgAsmBlock *block: AST::Traversal::findDescendantsTyped<SgAsmBlock>(function)) {
                    if (BaseSemantics::SValue::Ptr blkAbs = basicBlockStackPointers(block->get_address()).second)
                        block->set_stackDeltaOut(toInt(ops->subtract(blkAbs, sp0)).orElse(SgAsmInstruction::INVALID_STACK_DELTA));
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
    for (rose_addr_t va: bblockStackPtrs_.keys())
        bblockVas.insert(va);
    for (rose_addr_t va: bblockDeltas_.keys())
        bblockVas.insert(va);
    for (rose_addr_t va: bblockVas) {
        out <<"    Basic block " <<StringUtility::addrToString(va) <<":\n";
        if (BaseSemantics::SValue::Ptr v = basicBlockStackPointers(va).first) {
            out <<"      Initial stack pointer: " <<*v <<"\n";
        } else {
            out <<"      Initial stack pointer: none\n";
        }
        if (BaseSemantics::SValue::Ptr v = basicBlockStackPointers(va).second) {
            out <<"      Final stack pointer:   " <<*v <<"\n";
        } else {
            out <<"      Final stack pointer:   none\n";
        }
        if (BaseSemantics::SValue::Ptr v = basicBlockStackDelta(va)) {
            out <<"      Stack delta:           " <<*v <<"\n";
        } else {
            out <<"      Stack delta:           none\n";
        }
    }

    out <<"  Instruction information:\n";
    std::set<rose_addr_t> insnVas;
    for (rose_addr_t va: insnStackPtrs_.keys())
        insnVas.insert(va);
    for (rose_addr_t va: insnSpDeltas_.keys())
        insnVas.insert(va);
    for (rose_addr_t va: insnVas) {
        out <<"    Instruction " <<StringUtility::addrToString(va) <<":\n";
        if (BaseSemantics::SValue::Ptr v = insnStackPtrs_.getOrDefault(va).first) {
            out <<"      Initial stack pointer: " <<*v <<"\n";
        } else {
            out <<"      Initial stack pointer: none\n";
        }
        if (BaseSemantics::SValue::Ptr v = insnStackPtrs_.getOrDefault(va).second) {
            out <<"      Final stack pointer:   " <<*v <<"\n";
        } else {
            out <<"      Final stack pointer:   none\n";
        }
        if (BaseSemantics::SValue::Ptr v = insnSpDeltas_.getOrDefault(va)) {
            out <<"      Stack delta:           " <<*v <<"\n";
        } else {
            out <<"      Stack delta:           none\n";
        }
        if (BaseSemantics::SValue::Ptr v = insnFramePtrs_.getOrDefault(va).first) {
            out <<"      Initial frame pointer: " <<*v <<"\n";
        } else {
            out <<"      Initial frame pointer: none\n";
        }
        if (BaseSemantics::SValue::Ptr v = insnFramePtrs_.getOrDefault(va).second) {
            out <<"      Final frame pointer:   " <<*v <<"\n";
        } else {
            out <<"      Final frame pointer:   none\n";
        }
    }
}

// class method
Sawyer::Optional<int64_t>
Analysis::toInt(const BaseSemantics::SValue::Ptr &v) {
    if (v) {
        return v->toSigned();
    } else {
        return Sawyer::Nothing();
    }
}

// class method
void
Analysis::clearAstStackDeltas(SgNode *ast) {
    AST::Traversal::forwardPre<SgNode>(ast, [](SgNode *node) {
        if (SgAsmFunction *func = isSgAsmFunction(node)) {
            func->set_stackDelta(SgAsmInstruction::INVALID_STACK_DELTA);
        } else if (SgAsmBlock *blk = isSgAsmBlock(node)) {
            blk->set_stackDeltaOut(SgAsmInstruction::INVALID_STACK_DELTA);
        }
    });
}

// internal
void
Analysis::adjustInstruction(SgAsmInstruction *insn, const BaseSemantics::SValue::Ptr &spIn,
                            const BaseSemantics::SValue::Ptr &spOut, const BaseSemantics::SValue::Ptr &spDelta,
                            const BaseSemantics::SValue::Ptr &fpIn, const BaseSemantics::SValue::Ptr &fpOut) {
    if (insn) {
        if (spIn || spOut)
            insnStackPtrs_.insert(insn->get_address(), SValuePair(spIn, spOut));
        if (spDelta)
            insnSpDeltas_.insert(insn->get_address(), spDelta);
        if (fpIn || fpOut)
            insnFramePtrs_.insert(insn->get_address(), SValuePair(fpIn, fpOut));
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
