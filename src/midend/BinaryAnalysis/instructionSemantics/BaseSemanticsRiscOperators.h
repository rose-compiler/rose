#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_RiscOperators_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_RiscOperators_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemanticsTypes.h>
#include <BinaryHotPatch.h>
#include <BinarySmtSolver.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/version.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC Operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for most instruction semantics RISC operators.
 *
 *  This class is responsible for defining the semantics of the RISC-like operations invoked by the translation object (e.g.,
 *  X86InstructionSemantics).  We omit the definitions for most of the RISC operations from the base class so that failure to
 *  implement them in a subclass is an error.
 *
 *  RISC operator arguments are, in general, SValue pointers.  However, if the width of a RISC operator's result depends on an
 *  argument's value (as opposed to depending on the argument width), then that argument must be a concrete value (i.e., an
 *  integral type).  This requirement is due to the fact that SMT solvers need to know the sizes of their bit
 *  vectors. Operators @ref extract, @ref unsignedExtend, @ref signExtend, @ref readRegister, and @ref readMemory fall into
 *  this category.
 *
 *  Operators with side effects (@ref writeRegister, @ref writeMemory, and possibly others) usually modify a @ref State object
 *  pointed to by the @ref currentState property. Keeping side effects in states allows @ref RiscOperators to be used in
 *  data-flow analysis where meeting control flow edges cause states to be merged.  Side effects that don't need to be part of
 *  a data-flow can be stored elsewhere, such as data members of a subclass or the @ref initialState property.
 *
 *  RiscOperator objects are allocated on the heap and reference counted.  The BaseSemantics::RiscOperator is an abstract class
 *  that defines the interface.  See the Rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts
 *  fit together. */
class RiscOperators: public boost::enable_shared_from_this<RiscOperators> {
    SValuePtr protoval_;                                // Prototypical value used for its virtual constructors
    StatePtr currentState_;                             // State upon which RISC operators operate
    StatePtr initialState_;                             // Lazily updated initial state; see readMemory
    SmtSolverPtr solver_;                               // Optional SMT solver
    SgAsmInstruction *currentInsn_;                     // Current instruction, as set by latest startInstruction call
    size_t nInsns_;                                     // Number of instructions processed
    std::string name_;                                  // Name to use for debugging
    HotPatch hotPatch_;                                 // Adjustments to the semantic state after each instruction.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(protoval_);
        s & BOOST_SERIALIZATION_NVP(currentState_);
        s & BOOST_SERIALIZATION_NVP(initialState_);
        s & BOOST_SERIALIZATION_NVP(solver_);
        s & BOOST_SERIALIZATION_NVP(currentInsn_);
        s & BOOST_SERIALIZATION_NVP(nInsns_);
        s & BOOST_SERIALIZATION_NVP(name_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(hotPatch_);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    // for serialization
    RiscOperators();

    explicit RiscOperators(const SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr());
    explicit RiscOperators(const StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr());

public:
    /** Shared-ownership pointer for a @ref RiscOperators object. See @ref heap_object_shared_ownership. */
    typedef RiscOperatorsPtr Ptr;

public:
    virtual ~RiscOperators();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors.  None needed since this class is abstract.


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors.
public:
    /** Virtual allocating constructor.  The @p protoval is a prototypical semantic value that is used as a factory to create
     *  additional values as necessary via its virtual constructors.  The state upon which the RISC operations operate must be
     *  set by modifying the  @ref currentState property. An optional SMT solver may be specified (see @ref solver). */
    virtual RiscOperatorsPtr create(const SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) const = 0;

    /** Virtual allocating constructor.  The supplied @p state is that upon which the RISC operations operate and is also used
     *  to define the prototypical semantic value. Other states can be supplied by setting @ref currentState. The prototypical
     *  semantic value is used as a factory to create additional values as necessary via its virtual constructors. An optional
     *  SMT solver may be specified (see @ref solver). */
    virtual RiscOperatorsPtr create(const StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts.  No-op since this is the base class.
public:
    static RiscOperatorsPtr promote(const RiscOperatorsPtr &x) {
        ASSERT_not_null(x);
        return x;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Other methods part of our API
public:
    /** Property: Prototypical semantic value.
     *
     *  The protoval is used to construct other values via its virtual constructors. */
    virtual SValuePtr protoval() const { return protoval_; }

    /** Property: Satisfiability module theory (SMT) solver.
     *
     *  This property holds a pointer to the satisfiability modulo theory (SMT) solver to use for certain operations.  An SMT
     *  solver is optional and not all semantic domains will make use of a solver.  Domains that use a solver will fall back to
     *  naive implementations when a solver is not available (for instance, equality of two values might be checked by looking
     *  at whether the values are identical).
     *
     * @{ */
    virtual SmtSolverPtr solver() const { return solver_; }
    virtual void solver(const SmtSolverPtr &s) { solver_ = s; }
    /** @} */

    /** Property: Post-instruction hot patches.
     *
     *  The hot patches describe how to modify the current state after each instruction is executed. This is performed by the
     *  @ref finishInstruction method.
     *
     * @{ */
    const HotPatch& hotPatch() const { return hotPatch_; }
    HotPatch& hotPatch() { return hotPatch_; }
    void hotPatch(const HotPatch &hp) { hotPatch_ = hp; }
    /** @} */

    /** Property: Current semantic state.
     *
     *  This is the state upon which the RISC operations operate. The state need not be set until the first instruction is
     *  executed (and even then, some RISC operations don't need any machine state; typically, only register and memory read
     *  and write operators need state).  Different state objects can be swapped in at pretty much any time.  Modifying the
     *  state has no effect on this object's prototypical value which was initialized by the constructor; new states should
     *  have a prototyipcal value of the same dynamic type.
     *
     *  See also, @ref initialState.
     *
     * @{ */
    virtual StatePtr currentState() const { return currentState_; }
    virtual void currentState(const StatePtr &s) { currentState_ = s; }
    /** @} */

    /** Property: Optional lazily updated initial state.
     *
     *  If non-null, then any calls to @ref readMemory or @ref readRegister which do not find that the address or register has
     *  a value, not only instantiate the value in the current state, but also write the same value to this initial state.  In
     *  effect, this is like Schrodinger's cat: every memory address and register has a value, we just don't know what it is
     *  until we try to read it.  Once we read it, it becomes instantiated in the current state and the initial state. The
     *  default initial state is the null pointer.
     *
     *  Changing the current state does not affect the initial state.  This makes it easier to use a state as part of a
     *  data-flow analysis, in which one typically swaps in different current states as the data-flow progresses.
     *
     *  The initial state need not be the same type as the current state, as long as they both have the same prototypical value
     *  type.  For instance, a symbolic domain could use a @ref MemoryCellList for its @ref currentState and a state based
     *  on a @ref MemoryMap of concrete values for its initial state, as long as those concrete values are converted to
     *  symbolic values when they're read.
     *
     *  <b>Caveats:</b> Not all semantic domains use the initial state. The order that values are added to an initial state
     *  depends on the order they're encountered during the analysis.
     *
     *  See also, @ref currentState.
     *
     *  @section example1 Example 1: Simple usage
     *
     *  This example, shows one way to use an initial state and the effect is has on memory and register I/O. It uses the same
     *  type for the initial state as it does for the current states.
     *
     *  @snippet testLazyInitialStates.C basicReadTest
     *
     *  @section example2 Example 2: Advanced usage
     *
     *  This example is somwewhat more advanced. It uses a custom state, which is a relatively common practice of users, and
     *  augments it to do something special when it's used as an initial state. When it's used as an initial state, it sets a
     *  flag for the values produced so that an analysis can presumably detect that the value is an initial value.
     *
     *  @snippet testLazyInitialStates.C advancedReadTest
     *
     * @{ */
    virtual StatePtr initialState() const { return initialState_; }
    virtual void initialState(const StatePtr &s) { initialState_ = s; }
    /** @} */

    /** Property: Name used for debugging.
     *
     *  This property is the name of the semantic domain and is used in diagnostic messages.
     *
     * @{ */
    virtual const std::string& name() const { return name_; }
    virtual void name(const std::string &s) { name_ = s; }
    /** @} */

    /** Print multi-line output for this object.
     * @{ */
    void print(std::ostream &stream, const std::string prefix="") const;
    virtual void print(std::ostream &stream, Formatter &fmt) const;
    /** @} */

    /** RiscOperators with formatter. See with_formatter(). */
    class WithFormatter {
        RiscOperatorsPtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const RiscOperatorsPtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing RISC operators with formatting. The usual way to use this is:
     * @code
     *  RiscOperatorsPtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     *
     * Since specifying a line prefix string for indentation purposes is such a common use case, the
     * indentation can be given instead of a format, as in the following code that indents the
     * prefixes each line of the expression with four spaces.
     *
     * @code
     *  std::cout <<"Machine state:\n" <<*(obj + "    ");
     * @code
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    WithFormatter operator+(const std::string &linePrefix);
    /** @} */

    /** Property: Number of instructions processed.
     *
     *  This counter is incremented at the beginning of each instruction.
     *
     * @{ */
    virtual size_t nInsns() const { return nInsns_; }
    virtual void nInsns(size_t n) { nInsns_ = n; }
    /** @} */

    /** Returns current instruction.
     *
     *  Returns the instruction which is being processed. This is set by @ref startInstruction and cleared by @ref
     *  finishInstruction. Returns null if we are not processing an instruction. */
    virtual SgAsmInstruction* currentInstruction() const {
        return currentInsn_;
    }

    /** Called at the beginning of every instruction.  This method is invoked every time the translation object begins
     *  processing an instruction.  Some policies use this to update a pointer to the current instruction. */
    virtual void startInstruction(SgAsmInstruction *insn);

    /** Called at the end of every instruction.  This method is invoked whenever the translation object ends processing for an
     *  instruction.  This is not called if there's an exception during processing. */
    virtual void finishInstruction(SgAsmInstruction *insn);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Value Construction Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The trailing underscores are necessary for for undefined_() on some machines, so we just add one to the end of all the
    // virtual constructors for consistency.

    /** Returns a new undefined value. Uses the prototypical value to virtually construct the new value.
     *
     * @{ */
    virtual SValuePtr undefined_(size_t nbits);
    virtual SValuePtr unspecified_(size_t nbits);
    /** @} */

    /** Returns a number of the specified bit width.  Uses the prototypical value to virtually construct a new value. */
    virtual SValuePtr number_(size_t nbits, uint64_t value);

    /** Returns a Boolean value. Uses the prototypical value to virtually construct a new value. */
    virtual SValuePtr boolean_(bool value);

    /** Returns a data-flow bottom value. Uses the prototypical value to virtually construct a new value. */
    virtual SValuePtr bottom_(size_t nbits);

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  x86-specific Operations (FIXME)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Invoked to filter call targets.  This method is called whenever the translation object is about to invoke a function
     *  call.  The target address is passed as an argument and a (new) target should be returned. */
    virtual SValuePtr filterCallTarget(const SValuePtr &a);

    /** Invoked to filter return targets.  This method is called whenever the translation object is about to return from a
     *  function call (such as for the x86 "RET" instruction).  The return address is passed as an argument and a (new) return
     *  address should be returned. */
    virtual SValuePtr filterReturnTarget(const SValuePtr &a);

    /** Invoked to filter indirect jumps.  This method is called whenever the translation object is about to unconditionally
     *  jump to a new address (such as for the x86 "JMP" instruction).  The target address is passed as an argument and a (new)
     *  target address should be returned. */
    virtual SValuePtr filterIndirectJumpTarget(const SValuePtr &a);

    /** Invoked for the x86 HLT instruction. */
    virtual void hlt() {}

    /** Invoked for the x86 CPUID instruction. FIXME: x86-specific stuff should be in the dispatcher. */
    virtual void cpuid() {}

    /** Invoked for the x86 RDTSC instruction. FIXME: x86-specific stuff should be in the dispatcher. */
    virtual SValuePtr rdtsc() { return unspecified_(64); }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Boolean Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Computes bit-wise AND of two values. The operands must both have the same width; the result must be the same width as
     *  the operands. */
    virtual SValuePtr and_(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Computes bit-wise OR of two values. The operands @p a and @p b must have the same width; the return value width will
     * be the same as @p a and @p b. */
    virtual SValuePtr or_(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Computes bit-wise XOR of two values. The operands @p a and @p b must have the same width; the result will be the same
     *  width as @p a and @p b. */
    virtual SValuePtr xor_(const SValuePtr &a, const SValuePtr &b) = 0;

    /** One's complement. The result will be the same size as the operand. */
    virtual SValuePtr invert(const SValuePtr &a) = 0;

    /** Extracts bits from a value.  The specified bits from begin_bit (inclusive) through end_bit (exclusive) are copied into
     *  the low-order bits of the return value (other bits in the return value are cleared). The least significant bit is
     *  number zero. The begin_bit and end_bit values must be valid for the width of @p a. */
    virtual SValuePtr extract(const SValuePtr &a, size_t begin_bit, size_t end_bit) = 0;

    /** Concatenates the bits of two values.  The bits of @p lowBits and @p highBits are concatenated so that the result has @p
     *  lowBits in the low-order bits and @p highBits in the high order bits. The width of the return value is the sum of the
     *  widths of @p lowBits and @p highBits.
     *
     *  Note that the order of arguments for this method is the reverse of the @ref SymbolicExpr concatenation function. */
    virtual SValuePtr concat(const SValuePtr &lowBits, const SValuePtr &highBits) = 0;

    /** Aliases for concatenation.
     *
     *  It's hard to remember whether the arguments of the concatenation operator are low bits followed by high bits, or high
     *  bits followed by low bits, and getting it wrong doesn't result in any kind of immediate error. This alias makes it more
     *  clear.
     *
     * @{ */
    virtual SValuePtr concatLoHi(const SValuePtr &lowBits, const SValuePtr &highBits) {
        return concat(lowBits, highBits);
    }
    virtual SValuePtr concatHiLo(const SValuePtr &highBits, const SValuePtr &lowBits) {
        return concat(lowBits, highBits);
    }
    /** @} */

    /** Split a value into two narrower values. Returns the two parts as a pair consisting of the low-order bits of @p a and
     *  the high-order bits of @p a. The returned low-order bits are bits zero (inclusive) to @p splitPoint (exclusvie) and
     *  has width @p splitPoint. The returned high-order bits are bits @p splitPoint (inclusive) to the width of @p a
     *  (exclusive) and has width which is the width of @p a minus @p splitPoint. This is not a pure virtual function because
     *  it can be implemented in terms of @ref extract. */
    virtual std::pair<SValuePtr /*low*/, SValuePtr /*high*/> split(const SValuePtr &a, size_t splitPoint);

    /** Returns position of least significant set bit; zero when no bits are set. The return value will have the same width as
     * the operand, although this can be safely truncated to the log-base-2 + 1 width. */
    virtual SValuePtr leastSignificantSetBit(const SValuePtr &a) = 0;

    /** Returns position of most significant set bit; zero when no bits are set. The return value will have the same width as
     * the operand, although this can be safely truncated to the log-base-2 + 1 width. */
    virtual SValuePtr mostSignificantSetBit(const SValuePtr &a) = 0;

    /** Count leading zero bits.
     *
     *  Counts the number of consecutive clear bits starting with the msb. The return value is the same width as the
     *  argument. */
    virtual SValuePtr countLeadingZeros(const SValuePtr &a);

    /** Count leading one bits.
     *
     *  Counts the number of consecutive set bits starting with the msb.  The return value is the same width as the
     *  argument. */
    virtual SValuePtr countLeadingOnes(const SValuePtr &a);

    /** Rotate bits to the left. The return value will have the same width as operand @p a.  The @p nbits is interpreted as
     *  unsigned. The behavior is modulo the width of @p a regardles of whether the implementation makes that a special case or
     *  handles it naturally. */
    virtual SValuePtr rotateLeft(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Rotate bits to the right. The return value will have the same width as operand @p a.  The @p nbits is interpreted as
     *  unsigned. The behavior is modulo the width of @p a regardles of whether the implementation makes that a special case or
     *  handles it naturally. */
    virtual SValuePtr rotateRight(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted left. The return value will have the same width as operand @p a.  The @p nbits is interpreted as
     *  unsigned. New bits shifted into the value are zero. If @p nbits is equal to or larger than the width of @p a then the
     *  result is zero. */
    virtual SValuePtr shiftLeft(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted right logically (no sign bit). The return value will have the same width as operand @p a. The @p
     *  nbits is interpreted as unsigned. New bits shifted into the value are zero. If  @p nbits is equal to or larger than the
     *  width of @p a then the result is zero. */
    virtual SValuePtr shiftRight(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted right arithmetically (with sign bit). The return value will have the same width as operand @p
     *  a. The @p nbits is interpreted as unsigned. New bits shifted into the value are the same as the most significant bit
     *  (the "sign bit"). If @p nbits is equal to or larger than the width of @p a then the result has all bits cleared or all
     *  bits set depending on whether the most significant bit was originally clear or set. */
    virtual SValuePtr shiftRightArithmetic(const SValuePtr &a, const SValuePtr &nbits) = 0;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Comparison Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /** Determines whether a value is equal to zero.  Returns true, false, or undefined (in the semantic domain) depending on
     *  whether argument is zero. */
    virtual SValuePtr equalToZero(const SValuePtr &a) = 0;

    /** If-then-else.  Returns operand @p a if @p cond is true, operand @p b if @p cond is false, or some other value if the
     *  condition is unknown. The @p condition must be one bit wide; the widths of @p a and @p b must be equal; the return
     *  value width will be the same as @p a and @p b. */
    virtual SValuePtr ite(const SValuePtr &cond, const SValuePtr &a, const SValuePtr &b) = 0;

    /** Equality comparison.
     *
     *  Returns a Boolean to indicate whether the relationship between @p a and @p b holds. Both operands must be the same
     *  width. It doesn't matter if they are interpreted as signed or unsigned quantities.
     *
     * @{ */
    virtual SValuePtr isEqual(const SValuePtr &a, const SValuePtr &b);
    virtual SValuePtr isNotEqual(const SValuePtr &a, const SValuePtr &b);
    /** @} */

    /** Comparison for unsigned values.
     *
     *  Returns a Boolean to indicate whether the relationship between @p a and @p b is true when @p a and @p b are interpreted
     *  as unsigned values.  Both values must have the same width.  This operation is a convenience wrapper around other RISC
     *  operators.
     *
     * @{ */
    virtual SValuePtr isUnsignedLessThan(const SValuePtr &a, const SValuePtr &b);
    virtual SValuePtr isUnsignedLessThanOrEqual(const SValuePtr &a, const SValuePtr &b);
    virtual SValuePtr isUnsignedGreaterThan(const SValuePtr &a, const SValuePtr &b);
    virtual SValuePtr isUnsignedGreaterThanOrEqual(const SValuePtr &a, const SValuePtr &b);
    /** @} */

    /** Comparison for signed values.
     *
     *  Returns a Boolean to indicate whether the relationship between @p a and @p b is true when @p a and @p b are interpreted
     *  as signed values.  Both values must have the same width.  This operation is a convenience wrapper around other RISC
     *  operators.
     *
     * @{ */
    virtual SValuePtr isSignedLessThan(const SValuePtr &a, const SValuePtr &b);
    virtual SValuePtr isSignedLessThanOrEqual(const SValuePtr &a, const SValuePtr &b);
    virtual SValuePtr isSignedGreaterThan(const SValuePtr &a, const SValuePtr &b);
    virtual SValuePtr isSignedGreaterThanOrEqual(const SValuePtr &a, const SValuePtr &b);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Integer Arithmetic Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Extend (or shrink) operand @p a so it is @p nbits wide by adding or removing high-order bits. Added bits are always
     *  zeros. The result will be the specified @p new_width. */
    virtual SValuePtr unsignedExtend(const SValuePtr &a, size_t new_width);

    /** Sign extends a value. The result will the the specified @p new_width, which must be at least as large as the original
     * width. */
    virtual SValuePtr signExtend(const SValuePtr &a, size_t new_width) = 0;

    /** Adds two integers of equal size.  The width of @p a and @p b must be equal; the return value will have the same width
     * as @p a and @p b. */
    virtual SValuePtr add(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Adds two integers of equal size and carry. The width of @p a and @p b must be the same, and the resulting sum will also
     *  have the same width. Returns the sum by value and a carry-out bit by reference.  An overflow bit is also returned and
     *  is useful when @p a and @p b are interpreted as signed values. This method is not pure abstract and is generally not
     *  overridden in subclasses because it can be implemented in terms of other operations. */
    virtual SValuePtr addCarry(const SValuePtr &a, const SValuePtr &b,
                               SValuePtr &carryOut /*out*/, SValuePtr &overflowed /*out*/);

    /** Subtract one value from another.
     *
     *  Subtracts the @p subtrahend from the @p minuend and returns the result. The two arguments must be the same width and
     *  the return value will also be that same width.  This method is not pure virtual and is not usually overridden by
     *  subclasses because it can be implemented in terms of other operations. */
    virtual SValuePtr subtract(const SValuePtr &minuend, const SValuePtr &subtrahend);

    /** Subtract one value from another and carry.
     *
     *  Subtracts the @p subtrahend from the @p minuend and returns the result. The two arguments must be the same width and
     *  the return value will also be that same width.  A carry-out bit and overflow bit are returned by reference. The carry
     *  out bit is simply the carry out from adding the minuend and negated subtrahend. The overflow bit is set if the result
     *  would overflow the width of the return value, and is calculated as the XOR of the two most significant carray-out
     *  bits. This method is not pure virtual and is not usually overridden by subclasses because it can be implemented in
     *  terms of other operations. */
    virtual SValuePtr subtractCarry(const SValuePtr &minuend, const SValuePtr &subtrahend,
                                    SValuePtr &carryOut /*out*/, SValuePtr &overflowed /*out*/);

    /** Add two values of equal size and a carry bit.  Carry information is returned via carry_out argument.  The carry_out
     *  value is the tick marks that are written above the first addend when doing long arithmetic like a 2nd grader would do
     *  (of course, they'd probably be adding two base-10 numbers).  For instance, when adding 00110110 and 11100100:
     *
     *  \code
     *    '''..'..         <-- carry tick marks: '=carry .=no carry
     *     00110110
     *   + 11100100
     *   ----------
     *    100011010
     *  \endcode
     *
     *  The carry_out value is 11100100.
     *
     *  The width of @p a and @p b must be equal; @p c must have a width of one bit; the return value and @p carry_out will be
     *  the same width as @p a and @p b.  The @p carry_out value is allocated herein. */
    virtual SValuePtr addWithCarries(const SValuePtr &a, const SValuePtr &b, const SValuePtr &c,
                                        SValuePtr &carry_out/*output*/) = 0;

    /** Two's complement. The return value will have the same width as the operand. */
    virtual SValuePtr negate(const SValuePtr &a) = 0;

    /** Divides two signed values. The width of the result will be the same as the width of the @p dividend. */
    virtual SValuePtr signedDivide(const SValuePtr &dividend, const SValuePtr &divisor) = 0;

    /** Calculates modulo with signed values. The width of the result will be the same as the width of operand @p b. */
    virtual SValuePtr signedModulo(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Multiplies two signed values. The width of the result will be the sum of the widths of @p a and @p b. */
    virtual SValuePtr signedMultiply(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Divides two unsigned values. The width of the result is the same as the width of the @p dividend. */
    virtual SValuePtr unsignedDivide(const SValuePtr &dividend, const SValuePtr &divisor) = 0;

    /** Calculates modulo with unsigned values. The width of the result is the same as the width of operand @p b. */
    virtual SValuePtr unsignedModulo(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Multiply two unsigned values. The width of the result is the sum of the widths of @p a and @p b. */
    virtual SValuePtr unsignedMultiply(const SValuePtr &a, const SValuePtr &b) = 0;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Interrupt and system calls
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Invoked for instructions that cause an interrupt.  The major and minor numbers are architecture specific.  For
     *  instance, an x86 INT instruction uses major number zero and the minor number is the interrupt number (e.g., 0x80 for
     *  Linux system calls), while an x86 SYSENTER instruction uses major number one. The minr operand for INT3 is -3 to
     *  distinguish it from the one-argument "INT 3" instruction which has slightly different semantics. */
    virtual void interrupt(int /*majr*/, int /*minr*/) {}


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Floating-point operations
    //
    // For now these all have default implementations that throw NotImplemented, but we might change them to pure virtual
    // sometime in the future so they're consistent with most other RISC operators. [Robb P. Matzke 2015-08-03]
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Construct a floating-point value from an integer value. */
    virtual SValuePtr fpFromInteger(const SValuePtr &intValue, SgAsmFloatType *fpType);

    /** Construct an integer value from a floating-point value.
     *
     *  The bits of @p fpValue are interpreted according to the @p fpType and converted to a signed integer value
     *  that fits in @p integerWidth bits. This is done by truncating the fractional part of the floating point number, thus
     *  rounding toward zero. If @p fpValue is not a number then @p dflt is returned. */
    virtual SValuePtr fpToInteger(const SValuePtr &fpValue, SgAsmFloatType *fpType, const SValuePtr &dflt);

    /** Convert from one floating-point type to another.
     *
     *  Converts the floating-point value @p a having type @p aType to the return value having @p retType. */
    virtual SValuePtr fpConvert(const SValuePtr &a, SgAsmFloatType *aType, SgAsmFloatType *retType);

    /** Whether a floating-point value is a special not-a-number bit pattern. */
    virtual SValuePtr fpIsNan(const SValuePtr &fpValue, SgAsmFloatType *fpType);

    /** Whether a floating-point value is denormalized. */
    virtual SValuePtr fpIsDenormalized(const SValuePtr &fpValue, SgAsmFloatType *fpType);

    /** Whether a floating-point value is equal to zero. */
    virtual SValuePtr fpIsZero(const SValuePtr &fpValue, SgAsmFloatType *fpType);

    /** Whether a floating-point value is infinity.
     *
     *  Returns true if the floating point value is plus or minus infinity.  Querying the sign bit will return the sign of the
     *  infinity. */
    virtual SValuePtr fpIsInfinity(const SValuePtr &fpValue, SgAsmFloatType *fpType);

    /** Sign of floating-point value.
     *
     *  Returns the value of the floating-point sign bit. */
    virtual SValuePtr fpSign(const SValuePtr &fpValue, SgAsmFloatType *fpType);

    /** Exponent of floating-point value.
     *
     *  Returns the exponent of the floating point value. For normalized values this returns the stored exponent minus the
     *  exponent bias.  For denormalized numbers this returns the stored exponent minus the exponent bias minus an additional
     *  amount to normalize the significand. */
    virtual SValuePtr fpEffectiveExponent(const SValuePtr &fpValue, SgAsmFloatType *fpType);

    /** Add two floating-point values.
     *
     *  Adds two floating-point values that have the same type and returns the sum in the same type. */
    virtual SValuePtr fpAdd(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType);

    /** Subtract one floating-point value from another.
     *
     *  Subtracts @p b from @p a and returns the difference. All three floating-point values have the same type.  The default
     *  implementation is in terms of negate and add. */
    virtual SValuePtr fpSubtract(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType);

    /** Multiply two floating-point values.
     *
     *  Multiplies two floating-point values and returns the product. All three values have the same type. */
    virtual SValuePtr fpMultiply(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType);

    /** Divide one floating-point value by another.
     *
     *  Computes @p a divided by @p b and returns the result. All three floating-point values have the same type. */
    virtual SValuePtr fpDivide(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType);

    /** Square root.
     *
     *  Computes and returns the square root of the specified floating-point value.  Both values have the same type. */
    virtual SValuePtr fpSquareRoot(const SValuePtr &a, SgAsmFloatType *fpType);

    /** Round toward zero.
     *
     *  Truncate the fractional part of the floating point number. */
    virtual SValuePtr fpRoundTowardZero(const SValuePtr &a, SgAsmFloatType *fpType);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Conversion operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Reinterpret an expression as a different type.
     *
     *  For semantic domains whose values don't carry type information this is a no-op. For other domains, this creates a
     *  new value having the same bits as the original value but a new type. The old and new types must be the same size. This
     *  is similar to a C++ @c reinterpret_cast. */
    virtual SValuePtr reinterpret(const SValuePtr &a, SgAsmType *retType);

    /** Convert value from one type to another.
     *
     *  This method converts the argument value from one type to another and returns the new value. This is more than what
     *  @ref reinterpret does. For instance, if the argument is an integer with the value "-42" and the @p dstType is an
     *  IEEE-754 32-bit floating-point type, then the return value will have an entirely different bit pattern that is "-42"
     *  as a floating-point value.
     *
     *  The @p srcType is necessary for semantic domains whose values don't carry type information, and is redundant for domains
     *  whose values do carry type information. In order to be more useful, the input value, @p a, is always reinterpreted as
     *  type @p srcType by calling @ref reinterpret before any conversion takes place. */
    virtual SValuePtr convert(const SValuePtr &a, SgAsmType *srcType, SgAsmType *dstType);

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  State Accessing Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /** Reads a value from a register.
     *
     *  The base implementation simply delegates to the current semantic State, which probably delegates to a register state,
     *  but subclasses are welcome to override this behavior at any level.
     *
     *  A register state will typically implement storage for hardware registers, but higher layers (the State, RiscOperators,
     *  Dispatcher, ...)  should not be concerned about the size of the register they're trying to read.  For example, a
     *  register state for a 32-bit x86 architecture will likely have a storage location for the 32-bit EAX register, but it
     *  should be possible to ask @ref readRegister to return the value of AX (the low-order 16-bits).  In order to accomplish
     *  this, some level of the readRegister delegations needs to invoke @ref extract to obtain the low 16 bits.  The
     *  RiscOperators object is passed along the delegation path for this purpose.  The inverse @ref concat operation will be
     *  needed at some level when we ask @ref readRegister to return a value that comes from multiple storage locations in the
     *  register state (such as can happen if an x86 register state holds individual status flags and we ask for the 32-bit
     *  EFLAGS register).
     *
     *  If the register state can distinguish between a register that has never been accessed and a register that has only been
     *  read, then the @p dflt value is stored into the register the first time it's read. This ensures that reading the
     *  register a second time with no intervening write will return the same value as the first read.  If a @p dflt is not
     *  provided then one is constructed by invoking @ref undefined_.
     *
     *  There needs to be a certain level of cooperation between the RiscOperators, State, and register state classes to decide
     *  which layer should invoke the @ref extract or @ref concat (or whatever other RISC operations might be necessary).
     *
     *  @{ */
    virtual SValuePtr readRegister(RegisterDescriptor reg) {   // old subclasses can still override this if they want,
        return readRegister(reg, undefined_(reg.nBits())); // but new subclasses should not override this method.
    }
    virtual SValuePtr readRegister(RegisterDescriptor reg, const SValuePtr &dflt); // new subclasses override this
    /** @} */

    /** Writes a value to a register.
     *
     *  The base implementation simply delegates to the current semantic State, which probably delegates to a register state,
     *  but subclasses are welcome to override this behavior at any level.
     *
     *  As with @ref readRegister, @ref writeRegister may need to perform various RISC operations in order to accomplish the
     *  task of writing a value to the specified register when the underlying register state doesn't actually store a value for
     *  that specific register. The RiscOperations object is passed along for that purpose.  See @ref readRegister for more
     *  details. */
    virtual void writeRegister(RegisterDescriptor reg, const SValuePtr &a);

    /** Obtain a register value without side effects.
     *
     *  This is a lower-level operation than @ref readRegister in that it doesn't cause the register to be marked as having
     *  been read. It is typically used in situations where the register is being accessed for analysis purposes rather than as
     *  part of an instruction emulation.
     *
     * @{ */
    virtual SValuePtr peekRegister(RegisterDescriptor, const SValuePtr &dflt);
    SValuePtr peekRegister(RegisterDescriptor reg) {
        return peekRegister(reg, undefined_(reg.nBits()));
    }
    /** @} */

    /** Reads a value from memory.
     *
     *  The implementation (in subclasses) will typically delegate much of the work to the current state's @ref
     *  State::readMemory "readMemory" method.
     *
     *  A MemoryState will implement storage for memory locations and might impose certain restrictions, such as "all memory
     *  values must be eight bits".  However, the @ref readMemory should not have these constraints so that it can be called
     *  from a variety of Dispatcher subclass (e.g., the DispatcherX86 class assumes that @ref readMemory is capable of reading
     *  32-bit values from little-endian memory). The designers of the MemoryState, State, and RiscOperators should collaborate
     *  to decide which layer (RiscOperators, State, or MemoryState) is reponsible for combining individual memory locations
     *  into larger values.  A RiscOperators object is passed along the chain of delegations for this purpose. The
     *  RiscOperators might also contain other data that's important during the process, such as an SMT solver.
     *
     *  The @p segreg argument is an optional segment register. Most architectures have a flat virtual address space and will
     *  pass a default-constructed register descriptor whose is_valid() method returns false.
     *
     *  The @p cond argument is a Boolean value that indicates whether this is a true read operation. If @p cond can be proven
     *  to be false then the read is a no-op and returns an arbitrary value.
     *
     *  The @p dflt argument determines the size of the value to be read. This argument is also passed along to the lower
     *  layers so that they can, if they desire, use it to initialize memory that has never been read or written before. */
    virtual SValuePtr readMemory(RegisterDescriptor segreg, const SValuePtr &addr, const SValuePtr &dflt,
                                 const SValuePtr &cond) = 0;

    /** Writes a value to memory.
     *
     *  The implementation (in subclasses) will typically delegate much of the work to the current state's @ref
     *  State::writeMemory "writeMemory" method.
     *
     *  The @p segreg argument is an optional segment register. Most architectures have a flat virtual address space and will
     *  pass a default-constructed register descriptor whose is_valid() method returns false.
     *
     *  The @p cond argument is a Boolean value that indicates whether this is a true write operation. If @p cond can be proved
     *  to be false then writeMemory is a no-op. */
    virtual void writeMemory(RegisterDescriptor segreg, const SValuePtr &addr, const SValuePtr &data,
                             const SValuePtr &cond) = 0;

    /** Read memory without side effects.
     *
     *  This is a lower-level operation than @ref readMemory in that it doesn't cause any side effects in the memory state. In
     *  all other respects, it's similar to @ref readMemory. */
    virtual SValuePtr peekMemory(RegisterDescriptor segreg, const SValuePtr &addr, const SValuePtr &dflt) = 0;
};

std::ostream& operator<<(std::ostream&, const RiscOperators&);
std::ostream& operator<<(std::ostream&, const RiscOperators::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperators, 1);

#endif
#endif
