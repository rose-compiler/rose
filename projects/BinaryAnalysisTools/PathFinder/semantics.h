#ifndef PathFinder_Semantics_H
#define PathFinder_Semantics_H

#include <PathFinder/PathFinder.h>
#include <SymbolicSemantics2.h>

namespace PathFinder {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::SValue SValue;
typedef rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::SValuePtr SValuePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RegisterState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RegisterState RegisterState;
typedef rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RegisterStatePtr RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// varies with path-finding mode


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<class State> StatePtr;

// Semantic state holds mapping from symbolic variable names to comments about where the variable came from.
class State: public rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::State {
public:
    typedef rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::State Super;

private:
    VarComments varComments_;

protected:
    State(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
          const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryStatePtr &memory)
        : Super(registers, memory) {}

    State(const State &other)
        : Super(other), varComments_(other.varComments_) {}

public:
    static rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr
    instance(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
             const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }

    static rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr
    instance(const StatePtr &other) {
        return StatePtr(new State(*other));
    }

    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr
    create(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStatePtr &registers,
           const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryStatePtr &memory) const ROSE_OVERRIDE {
        return instance(registers, memory);
    }

    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr
    clone() const ROSE_OVERRIDE {
        return StatePtr(new State(*this));
    }

    static StatePtr
    promote(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &x) {
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
class RiscOperators: public rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RiscOperators {
    typedef rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RiscOperators Super;
public:
    size_t pathInsnIndex_;                              // current location in path, or -1
    const rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner_;

protected:
    RiscOperators(const rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
                  const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                  rose::BinaryAnalysis::SMTSolver *solver)
        : Super(protoval, solver), pathInsnIndex_(-1), partitioner_(partitioner) {
        name("FindPath");
    }

    RiscOperators(const rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
                  const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
                  rose::BinaryAnalysis::SMTSolver *solver)
        : Super(state, solver), pathInsnIndex_(-1), partitioner_(partitioner) {
        name("FindPath");
    }

public:
    static RiscOperatorsPtr
    instance(const rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
             const RegisterDictionary *regdict,
             rose::BinaryAnalysis::SMTSolver *solver=NULL);

    static RiscOperatorsPtr
    instance(const rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
             const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
             rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, protoval, solver));
    }

    static RiscOperatorsPtr
    instance(const rose::BinaryAnalysis::Partitioner2::Partitioner *partitioner,
             const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
             rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(partitioner, state, solver));
    }

public:
    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           rose::BinaryAnalysis::SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(NULL, protoval, solver);
    }

    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::StatePtr &state,
           rose::BinaryAnalysis::SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(NULL, state, solver);
    }

public:
    static RiscOperatorsPtr
    promote(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &x) {
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

    void partitioner(const rose::BinaryAnalysis::Partitioner2::Partitioner *p) {
        partitioner_ = p;
    }

private:
    /** Create a comment to describe a variable stored in a register. */
    std::string
    commentForVariable(const RegisterDescriptor&, const std::string &accessMode) const;

    /** Create a comment to describe a memory address if possible. The nBytes will be non-zero when we're describing
     *  an address as opposed to a value stored across some addresses. */
    std::string
    commentForVariable(const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                       const std::string &accessMode, size_t byteNumber=0, size_t nBytes=0) const;

public:
    virtual void
    startInstruction(SgAsmInstruction *insn);

    virtual void
    finishInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE;

    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr
    readRegister(const RegisterDescriptor &reg,
                 const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;

    virtual void
    writeRegister(const RegisterDescriptor &reg,
                  const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value) ROSE_OVERRIDE;

    virtual rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(const RegisterDescriptor &segreg,
               const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &dflt_,
               const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

    virtual void
    writeMemory(const RegisterDescriptor &segreg,
                const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &value,
                const rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::Formatter
symbolicFormat(const std::string &prefix="");

} // namespace
#endif
