#ifndef ROSE_RSIM_Semantics_H
#define ROSE_RSIM_Semantics_H

#include "RSIM_Common.h"

#include <ConcreteSemantics2.h>
#include <NullSemantics2.h>
#include <DispatcherX86.h>

class RSIM_Thread;

namespace RSIM_Semantics {

typedef rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::SValue SValue;
typedef rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::SValuePtr SValuePtr;
typedef rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::MemoryState MemoryState;
typedef rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::MemoryStatePtr MemoryStatePtr;
typedef rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RegisterState RegisterState;
typedef rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RegisterStatePtr RegisterStatePtr;
typedef rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::State State;
typedef rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::StatePtr StatePtr;
typedef rose::BinaryAnalysis::InstructionSemantics2::DispatcherX86 Dispatcher;
typedef rose::BinaryAnalysis::InstructionSemantics2::DispatcherX86Ptr DispatcherPtr;

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Exception thrown by HLT instructions.
 *
 *  In an x86 CPU, the HLT instruction pauses the CPU until an interrupt arrives. The simulator handles this instruction by
 *  throwing an exception. */
struct Halt {
    rose_addr_t ip;                                 // address of HLT instruction
    Halt(rose_addr_t ip): ip(ip) {}
};

/** Exception thrown by INT instructions if the interrupt is not handled by the simulator. */
struct Interrupt {
    rose_addr_t ip;                                 // address of INT instruction
    int inum;                                       // interrupt number
    Interrupt(rose_addr_t ip, int inum): ip(ip), inum(inum) {}
};

/** RSIM Instruction Semantics.
 *
 *  The simulator uses ConcreteSemantics with a few relatively minor adjustments:
 *
 *  <ul>
 *    <li>There is a 1:1 correspondence between simulated threads and RiscOperators objects, and each has a pointer to the
 *        other.</li>
 *    <li>Writing to a segment register will cause its shadow register to be updated. The shadow registers are not stored in
 *        the semantic state, but rather are stored in a SegmentInfo table inedexed by segment register.</li>
 *    <li>All memory reads and writes resolve to the simulated process MemoryMap rather than being stored in the semantic
 *        state.  The addresses supplied to these methods are adjusted according to the segment shadow registers.</li>
 *    <li>The @c interrupt operator is intercepted and results in either a syscall being processed or an interrupt exception
 *        being thrown.</li>
 *  </ul> */
class RiscOperators: public rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RiscOperators {
public:
    typedef rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RiscOperators Super;

    struct SegmentInfo {
        rose_addr_t base, limit;
        bool present;
        SegmentInfo(): base(0), limit(0), present(false) {}
        SegmentInfo(const SegmentDescriptor &ud) {
            base = ud.base_addr;
            limit = ud.limit_in_pages ? (ud.limit << 12) | 0xfff : ud.limit;
            present = true;  // present = !ud.seg_not_present && ud.useable; // NOT USED BY LINUX
        }
    };
private:
    RSIM_Thread *thread_;
    typedef Sawyer::Container::Map<X86SegmentRegister, SegmentInfo> SegmentInfoMap;
    SegmentInfoMap segmentInfo_; // indexed by segment registers
    
protected:
    RiscOperators(RSIM_Thread *thread, const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                  rose::BinaryAnalysis::SMTSolver *solver)
        : Super(protoval, solver), thread_(thread) {}

    RiscOperators(RSIM_Thread *thread, const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
                  rose::BinaryAnalysis::SMTSolver *solver)
        : Super(state, solver), thread_(thread) {}

public:
    static RiscOperatorsPtr instance(RSIM_Thread *thread, const RegisterDictionary *regdict,
                                     rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        using namespace rose::BinaryAnalysis::InstructionSemantics2;
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(thread, state, solver));
    }

    static RiscOperatorsPtr instance(RSIM_Thread *thread,
                                     const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                                     rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(thread, protoval, solver));
    }

    static RiscOperatorsPtr instance(RSIM_Thread *thread,
                                     const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
                                     rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(thread, state, solver));
    }

public:
    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(RSIM_Thread *thread, const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           rose::BinaryAnalysis::SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(thread, protoval, solver);
    }

    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(RSIM_Thread *thread, const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
           rose::BinaryAnalysis::SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(thread, state, solver);
    }

public:
    static RiscOperatorsPtr promote(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    // Thread that owns these operators and the associated register state
    RSIM_Thread* thread() const { return thread_; }

    // Load shadow register with an entry from the GDT
    void loadShadowRegister(X86SegmentRegister, unsigned gdtIdx);

    // Semantics of an x86 POP instruction so the simulator can easily pop a word from the top of the stack.
    rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr pop();

    // Dump lots of info to the TRACE_STATE stream (even if it is disabled)
    void dumpState();

public:
    virtual void hlt() {
        throw Halt(get_insn()->get_address());
    }

    virtual void startInstruction(SgAsmInstruction*) ROSE_OVERRIDE;

    // Handles INT, SYSENTER, etc.
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;

    // Special handling for segment registers.
    virtual void writeRegister(const RegisterDescriptor &reg,
                               const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value) ROSE_OVERRIDE;

    // Read and write memory from the memory map directly.
    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(const RegisterDescriptor &segreg,
               const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &address,
               const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

    virtual void
    writeMemory(const RegisterDescriptor &segreg,
                const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &address,
                const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value,
                const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

/** Create a new dispatcher.
 *
 *  Every thread will have its own dispatcher so that each thread also conveniently has its own register state by virtue of
 *  each dispatcher having its own RiscOperators. */
DispatcherPtr createDispatcher(RSIM_Thread *owningThread);

} // namespace
#endif
