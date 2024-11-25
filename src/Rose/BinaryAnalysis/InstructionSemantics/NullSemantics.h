#ifndef ROSE_BinaryAnalysis_InstructionSemantics_NullSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_NullSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

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
using SValuePtr = Sawyer::SharedPointer<class SValue>;

/** Values in the NullSemantics domain.  Values are essentially void. */
class SValue: public BaseSemantics::SValue {
public:
    /** Base type. */
    using Super = BaseSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors.
public:
    ~SValue();

protected:
    explicit SValue(size_t nbits);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical values. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance();

    /** Instantiate a new undefined value. */
    static SValuePtr instance(size_t nbits);

    /** Instantiate a new concrete value. */
    static SValuePtr instance(size_t nbits, uint64_t /*number*/);

    /** Instantiate a new copy of an existing value. */
    static SValuePtr instance(const SValuePtr &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nBits) const override;
    virtual BaseSemantics::SValuePtr undefined_(size_t nBits) const override;
    virtual BaseSemantics::SValuePtr unspecified_(size_t nBits) const override;
    virtual BaseSemantics::SValuePtr number_(size_t nBits, uint64_t number) const override;
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const override;
    virtual Sawyer::Optional<BaseSemantics::SValuePtr> createOptionalMerge(const BaseSemantics::SValuePtr&,
                                                                           const BaseSemantics::MergerPtr&,
                                                                           const SmtSolverPtr&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casting
public:
    /** Promote a base value to a NullSemantics value.  The value @p v must have a NullSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Implementations of functions inherited
public:
    virtual bool isBottom() const override;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;
    virtual void hash(Combinatorics::Hasher&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override legacy members. These now are called by the camelCase names in the base class. Eventually we'll switch the
    // camelCase names to be the virtual functions and get rid of the snake_case names, so be sure to specify "override" in
    // your own code so you know when we make the switch.
public:
    // See isConcrete
    virtual bool is_number() const override;

    // See toUnsigned and toSigned
    virtual uint64_t get_number() const override;

    // See mayEqual
    virtual bool may_equal(const BaseSemantics::SValuePtr&, const SmtSolverPtr& = SmtSolverPtr()) const override;

    // See mustEqual
    virtual bool must_equal(const BaseSemantics::SValuePtr &other, const SmtSolverPtr& = SmtSolverPtr()) const override;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
using RegisterStatePtr = boost::shared_ptr<class RegisterState>;

/** Null register state.
 *
 *  This register state does not store any values.  Read operations always return (new) undefined values. */
class RegisterState: public BaseSemantics::RegisterState {
public:
    /** Base type. */
    using Super = BaseSemantics::RegisterState;

    /** Shared-ownership pointer. */
    using Ptr = RegisterStatePtr;

public:
    ~RegisterState();

protected:
    RegisterState(const RegisterState &other);
    RegisterState(const BaseSemantics::SValuePtr &protoval, const RegisterDictionaryPtr&);

public:
    static RegisterStatePtr instance(const BaseSemantics::SValuePtr &protoval, const RegisterDictionaryPtr&);
    virtual BaseSemantics::RegisterStatePtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const RegisterDictionaryPtr &regdict) const override;
    virtual BaseSemantics::RegisterStatePtr clone() const override;
    static RegisterStatePtr promote(const BaseSemantics::RegisterStatePtr &from);

    virtual bool merge(const BaseSemantics::RegisterStatePtr&, BaseSemantics::RiscOperators*) override;
    virtual void clear() override;
    virtual void zero() override;
    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor, const BaseSemantics::SValuePtr &/*dflt*/,
                                                  BaseSemantics::RiscOperators*) override;
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor, const BaseSemantics::SValuePtr &/*dflt*/,
                                                  BaseSemantics::RiscOperators*) override;
    virtual void writeRegister(RegisterDescriptor, const BaseSemantics::SValuePtr&, BaseSemantics::RiscOperators*) override;
    virtual void updateReadProperties(RegisterDescriptor) override;
    virtual void updateWriteProperties(RegisterDescriptor, BaseSemantics::InputOutputProperty) override;
    virtual void hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators*) const override;
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
using MemoryStatePtr = boost::shared_ptr<class MemoryState>;

/** Null memory.
 *
 *  This memory state does not store any values. Read operations always return (new) undefined values. */
class MemoryState: public BaseSemantics::MemoryState {
public:
    /** Base type. */
    using Super = BaseSemantics::MemoryState;

    /** Shared-ownership pointer. */
    using Ptr = MemoryStatePtr;

public:
    ~MemoryState();

protected:
    MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);
    MemoryState(const MemoryStatePtr &other);

public:
    static MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);

public:
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const override;
    virtual BaseSemantics::MemoryStatePtr clone() const override;

public:
    static MemoryStatePtr promote(const BaseSemantics::MemoryStatePtr&);

public:
    virtual void clear() override {}

    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &/*address*/,
                                                const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators */*addrOps*/,
                                                BaseSemantics::RiscOperators */*valOps*/) override;

    virtual void writeMemory(const BaseSemantics::SValuePtr &/*addr*/, const BaseSemantics::SValuePtr &/*value*/,
                             BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/) override;

    virtual BaseSemantics::SValuePtr peekMemory(const BaseSemantics::SValuePtr &/*address*/, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators */*addrOps*/,
                                                BaseSemantics::RiscOperators */*valOps*/) override;

    virtual void hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators */*addrOps*/,
                      BaseSemantics::RiscOperators */*valOps*/) const override;

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;

    virtual bool merge(const BaseSemantics::MemoryStatePtr &/*other*/, BaseSemantics::RiscOperators */*addrOps*/,
                       BaseSemantics::RiscOperators */*valOps*/) override;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using State = BaseSemantics::State;
using StatePtr = BaseSemantics::StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
using RiscOperatorsPtr = boost::shared_ptr<class RiscOperators>;

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
