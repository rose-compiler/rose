#include <sage3basic.h>
#include <BinaryNoOperation.h>
#include <Disassembler.h>
#include <SymbolicSemantics2.h>

namespace rose {
namespace BinaryAnalysis {

using namespace rose::BinaryAnalysis::InstructionSemantics2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StateNormalizer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSemantics::StatePtr
NoOperation::StateNormalizer::initialState(const BaseSemantics::DispatcherPtr &cpu) {
    BaseSemantics::StatePtr state = cpu->get_state()->clone();
    state->clear();

    BaseSemantics::RegisterStateGenericPtr rstate = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    if (rstate)
        rstate->initialize_large();

    return state;
}

std::string
NoOperation::StateNormalizer::toString(const BaseSemantics::DispatcherPtr &cpu, const BaseSemantics::StatePtr &state_) {
    BaseSemantics::StatePtr state = state_;
    if (!state)
        return "";
    bool isCloned = false;                              // do we have our own copy of the state?

    // If possible and appropriate, remove the instruction pointer register
    const RegisterDescriptor regIp = cpu->instructionPointerRegister();
    BaseSemantics::RegisterStateGenericPtr rstate = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
    if (rstate && rstate->is_partly_stored(regIp)) {
        BaseSemantics::SValuePtr ip = cpu->get_operators()->readRegister(cpu->instructionPointerRegister());
        if (ip->is_number()) {
            state = state->clone();
            isCloned = true;
            rstate = BaseSemantics::RegisterStateGeneric::promote(state->get_register_state());
            rstate->erase_register(regIp, cpu->get_operators().get());
        }
    }

    // If possible, erase memory that has never been written.
    BaseSemantics::MemoryCellListPtr mstate = BaseSemantics::MemoryCellList::promote(state->get_memory_state());
    if (mstate) {
        if (!isCloned) {
            state = state->clone();
            isCloned = true;
            mstate = BaseSemantics::MemoryCellList::promote(state->get_memory_state());
            mstate->clearNonWritten();
        }
    }

    std::ostringstream ss;
    ss <<*state;
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
        ops->set_compute_usedef(false);
        ops->set_compute_memwriters(false);

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
NoOperation::initialState() const {
    ASSERT_not_null(cpu_);
    BaseSemantics::StatePtr state = cpu_->get_state()->clone();
    state->clear();
    return normalizer_ ? normalizer_->initialState(cpu_) : state;
}

bool
NoOperation::isNoop(SgAsmInstruction *insn) const {
    if (!cpu_ || !insn)
        return false;                                   // assume instruction has an effect if we can't prove otherwise.
    return isNoop(std::vector<SgAsmInstruction*>(1, insn));
}

bool
NoOperation::isNoop(const std::vector<SgAsmInstruction*> &insns) const {
    if (!cpu_ || insns.empty())
        return false;                                   // assume sequence has effect if we can't prove otherwise

    cpu_->get_operators()->set_state(initialState());
    std::string startState = normalizeState(cpu_->get_state());
    try {
        BOOST_FOREACH (SgAsmInstruction *insn, insns)
            cpu_->processInstruction(insn);
    } catch (const BaseSemantics::Exception&) {
        return false;
    }

    std::string endState = normalizeState(cpu_->get_state());
    return startState == endState;
}
    
NoOperation::IndexIntervals
NoOperation::findNoopSubsequences(const std::vector<SgAsmInstruction*> &insns) const {
    IndexIntervals retval;

    // If we have no instruction semantics then assume that all instructions have an effect.
    if (!cpu_ || insns.empty())
        return retval;

    // Process each instruction as if insns were a basic block. Store insns[i]'s initial state in states[i] and its final state
    // in states[i+1].  States don't generally have a way to compare them for equality, so use a simple string-based comparison
    // for now. FIXME[Robb P. Matzke 2015-05-11]
    std::vector<std::string> states;
    bool hadError = false;
    cpu_->get_operators()->set_state(initialState());
    const RegisterDescriptor regIP = cpu_->instructionPointerRegister();
    try {
        BOOST_FOREACH (SgAsmInstruction *insn, insns) {
            cpu_->get_operators()->writeRegister(regIP, cpu_->get_operators()->number_(regIP.get_nbits(), insn->get_address()));
            states.push_back(normalizeState(cpu_->get_state()));
            cpu_->processInstruction(insn);
        }
    } catch (const BaseSemantics::Exception&) {
        hadError = true;
    }
    if (!hadError)
        states.push_back(normalizeState(cpu_->get_state()));

#if 0 // DEBUGGING [Robb P. Matzke 2015-05-12]
    std::cerr <<"ROBB: states:\n";
    for (size_t i=0; i<states.size(); ++i) {
        std::cerr <<"      [" <<std::setw(2) <<i <<"] ";
        std::cerr <<StringUtility::prefixLines(states[i], "           ", false);
    }
#endif

    // Look for pairs of states that are the same, and call that sequence of instructions a no-op
    for (size_t i=0; i+1<states.size(); ++i) {
        for (size_t j=i+1; j<states.size(); ++j) {
            if (states[i]==states[j])
                retval.push_back(IndexInterval::hull(i, j-1));
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
