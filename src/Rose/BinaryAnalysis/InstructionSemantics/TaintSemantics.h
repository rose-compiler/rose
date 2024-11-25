#ifndef ROSE_BinaryAnalysis_InstructionSemantics_TaintSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_TaintSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Adds taint information to all symbolic values. */
namespace TaintSemantics {

/** Whether a value is tainted. */
enum class Taintedness {
    BOTTOM,                                             /**< Neither tainted nor untainted. */
    UNTAINTED,                                          /**< Untainted. */
    TAINTED,                                            /**< Tainted. */
    TOP                                                 /**< Both tainted and untainted at the same time. */
};

using ExprPtr = SymbolicSemantics::ExprPtr;
using ExprNode = SymbolicSemantics::ExprNode;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Merging values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Controls merging of tainted values. */
using Merger = SymbolicSemantics::Merger;

/** Shared-ownership pointer for a merge control object. */
using MergerPtr = Sawyer::SharedPointer<Merger>;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for symbolic semantic value. */
using SValuePtr = Sawyer::SharedPointer<class SValue>;

/** Formatter for taint values. */
using Formatter = SymbolicSemantics::Formatter;

/** Type of values manipulated by the SymbolicSemantics domain.
 *
 *  Values of type type are used whenever a value needs to be stored, such as memory addresses, the values stored at those
 *  addresses, the values stored in registers, the operands for RISC operations, and the results of those operations.
 *
 *  Taint SValues are symbolic SValues with additional taint information. Taintedness is a set represented by the four possible
 *  values of the @ref Taintedness enum. Values can be tainted, untainted, neither, or both. The "neither" case means we don't
 *  know anything about the taintedness of the value and is also called "bottom" in dataflow parlance. When data flow merges
 *  two values and one is tainted and the other is untainted, then the merged result is both tainted and untainted at the same
 *  time, which is called "top" in data flow parlance. <b>Note that the @ref isBottom method inherited from @ref
 *  SymbolicSemantics::SValue tests whether the symbolic value is a bottom value, not whether the taintedness is bottom.</b> */
class SValue: public SymbolicSemantics::SValue {
    Taintedness taintedness_ = Taintedness::BOTTOM;

public:
    /** Base type. */
    using Super = SymbolicSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
public:
    ~SValue();

protected:
    SValue();                                           // needed for serialization

    explicit SValue(size_t nbits);
    SValue(size_t nbits, uint64_t number);
    SValue(ExprPtr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance();

    /** Instantiate a new data-flow bottom value of specified width.
     *
     *  The symbolic value is what is set to bottom in this case. The taintedness is always set to bottom by all the static
     *  allocating constructors. If you need a different taintedness then you need to change it with the @ref taintedness
     *  property. */
    static SValuePtr instance_bottom(size_t nbits);

    /** Instantiate a new undefined value of specified width. */
    static SValuePtr instance_undefined(size_t nbits);

    /** Instantiate a new unspecified value of specified width. */
    static SValuePtr instance_unspecified(size_t nbits);

    /** Instantiate a new concrete value. */
    static SValuePtr instance_integer(size_t nbits, uint64_t value);

    /** Instantiate a new symbolic value. */
    static SValuePtr instance_symbolic(const SymbolicExpression::Ptr &value);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual allocating constructors
public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const override;

    /*  Instantiate a new data-flow bottom value of specified width.
     *
     *  The symbolic value is what is set to bottom in this case. The taintedness is always set to bottom by all the static
     *  allocating constructors. If you need a different taintedness then you need to change it with the @ref taintedness
     *  property. */
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const override;

    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const override;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const override;
    virtual BaseSemantics::SValuePtr boolean_(bool value) const override;
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const override;

    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other, const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Promote a base value to a TaintSemantics value.  The value @p v must have a TaintSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override virtual methods...
public:
    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;

    virtual void hash(Combinatorics::Hasher&) const override;

protected: // when implementing use these names; but when calling, use the camelCase names
    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const override;
    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const override;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Additional methods first declared in this class...
public:
    /** Property: Taintedness.
     *
     *  True if the value is considered to be tainted, and false otherwise.
     *
     * @{ */
    Taintedness taintedness() const;
    void taintedness(Taintedness);
    /** @} */

public:
    // Merge two taintedness values
    static Taintedness mergeTaintedness(Taintedness, Taintedness);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using RegisterState = SymbolicSemantics::RegisterState;
using RegisterStatePtr = SymbolicSemantics::RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using MemoryListState = SymbolicSemantics::MemoryListState;
using MemoryListStatePtr = SymbolicSemantics::MemoryListStatePtr;

using MemoryMapState = SymbolicSemantics::MemoryMapState;
using MemoryMapStatePtr = SymbolicSemantics::MemoryMapStatePtr;

using MemoryState = MemoryListState;
using MemoryStatePtr = MemoryListStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using State = SymbolicSemantics::State;
using StatePtr = SymbolicSemantics::StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to symbolic RISC operations. */
using RiscOperatorsPtr = boost::shared_ptr<class RiscOperators>;

/** Defines RISC operators for the TaintSemantics domain.
 *
 *  These RISC operators depend on functionality introduced into the SValue class hierarchy at the TaintSemantics::SValue
 *  level. Therefore, the prototypical value supplied to the constructor or present in the supplied state object must have a
 *  dynamic type which is a TaintSemantics::SValue.
 *
 *  Each RISC operator should return a newly allocated semantic value so that the caller can adjust taintedness for the result
 *  without affecting any of the inputs. For example, a no-op that returns its argument should be implemented like this:
 *
 * @code
 *  BaseSemantics::SValuePtr noop(const BaseSemantics::SValuePtr &arg) {
 *      return arg->copy();     //correct
 *      return arg;             //incorrect
 *  }
 * @endcode
 */
class RiscOperators: public SymbolicSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = SymbolicSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    RiscOperators();                                    // for serialization

    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

    explicit RiscOperators(const BaseSemantics::StatePtr&, const SmtSolverPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    ~RiscOperators();

    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     *  TaintSemantics. */
    static RiscOperatorsPtr instanceFromRegisters(const RegisterDictionaryPtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators object with specified prototypical values.
     *
     *  An SMT solver may be specified as the second argument for convenience. See @ref solver for details. */
    static RiscOperatorsPtr instanceFromProtoval(const BaseSemantics::SValuePtr &protoval,
                                                 const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators object with specified state.  An SMT solver may be specified as the second argument
     *  for convenience. See @ref solver for details. */
    static RiscOperatorsPtr instanceFromState(const BaseSemantics::StatePtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr&,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to symbolic operators. This is a checked conversion--it
     *  will fail if @p x does not point to a SymbolicSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
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
    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) override;
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
    virtual BaseSemantics::SValuePtr fpConvert(const BaseSemantics::SValuePtr &a, SgAsmFloatType *aType,
                                               SgAsmFloatType *retType) override;
    virtual BaseSemantics::SValuePtr reinterpret(const BaseSemantics::SValuePtr&, SgAsmType*) override;

private:
    static Taintedness mergeTaintedness(const BaseSemantics::SValuePtr&, const BaseSemantics::SValue::Ptr&);
};

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::TaintSemantics::SValue);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::TaintSemantics::RiscOperators);
#endif

#endif
#endif
