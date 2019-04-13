#ifndef ROSE_RSIM_Semantics_H
#define ROSE_RSIM_Semantics_H

#include "RSIM_Common.h"

#include <ConcreteSemantics2.h>
#include <NullSemantics2.h>
#include <DispatcherX86.h>
#include <DispatcherM68k.h>

class RSIM_Thread;

namespace RSIM_Semantics {

typedef Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::SValue SValue;
typedef Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::SValuePtr SValuePtr;
typedef Rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::MemoryState MemoryState;
typedef Rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::MemoryStatePtr MemoryStatePtr;
typedef Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RegisterState RegisterState;
typedef Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RegisterStatePtr RegisterStatePtr;
typedef Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::State State;
typedef Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::StatePtr StatePtr;
typedef Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::Dispatcher Dispatcher;
typedef Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::DispatcherPtr DispatcherPtr;

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
 *    <li>Writing to an x86 segment register will cause its shadow register to be updated. The shadow registers are not stored
 *        in the semantic state, but rather are stored in a SegmentInfo table inedexed by segment register.</li>
 *    <li>All memory reads and writes resolve to the simulated process MemoryMap rather than being stored in the semantic
 *        state.  The addresses supplied to these methods are adjusted according to the segment shadow registers for x86.</li>
 *    <li>The x86 @c interrupt operator is intercepted and results in either a syscall being processed or an interrupt
 *        exception being thrown.</li>
 *  </ul> */
class RiscOperators: public Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RiscOperators {
public:
    typedef Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RiscOperators Super;

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
    Architecture architecture_;
    bool allocateOnDemand_;                             // allocate rather than returning an error?
    RSIM_Thread *thread_;
    typedef Sawyer::Container::Map<Rose::BinaryAnalysis::X86SegmentRegister, SegmentInfo> SegmentInfoMap;

    // In x86-64 the CS, SS, DS, and ES segment selector registers are forced to zero, have a base of zero and a limit of
    // 2^64. The FS and GS registers can still be used, but only their descriptors' base address is used, not the limit.
    SegmentInfoMap segmentInfo_;
    
protected:
    RiscOperators(Architecture arch, RSIM_Thread *thread,
                  const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                  const Rose::BinaryAnalysis::SmtSolverPtr &solver)
        : Super(protoval, solver), architecture_(arch), allocateOnDemand_(false), thread_(thread) {}

    RiscOperators(Architecture arch, RSIM_Thread *thread,
                  const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
                  const Rose::BinaryAnalysis::SmtSolverPtr &solver)
        : Super(state, solver), architecture_(arch), allocateOnDemand_(false), thread_(thread) {}

public:
    static RiscOperatorsPtr instance(Architecture arch, RSIM_Thread *thread,
                                     const Rose::BinaryAnalysis::RegisterDictionary *regdict,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) {
        using namespace Rose::BinaryAnalysis::InstructionSemantics2;
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(arch, thread, state, solver));
    }

    static RiscOperatorsPtr instance(Architecture arch, RSIM_Thread *thread,
                                     const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(arch, thread, protoval, solver));
    }

    static RiscOperatorsPtr instance(Architecture arch, RSIM_Thread *thread,
                                     const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
                                     const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(arch, thread, state, solver));
    }

public:
    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_reachable("no architecture or thread available");
    }

    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(Architecture arch, RSIM_Thread *thread,
           const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) const {
        return instance(arch, thread, protoval, solver);
    }

    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_reachable("no architecture or thread available");
    }

    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(Architecture arch, RSIM_Thread *thread,
           const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) const {
        return instance(arch, thread, state, solver);
    }

public:
    static RiscOperatorsPtr promote(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    // Property that controls allocate or error.
    bool allocateOnDemand() const { return allocateOnDemand_; }
    void allocateOnDemand(bool b) { allocateOnDemand_ = b; }

    // Thread that owns these operators and the associated register state
    RSIM_Thread* thread() const { return thread_; }

    // Load shadow register with an entry from the GDT
    void loadShadowRegister(Rose::BinaryAnalysis::X86SegmentRegister, unsigned gdtIdx);

    // Return segment info for register
    SegmentInfo& segmentInfo(Rose::BinaryAnalysis::X86SegmentRegister sr);

    // Semantics of an x86 POP instruction so the simulator can easily pop a word from the top of the stack.
    Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr pop();

    // Dump lots of info to the TRACE_STATE stream (even if it is disabled)
    void dumpState();

    // Architecture
    Architecture architecture() const { return architecture_; }

public:
    virtual void hlt() ROSE_OVERRIDE {
        throw Halt(currentInstruction()->get_address());
    }

    virtual void startInstruction(SgAsmInstruction*) ROSE_OVERRIDE;

    // Handles INT, SYSENTER, etc.
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;

    // Special handling for segment registers.
    virtual void writeRegister(Rose::BinaryAnalysis::RegisterDescriptor reg,
                               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value) ROSE_OVERRIDE;

    // Read and write memory from the memory map directly.
    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(Rose::BinaryAnalysis::RegisterDescriptor segreg,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &address,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

    virtual void
    writeMemory(Rose::BinaryAnalysis::RegisterDescriptor segreg,
                const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &address,
                const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value,
                const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

/** Create a new dispatcher.
 *
 *  Every thread will have its own dispatcher so that each thread also conveniently has its own register state by virtue of
 *  each dispatcher having its own RiscOperators. */
DispatcherPtr createDispatcher(RSIM_Thread *owningThread);

} // namespace
#endif
