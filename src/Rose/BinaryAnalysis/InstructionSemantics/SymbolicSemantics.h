#ifndef ROSE_BinaryAnalysis_InstructionSemantics_SymbolicSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_SymbolicSemantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellMap.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterStateGeneric.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#include <Cxx_GrammarSerialization.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/set.hpp>
#endif

#include <inttypes.h>
#include <map>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** A fully symbolic semantic domain.
*
*  This semantic domain can be used to emulate the execution of a single basic block of instructions.  It is similar in nature
*  to PartialSymbolicSemantics, but with a different type of semantics value (SValue): instead of values being a constant or
*  variable with offset, values here are expression trees.
*
*  <ul>
*    <li>SValue: the values stored in registers and memory and used for memory addresses.</li>
*    <li>MemoryCell: an address-expression/value-expression pair for memory.</li>
*    <li>MemoryState: the collection of MemoryCells that form a complete memory state.</li>
*    <li>RegisterState: the collection of registers that form a complete register state.</li>
*    <li>State: represents the state of the virtual machine&mdash;its registers and memory.</li>
*    <li>RiscOperators: the low-level operators called by instruction dispatchers (e.g., DispatcherX86).</li>
*  </ul>
*
*  If an SMT solver is supplied a to the RiscOperators then that SMT solver will be used to answer various questions such as
*  when two memory addresses can alias one another.  When an SMT solver is lacking, the questions will be answered by very
*  naive comparison of the expression trees. */
namespace SymbolicSemantics {

using LeafNode = SymbolicExpression::Leaf;
using LeafPtr = SymbolicExpression::LeafPtr;
using InteriorNode = SymbolicExpression::Interior;
using InteriorPtr = SymbolicExpression::InteriorPtr;
using ExprNode = SymbolicExpression::Node;
using ExprPtr = SymbolicExpression::Ptr;
using InsnSet = std::set<SgAsmInstruction*>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Boolean flags
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Boolean for allowing side effects. */
namespace AllowSideEffects {
    enum Flag {NO, YES};
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Merging symbolic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for a merge control object. */
using MergerPtr = Sawyer::SharedPointer<class Merger>;

/** Controls merging of symbolic values. */
class Merger: public BaseSemantics::Merger {
    size_t setSizeLimit_ = 1;
protected:
    Merger();

public:
    /** Shared-ownership pointer for a @ref Merger object. */
    typedef MergerPtr Ptr;

    /** Allocating constructor. */
    static Ptr instance();

    /** Allocating constructor. */
    static Ptr instance(size_t);

    /** Property: Maximum set size.
     *
     *  The maximum number of members in a set when merging two expressions.  For instance, when merging expressions "x" and
     *  "y" with a limit of one (the default), the return value is bottom, but if the size limit is two or more, the return
     *  value is (set x y).  Merging two sets (or a set and a singlton) works the same way: if the union of the two sets is
     *  larger than the size limit then bottom is returned, otherwise the union is returned.
     *
     *  A limit of zero has the same effect as a limit of one since a singleton set is represented by just the naked member
     *  (that is, (set x) gets simplified to just x).
     *
     * @{ */
    size_t setSizeLimit() const { return setSizeLimit_; }
    void setSizeLimit(size_t n) { setSizeLimit_ = n; }
    /** @} */
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for symbolic semantic value. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Formatter for symbolic values. */
class Formatter: public BaseSemantics::Formatter {
public:
    SymbolicExpression::Formatter expr_formatter;
};

/** Type of values manipulated by the SymbolicSemantics domain.
 *
 *  Values of type type are used whenever a value needs to be stored, such as memory addresses, the values stored at those
 *  addresses, the values stored in registers, the operands for RISC operations, and the results of those operations.
 *
 *  An SValue points to an expression composed of the ExprNode types defined in Rose/BinaryAnalysis/SymbolicExpression.h, and
 *  also stores the set of instructions that were used to define the value.  This provides a framework for some simple forms of
 *  value-based def-use analysis. See get_defining_instructions() for details.
 *
 *  @section symbolic_semantics_unknown Unknown versus Uninitialized Values
 *
 *  One sometimes needs to distinguish between registers (or other named storage locations) that contain an
 *  "unknown" value versus registers that have not been initialized. By "unknown" we mean a value that has no
 *  constraints except for its size (e.g., a register that contains "any 32-bit value").  By "uninitialized" we
 *  mean a register that still contains the value that was placed there before we started symbolically evaluating
 *  instructions (i.e., a register to which symbolic evaluation hasn't written a new value).
 *
 *  An "unknown" value might be produced by a RISC operation that is unable/unwilling to express its result
 *  symbolically.  For instance, the RISC "add(A,B)" operation could return an unknown/unconstrained result if
 *  either A or B are unknown/unconstrained (in other words, add(A,B) returns C). An unconstrained value is
 *  represented by a free variable. ROSE's SymbolicSemantics RISC operations never return unconstrained values, but
 *  rather always return a new expression (add(A,B) returns the symbolic expression A+B). However, user-defined
 *  subclasses of ROSE's SymbolicSemantics classes might return unconstrained values, and in fact it is quite
 *  common for a programmer to first stub out all the RISC operations to return unconstrained values and then
 *  gradually implement them as they're required.  When a RISC operation returns an unconstrained value, it should
 *  set the returned value's defining instructions to the CPU instruction that caused the RISC operation to be
 *  called (and possibly the union of the sets of instructions that defined the RISC operation's operands).
 *
 *  An "uninitialized" register (or other storage location) is a register that hasn't ever had a value written to
 *  it as a side effect of a machine instruction, and thus still contains the value that was initialially stored
 *  there before analysis started (perhaps by a default constructor).  Such values will generally be unconstrained
 *  (i.e., "unknown" as defined above) but will have an empty defining instruction set.  The defining instruction
 *  set is empty because the register contains a value that was not generated as the result of simulating some
 *  machine instruction.
 *
 *  Therefore, it is possible to destinguish between an uninitialized register and an unconstrained register by
 *  looking at its value.  If the value is a variable with an empty set of defining instructions, then it must be
 *  an initial value.  If the value is a variable but has a non-empty set of defining instructions, then it must be
 *  a value that came from some RISC operation invoked on behalf of a machine instruction.
 *
 *  One should note that a register that contains a variable is not necessarily unconstrained: another register
 *  might contain the same variable, in which case the two registers are constrained to have the same value,
 *  although that value could be anything.  Consider the following example:
 *
 *  Step 1: Initialize registers. At this point EAX contains v1[32]{}, EBX contains v2[32]{}, and ECX contains
 *  v3[32]{}. The notation "vN" is a variable, "[32]" means the variable is 32-bits wide, and "{}" indicates that
 *  the set of defining instructions is empty. Since the defining sets are empty, the registers can be considered
 *  to be "uninitialized" (more specifically, they contain initial values that were created by the symbolic machine
 *  state constructor, or by the user explicitly initializing the registers; they don't contain values that were
 *  put there as a side effect of executing some machine instruction).
 *
 *  Step 2: Execute an x86 "I1: MOV EAX, EBX" instruction that moves the value stored in EBX into the EAX register.
 *  After this instruction executes, EAX contains v2[32]{I1}, EBX contains v2[32]{}, and ECX contains
 *  v3[32]{}. Registers EBX and ECX continue to have empty sets of defining instructions and thus contain their
 *  initial values.  Reigister EAX refers to the same variable (v2) as EBX and therefore must have the same value
 *  as EBX, although that value can be any 32-bit value.  We can also tell that EAX no longer contains its initial
 *  value because the set of defining instructions is non-empty ({I1}).
 *
 *  Step 3: Execute the imaginary "I2: FOO ECX, EAX" instruction and presume that it performs an operation using
 *  ECX and EAX and stores the result in ECX.  The operation is implemented by a new user-defined RISC operation
 *  called "doFoo(A,B)". Furthermore, presume that the operation encoded by doFoo(A,B) cannot be represented by
 *  ROSE's expression trees either directly or indirectly via other expression tree operations. Therefore, the
 *  implementation of doFoo(A,B) is such that it always returns an unconstrained value (i.e., a new variable):
 *  doFoo(A,B) returns C.  After this instruction executes, EAX and EBX continue to contain the results they had
 *  after step 2, and ECX now contains v4[32]{I2}.  We can tell that ECX contains an unknown value (because its
 *  value is a variable)  that is 32-bits wide.  We can also tell that ECX no longer contains its initial value
 *  because its set of defining instructions is non-empty ({I2}).
 */
class SValue: public BaseSemantics::SValue {
public:
    /** Base type. */
    using Super = BaseSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

protected:
    /** The symbolic expression for this value.  Symbolic expressions are reference counted. */
    ExprPtr expr;

    /** Instructions defining this value.  Any instruction that saves the value to a register or memory location
     *  adds itself to the saved value. */
    InsnSet defs;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        roseAstSerializationRegistration(s);            // "defs" has SgAsmInstruction ASTs
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(expr);
        s & BOOST_SERIALIZATION_NVP(defs);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    SValue();                                           // needed for serialization
    explicit SValue(size_t nbits);
    SValue(size_t nbits, uint64_t number);
    SValue(ExprPtr expr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance();

    /** Instantiate a new data-flow bottom value of specified width. */
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
    /** Promote a base value to a SymbolicSemantics value.  The value @p v must have a SymbolicSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override virtual methods...
public:
    virtual bool isBottom() const override;

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override;

    virtual void hash(Combinatorics::Hasher&) const override;

protected: // when implementing use these names; but when calling, use the camelCase names
    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const override;
    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    // It's not possible to change the size of a symbolic expression in place. That would require that we recursively change
    // the size of the SymbolicExpression, which might be shared with many unrelated values whose size we don't want to affect.
    virtual void set_width(size_t nbits) override;
    virtual bool is_number() const override;
    virtual uint64_t get_number() const override;
    virtual std::string get_comment() const override;
    virtual void set_comment(const std::string&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Additional methods first declared in this class...
public:
    /** Substitute one value for another throughout a value.
     *
     *  For example, if this value is "(add esp_0, -12)" and we substitute "esp_0" with "(add stack_frame 4)", this method
     *  would return "(add stack_frame -8)".  It is also possible for the @p from value to be a more complicated
     *  expression. This method attempts to match @p from at all nodes of this expression and substitutes at eac node that
     *  matches.  The @p from and @p to must have the same width. The @p solver is optional and used during simplification of
     *  the result. */
    virtual SValuePtr substitute(const SValuePtr &from, const SValuePtr &to, const SmtSolverPtr &solver) const;

    /** Adds instructions to the list of defining instructions.
     *
     *  Adds the specified instruction and defining sets into this value and returns a reference to this value. See also
     *  add_defining_instructions().
     *
     * @{ */
    virtual void defined_by(SgAsmInstruction *insn, const InsnSet &set1, const InsnSet &set2, const InsnSet &set3);
    virtual void defined_by(SgAsmInstruction *insn, const InsnSet &set1, const InsnSet &set2);
    virtual void defined_by(SgAsmInstruction *insn, const InsnSet &set1);
    virtual void defined_by(SgAsmInstruction *insn);
    /** @} */

    /** Returns the expression stored in this value.
     *
     *  Expressions are reference counted; the reference count of the returned expression is not incremented. */
    virtual const ExprPtr& get_expression() const;

    /** Changes the expression stored in the value.
     * @{ */
    virtual void set_expression(const ExprPtr &new_expr);
    virtual void set_expression(const SValuePtr &source);
    /** @} */

    /** Returns the set of instructions that defined this value.
     *
     *  The return value is a flattened lattice represented as a set.  When analyzing this basic block starting with an initial
     *  default state:
     *
     *  @code
     *  1: mov eax, 2
     *  2: add eax, 1
     *  3: mov ebx, eax;
     *  4: mov ebx, 3
     *  @endcode
     *
     *  the defining set for the value stored in EAX will be instructions {1, 2} and the defining set for the value stored in
     *  EBX will be {4}.  Defining sets for values stored in other registers are the empty set. */
    virtual const InsnSet& get_defining_instructions() const;

    /** Adds definitions to the list of defining instructions.
     *
     *  Returns the number of items added that weren't already in the list of defining instructions.
     *
     * @{ */
    virtual size_t add_defining_instructions(const InsnSet &to_add);
    virtual size_t add_defining_instructions(const SValuePtr &source);
    virtual size_t add_defining_instructions(SgAsmInstruction *insn);
    /** @} */

    /** Set defining instructions.
     *
     *  This discards the old set of defining instructions and replaces it with the specified set.
     *
     *  @{ */
    virtual void set_defining_instructions(const InsnSet &new_defs);
    virtual void set_defining_instructions(const SValuePtr &source);
    virtual void set_defining_instructions(SgAsmInstruction *insn);
    /** @} */
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::RegisterStateGeneric RegisterState;
typedef BaseSemantics::RegisterStateGenericPtr RegisterStatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      List-based Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for symbolic list-based memory state. */
typedef boost::shared_ptr<class MemoryListState> MemoryListStatePtr;

/** Byte-addressable memory.
 *
 *  This class represents an entire state of memory via a list of memory cells.  The memory cell list is sorted in
 *  reverse chronological order and addresses that satisfy a "must-alias" predicate are pruned so that only the
 *  must recent such memory cell is in the table.
 *
 *  A memory write operation prunes away any existing memory cell that must-alias the newly written address, then
 *  adds a new memory cell to the front of the memory cell list.
 *
 *  A memory read operation scans the memory cell list in reverse chronological order to obtain the list of cells that
 *  may-alias the address being read (stopping when it hits a must-alias cell).  If no must-alias cell is found, then a new
 *  cell is added to the memory and the may-alias list.  In any case, if the may-alias list contains exactly one cell, that
 *  cell's value is returned; otherwise a CellCompressor is called.  The default CellCompressor either returns a McCarthy
 *  expression or the default value depending on whether an SMT solver is being used.
 *
 *  @sa MemoryMapState */
class MemoryListState: public BaseSemantics::MemoryCellList {
public:
    /** Base type. */
    using Super = BaseSemantics::MemoryCellList;

    /** Shared-ownership pointer. */
    using Ptr = MemoryListStatePtr;

    /** Functor for handling a memory read that found more than one cell that might alias the requested address. */
    class CellCompressor: public Sawyer::SharedObject {
    public:
        using Ptr = Sawyer::SharedPointer<CellCompressor>;
    protected:
        CellCompressor() {}
    public:
        virtual ~CellCompressor() {}

        /** Compress the cells into a single value. */
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const BaseSemantics::CellList &cells) = 0;
    };

    /** Functor for handling a memory read whose address matches more than one memory cell.  This functor returns a symbolic
     * expression that consists of a read operation on a memory state.  The returned expression is essentially a McCarthy
     * expression that encodes this if-then-else structure:
     *
     * @code
     *  define readMemory(Address A): {
     *     if A == Cell[0].address then return Cell[0].value
     *     else if A == Cell[1].address then return Cell[1].value
     *     else if A == Cell[2].address then return Cell[2].value
     *     ...
     *  }
     * @endcode
     */
    class CellCompressorMcCarthy: public CellCompressor {
    public:
        static Ptr instance();                          /**< Allocating constructor. */
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const BaseSemantics::CellList &cells) override;
    };

    /** Functor for handling a memory read whose address matches more than one memory cell.  Simply returns the @p dflt value. */
    class CellCompressorSimple: public CellCompressor {
    public:
        static Ptr instance();                          /**< Allocating constructor. */
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const BaseSemantics::CellList &cells) override;
    };

    /** Functor for handling a memory read whose address matches more than one memory cell.  This is the default cell
     *  compressor and simply calls either CellCompressionMcCarthy or CellCompressionSimple depending on whether an SMT
     *  solver is being used. */
    class CellCompressorChoice: public CellCompressor {
        CellCompressor::Ptr mccarthy_;
        CellCompressor::Ptr simple_;
    protected:
        CellCompressorChoice();
    public:
        static Ptr instance();                          /**< Allocating constructor. */
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const BaseSemantics::CellList &cells) override;
    };

    /** Functor for handling a memory read whose address matches more than one memory cell.
     *
     *  The return value is the set of possibly matching values. */
    class CellCompressorSet: public CellCompressor {
    public:
        static Ptr instance();                          /**< Allocating constructor. */
        virtual SValuePtr operator()(const SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                     BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps,
                                     const BaseSemantics::CellList &cells) override;
    };

private:
    CellCompressor::Ptr cellCompressor_;                // Callback when a memory read aliases multiple memory cells.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
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
    MemoryListState();                                  // for serialization
    explicit MemoryListState(const BaseSemantics::MemoryCellPtr &protocell);
    MemoryListState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);
    MemoryListState(const MemoryListState &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new memory state having specified prototypical cells and value. */
    static MemoryListStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell);

    /** Instantiates a new memory state having specified prototypical value.  This constructor uses BaseSemantics::MemoryCell
     * as the cell type. */
    static  MemoryListStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);

    /** Instantiates a new deep copy of an existing state. */
    static MemoryListStatePtr instance(const MemoryListStatePtr &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual constructor. Creates a memory state having specified prototypical value.  This constructor uses
     * BaseSemantics::MemoryCell as the cell type. */
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const override;

    /** Virtual constructor. Creates a new memory state having specified prototypical cells and value. */
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell) const override;

    /** Virtual copy constructor. Creates a new deep copy of this memory state. */
    virtual BaseSemantics::AddressSpacePtr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Recasts a base pointer to a symbolic memory state. This is a checked cast that will fail if the specified pointer does
     *  not have a run-time type that is a SymbolicSemantics::MemoryListState or subclass thereof. */
    static MemoryListStatePtr promote(const BaseSemantics::AddressSpacePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    /** Read a byte from memory.
     *
     *  In order to read a multi-byte value, use RiscOperators::readMemory(). */
    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) override;

    /** Read a byte from memory with no side effects.
     *
     *  In order to read a multi-byte value, use RiscOperators::peekMemory(). */
    virtual BaseSemantics::SValuePtr peekMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) override;

    /** Write a byte to memory.
     *
     *  In order to write a multi-byte value, use RiscOperators::writeMemory(). */
    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) override;

protected:
    BaseSemantics::SValuePtr readOrPeekMemory(const BaseSemantics::SValuePtr &address,
                                              const BaseSemantics::SValuePtr &dflt,
                                              BaseSemantics::RiscOperators *addrOps,
                                              BaseSemantics::RiscOperators *valOps,
                                              AllowSideEffects::Flag allowSideEffects);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared in this class
public:
    /** Callback for handling a memory read whose address matches more than one memory cell.
     *
     *  See also, cell_compression_mccarthy(), cell_compression_simple(), cell_compression_choice().
     *
     * @{ */
    CellCompressor::Ptr cellCompressor() const;
    void cellCompressor(const CellCompressor::Ptr&);
    /** @} */

    // Deprecated [Robb Matzke 2021-12-15]
    CellCompressor::Ptr get_cell_compressor() const ROSE_DEPRECATED("use cellCompressor");
    void set_cell_compressor(const CellCompressor::Ptr&) ROSE_DEPRECATED("use cellCompressor");
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Map-based Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to symbolic memory state. */
typedef boost::shared_ptr<class MemoryMapState> MemoryMapStatePtr;

/** Byte-addressable memory.
 *
 *  This class represents an entire state of memory via a map of memory cells.  The cells are indexed in the map using the hash
 *  of their symbolic virtual address, therefore querying using an address that is equal but structurally different will fail
 *  to find the cell. This memory state does not resolve aliasing.  For instance, storing a value at virtual address esp + 24
 *  and then querying ebp + 8 will always assume that they are two non-aliasing addresses unless ROSE is able to simplify one
 *  of the expressions to exactly match the other.
 *
 *  Although this state has less precision than the list-based state (@ref MemoryListState), it operatates in logorithmic time
 *  instead of linear time, and by using hashing it avoids a relatively expensive comparison of address expressions at each
 *  step.
 *
 *  This class should not be confused with @ref MemoryMap. The former is used by instruction semantics to represent the state
 *  of memory such as during data-flow, while the latter is a model for mapping concrete values to concrete addresses similar
 *  to how operating systems map parts of files into an address space.
 *
 *  @sa MemoryListState */
class MemoryMapState: public BaseSemantics::MemoryCellMap {
public:
    /** Base type. */
    using Super = BaseSemantics::MemoryCellMap;

    /** Shared-ownership pointer. */
    using Ptr = MemoryMapStatePtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
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
    MemoryMapState();                                   // for serialization

    explicit MemoryMapState(const BaseSemantics::MemoryCellPtr &protocell);

    MemoryMapState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new memory state having specified prototypical cells and value. */
    static MemoryMapStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell);

    /** Instantiates a new memory state having specified prototypical value.  This constructor uses BaseSemantics::MemoryCell
     *  as the cell type. */
    static MemoryMapStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval);

    /** Instantiates a new deep copy of an existing state. */
    static MemoryMapStatePtr instance(const MemoryMapStatePtr &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual constructor. Creates a memory state having specified prototypical value.  This constructor uses
     * BaseSemantics::MemoryCell as the cell type. */
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const override;

    /** Virtual constructor. Creates a new memory state having specified prototypical cells and value. */
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell) const;

    /** Virtual copy constructor. Creates a new deep copy of this memory state. */
    virtual BaseSemantics::AddressSpacePtr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Recasts a base pointer to a symbolic memory state. This is a checked cast that will fail if the specified pointer does
     *  not have a run-time type that is a SymbolicSemantics::MemoryMapState or subclass thereof. */
    static MemoryMapStatePtr promote(const BaseSemantics::AddressSpacePtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we override from the super class (documented in the super class)
public:
    virtual CellKey generateCellKey(const BaseSemantics::SValuePtr &addr_) const override;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Default memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// List-base memory was the type originally used by this domain. We must keep it that way because some analysis, including 3rd
// party, assumes that the state is list-based.  New analysis can use the map-based state by instantiating it when the symbolic
// risc operators are constructed.
typedef MemoryListState MemoryState;
typedef MemoryListStatePtr MemoryStatePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::State State;
typedef BaseSemantics::StatePtr StatePtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** How to update the list of writers stored at each abstract location. */
enum WritersMode {
    TRACK_NO_WRITERS,                                   /**< Do not track writers. */
    TRACK_LATEST_WRITER,                                /**< Save only the latest writer. */
    TRACK_ALL_WRITERS                                   /**< Save all writers. */
};

/** How to update the list of definers stored in each semantic value. */
enum DefinersMode {
    TRACK_NO_DEFINERS,                                  /**< Do not track definers. */
    TRACK_LATEST_DEFINER,                               /**< Save only the latest definer. */
    TRACK_ALL_DEFINERS                                  /**< Save all definers. */
};

/** Shared-ownership pointer to symbolic RISC operations. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Defines RISC operators for the SymbolicSemantics domain.
 *
 *  These RISC operators depend on functionality introduced into the SValue class hierarchy at the SymbolicSemantics::SValue
 *  level. Therefore, the prototypical value supplied to the constructor or present in the supplied state object must have a
 *  dynamic type which is a SymbolicSemantics::SValue.
 *
 *  The RiscOperators object also controls whether use-def information is computed and stored in the SValues.  The default is
 *  to not compute this information.  The set_compute_usedef() method can be used to enable this feature.
 *
 *  Each RISC operator should return a newly allocated semantic value so that the caller can adjust definers for the result
 *  without affecting any of the inputs. For example, a no-op that returns its argument should be implemented like this:
 *
 * @code
 *  BaseSemantics::SValuePtr noop(const BaseSemantics::SValuePtr &arg) {
 *      return arg->copy();     //correct
 *      return arg;             //incorrect
 *  }
 * @endcode
 */
class RiscOperators: public BaseSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = BaseSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

protected:
    bool omit_cur_insn;                                 // if true, do not include cur_insn as a definer
    DefinersMode computingDefiners_;                    // whether to track definers (instruction VAs) of SValues
    WritersMode computingMemoryWriters_;                // whether to track writers (instruction VAs) to memory.
    WritersMode computingRegisterWriters_;              // whether to track writers (instruction VAs) to registers.
    uint64_t trimThreshold_;                            // max size of expressions (zero means no maximimum)
    bool reinterpretMemoryReads_;                       // cast data to unsigned integer when reading from memory
    bool reinterpretRegisterReads_;                     // cast data to unsigned integer when reading from registers
    size_t nTrimmed_ = 0;                               // number of expressions trimmed down to a new variable


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(omit_cur_insn);
        s & BOOST_SERIALIZATION_NVP(computingDefiners_);
        s & BOOST_SERIALIZATION_NVP(computingMemoryWriters_);
        s & BOOST_SERIALIZATION_NVP(computingRegisterWriters_);
        s & BOOST_SERIALIZATION_NVP(trimThreshold_);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    RiscOperators();                                    // for serialization

    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver);

    explicit RiscOperators(const BaseSemantics::StatePtr &state, const SmtSolverPtr &solver);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    ~RiscOperators();

    /** Instantiates a new RiscOperators object and configures it to use semantic values and states that are defaults for
     * SymbolicSemantics. */
    static RiscOperatorsPtr instanceFromRegisters(const RegisterDictionaryPtr&, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Instantiates a new RiscOperators object with specified prototypical values.  An SMT solver may be specified as the
     *  second argument for convenience. See @ref solver for details. */
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
    // Inherited methods for constructing values.
public:
    virtual BaseSemantics::SValuePtr boolean_(bool b) override;
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // New methods for constructing values, so we don't have to write so many SValue::promote calls in the RiscOperators
    // implementations.
    SValuePtr svalueExpr(const ExprPtr &expr, const InsnSet &defs=InsnSet());

protected:
    SValuePtr svalueUndefined(size_t nbits);
    SValuePtr svalueBottom(size_t nbits);
    SValuePtr svalueUnspecified(size_t nbits);
    SValuePtr svalueNumber(size_t nbits, uint64_t value);
    SValuePtr svalueBoolean(bool b);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Configuration properties
public:

    /** Property: Track which instructions define a semantic value.
     *
     *  Each semantic value (@ref SymbolicSemantics::SValue) is capable of storing a set of instruction addresses. This
     *  property controls how operations that produce new semantic values adjust those definers-sets in the new value.
     *
     *  @li @c TRACK_NO_DEFINERS: Each new semantic value will have a default-constructed definers-set (probably empty). Using
     *      this setting makes the definers-set available for other uses.
     *
     *  @li @c TRACK_LATEST_DEFINER: The new values will have the default-constructed definers-set unioned with the address of
     *      the current instruction (if there is a current instruction).
     *
     *  @li @c TRACK_ALL_DEFINERS: The new values will have a default-constructed definers-set unioned with the address of the
     *      current instruciton (if there is one), and the addresses of the definers-sets of the operands. Certain operations
     *      are able to simplify these sets. For example, an exclusive-or whose two operands are equal will return a zero
     *      result whose only definer is the current instruction.
     *
     * @{ */
    void computingDefiners(DefinersMode m) { computingDefiners_ = m; }
    DefinersMode computingDefiners() const { return computingDefiners_; }
    /** @} */

    /** Property: Track which instructions write to each memory location.
     *
     *  Each memory location stores a set of addresses that represent the instructions that wrote to that location. This
     *  property controls how each @ref writeMemory operation updates that set.
     *
     *  @li @c TRACK_NO_WRITERS: Does not update the memory state's writers information. Using this setting will make that
     *      data structure available for other purposes. The data structure can store a set of addresses independently for each
     *      memory cell.
     *
     *  @li @c TRACK_LATEST_WRITER:  Each write operation clobbers all previous write information for the affected
     *      memory address and stores the address of the current instruction (if there is one).
     *
     *  @li @c TRACK_ALL_WRITERS: Each write operation inserts the instruction address into the set of addresses stored for the
     *      affected memory cell without removing any addresses that are already associated with that cell. While this works
     *      well for analysis over a small region of code (like a single function), it might cause the writer sets to become
     *      very large when the same memory state is used over large regions (like a whole program).
     *
     * @{ */
    void computingMemoryWriters(WritersMode m) { computingMemoryWriters_ = m; }
    WritersMode computingMemoryWriters() const { return computingMemoryWriters_; }
    /** @} */

    /** Property: Track latest writer to each register.
     *
     *  Controls whether each @ref writeRegister operation updates the list of writers.  The following values are allowed for
     *  this property:
     *
     *  @li @c TRACK_NO_WRITERS: Does not update the register state's writers information. Using this setting will make that
     *      data structure available for other purposes. The data structure can store a set of addresses independently for each
     *      bit of each register.
     *
     *  @li @c TRACK_LATEST_WRITER:  Each write operation clobbers all previous write information for the affected
     *      register. This information is stored per bit so that if instruction 1 writes to EAX and then instruction 2 writes
     *      to AX then the high-order 16 bits of EAX will have {1} as the writer set while the low order bits will have {2} as
     *      its writer set.
     *
     *  @li @c TRACK_ALL_WRITERS: Each write operation inserts the instruction address into the set of addresses stored for the
     *      affected register (or register part) without removing any addresses that are already associated with that
     *      register. While this works well for analysis over a small region of code (like a single function), it might cause
     *      the writer sets to become very large when the same register state is used over large regions (like a whole
     *      program).
     *
     * @{ */
    void computingRegisterWriters(WritersMode m) { computingRegisterWriters_ = m; }
    WritersMode computingRegisterWriters() const { return computingRegisterWriters_; }
    /** @} */

    // Used internally to control whether cur_insn should be omitted from the list of definers.
    bool getset_omit_cur_insn(bool b) { bool retval = omit_cur_insn; omit_cur_insn=b; return retval; }

    /** Property: Maximum size of expressions.
     *
     *  Symbolic expressions can get very large very quickly. This property controls how large a symbolic expression can grow
     *  before it's substituted with a new variable.  The default, zero, means to never limit the size of expressions.
     *
     * @{ */
    void trimThreshold(uint64_t n) { trimThreshold_ = n; }
    uint64_t trimThreshold() const { return trimThreshold_; }
    /** @} */

    /** Property: Number of symbolic expressions trimmed.
     *
     *  Each time the trim threshold causes an expression to be replaced by a new variable, this property is incremented. The
     *  counter starts at zero when this object is created, but can be adjusted (usually back to zero) by the user.
     *
     * @{ */
    size_t nTrimmed() const { return nTrimmed_; }
    void nTrimmed(size_t n) { nTrimmed_ = n; }
    /** @} */

    /** Property: Reinterpret data as unsigned integers when reading from memory or registers.
     *
     *  If this property is set, then a call to @ref reinterpret is used to convert the return value to an unsigned integer if
     *  necessary.  This property should normally be enabled because many of the older parts of ROSE assume that memory only
     *  contains integers.
     *
     * @{ */
    bool reinterpretMemoryReads() const { return reinterpretMemoryReads_; }
    void reinterpretMemoryReads(bool b) { reinterpretMemoryReads_ = b; }
    bool reinterpretRegisterReads() const { return reinterpretRegisterReads_; }
    void reinterpretRegisterReads(bool b) { reinterpretRegisterReads_ = b; }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first defined at this level of the class hierarchy
public:
    /** Substitute all occurrences of @p from with @p to in the current state.  For instance, in functions that use a frame
     *  pointer set up with "push ebp; mov ebp, esp", it is convenient to see stack offsets in terms of the function's stack
     *  frame rather than in terms of the original esp value.  This convenience comes from the fact that compilers tend to
     *  emit stack accessing code where the addresses are offsets from the function's stack frame.
     *
     *  For instance, after the "push ebp; mov ebp, esp" prologue, the machine state is:
     * @code
     *  registers:
     *    esp    = (add[32] esp_0[32] -4[32])
     *    ebp    = (add[32] esp_0[32] -4[32])
     *    eip    = 0x080480a3[32]
     *  memory:
     *    addr=(add[32] esp_0[32] -1[32]) value=(extract[8] 24[32] 32[32] ebp_0[32])
     *    addr=(add[32] esp_0[32] -2[32]) value=(extract[8] 16[32] 24[32] ebp_0[32])
     *    addr=(add[32] esp_0[32] -3[32]) value=(extract[8] 8[32] 16[32] ebp_0[32])
     *    addr=(add[32] esp_0[32] -4[32]) value=(extract[8] 0[32] 8[32] ebp_0[32])
     * @endcode
     *
     *  If we create a new variable called "stack_frame" where
     *
     * @code
     *  stack_frame = esp_0 - 4
     * @endcode
     *
     *  Solving for esp_0:
     *
     * @code
     *  esp_0 = stack_frame + 4
     * @endcode
     *
     * Then replacing the lhs (esp_0) with the rhs (stack_frame + 4) in the machine state causes the expressions to be
     * rewritten in terms of stack_frame instead of esp_0:
     *
     * @code
     *  registers:
     *    esp    = stack_frame[32]
     *    ebp    = stack_frame[32]
     *    eip    = 0x080480a3[32]
     *  memory:
     *    addr=(add[32] stack_frame[32] 3[32]) value=(extract[8] 24[32] 32[32] ebp_0[32])
     *    addr=(add[32] stack_frame[32] 2[32]) value=(extract[8] 16[32] 24[32] ebp_0[32])
     *    addr=(add[32] stack_frame[32] 1[32]) value=(extract[8] 8[32] 16[32] ebp_0[32])
     *    addr=stack_frame[32] value=(extract[8] 0[32] 8[32] ebp_0[32])
     * @endcode
     *
     * Here's the source code for that substitution:
     *
     * @code
     *  SymbolicSemantics::RiscOperatorsPtr operators = ...;
     *  SymbolicSemantics::SValuePtr original_esp = ...; //probably read from the initial state
     *  BaseSemantics::SValuePtr stack_frame = operators->undefined_(32);
     *  stack_frame->comment("stack_frame"); //just so output looks nice
     *  SymbolicSemantics::SValuePtr rhs = SymbolicSemantics::SValue::promote(
     *      operators->add(stack_frame, operators->number_(32, 4))
     *  );
     *
     *  std::cerr <<"Prior to state:\n" <<*operators;
     *  operators->substitute(original_esp, rhs);
     *  std::cerr <<"Substituted state:\n" <<*operators;
     * @endcode
     */
    virtual void substitute(const SValuePtr &from, const SValuePtr &to);

    /** Filters results from RISC operators.
     *
     *  Checks that the size of the specified expression doesn't exceed the @ref trimThreshold. If not (or the threshold is
     *  zero), returns the argument, otherwise returns a new variable. */
    virtual BaseSemantics::SValuePtr filterResult(const BaseSemantics::SValuePtr&);

    /** Tests whether a SgAsmType is an IEEE-754 floating-point type.
     *
     *  If the argument is an IEEE-754 floating-point type then returns the argument dynamically cast to a @ref SgAsmFloatType,
     *  otherwise returns NULL.  Not all @ref SgAsmFloatType objects are IEEE-754 floating-point types. */
    static SgAsmFloatType* sgIsIeee754(SgAsmType*);

    /** Convert a SgAsmType to a symbolic type.
     *
     *  If the @ref SgAsmType cannot be converted to a @ref SymbolicExpression::Type then throws @ref Exception. */
    virtual SymbolicExpression::Type sgTypeToSymbolicType(SgAsmType*);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.  These are the RISC operators that are invoked by a Dispatcher.
public:
    virtual void interrupt(int majr, int minr) override;
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
    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) override;
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) override;
    virtual void writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a_) override;
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

public:
    BaseSemantics::SValuePtr readOrPeekMemory(RegisterDescriptor segreg,
                                              const BaseSemantics::SValuePtr &addr,
                                              const BaseSemantics::SValuePtr &dflt,
                                              AllowSideEffects::Flag);
};

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::SValue);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::MemoryListState);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::MemoryMapState);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::RiscOperators);
#endif

#endif
#endif
