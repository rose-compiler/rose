#ifndef Rose_NullSemantics2_H
#define Rose_NullSemantics2_H

#include "x86InstructionSemantics.h"
#include "BaseSemantics2.h"

namespace BinaryAnalysis { // documented elsewhere
namespace InstructionSemantics2 { // documented elsewhere
        

/** Semantic domain that does nothing, but is well documented.
 *
 *  This semantic domain is useful for testing, as a no-op in a multi-policy, as documentation for the instruction
 *  semantics API (the RISC operations), or for debugging. */
namespace NullSemantics {

/*******************************************************************************************************************************
 *                                      ValueType
 *******************************************************************************************************************************/

/** Smart pointer to an SValue object.  SValue objects are reference counted and should not be explicitly deleted. */
typedef BaseSemantics::Pointer<class SValue> SValuePtr;

/** Values in the NullSemantics domain.  Values are essentially void. */
class SValue: public BaseSemantics::SValue {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors.
protected:
    explicit SValue(size_t nbits): BaseSemantics::SValue(nbits) {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical values. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1));
    }

    /** Instantiate a new undefined value. */
    static SValuePtr instance(size_t nbits) {
        return SValuePtr(new SValue(nbits));
    }

    /** Instantiate a new concrete value. */
    static SValuePtr instance(size_t nbits, uint64_t number) {
        return SValuePtr(new SValue(nbits)); // the number is not important in this domain
    }

    /** Instantiate a new copy of an existing value. */
    static SValuePtr instance(const SValuePtr &other) {
        return SValuePtr(new SValue(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::SValuePtr undefined_(size_t nBits) const /*override*/ {
        return instance(nBits);
    }
    virtual BaseSemantics::SValuePtr number_(size_t nBits, uint64_t number) const /*override*/ {
        return instance(nBits, number);
    }
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const /*override*/ {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casting
public:
    /** Promote a base value to a NullSemantics value.  The value @p v must have a NullSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) {
        SValuePtr retval = BaseSemantics::dynamic_pointer_cast<SValue>(v);
        assert(retval!=NULL);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Implementations of functions inherited
public:
    virtual bool is_number() const { return false; }
    virtual uint64_t get_number() const { assert(!"not a number"); abort(); uint64_t retval; return retval;}

    virtual bool may_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const /*override*/ {
        return true;
    }

    virtual bool must_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const /*override*/ {
        return this == other.get(); // must be equal if they're both the same object
    }

    virtual void print(std::ostream &stream, BaseSemantics::Formatter&) const /*override*/ {
        stream <<"VOID[" <<get_width() <<"]";
    }
};


/*******************************************************************************************************************************
 *                                      Registers
 *******************************************************************************************************************************/

typedef boost::shared_ptr<class RegisterState> RegisterStatePtr;

/** Null register state.
 *
 *  This register state does not store any values.  Read operations always return (new) undefined values. */
class RegisterState: public BaseSemantics::RegisterState {
protected:
    RegisterState(const RegisterState &other)
        : BaseSemantics::RegisterState(other) {}

    RegisterState(const BaseSemantics::SValuePtr &protoval, const RegisterDictionary *regdict)
        : BaseSemantics::RegisterState(protoval, regdict) {}

public:
    static RegisterStatePtr instance(const BaseSemantics::SValuePtr &protoval, const RegisterDictionary *regdict) {
        return RegisterStatePtr(new RegisterState(protoval, regdict));
    }

    virtual BaseSemantics::RegisterStatePtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const RegisterDictionary *regdict) const /*override*/ {
        return instance(protoval, regdict);
    }

    virtual BaseSemantics::RegisterStatePtr clone() const /*override*/ {
        return RegisterStatePtr(new RegisterState(*this));
    }
    
    static RegisterStatePtr promote(const BaseSemantics::RegisterStatePtr &from) {
        RegisterStatePtr retval = boost::dynamic_pointer_cast<RegisterState>(from);
        assert(retval!=NULL);
        return retval;
    }

    virtual void clear() /*override*/ {}
    virtual void zero() /*override*/ {}

    virtual BaseSemantics::SValuePtr readRegister(const RegisterDescriptor &reg, BaseSemantics::RiscOperators *ops) /*override*/ {
        return get_protoval()->undefined_(reg.get_nbits());
    }

    virtual void writeRegister(const RegisterDescriptor &reg, const BaseSemantics::SValuePtr &value,
                               BaseSemantics::RiscOperators *ops) /*override*/ {}

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const /*override*/ {}
};


/*******************************************************************************************************************************
 *                                      Memory
 *******************************************************************************************************************************/

typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Null memory.
 *
 *  This memory state does not store any values. Read operations always return (new) undefined values. */
class MemoryState: public BaseSemantics::MemoryState {
protected:
    MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval)
        : BaseSemantics::MemoryState(addrProtoval, valProtoval) {}

    MemoryState(const MemoryStatePtr &other)
        : BaseSemantics::MemoryState(other) {}

public:
    static MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval) {
        return MemoryStatePtr(new MemoryState(addrProtoval, valProtoval));
    }

public:
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const /*override*/ {
        return instance(addrProtoval, valProtoval);
    }

    virtual BaseSemantics::MemoryStatePtr clone() const {
        return MemoryStatePtr(new MemoryState(*this));
    }

public:
    static MemoryStatePtr promote(const BaseSemantics::MemoryStatePtr &x) {
        MemoryStatePtr retval = boost::dynamic_pointer_cast<MemoryState>(x);
        assert(x!=NULL);
        return retval;
    }

public:
    virtual void clear() /*override*/ {}
    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                size_t nbits, BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) /*override*/ {
        return get_val_protoval()->undefined_(nbits);
    }
    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) /*override*/ {}
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const /*override*/ {}
};

/*******************************************************************************************************************************
 *                                      RISC Operators
 *******************************************************************************************************************************/

/** Smart pointer to a RiscOperators object.  RiscOperators objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** NullSemantics operators always return a new undefined value.  They do, however, check certain preconditions. */
class RiscOperators: public BaseSemantics::RiscOperators {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(protoval, solver) {
        set_name("Null");
    }
    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : BaseSemantics::RiscOperators(state, solver) {
        set_name("Null");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new RiscOperators object and configures it to use semantic values and states
     * that are defaults for NullSemantics. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict);

    /** Static allocating constructor. See the virtual constructor, create(), for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Constructor. See the virtual constructor, create(), for details. */
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
    // Risc operators inherited
public:
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

    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) /*override*/;

    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/;

    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/) /*override*/;

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

    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &cond,
                                                size_t nbits) /*override*/;

    virtual void writeMemory(const RegisterDescriptor &segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) /*override*/;
};

} /*namespace*/
} /*namespace*/
} /*namespace*/

#endif
