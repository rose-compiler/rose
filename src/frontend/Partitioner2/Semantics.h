#ifndef ROSE_BinaryAnalysis_Partitioner_Semantics_H
#define ROSE_BinaryAnalysis_Partitioner_Semantics_H

#include "SymbolicSemantics2.h"

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Semantics {

typedef InstructionSemantics2::SymbolicSemantics::SValue SValue;
typedef InstructionSemantics2::SymbolicSemantics::SValuePtr SValuePtr;

typedef InstructionSemantics2::BaseSemantics::RegisterStateGeneric RegisterState;
typedef InstructionSemantics2::BaseSemantics::RegisterStateGenericPtr RegisterStateGenericPtr;

typedef InstructionSemantics2::BaseSemantics::State State;
typedef InstructionSemantics2::BaseSemantics::StatePtr StatePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Memory state.
 *
 *  Reading from an address that is either not writable (lacks MemoryMap::WRITABLE) or which is initialized (has
 *  MemoryMap::INITIALIZED) obtains the data directly from the memory map.
 *
 *  Addresses for each read operation are saved in a list which is nominally reset at the beginning of each instruction. */
class MemoryState: public InstructionSemantics2::SymbolicSemantics::MemoryState {
    const MemoryMap *map_;
    std::vector<SValuePtr> addressesRead_;

protected:
    explicit MemoryState(const InstructionSemantics2::BaseSemantics::MemoryCellPtr &protocell)
        : InstructionSemantics2::SymbolicSemantics::MemoryState(protocell), map_(NULL) {}
    MemoryState(const InstructionSemantics2::BaseSemantics::SValuePtr &addrProtoval,
                const InstructionSemantics2::BaseSemantics::SValuePtr &valProtoval)
        : InstructionSemantics2::SymbolicSemantics::MemoryState(addrProtoval, valProtoval), map_(NULL) {}
    MemoryState(const MemoryState &other)
        : InstructionSemantics2::SymbolicSemantics::MemoryState(other), map_(other.map_) {}

public:
    /** Instantiates a new memory state having specified prototypical cells and value. */
    static MemoryStatePtr instance(const InstructionSemantics2::BaseSemantics::MemoryCellPtr &protocell) {
        return MemoryStatePtr(new MemoryState(protocell));
    }

    /** Instantiates a new memory state having specified prototypical value. */
    static  MemoryStatePtr instance(const InstructionSemantics2::BaseSemantics::SValuePtr &addrProtoval,
                                    const InstructionSemantics2::BaseSemantics::SValuePtr &valProtoval) {
        return MemoryStatePtr(new MemoryState(addrProtoval, valProtoval));
    }

    /** Instantiates a new deep copy of an existing state. */
    static MemoryStatePtr instance(const MemoryStatePtr &other) {
        return MemoryStatePtr(new MemoryState(*other));
    }

public:
    /** Virtual constructor. Creates a memory state having specified prototypical value. */
    virtual InstructionSemantics2::BaseSemantics::MemoryStatePtr
    create(const InstructionSemantics2::BaseSemantics::SValuePtr &addrProtoval,
           const InstructionSemantics2::BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }

    /** Virtual constructor. Creates a new memory state having specified prototypical cells and value. */
    virtual InstructionSemantics2::BaseSemantics::MemoryStatePtr
    create(const InstructionSemantics2::BaseSemantics::MemoryCellPtr &protocell) const ROSE_OVERRIDE {
        return instance(protocell);
    }

    /** Virtual copy constructor. Creates a new deep copy of this memory state. */
    virtual InstructionSemantics2::BaseSemantics::MemoryStatePtr
    clone() const ROSE_OVERRIDE {
        return MemoryStatePtr(new MemoryState(*this));
    }

public:
    /** Recasts a base pointer to a symbolic memory state. This is a checked cast that will fail if the specified pointer does
     *  not point to an object of our class. */
    static MemoryStatePtr
    promote(const InstructionSemantics2::BaseSemantics::MemoryStatePtr &x) {
        MemoryStatePtr retval = boost::dynamic_pointer_cast<MemoryState>(x);
        assert(x!=NULL);
        return retval;
    }

public:
    /** The memory map for the specimen.
     *
     *  If this memory map exists and contains segments that have read permission but lack write permission, then any reads
     *  from such addresses will return the concrete values read from the map.  Any writes to such addresses will cause
     *  warnings and no operation to be performed.
     *
     *  @{ */
    const MemoryMap* memoryMap() const;
    void memoryMap(const MemoryMap *map) { map_=map; }
    /** @} */

    /** Property: concrete virtual addresses that were read.
     *
     * @{ */
    const std::vector<SValuePtr>& addressesRead() const { return addressesRead_; }
    std::vector<SValuePtr>& addressesRead() { return addressesRead_; }
    /** @} */

public:
    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
               InstructionSemantics2::BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC Operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Semantic operators.
 *
 *  Most operations are delegated to the symbolic state. The return value from the symbolic state is replaced with an unknown
 *  if the expression grows beyond a certain complexity. */
class RiscOperators: public InstructionSemantics2::SymbolicSemantics::RiscOperators {
private:
    static const size_t TRIM_THRESHOLD_DFLT = 100;
    size_t trimThreshold_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const InstructionSemantics2::BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : InstructionSemantics2::SymbolicSemantics::RiscOperators(protoval, solver), trimThreshold_(TRIM_THRESHOLD_DFLT) {
        set_name("PartitionerSemantics");
        (void)SValue::promote(protoval);                // make sure its dynamic type is appropriate
    }

    explicit RiscOperators(const InstructionSemantics2::BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : InstructionSemantics2::SymbolicSemantics::RiscOperators(state, solver), trimThreshold_(TRIM_THRESHOLD_DFLT) {
        set_name("PartitionerSemantics");
        (void)SValue::promote(state->get_protoval());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new RiscOperators object and configure it using default values. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, SMTSolver *solver=NULL) {
        InstructionSemantics2::BaseSemantics::SValuePtr protoval = SValue::instance();
        InstructionSemantics2::BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        InstructionSemantics2::BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
        InstructionSemantics2::BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Instantiate a new RiscOperators object with specified prototypical values. */
    static RiscOperatorsPtr
    instance(const InstructionSemantics2::BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiate a new RiscOperators with specified state. */
    static RiscOperatorsPtr
    instance(const InstructionSemantics2::BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::StatePtr &state,
           SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to our operators. This is a checked conversion--it
     *  will fail if @p x does not point to our object. */
    static RiscOperatorsPtr
    promote(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &x) {
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
    InstructionSemantics2::BaseSemantics::SValuePtr trim(const InstructionSemantics2::BaseSemantics::SValuePtr &a_);
    /** @} */
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
    virtual void startInstruction(SgAsmInstruction*) ROSE_OVERRIDE;
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    and_(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
         const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    or_(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
        const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    xor_(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
         const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    invert(const InstructionSemantics2::BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    extract(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
            size_t begin_bit, size_t end_bit) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    concat(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
           const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    leastSignificantSetBit(const InstructionSemantics2::BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    mostSignificantSetBit(const InstructionSemantics2::BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    rotateLeft(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
               const InstructionSemantics2::BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    rotateRight(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                const InstructionSemantics2::BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    shiftLeft(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
              const InstructionSemantics2::BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    shiftRight(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
               const InstructionSemantics2::BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    shiftRightArithmetic(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                         const InstructionSemantics2::BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    equalToZero(const InstructionSemantics2::BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    ite(const InstructionSemantics2::BaseSemantics::SValuePtr &sel_,
        const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
        const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    unsignedExtend(const InstructionSemantics2::BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    signExtend(const InstructionSemantics2::BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    add(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
        const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    addWithCarries(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                   const InstructionSemantics2::BaseSemantics::SValuePtr &b_,
                   const InstructionSemantics2::BaseSemantics::SValuePtr &c_,
                   InstructionSemantics2::BaseSemantics::SValuePtr &carry_out/*out*/);

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    negate(const InstructionSemantics2::BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    signedDivide(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                 const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    signedModulo(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                 const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    signedMultiply(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                   const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    unsignedDivide(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                   const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    unsignedModulo(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                   const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    unsignedMultiply(const InstructionSemantics2::BaseSemantics::SValuePtr &a_,
                     const InstructionSemantics2::BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readRegister(const RegisterDescriptor &reg) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(const RegisterDescriptor &segreg,
               const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               const InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
