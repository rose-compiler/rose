#ifndef Rose_NullSemantics2_H
#define Rose_NullSemantics2_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "BaseSemantics2.h"

namespace Rose {
namespace BinaryAnalysis { // documented elsewhere
namespace InstructionSemantics2 { // documented elsewhere
        

/** Semantic domain that does nothing, but is well documented.
 *
 *  This semantic domain is useful for testing, as a no-op in a multi-policy, as documentation for the instruction
 *  semantics API (the RISC operations), or for debugging. */
namespace NullSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to a null semantic value. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

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
    virtual BaseSemantics::SValuePtr bottom_(size_t nBits) const ROSE_OVERRIDE {
        return instance(nBits);
    }
    virtual BaseSemantics::SValuePtr undefined_(size_t nBits) const ROSE_OVERRIDE {
        return instance(nBits);
    }
    virtual BaseSemantics::SValuePtr unspecified_(size_t nBits) const ROSE_OVERRIDE {
        return instance(nBits);
    }
    virtual BaseSemantics::SValuePtr number_(size_t nBits, uint64_t number) const ROSE_OVERRIDE {
        return instance(nBits, number);
    }
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const ROSE_OVERRIDE {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        return retval;
    }
    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other, const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const ROSE_OVERRIDE {
        return Sawyer::Nothing();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casting
public:
    /** Promote a base value to a NullSemantics value.  The value @p v must have a NullSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) {
        SValuePtr retval = v.dynamicCast<SValue>();
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Implementations of functions inherited
public:
    virtual bool isBottom() const ROSE_OVERRIDE {
        return false;
    }

    virtual bool is_number() const ROSE_OVERRIDE {
        return false;
    }

    virtual uint64_t get_number() const ROSE_OVERRIDE {
        ASSERT_not_reachable("not a number");
        uint64_t retval;
        return retval;
    }

    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return true;
    }

    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return this == getRawPointer(other); // must be equal if they're both the same object
    }

    virtual void print(std::ostream &stream, BaseSemantics::Formatter&) const ROSE_OVERRIDE {
        stream <<"VOID[" <<get_width() <<"]";
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to null register state. See @ref heap_object_shared_ownership. */
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
                                                   const RegisterDictionary *regdict) const ROSE_OVERRIDE {
        return instance(protoval, regdict);
    }

    virtual BaseSemantics::RegisterStatePtr clone() const ROSE_OVERRIDE {
        return RegisterStatePtr(new RegisterState(*this));
    }
    
    static RegisterStatePtr promote(const BaseSemantics::RegisterStatePtr &from) {
        RegisterStatePtr retval = boost::dynamic_pointer_cast<RegisterState>(from);
        ASSERT_not_null(retval);
        return retval;
    }

    virtual bool merge(const BaseSemantics::RegisterStatePtr &other_, BaseSemantics::RiscOperators*) ROSE_OVERRIDE {
        return false;
    }

    virtual void clear() ROSE_OVERRIDE {}
    virtual void zero() ROSE_OVERRIDE {}

    virtual BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt,
                 BaseSemantics::RiscOperators *ops) ROSE_OVERRIDE {
        return protoval()->undefined_(reg.nBits());
    }

    virtual BaseSemantics::SValuePtr
    peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt,
                 BaseSemantics::RiscOperators *ops) ROSE_OVERRIDE {
        return protoval()->undefined_(reg.nBits());
    }
    
    virtual void writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &value,
                               BaseSemantics::RiscOperators *ops) ROSE_OVERRIDE {}

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE {}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to null register state. See @ref heap_object_shared_ownership. */
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
                                                 const BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }

    virtual BaseSemantics::MemoryStatePtr clone() const ROSE_OVERRIDE {
        return MemoryStatePtr(new MemoryState(*this));
    }

public:
    static MemoryStatePtr promote(const BaseSemantics::MemoryStatePtr &x) {
        MemoryStatePtr retval = boost::dynamic_pointer_cast<MemoryState>(x);
        ASSERT_not_null(x);
        return retval;
    }

public:
    virtual void clear() ROSE_OVERRIDE {}

    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        return dflt->copy();
    }

    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {}

    virtual BaseSemantics::SValuePtr peekMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        return dflt->copy();
    }

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE {}

    virtual bool merge(const BaseSemantics::MemoryStatePtr &other, BaseSemantics::RiscOperators *addrOps,
                       BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        return false;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::State State;
typedef BaseSemantics::StatePtr StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to null RISC operations. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** NullSemantics operators always return a new undefined value.  They do, however, check certain preconditions. */
class RiscOperators: public BaseSemantics::RiscOperators {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(protoval, solver) {
        name("Null");
    }
    explicit RiscOperators(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(state, solver) {
        name("Null");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new RiscOperators object and configures it to use semantic values and states
     * that are defaults for NullSemantics. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict);

    /** Static allocating constructor. See the virtual constructor, create(), for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Constructor. See the virtual constructor, create(), for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Risc operators inherited
public:
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;
    
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr &sel_,
                                         const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;

    virtual void writeMemory(RegisterDescriptor segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
