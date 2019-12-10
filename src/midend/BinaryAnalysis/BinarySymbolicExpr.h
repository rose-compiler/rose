#ifndef ROSE_BinaryAnalysis_SymbolicExpr_H
#define ROSE_BinaryAnalysis_SymbolicExpr_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include "Map.h"

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/unordered_map.hpp>
#include <cassert>
#include <inttypes.h>
#include <RoseException.h>
#include <Sawyer/Attribute.h>
#include <Sawyer/BitVector.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Set.h>
#include <Sawyer/SharedPointer.h>
#include <Sawyer/SmallObject.h>
#include <set>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {

class SmtSolver;
typedef Sawyer::SharedPointer<SmtSolver> SmtSolverPtr;

/** Namespace supplying types and functions for symbolic expressions.
 *
 *  These are used by certain instruction semantics policies and satisfiability modulo theory (SMT) solvers. These expressions
 *  are tailored to bit-vector and integer difference logics, whereas the expression nodes in other parts of ROSE have
 *  different goals. */
namespace SymbolicExpr {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Basic Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Whether to use abbreviated or full output. */
namespace TypeStyle {
    /** Flag to pass as type stringification style. */
    enum Flag {
        FULL,                                           /**< Show all details. */
        ABBREVIATED                                     /**< Use abbreviated names if possible. */
    };
}


/** Exceptions for symbolic expressions. */
class Exception: public Rose::Exception {
public:
    explicit Exception(const std::string &mesg): Rose::Exception(mesg) {}
};

/** Operators for interior nodes of the expression tree.
 *
 *  Commutative operators generally take one or more operands.  Operators such as shifting, extending, and truncating have the
 *  size operand appearing before the bit vector on which to operate (this makes the output more human-readable since the size
 *  operand is often a constant). */
enum Operator {
    OP_ADD,                 /**< Addition. One or more operands, all the same width. */
    OP_AND,                 /**< Bitwise conjunction. One or more operands all the same width. */
    OP_ASR,                 /**< Arithmetic shift right. Operand B shifted by A bits; 0 <= A < width(B). A is unsigned. */
    OP_CONCAT,              /**< Concatenation. Operand A becomes high-order bits. Any number of operands. Result is integer type. */
    OP_EQ,                  /**< Equality. Two operands, both the same width. */
    OP_EXTRACT,             /**< Extract subsequence of bits. Extract bits [A..B) of C. 0 <= A < B <= width(C). */
    OP_INVERT,              /**< Bitwise inversion. One operand. */
    OP_ITE,                 /**< If-then-else. A must be one bit. Returns B if A is set, C otherwise. */
    OP_LET,                 /**< Let expression. Deferred substitution. Substitutes A for B in C. */
    OP_LSSB,                /**< Least significant set bit or zero. One operand. */
    OP_MSSB,                /**< Most significant set bit or zero. One operand. */
    OP_NE,                  /**< Inequality. Two operands, both the same width. */
    OP_NEGATE,              /**< Arithmetic negation. One operand. For Booleans, use OP_INVERT (2's complement is a no-op). */
    OP_NOOP,                /**< No operation. Used only by the default constructor. */
    OP_OR,                  /**< Bitwise disjunction. One or more operands all the same width. */
    OP_READ,                /**< Read a value from memory.  Arguments are the memory state and the address expression. */
    OP_ROL,                 /**< Rotate left. Rotate bits of B left by A bits.  0 <= A < width(B). A is unsigned. */
    OP_ROR,                 /**< Rotate right. Rotate bits of B right by A bits. 0 <= B < width(B). A is unsigned.  */
    OP_SDIV,                /**< Signed division. Two operands, A/B. Result width is width(A). */
    OP_SET,                 /**< Set of expressions. Any number of operands in any order. */
    OP_SEXTEND,             /**< Signed extension at msb. Extend B to A bits by replicating B's most significant bit. */
    OP_SGE,                 /**< Signed greater-than-or-equal. Two operands of equal width. Result is Boolean. */
    OP_SGT,                 /**< Signed greater-than. Two operands of equal width. Result is Boolean. */
    OP_SHL0,                /**< Shift left, introducing zeros at lsb. Bits of B are shifted by A, where 0 <=A < width(B). */
    OP_SHL1,                /**< Shift left, introducing ones at lsb. Bits of B are shifted by A, where 0 <=A < width(B). */
    OP_SHR0,                /**< Shift right, introducing zeros at msb. Bits of B are shifted by A, where 0 <=A <width(B). */
    OP_SHR1,                /**< Shift right, introducing ones at msb. Bits of B are shifted by A, where 0 <=A <width(B). */
    OP_SLE,                 /**< Signed less-than-or-equal. Two operands of equal width. Result is Boolean. */
    OP_SLT,                 /**< Signed less-than. Two operands of equal width. Result is Boolean. */
    OP_SMOD,                /**< Signed modulus. Two operands, A%B. Result width is width(B). */
    OP_SMUL,                /**< Signed multiplication. Two operands A*B. Result width is width(A)+width(B). */
    OP_UDIV,                /**< Signed division. Two operands, A/B. Result width is width(A). */
    OP_UEXTEND,             /**< Unsigned extention at msb. Extend B to A bits by introducing zeros at the msb of B. */
    OP_UGE,                 /**< Unsigned greater-than-or-equal. Two operands of equal width. Boolean result. */
    OP_UGT,                 /**< Unsigned greater-than. Two operands of equal width. Result is Boolean. */
    OP_ULE,                 /**< Unsigned less-than-or-equal. Two operands of equal width. Result is Boolean. */
    OP_ULT,                 /**< Unsigned less-than. Two operands of equal width. Result is Boolean (1-bit vector). */
    OP_UMOD,                /**< Unsigned modulus. Two operands, A%B. Result width is width(B). */
    OP_UMUL,                /**< Unsigned multiplication. Two operands, A*B. Result width is width(A)+width(B). */
    OP_WRITE,               /**< Write (update) memory with a new value. Arguments are memory, address and value. */
    OP_XOR,                 /**< Bitwise exclusive disjunction. One or more operands, all the same width. */
    OP_ZEROP,               /**< Equal to zero. One operand. Result is a single bit, set iff A is equal to zero. */

    OP_FP_ABS,              /**< Floating-point absolute value. Argument is FP value. */
    OP_FP_NEGATE,           /**< Floating-point negation. Argument is FP value. */
    OP_FP_ADD,              /**< Floating-point addition. Args are the FP addends. */
    OP_FP_MUL,              /**< Floating-point multiply. Args are the FP factors. */
    OP_FP_DIV,              /**< Floating-point division. Args are FP dividend and FP divisor. */
    OP_FP_MULADD,           /**< Floating-point multiply-add. For xy+z, args are x, y, z. */
    OP_FP_SQRT,             /**< Floating-point square root. Argument is the FP square. */
    OP_FP_MOD,              /**< Floating-point remainder. Args are FP dividend and FP divisor. */
    OP_FP_ROUND,            /**< Floating-point round to integer as FP type. Argument and result are both FP values. */
    OP_FP_MIN,              /**< Floating-point minimum. Args are one or more FP values. */
    OP_FP_MAX,              /**< Floating-point maximum. Args are one or more FP values. */
    OP_FP_LE,               /**< Floating-point less-than or equal. Args are the two FP values to compare. */
    OP_FP_LT,               /**< Floating-point less-than. Args are the two FP values to compare. */
    OP_FP_GE,               /**< Floating-point greater-than or equal. Args are the two FP values to compare. */
    OP_FP_GT,               /**< Floating-point greater than. Args are the two FP values to compare. */
    OP_FP_EQ,               /**< Floating-point equality. Args are the two FP values to compare. */
    OP_FP_ISNORM,           /**< Floating-point normal class. Argument is the FP value to check. */
    OP_FP_ISSUBNORM,        /**< Floating-point subnormal class. Argument is the FP value to check. */
    OP_FP_ISZERO,           /**< Floating-point zero class. Argument is the FP value to check. */
    OP_FP_ISINFINITE,       /**< Floating-point infinity class. Argument is the FP value to check. */
    OP_FP_ISNAN,            /**< Floating-point NaN class. Argument is the FP value to check. */
    OP_FP_ISNEG,            /**< Floating-point negative class. Argument is the FP value to check. */
    OP_FP_ISPOS,            /**< Floating-point positive class. Argument is the FP value to check. */

    OP_CONVERT,             /**< Convert from one type to another. Argument is the destination type. */
    OP_REINTERPRET,         /**< Interpret the value as a different type without converting. Argument is the destination type. */

    OP_NONE,                /**< No operation. Result of getOperator on a node that doesn't have an operator. */

    OP_BV_AND = OP_AND,                                 // [Robb Matzke 2017-11-14]: deprecated NO_STRINGIFY
    OP_BV_OR = OP_OR,                                   // [Robb Matzke 2017-11-14]: deprecated NO_STRINGIFY
    OP_BV_XOR = OP_XOR                                  // [Robb Matzke 2017-11-14]: deprecated NO_STRINGIFY
};

std::string toStr(Operator);

class Node;
class Interior;
class Leaf;
class ExprExprHashMap;

/** Shared-ownership pointer to an expression @ref Node. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<Node> Ptr;

/** Shared-ownership pointer to an expression @ref Interior node. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<Interior> InteriorPtr;

/** Shared-ownership pointer to an expression @ref Leaf node. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<Leaf> LeafPtr;

typedef std::vector<Ptr> Nodes;
typedef Map<uint64_t, uint64_t> RenameMap;

/** Hash of symbolic expression. */
typedef uint64_t Hash;

/** Controls formatting of expression trees when printing. */
struct Formatter {
    enum ShowComments {
        CMT_SILENT,                             /**< Do not show comments. */
        CMT_AFTER,                              /**< Show comments after the node. */
        CMT_INSTEAD,                            /**< Like CMT_AFTER, but show comments instead of variable names. */
    };
    Formatter()
        : show_comments(CMT_INSTEAD), do_rename(false), add_renames(true), use_hexadecimal(true),
          max_depth(0), cur_depth(0), show_type(true), show_flags(true) {}
    ShowComments show_comments;                 /**< Show node comments when printing? */
    bool do_rename;                             /**< Use the @p renames map to rename variables to shorter names? */
    bool add_renames;                           /**< Add additional entries to the @p renames as variables are encountered? */
    bool use_hexadecimal;                       /**< Show values in hexadecimal and decimal rather than just decimal. */
    size_t max_depth;                           /**< If non-zero, then replace deep parts of expressions with "...". */
    size_t cur_depth;                           /**< Depth in expression. */
    RenameMap renames;                          /**< Map for renaming variables to use smaller integers. */
    bool show_type;                             /**< Show data type inside square brackets. */
    bool show_flags;                            /**< Show user-defined flags inside square brackets. */
};

/** Return type for visitors. */
enum VisitAction {
    CONTINUE,                               /**< Continue the traversal as normal. */
    TRUNCATE,                               /**< For a pre-order depth-first visit, do not descend into children. */
    TERMINATE,                              /**< Terminate the traversal. */
};

/** Maximum number of nodes that can be reported.
 *
 *  If @ref nnodes returns this value then the size of the expressions could not be counted. This can happens when the
 *  expression contains a large number of common subexpressions. */
extern const uint64_t MAX_NNODES;       // defined in .C so we don't pollute user namespace with limit macros

/** Base class for visiting nodes during expression traversal.  The preVisit method is called before children are visited, and
 *  the postVisit method is called after children are visited.  If preVisit returns TRUNCATE, then the children are not
 *  visited, but the postVisit method is still called.  If either method returns TERMINATE then the traversal is immediately
 *  terminated. */
class Visitor {
public:
    virtual ~Visitor() {}
    virtual VisitAction preVisit(const Ptr&) = 0;
    virtual VisitAction postVisit(const Ptr&) = 0;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Expression type information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Type of symbolic expression. */
class Type {
public:
    /** Type class. */
    enum TypeClass {
        INTEGER,                                        /**< Integer type. */
        FP,                                             /**< Floating-point type. */
        MEMORY,                                         /**< Memory type. */
        INVALID                                         /**< Default constructed. */
    };

private:
    TypeClass typeClass_;
    size_t totalWidth_;
    size_t secondaryWidth_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(typeClass_);
        s & BOOST_SERIALIZATION_NVP(totalWidth_);
        s & BOOST_SERIALIZATION_NVP(secondaryWidth_);
    }
#endif

    /** Create an invalid, empty type.
     *
     *  This is used mainly for default arguments. */
public:
    Type()
        : typeClass_(INVALID), totalWidth_(0), secondaryWidth_(0) {}

private:
    Type(TypeClass tc, size_t w1, size_t w2)
        : typeClass_(tc), totalWidth_(w1), secondaryWidth_(w2) {}

public:
    /** Create no type.
     *
     *  This is the same as a default constructor, but somewhat more self-documenting. */
    static Type none() {
        return Type();
    }
    
    /** Create a new integer type.
     *
     *  This is an integer type whose size is specified in bits. Whether an integer is signed is determined by the context in
     *  which it's used. For instance, unsigned less-than operation will interpret its arguments as unsigned integers, whereas
     *  a signed less-than operation will interpret its arguments as 2's complement signed integers. */
    static Type integer(size_t nBits) {
        return Type(INTEGER, nBits, 0);
    }

    /** Create a new memory type.
     *
     *  A memory type has an address width and a value width. The value width is the size of the value stored at each address. */
    static Type memory(size_t addressWidth, size_t valueWidth) {
        return Type(MEMORY, valueWidth, addressWidth);
    }

    /** Create a new floating-point type.
     *
     *  A floating point type describes an IEEE-754 style of value and is parameterized by two properties: the width of the
     *  exponent field, and the width of the significand field including the implied bit.  The actual storage size of the
     *  floating point value is the sum of these two widths since although the implied bit is not stored, a sign bit is
     *  stored. */
    static Type floatingPoint(size_t exponentWidth, size_t significandWidth) {
        return Type(FP, 1 /*sign bit*/ + exponentWidth + significandWidth - 1 /*implied bit*/, exponentWidth);
    }

    /** Check whether this object is valid.
     *
     *  A default constructed type is invalid. */
    bool isValid() const {
        return typeClass_ != INVALID;
    }
    
    /** Property: Type class.
     *
     *  The type class specifies whether this is an integer type, a floating-point type, or a memory type. */
    TypeClass typeClass() const {
        return typeClass_;
    }

    /** Property: Total width of values.
     *
     *  This is the total width in bits of the values represented by this type. For memory types, it's the width of the value
     *  stored at each address. */
    size_t nBits() const {
        return totalWidth_;
    }

    /** Property: Width of memory addresses.
     *
     *  Returns the width in bits of each memory address for a memory type. This should only be invoked on types for which @ref
     *  typeClass returns @ref MEMORY. */
    size_t addressWidth() const {
        ASSERT_require(MEMORY == typeClass_);
        return secondaryWidth_;
    }

    /** Property: Exponent width.
     *
     *  Returns the width in bits of the exponent for floating-point types. This should only be invoked on types for which @ref
     *  typeClass returns @ref FP. */
    size_t exponentWidth() const {
        ASSERT_require(FP == typeClass_);
        return secondaryWidth_;
    }

    /** Property: Significand width.
     *
     *  Returns the logical width in bits of the significand for floating-point types, which includes the implied bit. The
     *  actual storage size of the significand is one bit fewer. This should only be invoked on types for which @ref typeClass
     *  returns FP. */
    size_t significandWidth() const {
        ASSERT_require(FP == typeClass_);
        return totalWidth_ - (1 /* sign bit */ + exponentWidth() - 1 /*implied bit*/);
    }

    /** Type equality.
     *
     *  Types are equivalent if they have the same class and sizes.
     *
     * @{ */
    bool operator==(const Type &other) const {
        return typeClass_ == other.typeClass_ && totalWidth_ == other.totalWidth_ && secondaryWidth_ == other.secondaryWidth_;
    }
    bool operator!=(const Type &other) const {
        return !(*this == other);
    }
    /** @} */

    /** Type comparison. */
    bool operator<(const Type &other) const;

    /** Print the type. */
    void print(std::ostream&, TypeStyle::Flag style = TypeStyle::FULL) const;

    /** Print the type to a string. */
    std::string toString(TypeStyle::Flag style = TypeStyle::FULL) const;
};

    
    


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Base Node Type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for symbolic expression nodes.
 *
 *  Every node has a specified width measured in bits that is constant over the life of the node. The width is always a
 *  concrete, positive value stored in a 64-bit field.  The corollary of this invariant is that if an expression's result
 *  width depends on the @em values of some of its arguments, those arguments must be concrete and not wider than 64 bits. Only
 *  a few operators fall into this category since most expressions depend on the @em widths of their arguments rather than the
 *  @em values of their arguments.
 *
 *  In order that subtrees can be freely assigned as children of other nodes (provided the structure as a whole remains a
 *  lattice and not a graph with cycles), two things are required: First, expression tree nodes are always referenced through
 *  shared-ownership pointers that collectively own the expression node (expressions are never explicitly deleted). Second,
 *  expression nodes are immutable once they're instantiated.  There are a handful of exceptions to the immutable rule:
 *  comments and attributes are allowed to change freely since they're not significant to hashing or arithmetic operations.
 *
 *  Each node has a bit flags property, the bits of which are defined by the user.  New nodes are created having all bits
 *  cleared unless the user specifies a value in the constructor.  Bits are significant for hashing. Simplifiers produce result
 *  expressions whose bits are set in a predictable manner with the following rules:
 *
 *  @li Interior Node Rule: The flags for an interior node are the union of the flags of its subtrees.
 *
 *  @li Simplification Discard Rule: If a simplification discards a subtree then that subtree does not contribute flags to the
 *      result.  E.g., cancellation of terms in an @c add operation.
 *
 *  @li Simplification Create Rule: If a simplification creates a new leaf node that doesn't depend on the input expression
 *      that new leaf node will have zero flags.  E.g., XOR of an expression with itself; an add operation where all the terms
 *      cancel each other resulting in zero.
 *
 *  @li Simplification Folding Rule: If a simplification creates a new expression from some combination of incoming expressions
 *      then the flags of the new expression are the union of the flags from the expressions on which it depends. E.g.,
 *      constant folding, which is therefore consistent with the Interior Node Rule.
 *
 *  @li Hashing Rule: User-defined flags are significant for hashing.  E.g., structural equivalence will return false if the
 *      two expressions have different flags since structural equivalence uses hashes.
 *
 *  @li Relational Operator Rule:  Simplification of relational operators to produce a Boolean constant will act as if they are
 *      performing constant folding even if the simplification is on variables.  E.g., <code>(ule v1 v1)</code> results in true
 *      with flags the same as @c v1. */
class Node
    : public Sawyer::SharedObject,
      public Sawyer::SharedFromThis<Node>,
      public Sawyer::SmallObject,
      public Sawyer::Attribute::Storage<> { // Attributes are not significant for hashing or arithmetic
protected:
    Type type_;
    unsigned flags_;                  /**< Bit flags. Meaning of flags is up to the user. Low-order 16 bits are reserved. */
    std::string comment_;             /**< Optional comment. Only for debugging; not significant for any calculation. */
    Hash hashval_;                    /**< Optional hash used as a quick way to indicate that two expressions are different. */
    boost::any userData_;             /**< Additional user-specified data. This is not part of the hash. */

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        if (version < 1)
            ASSERT_not_reachable("SymbolicExpr version " + boost::lexical_cast<std::string>(version) + " is no longer supported");
        s & BOOST_SERIALIZATION_NVP(type_);
        s & BOOST_SERIALIZATION_NVP(flags_);
        s & BOOST_SERIALIZATION_NVP(comment_);
        s & BOOST_SERIALIZATION_NVP(hashval_);
        // s & userData_;
    }
#endif

public:
    // Bit flags

    /** These flags are reserved for use within ROSE. */
    static const unsigned RESERVED_FLAGS = 0x0000ffff;

    /** Value is somehow indeterminate. E.g., read from writable memory. */
    static const unsigned INDETERMINATE  = 0x00000001;

    /** Value is somehow unspecified. A value that is intantiated as part of processing a machine instruction where the ISA
     * documentation is incomplete or says that some result is unspecified or undefined. Intel documentation for the x86 shift
     * and rotate instructions, for example, states that certain status bits have "undefined" values after the instruction
     * executes. */
    static const unsigned UNSPECIFIED    = 0x00000002;

    /** Value represents bottom in dataflow analysis.  If this flag is used by ROSE's dataflow engine to represent a bottom
     *  value in a lattice. */
    static const unsigned BOTTOM         = 0x00000004;

protected:
    Node()
        : type_(Type::integer(0)), flags_(0), hashval_(0) {}
    explicit Node(const std::string &comment, unsigned flags=0)
        : type_(Type::integer(0)), flags_(flags), comment_(comment), hashval_(0) {}

public:
    /** Type of value. */
    Type type() const {
        return type_;
    }

    /** User-supplied predicate to augment alias checking.
     *
     * If this pointer is non-null, then the @ref mayEqual methods invoke this function. If this function returns true
     * or false, then its return value becomes the return value of @ref mayEqual, otherwise @ref mayEqual continues
     * as it normally would.  This user-defined function is invoked by @ref mayEqual after trivial situations are checked
     * and before any calls to an SMT solver. The SMT solver argument is optional (may be null). */
    static boost::logic::tribool (*mayEqualCallback)(const Ptr &a, const Ptr &b, const SmtSolverPtr&);
    
    /** Returns true if two expressions must be equal (cannot be unequal).
     *
     *  If an SMT solver is specified then that solver is used to answer this question, otherwise equality is established by
     *  looking only at the structure of the two expressions. Two expressions can be equal without being the same width (e.g.,
     *  a 32-bit constant zero is equal to a 16-bit constant zero). */
    virtual bool mustEqual(const Ptr &other, const SmtSolverPtr &solver = SmtSolverPtr()) = 0;

    /** Returns true if two expressions might be equal, but not necessarily be equal. */
    virtual bool mayEqual(const Ptr &other, const SmtSolverPtr &solver = SmtSolverPtr()) = 0;

    /** Tests two expressions for structural equivalence.
     *
     *  Two leaf nodes are equivalent if they are the same width and have equal values or are the same variable. Two interior
     *  nodes are equivalent if they are the same width, the same operation, have the same number of children, and those
     *  children are all pairwise equivalent. */
    virtual bool isEquivalentTo(const Ptr &other) = 0;

    /** Compare two expressions structurally for sorting.
     *
     *  Returns -1 if @p this is less than @p other, 0 if they are structurally equal, and 1 if @p this is greater than @p
     *  other.  This function returns zero when an only when @ref isEquivalentTo returns zero, but @ref isEquivalentTo can be
     *  much faster since it uses hashing. */
    virtual int compareStructure(const Ptr &other) = 0;

    /** Substitute one value for another.
     *
     *  Finds all occurrances of @p from in this expression and replace them with @p to. If a substitution occurs, then a new
     *  expression is returned. The matching of @p from to sub-parts of this expression uses structural equivalence, the
     *  @ref isEquivalentTo predicate. The @p from and @p to expressions must have the same width. */
    virtual Ptr substitute(const Ptr &from, const Ptr &to, const SmtSolverPtr &solver = SmtSolverPtr()) = 0;

    /** Rewrite expression by substituting subexpressions.
     *
     *  This expression is rewritten by doing a depth-first traversal. At each step of the traversal, the subexpression is
     *  looked up by hash in the supplied substitutions table. If found, a new expression is created using the value found in
     *  the table and the traversal does not descend into the new expression.  If no substitutions were performed then @p this
     *  expression is returned, otherwise a new expression is returned. An optional solver, which may be null, is used during
     *  the simplification step. */
    Ptr substituteMultiple(const ExprExprHashMap &substitutions, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Rewrite using lowest numbered variable names.
     *
     *  Given an expression, use the specified index to rewrite variables. The index uses expression hashes to look up the
     *  replacement expression. If the traversal finds a variable which is not in the index then a new variable is created. The
     *  new variable has the same type as the original variable, but it's name is generated starting at @p nextVariableId and
     *  incrementing after each replacement is generated. The optional solver is used during the simplification process and may
     *  be null. */
    Ptr renameVariables(ExprExprHashMap &index /*in,out*/, size_t &nextVariableId /*in,out*/,
                        const SmtSolverPtr &solver = SmtSolverPtr());

    /** Operator for interior nodes.
     *
     *  Return the operator for interior nodes, or @ref OP_NONE for leaf nodes that have no operator. */
    virtual Operator getOperator() const = 0;

    /** Number of arguments.
     *
     *  Returns the number of children for an interior node, zero for leaf nodes. */
    virtual size_t nChildren() const = 0;

    /** Argument.
     *
     *  Returns the specified argument by index. If the index is out of range, then returns null. A leaf node always returns
     *  null since it never has children. */
    virtual Ptr child(size_t idx) const = 0;

    /** Arguments.
     *
     *  Returns the arguments of an operation for an interior node, or an empty list for a leaf node. */
    virtual const Nodes& children() const = 0;

    /** The unsigned integer value of the expression.
     *
     *  Returns nothing if the expression is not a concrete integer value or the value is too wide to be represented by the
     *  return type. */
    virtual Sawyer::Optional<uint64_t> toUnsigned() const = 0;

    /** The signed integer value of the expression.
     *
     *  Returns nothing if the expression is not a concrete integer value or the value doesn't fit in the return type. */
    virtual Sawyer::Optional<int64_t> toSigned() const = 0;

    /** True if this expression is of an integer type. */
    bool isIntegerExpr() const {
        return type_.typeClass() == Type::INTEGER;
    }

    /** True if this expression is of a floating-point type. */
    bool isFloatingPointExpr() const {
        return type_.typeClass() == Type::FP;
    }

    /** True if this expression is of a memory type. */
    bool isMemoryExpr() const {
        return type_.typeClass() == Type::MEMORY;
    }

    /** True if the expression is a scalar type.
     *
     *  Integers and floating-point expressions are scalar, memory is not. */
    bool isScalarExpr() const {
        return isIntegerExpr() || isFloatingPointExpr();
    }
    
    /** True if this expression is a constant. */
    virtual bool isConstant() const = 0;

    /** True if this expression is an integer constant. */
    bool isIntegerConstant() const {
        return isIntegerExpr() && isConstant();
    }

    /** True if this epxression is a floating-point constant. */
    bool isFloatingPointConstant() const {
        return isFloatingPointExpr() && isConstant();
    }

    /** True if this expression is a scalar constant.
     *
     *  Integer and floating-point constants are scalar. */
    bool isScalarConstant() const {
        return isIntegerConstant() || isFloatingPointConstant();
    }
    
    /** True if this expression is a floating-point NaN constant. */
    bool isFloatingPointNan() const;

    /** True if this expression is a variable.
     *
     *  Warning: Leaf nodes have a deprecated isVariable method that returns false for memory state variables, thus this
     *  method has a "2" appended to its name. After a suitable period of deprecation for Leaf::isVariable, a new isVariable
     *  will be added to this class hiearchy and will have the same semantics as isVariable2, which will become deprecated. */
    virtual bool isVariable2() const = 0;
    
    /** True if this expression is an integer variable. */
    bool isIntegerVariable() const {
        return isIntegerExpr() && isVariable2();
    }

    /** True if this expression is a floating-point variable. */
    bool isFloatingPointVariable() const {
        return isFloatingPointExpr() && isVariable2();
    }

    /** True if this expression is a memory state variable. */
    bool isMemoryVariable() const {
        return isMemoryExpr() && isVariable2();
    }

    /** True if this expression is a scalar variable.
     *
     *  Integer and floating-point variables are scalar, memory variables are not. */
    bool isScalarVariable() const {
        return isIntegerVariable() || isFloatingPointVariable();
    }
    
    /** Property: Comment.
     *
     *  Comments can be changed after a node has been created since the comment is not intended to be used for anything but
     *  annotation and/or debugging. If many expressions are sharing the same node, then the comment is changed in all those
     *  expressions. Changing the comment property is allowed even though nodes are generally immutable because comments are
     *  not considered significant for comparisons, computing hash values, etc.
     *
     * @{ */
    const std::string& comment() const {
        return comment_;
    }
    void comment(const std::string &s) {
        comment_ = s;
    }
    /** @} */

    /** Property: User-defined data.
     *
     *  User defined data is always optional and does not contribute to the hash value of an expression. The user-defined data
     *  can be changed at any time by the user even if the expression node to which it is attached is shared between many
     *  expressions.
     *
     * @{ */
    void userData(boost::any &data) {
        userData_ = data;
    }
    const boost::any& userData() const {
        return userData_;
    }
    /** @} */

    /** Property: Number of significant bits.
     *
     *  An expression with a known value is guaranteed to have all higher-order bits cleared. */
    size_t nBits() const {
        return type_.nBits();
    }

    /** Property: User-defined bit flags.
     *
     *  This property is significant for hashing, comparisons, and possibly other operations, therefore it is immutable.  To
     *  change the flags one must create a new expression; see @ref newFlags. */
    unsigned flags() const {
        return flags_;
    }

    /** Sets flags. Since symbolic expressions are immutable it is not possible to change the flags directly. Therefore if the
     *  desired flags are different than the current flags a new expression is created that is the same in every other
     *  respect. If the flags are not changed then the original expression is returned. */
    Ptr newFlags(unsigned flags) const;

    /** Property: Width for memory expressions.
     *
     *  The return value is non-zero if and only if this tree node is a memory expression. */
    size_t domainWidth() const {
        return type_.addressWidth();
    }

    /** Check whether expression is scalar.
     *
     *  Everything is scalar except for memory. */
    bool isScalar() const {
        return type_.typeClass() != Type::MEMORY;
    }

    /** Traverse the expression.
     *
     *  The expression is traversed in a depth-first visit.  The final return value is the final return value of the last call
     *  to the visitor. */
    virtual VisitAction depthFirstTraversal(Visitor&) const = 0;

    /** Computes the size of an expression by counting the number of nodes.
     *
     *  Operates in constant time.  Note that it is possible (even likely) for the 64-bit return value to overflow in
     *  expressions when many nodes are shared.  For instance, the following loop will create an expression that contains more
     *  than 2^64 nodes:
     *
     *  @code
     *   SymbolicExpr expr = Leaf::createVariable(32);
     *   for(size_t i=0; i<64; ++i)
     *       expr = makeAdd(expr, expr)
     *  @endcode
     *
     *  When an overflow occurs the result is meaningless.
     *
     *  @sa nNodesUnique */
    virtual uint64_t nNodes() const = 0;

    /** Number of unique nodes in expression. */
    uint64_t nNodesUnique() const;

    /** Returns the variables appearing in the expression. */
    std::set<LeafPtr> getVariables() const;

    /** Dynamic cast of this object to an interior node.
     *
     *  Returns null if the cast is not valid. */
    InteriorPtr isInteriorNode() const;

    /** Dynamic cast of this object to a leaf node.
     *
     *  Returns null if the cast is not valid. */
    LeafPtr isLeafNode() const;

    /** Returns true if this node has a hash value computed and cached. The hash value zero is reserved to indicate that no
     *  hash has been computed; if a node happens to actually hash to zero, it will not be cached and will be recomputed for
     *  every call to hash(). */
    bool isHashed() const {
        return hashval_ != 0;
    }

    /** Returns (and caches) the hash value for this node.  If a hash value is not cached in this node, then a new hash value
     *  is computed and cached. */
    Hash hash() const;

    // used internally to set the hash value
    void hash(Hash);

    /** A node with formatter. See the with_format() method. */
    class WithFormatter {
    private:
        Ptr node;
        Formatter &formatter;
    public:
        WithFormatter(const Ptr &node, Formatter &formatter): node(node), formatter(formatter) {}
        void print(std::ostream &stream) const { node->print(stream, formatter); }
    };

    /** Combines a node with a formatter for printing.  This is used for convenient printing with the "<<" operator. For
     *  instance:
     *
     * @code
     *  Formatter fmt;
     *  fmt.show_comments = Formatter::CMT_AFTER; //show comments after the variable
     *  Ptr expression = ...;
     *  std::cout <<"method 1: "; expression->print(std::cout, fmt); std::cout <<"\n";
     *  std::cout <<"method 2: " <<expression->withFormat(fmt) <<"\n";
     *  std::cout <<"method 3: " <<*expression+fmt <<"\n";
     * 
     * @endcode
     * @{ */
    WithFormatter withFormat(Formatter &fmt) { return WithFormatter(sharedFromThis(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return withFormat(fmt); }
    /** @} */


    /** Print the expression to a stream.  The output is an S-expression with no line-feeds. The format of the output is
     *  controlled by the mutable Formatter argument.
     *  @{ */
    virtual void print(std::ostream&, Formatter&) const = 0;
    void print(std::ostream &o) const { Formatter fmt; print(o, fmt); }
    /** @} */

    /** Asserts that expressions are acyclic. This is intended only for debugging. */
    void assertAcyclic() const;

    /** Find common subexpressions.
     *
     *  Returns a vector of the largest common subexpressions. The list is computed by performing a depth-first search on this
     *  expression and adding expressions to the return vector whenever a subtree is encountered a second time. Therefore the
     *  if a common subexpression A contains another common subexpression B then B will appear earlier in the list than A. */
    std::vector<Ptr> findCommonSubexpressions() const;

    /** Determine whether an expression is a variable plus a constant.
     *
     *  If this expression is of the form V + X or X + V where V is an integer variable and X is an integer constant, return
     *  true and make @p variable point to the variable and @p constant point to the constant.  If the expression is not one of
     *  these forms, then return false without modifying the arguments. */
    bool matchAddVariableConstant(LeafPtr &variable/*out*/, LeafPtr &constant/*out*/) const;

    /** True (non-null) if this node is the specified operator. */
    InteriorPtr isOperator(Operator) const;

protected:
    void printFlags(std::ostream &o, unsigned flags, char &bracket) const;

public:
    // Deprecated [Robb Matzke 2019-09-27]
    bool isNumber() const ROSE_DEPRECATED("use isIntegerConstant instead") {
        return isIntegerConstant();
    }

    // Deprecated [Robb Matzke 2019-09-27]
    uint64_t toInt() ROSE_DEPRECATED("use toUnsigned() instead") {
        return toUnsigned().get();
    }
};

/** Operator-specific simplification methods. */
class Simplifier {
public:
    virtual ~Simplifier() {}

    /** Constant folding. The range @p begin (inclusive) to @p end (exclusive) must contain at least two nodes and all of
     *  the nodes must be leaf nodes with known values.  This method returns a new folded node if folding is possible, or
     *  the null pointer if folding is not possible. */
    virtual Ptr fold(Nodes::const_iterator /*begin*/, Nodes::const_iterator /*end*/) const {
        return Ptr();
    }

    /** Rewrite the entire expression to something simpler. Returns the new node if the node can be simplified, otherwise
     *  returns null. */
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const {
        return Ptr();
    }
};

struct ExprExprHashMapHasher {
     size_t operator()(const Ptr &expr) const {
        return expr->hash();
    }
};

struct ExprExprHashMapCompare {
    bool operator()(const Ptr &a, const Ptr &b) const {
        return a->isEquivalentTo(b);
    }
};

/** Compare two expressions for STL containers. */
class ExpressionLessp {
public:
    bool operator()(const Ptr &a, const Ptr &b);
};

/** Mapping from expression to expression. */
class ExprExprHashMap: public boost::unordered_map<SymbolicExpr::Ptr, SymbolicExpr::Ptr,
                                                   ExprExprHashMapHasher, ExprExprHashMapCompare> {
public:
    ExprExprHashMap invert() const;
};

/** Set of expressions ordered by hash. */
typedef Sawyer::Container::Set<Ptr, ExpressionLessp> ExpressionSet;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Simplification
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct AddSimplifier: Simplifier {
    virtual Ptr fold(Nodes::const_iterator, Nodes::const_iterator) const ROSE_OVERRIDE;
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct AndSimplifier: Simplifier {
    virtual Ptr fold(Nodes::const_iterator, Nodes::const_iterator) const ROSE_OVERRIDE;
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct ConvertSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct OrSimplifier: Simplifier {
    virtual Ptr fold(Nodes::const_iterator, Nodes::const_iterator) const ROSE_OVERRIDE;
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct XorSimplifier: Simplifier {
    virtual Ptr fold(Nodes::const_iterator, Nodes::const_iterator) const ROSE_OVERRIDE;
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SmulSimplifier: Simplifier {
    virtual Ptr fold(Nodes::const_iterator, Nodes::const_iterator) const ROSE_OVERRIDE;
};
struct UmulSimplifier: Simplifier {
    virtual Ptr fold(Nodes::const_iterator, Nodes::const_iterator) const ROSE_OVERRIDE;
};
struct ConcatSimplifier: Simplifier {
    virtual Ptr fold(Nodes::const_iterator, Nodes::const_iterator) const ROSE_OVERRIDE;
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct ExtractSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct AsrSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct InvertSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct NegateSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct IteSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct NoopSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct ReinterpretSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct RolSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct RorSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct UextendSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SextendSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct EqSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SgeSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SgtSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SleSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SltSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct UgeSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct UgtSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct UleSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct UltSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct ZeropSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SdivSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SmodSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct UdivSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct UmodSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct ShiftSimplifier: Simplifier {
    bool newbits;
    ShiftSimplifier(bool newbits): newbits(newbits) {}
    Ptr combine_strengths(Ptr strength1, Ptr strength2, size_t value_width, const SmtSolverPtr &solver) const;
};
struct ShlSimplifier: ShiftSimplifier {
    ShlSimplifier(bool newbits): ShiftSimplifier(newbits) {}
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct ShrSimplifier: ShiftSimplifier {
    ShrSimplifier(bool newbits): ShiftSimplifier(newbits) {}
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct LssbSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct MssbSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};
struct SetSimplifier: Simplifier {
    virtual Ptr rewrite(Interior*, const SmtSolverPtr&) const ROSE_OVERRIDE;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Interior Nodes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Interior node of an expression tree for instruction semantics.
 *
 *  Each interior node has an operator (constant for the life of the node and obtainable with get_operator()) and zero or more
 *  children. Children are added to the interior node during the construction phase. Once construction is complete, the
 *  children should only change in ways that don't affect the value of the node as a whole (since this node might be pointed to
 *  by any number of expressions). */
class Interior: public Node {
private:
    Operator op_;
    Nodes children_;
    uint64_t nnodes_;                                   // total number of nodes; self + children's nnodes

    //--------------------------------------------------------
    // Serialization
    //--------------------------------------------------------
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Node);
        s & BOOST_SERIALIZATION_NVP(op_);
        s & BOOST_SERIALIZATION_NVP(children_);
        s & BOOST_SERIALIZATION_NVP(nnodes_);
    }
#endif

    //--------------------------------------------------------
    // Real constructors
    //--------------------------------------------------------
private:
    Interior();                                         // needed for serialization
    // Only constructs, does not simplify.
    Interior(const Type&, Operator, const Nodes &arguments, const std::string &comment, unsigned flags);

    //--------------------------------------------------------
    // Allocating constructors
    //--------------------------------------------------------
public:
    /** Create a new expression node.
     *
     * @{ */
    static Ptr instance(Operator op, const Ptr &a,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
    static Ptr instance(const Type &type, Operator op, const Ptr &a,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
    static Ptr instance(Operator op, const Ptr &a, const Ptr &b,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
    static Ptr instance(const Type &type, Operator op, const Ptr &a, const Ptr &b,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
    static Ptr instance(Operator op, const Ptr &a, const Ptr &b, const Ptr &c,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
    static Ptr instance(const Type &type, Operator op, const Ptr &a, const Ptr &b, const Ptr &c,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
    static Ptr instance(Operator op, const Nodes &arguments,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
    static Ptr instance(const Type &type, Operator op, const Nodes &arguments,
                        const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
    /** @} */

    //--------------------------------------------------------
    // Overrides
    //--------------------------------------------------------
public:
    virtual bool mustEqual(const Ptr &other, const SmtSolverPtr &solver = SmtSolverPtr()) ROSE_OVERRIDE;
    virtual bool mayEqual(const Ptr &other, const SmtSolverPtr &solver = SmtSolverPtr()) ROSE_OVERRIDE;
    virtual bool isEquivalentTo(const Ptr &other) ROSE_OVERRIDE;
    virtual int compareStructure(const Ptr& other) ROSE_OVERRIDE;
    virtual Ptr substitute(const Ptr &from, const Ptr &to, const SmtSolverPtr &solver = SmtSolverPtr()) ROSE_OVERRIDE;
    virtual VisitAction depthFirstTraversal(Visitor&) const ROSE_OVERRIDE;
    virtual uint64_t nNodes() const ROSE_OVERRIDE { return nnodes_; }
    virtual const Nodes& children() const ROSE_OVERRIDE { return children_; }
    virtual Operator getOperator() const ROSE_OVERRIDE { return op_; }
    virtual size_t nChildren() const ROSE_OVERRIDE { return children_.size(); }
    virtual Ptr child(size_t idx) const ROSE_OVERRIDE { return idx < children_.size() ? children_[idx] : Ptr(); }
    virtual Sawyer::Optional<uint64_t> toUnsigned() const ROSE_OVERRIDE { return Sawyer::Nothing(); }
    virtual Sawyer::Optional<int64_t> toSigned() const ROSE_OVERRIDE { return Sawyer::Nothing(); }
    virtual bool isConstant() const ROSE_OVERRIDE { return false; }
    virtual bool isVariable2() const ROSE_OVERRIDE { return false; }

    //--------------------------------------------------------
    // Simplification
    //--------------------------------------------------------
public:
    /** Simplifies the specified interior node.
     *
     *  Returns a new node if necessary, otherwise returns this. The SMT solver is optional and my be the null pointer. */
    Ptr simplifyTop(const SmtSolverPtr &solver = SmtSolverPtr());

    /** Perform constant folding.  This method returns either a new expression (if changes were mde) or the original
     *  expression. The simplifier is specific to the kind of operation at the node being simplified. */
    Ptr foldConstants(const Simplifier&);

    /** Simplifies non-associative operators by flattening the specified interior node with its children that are the same
     *  interior node type. Call this only if the top node is a truly non-associative. A new node is returned only if
     *  changed. When calling both nonassociative and commutative, it's usually more appropriate to call nonassociative
     *  first. */
    InteriorPtr associative();

    /** Simplifies commutative operators by sorting arguments. The arguments are sorted so that all the interior nodes come
     *  before the leaf nodes. Call this only if the top node is truly commutative.  A new node is returned only if
     *  changed. When calling both nonassociative and commutative, it's usually more appropriate to call nonassociative
     *  first. */
    InteriorPtr commutative();

    /** Simplifies idempotent operators.
     *
     *  An idempotent operator I is one such that X I X = X. For operators that have more than two operands, only those
     *  repeated neighboring operands are reduced to a single operand. Therefore, if the operator is commutative, then do the
     *  commutative simplification before the idempotent simplification. Returns either a new, simplified expression or the
     *  original unmodified expression. */
    InteriorPtr idempotent(const SmtSolverPtr &solver = SmtSolverPtr());

    /** Simplifies involutary operators.  An involutary operator is one that is its own inverse.  This method should only be
     *  called if this node is an interior node whose operator has the involutary property (such as invert or negate). Returns
     *  either a new expression that is simplified, or the original expression. */
    Ptr involutary();

    /** Simplifies nested shift-like operators.
     *
     *  Simplifies (shift AMT1 (shift AMT2 X)) to (shift (add AMT1 AMT2) X). The SMT solver may be null. */
    Ptr additiveNesting(const SmtSolverPtr &solver = SmtSolverPtr());

    /** Removes identity arguments.
     *
     *  Returns either a new expression or the original expression. The solver may be a null pointer. */
    Ptr identity(uint64_t ident, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Returns NaN if any argument is NaN.
     *
     *  If any argument is NaN then a new NaN constant expression is returned, otherwise the original expression is returned.
     *  The solver may be a null pointer. */
    Ptr poisonNan(const SmtSolverPtr &solver = SmtSolverPtr());

    /** Replaces a binary operator with its only argument. Returns either a new expression or the original expression. */
    Ptr unaryNoOp();

    /** Simplify an interior node. Returns a new node if this node could be simplified, otherwise returns this node. When
     *  the simplification could result in a leaf node, we return an OP_NOOP interior node instead. The SMT solver is optional
     *  and may be the null pointer. */
    Ptr rewrite(const Simplifier &simplifier, const SmtSolverPtr &solver = SmtSolverPtr());

    //--------------------------------------------------------
    // Functions specific to internal nodes
    //--------------------------------------------------------
public:
    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;

protected:
    /** Appends @p child as a new child of this node. This must only be called from constructors. */
    void addChild(const Ptr &child);

    /** Adjust width based on operands.
     *
     *  This must only be called from constructors. The type is needed for certain operations such as convert. */
    void adjustWidth(const Type&);

    /** Adjust user-defined bit flags. This must only be called from constructors.  Flags are the union of the operand flags
     *  subject to simplification rules, unioned with the specified flags. */
    void adjustBitFlags(unsigned extraFlags);

    //--------------------------------------------------------
    // Deprecated [Robb Matzke 2019-10-01]
    //--------------------------------------------------------
public:
    static Ptr create(size_t nbits, Operator op, const Ptr &a,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use instance instead");
    static Ptr create(size_t nbits, Operator op, const Ptr &a, const Ptr &b,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use instance instead");
    static Ptr create(size_t nbits, Operator op, const Ptr &a, const Ptr &b, const Ptr &c,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use instance instead");
    static Ptr create(size_t nbits, Operator op, const Nodes &children,
                      const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use instance instead");

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Leaf Nodes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Leaf node of an expression tree for instruction semantics.
 *
 *  A leaf node is either a known bit vector value, a free bit vector variable, or a memory state. */
class Leaf: public Node {
private:
    Sawyer::Container::BitVector bits_; // Value for constants if size > 0
    uint64_t name_;                     // Variable ID for variables when bits_.size() == 0

    //--------------------------------------------------------
    // Serialization
    //--------------------------------------------------------
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Node);
        s & BOOST_SERIALIZATION_NVP(bits_);
        s & BOOST_SERIALIZATION_NVP(name_);
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Node);
        s & BOOST_SERIALIZATION_NVP(bits_);
        s & BOOST_SERIALIZATION_NVP(name_);
        nextNameCounter(name_);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

    //--------------------------------------------------------
    // Private constructors. Use create methods instead.
    //--------------------------------------------------------
private:
    Leaf()
        : name_(0) {}
    explicit Leaf(const std::string &comment, unsigned flags=0)
        : Node(comment, flags), name_(0) {}

    // Allocating constructors.
public:
    /** Create a new variable. */
    static LeafPtr createVariable(const Type&,
                                  const std::string &comment = "", unsigned flags = 0);

    /** Create an existing variable. */
    static LeafPtr createVariable(const Type&, const uint64_t id,
                                  const std::string &comment = "", unsigned flags = 0);

    /** Create a constant. */
    static LeafPtr createConstant(const Type&, const Sawyer::Container::BitVector&,
                                  const std::string &comment = "", unsigned flags = 0);

    //--------------------------------------------------------
    // Override base class implementations
    //--------------------------------------------------------
public:
    virtual size_t nChildren() const ROSE_OVERRIDE { return 0; }
    virtual Ptr child(size_t idx) const ROSE_OVERRIDE { return Ptr(); }
    virtual const Nodes& children() const ROSE_OVERRIDE;
    virtual Operator getOperator() const ROSE_OVERRIDE { return OP_NONE; }
    virtual bool mustEqual(const Ptr &other, const SmtSolverPtr &solver = SmtSolverPtr()) ROSE_OVERRIDE;
    virtual bool mayEqual(const Ptr &other, const SmtSolverPtr &solver = SmtSolverPtr()) ROSE_OVERRIDE;
    virtual bool isEquivalentTo(const Ptr &other) ROSE_OVERRIDE;
    virtual int compareStructure(const Ptr& other) ROSE_OVERRIDE;
    virtual Ptr substitute(const Ptr &from, const Ptr &to, const SmtSolverPtr &solver = SmtSolverPtr()) ROSE_OVERRIDE;
    virtual VisitAction depthFirstTraversal(Visitor&) const ROSE_OVERRIDE;
    virtual uint64_t nNodes() const ROSE_OVERRIDE { return 1; }
    virtual Sawyer::Optional<uint64_t> toUnsigned() const ROSE_OVERRIDE;
    virtual Sawyer::Optional<int64_t> toSigned() const ROSE_OVERRIDE;
    virtual bool isConstant() const ROSE_OVERRIDE { return !bits_.isEmpty(); }
    virtual bool isVariable2() const ROSE_OVERRIDE { return !isConstant(); }
    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;

    //--------------------------------------------------------
    // Leaf-specific methods
    //--------------------------------------------------------
public:
    /** Property: Bits stored for numeric constants. */
    const Sawyer::Container::BitVector& bits() const;

    /** Is this node an integer variable? */
    bool isIntegerVariable() const {
        return type().typeClass() == Type::INTEGER && !isConstant();
    }

    /** Is this node a floating-point variable? */
    bool isFloatingPointVariable() const {
        return type().typeClass() == Type::FP && !isConstant();
    }

    /** Is this node a floating-point NaN constant? */
    bool isFloatingPointNan() const;

    /** Is this node a memory variable? */
    bool isMemoryVariable() const {
        return type().typeClass() == Type::MEMORY && !isConstant();
    }
    
    /** Returns the name ID of a free variable.
     *
     *  The output functions print variables as "vN" where N is an integer. It is this N that this method returns.  It should
     *  only be invoked on leaf nodes for which @ref isConstant returns false. */
    uint64_t nameId() const;

    /** Returns a string for the leaf.
     *
     *  Integer and floating-point variables are returned as "vN", memory variables are returned as "mN", and constants are
     *  returned as a hexadecimal string, where N is a variable identification number. */
    std::string toString() const;

    /** Prints an integer constant interpreted as a signed value. */
    void printAsSigned(std::ostream&, Formatter&, bool asSigned = true) const;

    /** Prints an integer constant interpreted as an unsigned value. */
    void printAsUnsigned(std::ostream &o, Formatter &f) const {
        printAsSigned(o, f, false);
    }

private:
    // Obtain or register a name ID
    static uint64_t nextNameCounter(uint64_t useThis = (uint64_t)(-1));

    // Deprecated functions
public:
    // Deprecated [Robb Matzke 2019-09-27]
    static LeafPtr createVariable(size_t nBits, const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use createVariable with type or makeIntegerVariable, etc.") {
        return createVariable(Type::integer(nBits), comment, flags);
    }
    
    // Deprecated [Robb Matzke 2019-09-27]
    static LeafPtr createExistingVariable(size_t nBits, uint64_t id, const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use createVariable or makeIntegerVariable, etc.") {
        return createVariable(Type::integer(nBits), id, comment, flags);
    }
    
    // Deprecated [Robb Matzke 2019-09-27]
    static LeafPtr createInteger(size_t nBits, uint64_t value, const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use createConstant or makeIntegerConstant, etc.");
    
    // Deprecated [Robb Matzke 2019-09-27]
    static LeafPtr createConstant(const Sawyer::Container::BitVector &bits, const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use createConstant with type or makeIntegerConstant, etc.") {
        return createConstant(Type::integer(bits.size()), bits, comment, flags);
    }
    
    // Deprecated [Robb Matzke 2019-09-27]
    static LeafPtr createBoolean(bool b, const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use createConstant or makeBooleanConstant");

    // Deprecated [Robb Matzke 2019-09-27]
    static LeafPtr createMemory(size_t addressWidth, size_t valueWidth, const std::string &comment="", unsigned flags=0)
        ROSE_DEPRECATED("use createVariable with type or makeMemoryVariable") {
        return createVariable(Type::memory(addressWidth, valueWidth), comment, flags);
    }
    
    // Deprecated [Robb Matzke 2019-09-27]
    static LeafPtr createExistingMemory(size_t addressWidth, size_t valueWidth, uint64_t id, const std::string &comment="",
                                        unsigned flags=0)
        ROSE_DEPRECATED("use createVariable with type or makeMemoryVariable") {
        return createVariable(Type::memory(addressWidth, valueWidth), id, comment, flags);
    }

    // Deprecated [Robb Matzke 2019-09-27]. The definition will eventually change to isVariable2()
    bool isVariable() const ROSE_DEPRECATED("use isIntegerVariable or isVariable2 instead") {
        return isIntegerVariable();
    }

    // Deprecated [Robb Matzke 2019-09-27]
    virtual bool isMemory() ROSE_DEPRECATED("use isMemoryVariable or isMemoryExpr instead") {
        return isMemoryVariable();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Factories
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Leaf constructor.
 *
 *  Constructs an expression leaf node. This is a wrapper around one of the "create" factory methods in @ref Leaf.
 *
 * @{ */
LeafPtr makeVariable(const Type&, const std::string &comment="", unsigned flags=0);
LeafPtr makeVariable(const Type&, uint64_t id, const std::string &comment="", unsigned flags=0);
LeafPtr makeConstant(const Type&, const Sawyer::Container::BitVector&, const std::string &comment="", unsigned flags=0);
LeafPtr makeIntegerVariable(size_t nBits, const std::string &comment="", unsigned flags=0);
LeafPtr makeIntegerVariable(size_t nBits, uint64_t id, const std::string &comment="", unsigned flags=0);
LeafPtr makeIntegerConstant(size_t nBits, uint64_t n, const std::string &comment="", unsigned flags=0);
LeafPtr makeIntegerConstant(const Sawyer::Container::BitVector&, const std::string &comment="", unsigned flags=0);
LeafPtr makeBooleanConstant(bool, const std::string &comment="", unsigned flags=0);
LeafPtr makeMemoryVariable(size_t addressWidth, size_t valueWidth, const std::string &comment="", unsigned flags=0);
LeafPtr makeMemoryVariable(size_t addressWidth, size_t valueWidth, uint64_t id, const std::string &comment="", unsigned flags=0);
LeafPtr makeFloatingPointVariable(size_t eb, size_t sb, const std::string &comment="", unsigned flags=0);
LeafPtr makeFloatingPointVariable(size_t eb, size_t sb, uint64_t id, const std::string &comment="", unsigned flags=0);
LeafPtr makeFloatingPointConstant(float, const std::string &comment="", unsigned flags=0);
LeafPtr makeFloatingPointConstant(double, const std::string &comment="", unsigned flags=0);
LeafPtr makeFloatingPointNan(size_t eb, size_t sb, const std::string &comment="", unsigned flags=0);
/** @} */

// Deprecated [Robb Matzke 2019-09-27]
Ptr makeVariable(size_t nbits, const std::string &comment="", unsigned flags=0)
    ROSE_DEPRECATED("use makeIntegerVariable instead");
Ptr makeExistingVariable(size_t nbits, uint64_t id, const std::string &comment="", unsigned flags=0)
    ROSE_DEPRECATED("use makeIntegerVariable instead");
Ptr makeInteger(size_t nbits, uint64_t n, const std::string &comment="", unsigned flags=0)
    ROSE_DEPRECATED("use makeIntegerConstant instead");
Ptr makeConstant(const Sawyer::Container::BitVector&, const std::string &comment="", unsigned flags=0)
    ROSE_DEPRECATED("use makeIntegerConstant instead");
Ptr makeBoolean(bool, const std::string &comment="", unsigned flags=0)
    ROSE_DEPRECATED("use makeBooleanConstant instead");
Ptr makeMemory(size_t addressWidth, size_t valueWidth, const std::string &comment="", unsigned flags=0)
    ROSE_DEPRECATED("use makeMemoryVariable instead");
Ptr makeExistingMemory(size_t addressWidth, size_t valueWidth, uint64_t id, const std::string &comment="", unsigned flags=0)
    ROSE_DEPRECATED("use makeMemoryVariable instead");

/** Interior node constructor.
 *
 *  Constructs an interior node. This is a wrapper around one of the "create" factory methods in @ref Interior. It
 *  interprets its operands as unsigned values unless the method has "Signed" in its name.
 *
 * @{ */
Ptr makeAdd(const Ptr&a, const Ptr &b,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeBooleanAnd(const Ptr &a, const Ptr &b,
                   const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0)
                   ROSE_DEPRECATED("use makeAnd instead"); // [Robb Matzke 2017-11-21]: deprecated
Ptr makeAsr(const Ptr &sa, const Ptr &a,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeAnd(const Ptr &a, const Ptr &b,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeOr(const Ptr &a, const Ptr &b,
           const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeXor(const Ptr &a, const Ptr &b,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeConcat(const Ptr &hi, const Ptr &lo,
               const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeConvert(const Ptr &a, const Type &b,
                const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeEq(const Ptr &a, const Ptr &b,
           const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeExtract(const Ptr &begin, const Ptr &end, const Ptr &a,
                const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeInvert(const Ptr &a,
               const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeIsInfinite(const Ptr &a,
                   const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeIsNan(const Ptr &a,
              const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeIsNeg(const Ptr &a,
              const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeIsNorm(const Ptr &a,
               const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeIsPos(const Ptr &a,
              const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeIsSubnorm(const Ptr &a,
                  const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeIte(const Ptr &cond, const Ptr &a, const Ptr &b,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeLet(const Ptr &a, const Ptr &b, const Ptr &c,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeLssb(const Ptr &a,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeMax(const Ptr &a, const Ptr &b,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeMin(const Ptr &a, const Ptr &b,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeMssb(const Ptr &a,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeMultiplyAdd(const Ptr &a, const Ptr &b, const Ptr &c,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeNe(const Ptr &a, const Ptr &b,
           const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeNegate(const Ptr &a,
               const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeBooleanOr(const Ptr &a, const Ptr &b,
                  const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0)
                  ROSE_DEPRECATED("use makeOr instead"); // [Robb Matzke 2017-11-21]: deprecated
Ptr makeRead(const Ptr &mem, const Ptr &addr,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeReinterpret(const Ptr &a, const Type &b,
                    const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeRol(const Ptr &sa, const Ptr &a,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeRor(const Ptr &sa, const Ptr &a,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeRound(const Ptr &a,
              const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSet(const Ptr &a, const Ptr &b,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSet(const Ptr &a, const Ptr &b, const Ptr &c,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedDiv(const Ptr &a, const Ptr &b,
                  const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignExtend(const Ptr &newSize, const Ptr &a,
                   const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedGe(const Ptr &a, const Ptr &b,
                 const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedGt(const Ptr &a, const Ptr &b,
                 const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeShl0(const Ptr &sa, const Ptr &a,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeShl1(const Ptr &sa, const Ptr &a,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeShr0(const Ptr &sa, const Ptr &a,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeShr1(const Ptr &sa, const Ptr &a,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeIsSignedPos(const Ptr &a,
                    const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedLe(const Ptr &a, const Ptr &b,
                 const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedLt(const Ptr &a, const Ptr &b,
                 const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedMax(const Ptr &a, const Ptr &b,
                  const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedMin(const Ptr &a, const Ptr &b,
                  const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedMod(const Ptr &a, const Ptr &b,
                  const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSignedMul(const Ptr &a, const Ptr &b,
                  const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeSqrt(const Ptr &a,
             const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeDiv(const Ptr &a, const Ptr &b,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeExtend(const Ptr &newSize, const Ptr &a,
               const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeGe(const Ptr &a, const Ptr &b,
           const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeGt(const Ptr &a, const Ptr &b,
           const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeLe(const Ptr &a, const Ptr &b,
           const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeLt(const Ptr &a, const Ptr &b,
           const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeMod(const Ptr &a, const Ptr &b,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeMul(const Ptr &a, const Ptr &b,
            const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeWrite(const Ptr &mem, const Ptr &addr, const Ptr &a,
              const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
Ptr makeZerop(const Ptr &a,
              const SmtSolverPtr &solver = SmtSolverPtr(), const std::string &comment="", unsigned flags=0);
/** @} */


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Miscellaneous functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::ostream& operator<<(std::ostream &o, Node&);
std::ostream& operator<<(std::ostream &o, const Node::WithFormatter&);

/** Convert a set to an ite expression. */
Ptr setToIte(const Ptr&, const SmtSolverPtr &solver = SmtSolverPtr(), const LeafPtr &var = LeafPtr());

/**  Hash zero or more expressions.
 *
 *   Computes the hash for each expression, then returns a single has which is a function of the individual hashes. The
 *   order of the expressions does not affect the returned hash. */
Hash hash(const std::vector<Ptr>&);

/** Counts the number of nodes.
 *
 *  Counts the total number of nodes in multiple expressions.  The return value is a saturated sum, returning MAX_NNODES if an
 *  overflow occurs. */
template<typename InputIterator>
uint64_t
nNodes(InputIterator begin, InputIterator end) {
    uint64_t total = 0;
    for (InputIterator ii=begin; ii!=end; ++ii) {
        uint64_t n = (*ii)->nnodes();
        if (MAX_NNODES==n)
            return MAX_NNODES;
        if (total + n < total)
            return MAX_NNODES;
        total += n;
    }
    return total;
}

/** Counts the number of unique nodes.
 *
 *  Counts the number of unique nodes across a number of expressions.  Nodes shared between two expressions are counted only
 *  one time, whereas the Node::nnodes virtual method counts shared nodes multiple times. */
template<typename InputIterator>
uint64_t
nNodesUnique(InputIterator begin, InputIterator end)
{
    struct T1: Visitor {
        typedef std::set<const Node*> SeenNodes;

        SeenNodes seen;                                 // nodes that we've already seen, and the subtree size
        uint64_t nUnique;                               // number of unique nodes

        T1(): nUnique(0) {}

        VisitAction preVisit(const Ptr &node) {
            if (seen.insert(getRawPointer(node)).second) {
                ++nUnique;
                return CONTINUE;                        // this node has not been seen before; traverse into children
            } else {
                return TRUNCATE;                        // this node has been seen already; skip over the children
            }
        }

        VisitAction postVisit(const Ptr &node) {
            return CONTINUE;
        }
    } visitor;

    VisitAction status = CONTINUE;
    for (InputIterator ii=begin; ii!=end && TERMINATE!=status; ++ii)
        status = (*ii)->depthFirstTraversal(visitor);
    return visitor.nUnique;
}

/** Find common subexpressions.
 *
 *  This is similar to @ref Node::findCommonSubexpressions except the analysis is over a collection of expressions
 *  rather than a single expression.
 *
 * @{ */
std::vector<Ptr> findCommonSubexpressions(const std::vector<Ptr>&);

template<typename InputIterator>
std::vector<Ptr>
findCommonSubexpressions(InputIterator begin, InputIterator end) {
    typedef Sawyer::Container::Map<Ptr, size_t> NodeCounts;
    struct T1: Visitor {
        NodeCounts nodeCounts;
        std::vector<Ptr> result;

        VisitAction preVisit(const Ptr &node) ROSE_OVERRIDE {
            size_t &nSeen = nodeCounts.insertMaybe(node, 0);
            if (2 == ++nSeen)
                result.push_back(node);
            return nSeen>1 ? TRUNCATE : CONTINUE;
        }

        VisitAction postVisit(const Ptr&) ROSE_OVERRIDE {
            return CONTINUE;
        }
    } visitor;

    for (InputIterator ii=begin; ii!=end; ++ii)
        (*ii)->depthFirstTraversal(visitor);
    return visitor.result;
}
/** @} */

/** On-the-fly substitutions.
 *
 *  This function uses a user-defined substitutor to generate values that are substituted into the specified expression. This
 *  operates by performing a depth-first search of the specified expression and calling the @p subber at each node. The @p
 *  subber is invoked with two arguments: an expression to be replaced, and an optional SMT solver for simplifications. It
 *  should return either the expression unmodified, or a new expression.  The return value of the @c substitute function as a
 *  whole is either the original expression (if no substitutions were performed) or a new expression. */
template<class Substitution>
Ptr substitute(const Ptr &src, Substitution &subber, const SmtSolverPtr &solver = SmtSolverPtr()) {
    if (!src)
        return Ptr();                                   // no input implies no output

    // Try substituting the whole expression, returning the result.
    Ptr dst = subber(src, solver);
    ASSERT_not_null(dst);
    if (dst != src)
        return dst;

    // Try substituting all the subexpressions.
    InteriorPtr inode = src->isInteriorNode();
    if (!inode)
        return src;
    bool anyChildChanged = false;
    Nodes newChildren;
    newChildren.reserve(inode->nChildren());
    BOOST_FOREACH (const Ptr &child, inode->children()) {
        Ptr newChild = substitute(child, subber, solver);
        if (newChild != child)
            anyChildChanged = true;
        newChildren.push_back(newChild);
    }
    if (!anyChildChanged)
        return src;

    // Some subexpression changed, so build a new expression
    return Interior::instance(inode->getOperator(), newChildren, solver, inode->comment(), inode->flags());
}

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::SymbolicExpr::Interior);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::SymbolicExpr::Leaf);
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::SymbolicExpr::Node, 1);
#endif

#endif
