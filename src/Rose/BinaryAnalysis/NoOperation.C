#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/NoOperation.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <SgAsmInstruction.h>

namespace Rose {
namespace BinaryAnalysis {

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis::InstructionSemantics;

Sawyer::Message::Facility NoOperation::mlog;

void
NoOperation::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::NoOperation");
        mlog.comment("determining insn sequences having no effect");
    }
}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StateNormalizer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSemantics::State::Ptr
NoOperation::StateNormalizer::initialState(const BaseSemantics::Dispatcher::Ptr &cpu, SgAsmInstruction *insn) {
    ASSERT_not_null(cpu);

    BaseSemantics::State::Ptr state = cpu->currentState()->clone();
    state->clear();

    BaseSemantics::RegisterStateGeneric::Ptr rstate = BaseSemantics::RegisterStateGeneric::promote(state->registerState());
    if (rstate)
        rstate->initialize_large();

    cpu->initializeState(state);

    RegisterDescriptor IP = cpu->instructionPointerRegister();
    state->writeRegister(IP, cpu->number_(IP.nBits(), insn->get_address()), cpu->operators().get());

    return state;
}

class CellErasurePredicate: public BaseSemantics::MemoryCell::Predicate {
    bool ignorePoppedMemory;
    BaseSemantics::RiscOperators::Ptr ops;
    BaseSemantics::SValue::Ptr stackCurVa;
    BaseSemantics::SValue::Ptr stackMinVa;
    
public:
    CellErasurePredicate(const BaseSemantics::RiscOperators::Ptr &ops, const BaseSemantics::SValue::Ptr &stackCurVa,
                         rose_addr_t closeness)
        : ignorePoppedMemory(closeness!=0), ops(ops), stackCurVa(stackCurVa) {
        stackMinVa = ops->subtract(stackCurVa, ops->number_(stackCurVa->nBits(), closeness));
    }

    virtual bool operator()(const BaseSemantics::MemoryCell::Ptr &cell) override {
        if (cell->getWriters().isEmpty())
            return true;
        
        // Erase memory that is above (lower address) and near the current stack pointer.
        if (ignorePoppedMemory) {
            BaseSemantics::SValue::Ptr isPopped =     // assume downward-growing stack
                ops->and_(ops->isUnsignedLessThan(cell->address(), stackCurVa),
                          ops->isUnsignedGreaterThanOrEqual(cell->address(), stackMinVa));
            return isPopped->isTrue();
        }

        return false;
    }
};

std::string
NoOperation::StateNormalizer::toString(const BaseSemantics::Dispatcher::Ptr &cpu, const BaseSemantics::State::Ptr &state_) {
    BaseSemantics::State::Ptr state = state_;
    BaseSemantics::RiscOperators::Ptr ops = cpu->operators();
    if (!state)
        return "";
    bool isCloned = false;                              // do we have our own copy of the state?

    // If possible and appropriate, remove the instruction pointer register
    const RegisterDescriptor regIp = cpu->instructionPointerRegister();
    BaseSemantics::RegisterStateGeneric::Ptr rstate = BaseSemantics::RegisterStateGeneric::promote(state->registerState());
    if (rstate && rstate->is_partly_stored(regIp)) {
        BaseSemantics::SValue::Ptr ip = ops->peekRegister(cpu->instructionPointerRegister());
        if (ip->isConcrete()) {
            state = state->clone();
            isCloned = true;
            rstate = BaseSemantics::RegisterStateGeneric::promote(state->registerState());
            rstate->erase_register(regIp, ops.get());
        }
    }

    // Get the memory state, cloning the state if not done so above.
    BaseSemantics::MemoryCellState::Ptr mem =
        boost::dynamic_pointer_cast<BaseSemantics::MemoryCellState>(state->memoryState());
    if (mem && !isCloned) {
        state = state->clone();
        isCloned = true;
        mem = BaseSemantics::MemoryCellState::promote(state->memoryState());
    }

    // Erase memory that has never been written (i.e., cells that sprang into existence by reading an address) of which appears
    // to have been recently popped from the stack.
    CellErasurePredicate predicate(ops, ops->peekRegister(cpu->stackPointerRegister()), ignorePoppedMemory_);
    if (mem)
        mem->eraseMatchingCells(predicate);

    BaseSemantics::Formatter fmt;
    fmt.set_show_latest_writers(false);
    fmt.set_show_properties(false);
    std::ostringstream ss;
    ss <<(*state+fmt);
    return ss.str();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      NoOperation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NoOperation::NoOperation() {}

NoOperation::NoOperation(const Disassembler::Base::Ptr &disassembler) {
    normalizer_ = StateNormalizer::instance();

    if (disassembler) {
        Architecture::Base::ConstPtr arch = disassembler->architecture();
        RegisterDictionary::Ptr registerDictionary = arch->registerDictionary();
        ASSERT_not_null(registerDictionary);

        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        SymbolicSemantics::RiscOperators::Ptr ops = SymbolicSemantics::RiscOperators::instanceFromRegisters(registerDictionary, solver);
        ops->computingDefiners(SymbolicSemantics::TRACK_NO_DEFINERS);
        ops->computingMemoryWriters(SymbolicSemantics::TRACK_LATEST_WRITER); // necessary to erase non-written memory

        BaseSemantics::MemoryCellList::Ptr mstate = BaseSemantics::MemoryCellList::promote(ops->currentState()->memoryState());
        ASSERT_not_null(mstate);
        mstate->occlusionsErased(true);

        cpu_ = arch->newInstructionDispatcher(ops);
    }
}

std::string
NoOperation::normalizeState(const BaseSemantics::State::Ptr &state) const {
    static size_t ncalls = 0;
    if (!normalizer_)
        return StringUtility::numberToString(++ncalls);
    return normalizer_->toString(cpu_, state);
}

BaseSemantics::State::Ptr
NoOperation::initialState(SgAsmInstruction *insn) const {
    ASSERT_not_null(insn);
    ASSERT_not_null(cpu_);
    BaseSemantics::State::Ptr state;
    if (normalizer_) {
        state = normalizer_->initialState(cpu_, insn);
    } else {
        state = cpu_->currentState()->clone();
        state->clear();
        RegisterDescriptor IP = cpu_->instructionPointerRegister();
        state->writeRegister(IP, cpu_->number_(IP.nBits(), insn->get_address()), cpu_->operators().get());
    }

    // Set the stack pointer to a concrete value
    if (initialSp_) {
        const RegisterDescriptor regSp = cpu_->stackPointerRegister();
        BaseSemantics::RiscOperators::Ptr ops = cpu_->operators();
        state->writeRegister(regSp, ops->number_(regSp.nBits(), *initialSp_), ops.get());
    }

    return state;
}

bool
NoOperation::isNoop(SgAsmInstruction *insn) const {
    if (!cpu_)
        return false;                                   // assume instruction has an effect if we can't prove otherwise.
    if (!insn)
        return true;
    return isNoop(std::vector<SgAsmInstruction*>(1, insn));
}

bool
NoOperation::isNoop(const std::vector<SgAsmInstruction*> &insns) const {
    if (!cpu_)
        return false;                                   // assume sequence has effect if we can't prove otherwise
    if (insns.empty())
        return true;

    cpu_->operators()->currentState(initialState(insns.front()));
    std::string startState = normalizeState(cpu_->currentState());
    try {
        for (SgAsmInstruction *insn: insns)
            cpu_->processInstruction(insn);
    } catch (const BaseSemantics::Exception&) {
        return false;
    }

    std::string endState = normalizeState(cpu_->currentState());
    SAWYER_MESG(mlog[DEBUG]) <<"== startState ==\n" <<startState <<"\n";
    SAWYER_MESG(mlog[DEBUG]) <<"== endState ==\n" <<endState   <<"\n";
    SAWYER_MESG(mlog[DEBUG]) <<"start and end states " <<(startState==endState ? "are equal":"differ") <<"\n";
    return startState == endState;
}
    
NoOperation::IndexIntervals
NoOperation::findNoopSubsequences(const std::vector<SgAsmInstruction*> &insns) const {
    IndexIntervals retval;
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    if (debug) {
        debug <<"findNoopSubsequences(\n";
        for (SgAsmInstruction *insn: insns)
            debug <<"  " <<insn->toString() <<"\n";
        debug <<")\n";
    }

    // If we have no instruction semantics then assume that all instructions have an effect.
    if (!cpu_ || insns.empty())
        return retval;

    // Process each instruction as if insns were a basic block. Store insns[i]'s initial state in states[i] and its final state
    // in states[i+1].  States don't generally have a way to compare them for equality, so use a simple string-based comparison
    // for now. FIXME[Robb P. Matzke 2015-05-11]
    std::vector<std::string> states;
    bool hadError = false;
    cpu_->operators()->currentState(initialState(insns.front()));
    const RegisterDescriptor regIP = cpu_->instructionPointerRegister();
    try {
        for (SgAsmInstruction *insn: insns) {
            cpu_->operators()->writeRegister(regIP, cpu_->operators()->number_(regIP.nBits(), insn->get_address()));
            states.push_back(normalizeState(cpu_->currentState()));
            if (debug) {
                debug <<"  normalized state #" <<states.size()-1 <<":\n" <<StringUtility::prefixLines(states.back(), "    ");
                debug <<"  instruction: " <<insn->toString() <<"\n";
            }
            cpu_->processInstruction(insn);
        }
    } catch (const BaseSemantics::Exception &e) {
        hadError = true;
        SAWYER_MESG(debug) <<"  semantic exception: " <<e <<"\n";
    }
    if (!hadError) {
        states.push_back(normalizeState(cpu_->currentState()));
        if (debug)
            debug <<"  normalized state #" <<states.size()-1 <<":\n" <<StringUtility::prefixLines(states.back(), "    ");
    }

    // Ignore terminal branches?  I.e., if set, then given a block like "inc eax; jmp x" where x is not the fall-through
    // address, the JMP is not a no-op. However, if a block consists of only a JMP then it is considered a no-op because the
    // predecessors could have branched directly to the JMP target instead.
    //
    // FIXME[Robb P Matzke 2017-05-31]: We look at the terminal instruction in isolation to find its successors, but maybe a
    // better way would be to use the instruction pointer register from the state we already computed. Doing so would be a
    // more accurate way to handle opaque predicates.
    //
    // The check for states.size()+1==insns.size() is because if there was an exception above, then there won't be as
    // many states as instructions and we're in effect already ignoring the state for the last instruction (and possibly
    // more). There's normally one more state than instructions because we've saved the initial state plus the state
    // after each instruction.
    if (ignoreTerminalBranches_ && insns.size() > 1 && states.size() == insns.size() + 1) {
        bool isComplete = true;
        AddressSet succs = cpu_->architecture()->getSuccessors(insns.back(), isComplete/*out*/);
        if (succs.size() > 1 || !isComplete) {
            states.pop_back();
        } else if (succs.size() == 1 && succs.least() != insns.back()->get_address() + insns.back()->get_size()) {
            states.pop_back();
        }
    }
    
    // Look for pairs of states that are the same, and call that sequence of instructions a no-op
    for (size_t i=0; i+1<states.size(); ++i) {
        for (size_t j=i+1; j<states.size(); ++j) {
            if (states[i]==states[j]) {
                retval.push_back(IndexInterval::hull(i, j-1));
                SAWYER_MESG(debug) <<"  no-op: " <<i <<".." <<(j-1) <<"\n";
            }
        }
    }
    
    return retval;
}

static bool
sortBySizeAddress(const NoOperation::IndexInterval &a, const NoOperation::IndexInterval &b) {
    ASSERT_forbid(0==a.size());                         // empty or overflow
    ASSERT_forbid(0==b.size());                         // empty or overflow
    if (a.size() != b.size())
        return a.size() > b.size();                     // sort by decreasing size
    return a.least() < b.least();                       // then increasing address
}

// class method
NoOperation::IndexIntervals
NoOperation::largestEarliestNonOverlapping(const NoOperation::IndexIntervals &in) {
    NoOperation::IndexIntervals sorted = in, retval;
    std::sort(sorted.begin(), sorted.end(), sortBySizeAddress);
    Sawyer::Container::IntervalSet<IndexInterval> seen;
    for (const NoOperation::IndexInterval &where: sorted) {
        if (!seen.overlaps(where)) {
            retval.push_back(where);
            seen.insert(where);
        }
    }
    return retval;
}

// class method
std::vector<bool>
NoOperation::toVector(const IndexIntervals &in, size_t size) {
    std::vector<bool> retval(size, false);
    for (const IndexInterval &where: in) {
        if (where.greatest()+1 < retval.size())
            retval.resize(where.greatest()+1, false);
        for (size_t i=where.least(); i<=where.greatest(); ++i)
            retval[i] = true;
    }
    return retval;
}

} // namespace
} // namespace

#endif
