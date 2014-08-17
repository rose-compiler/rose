#ifndef ROSE_BinaryAnalysis_Partitioner_Semantics_H
#define ROSE_BinaryAnalysis_Partitioner_Semantics_H

#include "SymbolicSemantics2.h"

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Semantics {

using namespace rose::BinaryAnalysis::InstructionSemantics2;

typedef SymbolicSemantics::SValue SValue;
typedef SymbolicSemantics::SValuePtr SValuePtr;

typedef BaseSemantics::RegisterStateGeneric RegisterState;
typedef BaseSemantics::RegisterStateGenericPtr RegisterStateGenericPtr;

typedef SymbolicSemantics::MemoryState MemoryState;
typedef SymbolicSemantics::MemoryStatePtr MemoryStatePtr;

typedef BaseSemantics::State State;
typedef BaseSemantics::StatePtr StatePtr;

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

class RiscOperators: public SymbolicSemantics::RiscOperators {
private:
    static const size_t TRIM_THRESHOLD_DFLT = 100;
    size_t trimThreshold_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : SymbolicSemantics::RiscOperators(protoval, solver), trimThreshold_(TRIM_THRESHOLD_DFLT) {
        set_name("PartitionerSemantics");
        (void)SValue::promote(protoval);                // make sure its dynamic type is appropriate
    }

    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : SymbolicSemantics::RiscOperators(state, solver), trimThreshold_(TRIM_THRESHOLD_DFLT) {
        set_name("PartitionerSemantics");
        (void)SValue::promote(state->get_protoval());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new RiscOperators object and configure it using default values. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, SMTSolver *solver=NULL) {
        BaseSemantics::SValuePtr protoval = SValue::instance();
        BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
        BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Instantiate a new RiscOperators object with specified prototypical values. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiate a new RiscOperators with specified state. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   SMTSolver *solver=NULL) const /*override*/ {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   SMTSolver *solver=NULL) const /*override*/ {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to our operators. This is a checked conversion--it
     *  will fail if @p x does not point to our object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        assert(retval!=NULL);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New methods introduced at this level of the hierarchy
public:
    /** Controls trimming of large expressions.
     *
     *  In order to keep symbolic expressions from blowing up and taking extraordinarily long to evaluate, a size limit can be
     *  imposed.  If the limit is reached, then the expression is replaced with a new undefined value of the same width.
     *  Setting the trim threshold to zero disables trimming.
     *
     * @{ */
    size_t trimThreshold() const { return trimThreshold_; }
    void trimThreshold(size_t n) { trimThreshold_ = n; }
    BaseSemantics::SValuePtr trim(const BaseSemantics::SValuePtr &a_);
    /** @} */
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
    virtual void interrupt(int majr, int minr) /*override*/;
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) /*override*/;
    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) /*override*/;
    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr &sel_,
                                         const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) /*override*/;
    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) /*override*/;
    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/);
    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) /*override*/;
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) /*override*/;
    virtual BaseSemantics::SValuePtr readRegister(const RegisterDescriptor &reg) /*override*/;
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) /*override*/;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
