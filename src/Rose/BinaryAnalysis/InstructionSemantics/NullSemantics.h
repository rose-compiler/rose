#ifndef ROSE_BinaryAnalysis_InstructionSemantics_NullSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_NullSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>

namespace Rose {
namespace BinaryAnalysis {                              // documented elsewhere
namespace InstructionSemantics {                        // documented elsewhere
        

/** Semantic domain that does nothing, but is well documented.
 *
 *  This semantic domain is useful for testing, as a no-op in a multi-policy, as documentation for the instruction
 *  semantics API (the RISC operations), or for debugging. */
namespace NullSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Values in the NullSemantics domain.  Values are essentially void. */
class SValue: public BaseSemantics::SValue {
public:
    /** Base type. */
    using Super = BaseSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

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
    static SValuePtr instance(size_t nbits, uint64_t /*number*/) {
        return SValuePtr(new SValue(nbits)); // the number is not important in this domain
    }

    /** Instantiate a new copy of an existing value. */
    static SValuePtr instance(const SValuePtr &other) {
        return SValuePtr(new SValue(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nBits) const override {
        return instance(nBits);
    }
    virtual BaseSemantics::SValuePtr undefined_(size_t nBits) const override {
        return instance(nBits);
    }
    virtual BaseSemantics::SValuePtr unspecified_(size_t nBits) const override {
        return instance(nBits);
    }
    virtual BaseSemantics::SValuePtr number_(size_t nBits, uint64_t number) const override {
        return instance(nBits, number);
    }
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const override {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->nBits())
            retval->set_width(new_width);
        return retval;
    }
    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr&, const BaseSemantics::MergerPtr&, const SmtSolverPtr&) const override {
        return Sawyer::Nothing();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casting
public:
    /** Promote a base value to a NullSemantics value.  The value @p v must have a NullSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) {
        SValuePtr retval = as<SValue>(v);
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Implementations of functions inherited
public:
    virtual bool isBottom() const override {
        return false;
    }

    virtual void print(std::ostream &stream, BaseSemantics::Formatter&) const override {
        stream <<"VOID[" <<nBits() <<"]";
    }

    virtual void hash(Combinatorics::Hasher &hasher) const override {
        hasher.insert(0);                               // hash depends on number of SValues hashed, but not any content
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override legacy members. These now are called by the camelCase names in the base class. Eventually we'll switch the
    // camelCase names to be the virtual functions and get rid of the snake_case names, so be sure to specify "override" in
    // your own code so you know when we make the switch.
public:
    // See isConcrete
    virtual bool is_number() const override {
        return false;
    }

    // See toUnsigned and toSigned
    virtual uint64_t get_number() const override {
        ASSERT_not_reachable("not a number");
        uint64_t retval;
        return retval;
    }

    // See mayEqual
    virtual bool may_equal(const BaseSemantics::SValuePtr&, const SmtSolverPtr& = SmtSolverPtr()) const override {
        return true;
    }

    // See mustEqual
    virtual bool must_equal(const BaseSemantics::SValuePtr &other, const SmtSolverPtr& = SmtSolverPtr()) const override {
        return this == getRawPointer(other); // must be equal if they're both the same object
    }

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
typedef boost::shared_ptr<class RegisterState> RegisterStatePtr;

/** Null register state.
 *
 *  This register state does not store any values.  Read operations always return (new) undefined values. */
class RegisterState: public BaseSemantics::RegisterState {
public:
    /** Base type. */
    using Super = BaseSemantics::RegisterState;

    /** Shared-ownership pointer. */
    using Ptr = RegisterStatePtr;

protected:
    RegisterState(const RegisterState &other)
        : BaseSemantics::RegisterState(other) {}

    RegisterState(const BaseSemantics::SValuePtr &protoval, const RegisterDictionaryPtr &regdict)
        : BaseSemantics::RegisterState(protoval, regdict) {}

public:
    static RegisterStatePtr instance(const BaseSemantics::SValuePtr &protoval, const RegisterDictionaryPtr &regdict) {
        return RegisterStatePtr(new RegisterState(protoval, regdict));
    }

    virtual BaseSemantics::RegisterStatePtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const RegisterDictionaryPtr &regdict) const override {
        return instance(protoval, regdict);
    }

    virtual BaseSemantics::RegisterStatePtr clone() const override {
        return RegisterStatePtr(new RegisterState(*this));
    }
    
    static RegisterStatePtr promote(const BaseSemantics::RegisterStatePtr &from) {
        RegisterStatePtr retval = as<RegisterState>(from);
        ASSERT_not_null(retval);
        return retval;
    }

    virtual bool merge(const BaseSemantics::RegisterStatePtr&, BaseSemantics::RiscOperators*) override {
        return false;
    }

    virtual void clear() override {}
    virtual void zero() override {}

    virtual BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &/*dflt*/, BaseSemantics::RiscOperators*) override {
        return protoval()->undefined_(reg.nBits());
    }

    virtual BaseSemantics::SValuePtr
    peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &/*dflt*/, BaseSemantics::RiscOperators*) override {
        return protoval()->undefined_(reg.nBits());
    }
    
    virtual void writeRegister(RegisterDescriptor, const BaseSemantics::SValuePtr&, BaseSemantics::RiscOperators*) override {}

    virtual void updateReadProperties(RegisterDescriptor) override {}
    virtual void updateWriteProperties(RegisterDescriptor, BaseSemantics::InputOutputProperty) override {}

    virtual void hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators*) const override {}

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override {}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Null memory.
 *
 *  This memory state does not store any values. Read operations always return (new) undefined values. */
class MemoryState: public BaseSemantics::MemoryState {
public:
    /** Base type. */
    using Super = BaseSemantics::MemoryState;

    /** Shared-ownership pointer. */
    using Ptr = MemoryStatePtr;

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
                                                 const BaseSemantics::SValuePtr &valProtoval) const override {
        return instance(addrProtoval, valProtoval);
    }

    virtual BaseSemantics::MemoryStatePtr clone() const override {
        return MemoryStatePtr(new MemoryState(*this));
    }

public:
    static MemoryStatePtr promote(const BaseSemantics::MemoryStatePtr &x) {
        MemoryStatePtr retval = as<MemoryState>(x);
        ASSERT_not_null(x);
        return retval;
    }

public:
    virtual void clear() override {}

    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &/*address*/,
                                                const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators */*addrOps*/,
                                                BaseSemantics::RiscOperators */*valOps*/) override {
        return dflt->copy();
    }

    virtual void writeMemory(const BaseSemantics::SValuePtr &/*addr*/, const BaseSemantics::SValuePtr &/*value*/,
                             BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/) override {}

    virtual BaseSemantics::SValuePtr peekMemory(const BaseSemantics::SValuePtr &/*address*/, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators */*addrOps*/,
                                                BaseSemantics::RiscOperators */*valOps*/) override {
        return dflt->copy();
    }

    virtual void hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators */*addrOps*/,
                      BaseSemantics::RiscOperators */*valOps*/) const override {}

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override {}

    virtual bool merge(const BaseSemantics::MemoryStatePtr &/*other*/, BaseSemantics::RiscOperators */*addrOps*/,
                       BaseSemantics::RiscOperators */*valOps*/) override {
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

/** Shared-ownership pointer. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** NullSemantics operators always return a new undefined value.  They do, however, check certain preconditions. */
class RiscOperators: public BaseSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = BaseSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

    explicit RiscOperators(const BaseSemantics::StatePtr&, const SmtSolverPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    ~RiscOperators();

    /** Instantiate a new RiscOperators object and configures it to use semantic values and states
     * that are defaults for NullSemantics. */
    static RiscOperatorsPtr instanceFromRegisters(const RegisterDictionaryPtr &regdict);

    /** Static allocating constructor. See the virtual constructor, create(), for details. */
    static RiscOperatorsPtr instanceFromProtoval(const BaseSemantics::SValuePtr &protoval,
                                                 const SmtSolverPtr &solver = SmtSolverPtr());

    /** Constructor. See the virtual constructor, create(), for details. */
    static RiscOperatorsPtr instanceFromState(const BaseSemantics::StatePtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr&,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Risc operators inherited
public:
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) override;
    
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) override;

    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) override;

    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) override;

    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) override;

    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) override;

    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) override;

    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) override;

    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) override;

    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) override;

    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) override;

    virtual BaseSemantics::SValuePtr iteWithStatus(const BaseSemantics::SValuePtr &sel_,
                                                   const BaseSemantics::SValuePtr &a_,
                                                   const BaseSemantics::SValuePtr &b_,
                                                   IteStatus&) override;

    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) override;

    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/) override;

    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) override;

    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) override;

    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) override;

    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) override;

    virtual void writeMemory(RegisterDescriptor segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
