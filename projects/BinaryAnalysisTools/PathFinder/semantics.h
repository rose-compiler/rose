#ifndef PathFinder_Semantics_H
#define PathFinder_Semantics_H

#include <PathFinder/PathFinder.h>
#include <SymbolicSemantics2.h>
#include <Partitioner2/Partitioner.h>

namespace PathFinder {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::SValue SValue;
typedef Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::SValuePtr SValuePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RegisterState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RegisterState RegisterState;
typedef Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RegisterStatePtr RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// varies with path-finding mode


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<class State> StatePtr;

// Semantic state holds mapping from symbolic variable names to comments about where the variable came from.
class State: public Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::State {
public:
    typedef Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::State Super;

private:
    VarComments varComments_;

protected:
    State(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
          const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryStatePtr &memory)
        : Super(registers, memory) {}

    State(const State &other)
        : Super(other), varComments_(other.varComments_) {}

public:
    static Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr
    instance(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
             const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }

    static Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr
    instance(const StatePtr &other) {
        return StatePtr(new State(*other));
    }

    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr
    create(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
           const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryStatePtr &memory) const ROSE_OVERRIDE {
        return instance(registers, memory);
    }

    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr
    clone() const ROSE_OVERRIDE {
        return StatePtr(new State(*this));
    }

    static StatePtr
    promote(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &x) {
        StatePtr retval = boost::dynamic_pointer_cast<State>(x);
        ASSERT_not_null(retval);
        return retval;
    }
    
    /** Set comment for variable name if none exists. */
    void varComment(const std::string &varName, const std::string &comment) {
        varComments_.insertMaybe(varName, comment);
    }

    /** Comment for variable name. */
    std::string varComment(const std::string &varName) const {
        return varComments_.getOrElse(varName, "");
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

// RiscOperators that add some additional tracking information for memory values.
class RiscOperators: public Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RiscOperators {
    typedef Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RiscOperators Super;
public:
    size_t pathInsnIndex_;                              // current location in path, or -1
    const Rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner_;

protected:
    RiscOperators(const Rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
                  const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                  const Rose::BinaryAnalysis::SmtSolverPtr &solver)
        : Super(protoval, solver), pathInsnIndex_(-1), partitioner_(partitioner) {
        name("FindPath");
    }

    RiscOperators(const Rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
                  const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
                  const Rose::BinaryAnalysis::SmtSolverPtr &solver)
        : Super(state, solver), pathInsnIndex_(-1), partitioner_(partitioner) {
        name("FindPath");
    }

public:
    static RiscOperatorsPtr
    instance(const Rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
             const Rose::BinaryAnalysis::RegisterDictionary *regdict,
             const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr());

    static RiscOperatorsPtr
    instance(const Rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
             const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
             const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, protoval, solver));
    }

    static RiscOperatorsPtr
    instance(const Rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
             const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
             const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, state, solver));
    }

public:
    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(NULL, protoval, solver);
    }

    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver=Rose::BinaryAnalysis::SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(NULL, state, solver);
    }

public:
    static RiscOperatorsPtr
    promote(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    std::string varComment(const std::string &varName) const;
    void varComment(const std::string &varName, const std::string &comment);

    size_t pathInsnIndex() const {
        return pathInsnIndex_;
    }

    void pathInsnIndex(size_t n) {
        pathInsnIndex_ = n;
    }

    void partitioner(const Rose::BinaryAnalysis::Partitioner2::Partitioner *p) {
        partitioner_ = p;
    }

private:
    /** Create a comment to describe a variable stored in a register. */
    std::string
    commentForVariable(Rose::BinaryAnalysis::RegisterDescriptor, const std::string &accessMode) const;

    /** Create a comment to describe a memory address if possible. The nBytes will be non-zero when we're describing
     *  an address as opposed to a value stored across some addresses. */
    std::string
    commentForVariable(const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                       const std::string &accessMode, size_t byteNumber=0, size_t nBytes=0) const;

public:
    virtual void
    startInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE;

    virtual void
    finishInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE;

    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr
    readRegister(Rose::BinaryAnalysis::RegisterDescriptor reg,
                 const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;

    virtual void
    writeRegister(Rose::BinaryAnalysis::RegisterDescriptor reg,
                  const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value) ROSE_OVERRIDE;

    virtual Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(Rose::BinaryAnalysis::RegisterDescriptor segreg,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &dflt_,
               const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

    virtual void
    writeMemory(Rose::BinaryAnalysis::RegisterDescriptor segreg,
                const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value,
                const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::Formatter
symbolicFormat(const std::string &prefix="");

} // namespace
#endif
