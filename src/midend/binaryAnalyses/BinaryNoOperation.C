#include <sage3basic.h>
#include <AsmUnparser_compat.h>
#include <BinaryNoOperation.h>
#include <Diagnostics.h>
#include <Disassembler.h>
#include <MemoryCellList.h>
#include <SymbolicSemantics2.h>

namespace rose {
namespace BinaryAnalysis {

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

Sawyer::Message::Facility NoOperation::mlog;

void
NoOperation::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::NoOperation", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StateNormalizer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSemantics::StatePtr
NoOperation::StateNormalizer::initialState(const BaseSemantics::DispatcherPtr &cpu, SgAsmInstruction *insn) {
    ASSERT_not_null(cpu);

    BaseSemantics::StatePtr state = cpu->get_state()->clone();
    state->clear();

    BaseSemantics::RegisterStateGenericPtr rstate = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    if (rstate)
        rstate->initialize_large();

    RegisterDescriptor IP = cpu->instructionPointerRegister();
    state->writeRegister(IP, cpu->number_(IP.get_nbits(), insn->get_address()), cpu->get_operators().get());

    return state;
}

std::string
NoOperation::StateNormalizer::toString(const BaseSemantics::DispatcherPtr &cpu, const BaseSemantics::StatePtr &state_) {
    BaseSemantics::StatePtr state = state_;
    BaseSemantics::RiscOperatorsPtr ops = cpu->get_operators();
    if (!state)
        return "";
    bool isCloned = false;                              // do we have our own copy of the state?

    // If possible and appropriate, remove the instruction pointer register
    const RegisterDescriptor regIp = cpu->instructionPointerRegister();
    BaseSemantics::RegisterStateGenericPtr rstate = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    if (rstate && rstate->is_partly_stored(regIp)) {
        BaseSemantics::SValuePtr ip = ops->readRegister(cpu->instructionPointerRegister());
        if (ip->is_number()) {
            state = state->clone();
            isCloned = true;
            rstate = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
            rstate->erase_register(regIp, ops.get());
        }
    }

    BaseSemantics::MemoryCellListPtr mstate = BaseSemantics::MemoryCellList::promote(state->get_memory_state());
    if (mstate) {
        if (!isCloned) {
            state = state->clone();
            isCloned = true;
            mstate = BaseSemantics::MemoryCellList::promote(state->get_memory_state());
        }

        // Erase memory that has never been written.
        mstate->clearNonWritten();

        // Erase memory that is above (lower address) and near the current stack pointer.
        if (ignorePoppedMemory_) {
            BaseSemantics::MemoryCellList::CellList &cells = mstate->get_cells();
            BaseSemantics::MemoryCellList::CellList::iterator ci=cells.begin();
            BaseSemantics::SValuePtr stackCurVa = ops->readRegister(cpu->stackPointerRegister());
            BaseSemantics::SValuePtr stackMinVa = ops->subtract(stackCurVa,
                                                                ops->number_(stackCurVa->get_width(), ignorePoppedMemory_));
            while (ci!=cells.end()) {
                BaseSemantics::MemoryCellPtr cell = *ci;
                BaseSemantics::SValuePtr isPopped =     // assume downward-growing stack
                    ops->and_(ops->isUnsignedLessThan(cell->get_address(), stackCurVa),
                              ops->isUnsignedGreaterThanOrEqual(cell->get_address(), stackMinVa));
                if (isPopped->is_number() && isPopped->get_number()) {
                    ci = cells.erase(ci);
                } else {
                    ++ci;
                }
            }
        }
    }

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

NoOperation::NoOperation(Disassembler *disassembler) {
    normalizer_ = StateNormalizer::instance();

    if (disassembler) {
        const RegisterDictionary *registerDictionary = disassembler->get_registers();
        ASSERT_not_null(registerDictionary);
        size_t addrWidth = disassembler->instructionPointerRegister().get_nbits();

        SMTSolver *solver = NULL;
        SymbolicSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(registerDictionary, solver);
        ops->computingUseDef(false);
        ops->computingMemoryWriters(SymbolicSemantics::RiscOperators::TRACK_LATEST_WRITER); // necessary to erase non-written memory

        BaseSemantics::MemoryCellListPtr mstate = BaseSemantics::MemoryCellList::promote(ops->get_state()->get_memory_state());
        ASSERT_not_null(mstate);
        mstate->occlusionsErased(true);

        cpu_ = disassembler->dispatcher()->create(ops, addrWidth, registerDictionary);
    }
}

std::string
NoOperation::normalizeState(const BaseSemantics::StatePtr &state) const {
    static size_t ncalls = 0;
    if (!normalizer_)
        return StringUtility::numberToString(++ncalls);
    return normalizer_->toString(cpu_, state);
}

BaseSemantics::StatePtr
NoOperation::initialState(SgAsmInstruction *insn) const {
    ASSERT_not_null(insn);
    ASSERT_not_null(cpu_);
    BaseSemantics::StatePtr state;
    if (normalizer_) {
        state = normalizer_->initialState(cpu_, insn);
    } else {
        state = cpu_->get_state()->clone();
        state->clear();
        RegisterDescriptor IP = cpu_->instructionPointerRegister();
        state->writeRegister(IP, cpu_->number_(IP.get_nbits(), insn->get_address()), cpu_->get_operators().get());
    }

    // Set the stack pointer to a concrete value
    if (initialSp_) {
        const RegisterDescriptor regSp = cpu_->stackPointerRegister();
        BaseSemantics::RiscOperatorsPtr ops = cpu_->get_operators();
        state->writeRegister(regSp, ops->number_(regSp.get_nbits(), *initialSp_), ops.get());
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

    cpu_->get_operators()->set_state(initialState(insns.front()));
    std::string startState = normalizeState(cpu_->get_state());
    try {
        BOOST_FOREACH (SgAsmInstruction *insn, insns)
            cpu_->processInstruction(insn);
    } catch (const BaseSemantics::Exception&) {
        return false;
    }

    std::string endState = normalizeState(cpu_->get_state());
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
        BOOST_FOREACH (SgAsmInstruction *insn, insns)
            debug <<"  " <<unparseInstructionWithAddress(insn) <<"\n";
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
    cpu_->get_operators()->set_state(initialState(insns.front()));
    const RegisterDescriptor regIP = cpu_->instructionPointerRegister();
    try {
        BOOST_FOREACH (SgAsmInstruction *insn, insns) {
            cpu_->get_operators()->writeRegister(regIP, cpu_->get_operators()->number_(regIP.get_nbits(), insn->get_address()));
            states.push_back(normalizeState(cpu_->get_state()));
            if (debug) {
                debug <<"  normalized state #" <<states.size()-1 <<":\n" <<StringUtility::prefixLines(states.back(), "    ");
                debug <<"  instruction: " <<unparseInstructionWithAddress(insn) <<"\n";
            }
            cpu_->processInstruction(insn);
        }
    } catch (const BaseSemantics::Exception &e) {
        hadError = true;
        SAWYER_MESG(debug) <<"  semantic exception: " <<e <<"\n";
    }
    if (!hadError) {
        states.push_back(normalizeState(cpu_->get_state()));
        if (debug)
            debug <<"  normalized state #" <<states.size()-1 <<":\n" <<StringUtility::prefixLines(states.back(), "    ");
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
    BOOST_FOREACH (const NoOperation::IndexInterval &where, sorted) {
        if (!seen.isOverlapping(where)) {
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
    BOOST_FOREACH (const IndexInterval &where, in) {
        if (where.greatest()+1 < retval.size())
            retval.resize(where.greatest()+1, false);
        for (size_t i=where.least(); i<=where.greatest(); ++i)
            retval[i] = true;
    }
    return retval;
}

} // namespace
} // namespace
