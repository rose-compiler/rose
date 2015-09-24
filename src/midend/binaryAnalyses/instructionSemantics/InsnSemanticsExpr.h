#ifndef Rose_InsnSemanticsExpr_H
#define Rose_InsnSemanticsExpr_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include "Map.h"

#include <cassert>
#include <inttypes.h>
#include <Sawyer/BitVector.h>
#include <Sawyer/SharedPointer.h>
#include <Sawyer/SmallObject.h>
#include <set>
#include <vector>

namespace rose {
namespace BinaryAnalysis {

class SMTSolver;

/** Namespace supplying types and functions for symbolic expressions. These are used by certain instruction semantics policies
 *  and satisfiability modulo theory (SMT) solvers. These expressions are tailored to bit-vector and integer difference logics,
 *  whereas the expression nodes in other parts of ROSE have different goals. */
namespace InsnSemanticsExpr {

/** Operators for internal nodes of the expression tree. Commutative operators generally take one or more operands.  Operators
 *  such as shifting, extending, and truncating have the size operand appearing before the bit vector on which to operate (this
 *  makes the output more human-readable since the size operand is often a constant). */
enum Operator {
    OP_ADD,                 /**< Addition. One or more operands, all the same width. */
    OP_AND,                 /**< Boolean AND. Operands are all Boolean (1-bit) values. See also OP_BV_AND. */
    OP_ASR,                 /**< Arithmetic shift right. Operand B shifted by A bits; 0 <= A < width(B). A is unsigned. */
    OP_BV_AND,              /**< Bitwise AND. One or more operands, all the same width. */
    OP_BV_OR,               /**< Bitwise OR. One or more operands, all the same width. */
    OP_BV_XOR,              /**< Bitwise exclusive OR. One or more operands, all the same width. */
    OP_CONCAT,              /**< Concatenation. Operand A becomes high-order bits. Any number of operands. */
    OP_EQ,                  /**< Equality. Two operands, both the same width. */
    OP_EXTRACT,             /**< Extract subsequence of bits. Extract bits [A..B) of C. 0 <= A < B <= width(C). */
    OP_INVERT,              /**< Boolean inversion. One operand. */
    OP_ITE,                 /**< If-then-else. A must be one bit. Returns B if A is set, C otherwise. */
    OP_LSSB,                /**< Least significant set bit or zero. One operand. */
    OP_MSSB,                /**< Most significant set bit or zero. One operand. */
    OP_NE,                  /**< Inequality. Two operands, both the same width. */
    OP_NEGATE,              /**< Arithmetic negation. One operand. */
    OP_NOOP,                /**< No operation. Used only by the default constructor. */
    OP_OR,                  /**< Boolean OR. Operands are all Boolean (1-bit) values. See also OP_BV_OR. */
    OP_READ,                /**< Read a value from memory.  Arguments are the memory state and the address expression. */
    OP_ROL,                 /**< Rotate left. Rotate bits of B left by A bits.  0 <= A < width(B). A is unsigned. */
    OP_ROR,                 /**< Rotate right. Rotate bits of B right by A bits. 0 <= B < width(B). A is unsigned.  */
    OP_SDIV,                /**< Signed division. Two operands, A/B. Result width is width(A). */
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
    OP_ZEROP,               /**< Equal to zero. One operand. Result is a single bit, set iff A is equal to zero. */
};

const char *to_str(Operator o);

class TreeNode;
class InternalNode;
class LeafNode;

typedef Sawyer::SharedPointer<const TreeNode> TreeNodePtr;
typedef Sawyer::SharedPointer<const InternalNode> InternalNodePtr;
typedef Sawyer::SharedPointer<const LeafNode> LeafNodePtr;
typedef std::vector<TreeNodePtr> TreeNodes;
typedef Map<uint64_t, uint64_t> RenameMap;

/** Controls formatting of expression trees when printing. */
struct Formatter {
    enum ShowComments {
        CMT_SILENT,                             /**< Do not show comments. */
        CMT_AFTER,                              /**< Show comments after the node. */
        CMT_INSTEAD,                            /**< Like CMT_AFTER, but show comments instead of variable names. */
    };
    Formatter()
        : show_comments(CMT_INSTEAD), do_rename(false), add_renames(true), use_hexadecimal(true),
          max_depth(0), cur_depth(0), show_width(true), show_flags(true) {}
    ShowComments show_comments;                 /**< Show node comments when printing? */
    bool do_rename;                             /**< Use the @p renames map to rename variables to shorter names? */
    bool add_renames;                           /**< Add additional entries to the @p renames as variables are encountered? */
    bool use_hexadecimal;                       /**< Show values in hexadecimal and decimal rather than just decimal. */
    size_t max_depth;                           /**< If non-zero, then replace deep parts of expressions with "...". */
    size_t cur_depth;                           /**< Depth in expression. */
    RenameMap renames;                          /**< Map for renaming variables to use smaller integers. */
    bool show_width;                            /**< Show width in bits inside square brackets. */
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
    virtual VisitAction preVisit(const TreeNodePtr&) = 0;
    virtual VisitAction postVisit(const TreeNodePtr&) = 0;
};

/** Any node of an expression tree for instruction semantics, from which the InternalNode and LeafNode classes are derived.
 *  Every node has a specified number of significant bits that is constant over the life of the node.
 *
 *  In order that subtrees can be freely assigned as children of other nodes (provided the structure as a whole remains a
 *  lattice and not a graph with cycles), tree nodes are always referenced through shared-ownership pointers
 *  (<code>Sawyer::SharedPointer<const T></code> where @t T is one of the tree node types: TreeNode, InternalNode, or LeafNode.
 *  For convenience, we define TreeNodePtr, InternalNodePtr, and LeafNodePtr typedefs.  The pointers themselves collectively
 *  own the pointer to the tree node and thus the tree node pointer should never be deleted explicitly.
 *
 *  Each node has a bit flags property, the bits of which are defined by the user.  New nodes are created having all bits
 *  cleared unless the user specifies a value in the constructor.  Bits are significant for hashing. Simplifiers produce
 *  result expressions whose bits are set in a predictable manner with the following rules:
 *
 *  @li Internal Node Rule: The flags for an internal node are the union of the flags of its subtrees.
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
 *      constant folding, which is therefore consistent with the Internal Node Rule.
 *
 *  @li Hashing Rule: User-defined flags are significant for hashing.  E.g., structural equivalence will return false if the
 *      two expressions have different flags since structural equivalence uses hashes.
 *
 *  @li Relational Operator Rule:  Simplification of relational operators to produce a Boolean constant will act as if they are
 *      performing constant folding even if the simplification is on variables.  E.g., <code>(ule v1 v1)</code> results in true
 *      with flags the same as @c v1. */
class TreeNode: public Sawyer::SharedObject, public Sawyer::SharedFromThis<TreeNode>, public Sawyer::SmallObject {
protected:
    size_t nbits;                /**< Number of significant bits. Constant over the life of the node. */
    size_t domainWidth_;         /**< Width of domain for unary functions. E.g., memory. */
    unsigned flags_;             /**< Bit flags. Meaning of flags is up to the user. Low-order 16 bits are reserved. */
    mutable std::string comment; /**< Optional comment. Only for debugging; not significant for any calculation. */
    mutable uint64_t hashval;    /**< Optional hash used as a quick way to indicate that two expressions are different. */

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
    TreeNode()
        : nbits(0), domainWidth_(0), flags_(0), hashval(0) {}
    explicit TreeNode(std::string comment, unsigned flags=0)
        : nbits(0), domainWidth_(0), flags_(flags), comment(comment), hashval(0) {}

public:
    /** Returns true if two expressions must be equal (cannot be unequal).  If an SMT solver is specified then that solver is
     * used to answer this question, otherwise equality is established by looking only at the structure of the two
     * expressions. Two expressions can be equal without being the same width (e.g., a 32-bit constant zero is equal to a
     * 16-bit constant zero). */
    virtual bool must_equal(const TreeNodePtr& other, SMTSolver*) const = 0;

    /** Returns true if two expressions might be equal, but not necessarily be equal. */
    virtual bool may_equal(const TreeNodePtr& other, SMTSolver*) const = 0;

    /** Tests two expressions for structural equivalence.  Two leaf nodes are equivalent if they are the same width and have
     *  equal values or are the same variable. Two internal nodes are equivalent if they are the same width, the same
     *  operation, have the same number of children, and those children are all pairwise equivalent. */
    virtual bool equivalent_to(const TreeNodePtr& other) const = 0;

    /** Compare two expressions structurally for sorting. Returns -1 if @p this is less than @p other, 0 if they are
     *  structurally equal, and 1 if @p this is greater than @p other.  This function returns zero when an only when @ref
     *  equivalent_to returns zero, but @ref equivalent_to can be much faster since it uses hashing. */
    virtual int structural_compare(const TreeNodePtr& other) const = 0;

    /** Substitute one value for another. Finds all occurrances of @p from in this expression and replace them with @p to. If a
     * substitution occurs, then a new expression is returned. The matching of @p from to sub-parts of this expression uses
     * structural equivalence, the equivalent_to() predicate. The @p from and @p to expressions must have the same width. */
    virtual TreeNodePtr substitute(const TreeNodePtr &from, const TreeNodePtr &to) const = 0;

    /** Returns true if the expression is a known value. */
    virtual bool is_known() const = 0;

    /** Returns the integer value of a node for which is_known() returns true.  The high-order bits, those beyond the number of
     *  significant bits returned by get_nbits(), are guaranteed to be zero. */
    virtual uint64_t get_value() const = 0;

    /** Accessors for the comment string associated with a node. Comments can be changed after a node has been created since
     *  the comment is not intended to be used for anything but annotation and/or debugging. I.e., comments are not
     *  considered significant for comparisons, computing hash values, etc.
     * @{ */
    const std::string& get_comment() const { return comment; }
    void set_comment(const std::string &s) const { comment=s; }
    /** @} */

    /** Returns the number of significant bits.  An expression with a known value is guaranteed to have all higher-order bits
     *  cleared. */
    size_t get_nbits() const { return nbits; }

    /** Returns the user-defined bit flags. */
    unsigned get_flags() const { return flags_; }

    /** Sets flags. Since symbolic expressions are immutable it is not possible to change the flags directly. Therefore if the
     *  desired flags are different than the current flags a new expression is created that is the same in every other
     *  respect. If the flags are not changed then the original expression is returned. */
    TreeNodePtr newFlags(unsigned flags) const;

    /** Returns address width for memory expressions.
     *
     *  The return value is non-zero if and only if this tree node is a memory expression. */
    size_t domainWidth() const { return domainWidth_; }

    /** Check whether expression is scalar.
     *
     *  Everything is scalar except for memory. */
    bool isScalar() const { return 0 == domainWidth_; }

    /** Traverse the expression.  The expression is traversed in a depth-first visit.  The final return value is the final
     *  return value of the last call to the visitor. */
    virtual VisitAction depth_first_traversal(Visitor&) const = 0;

    /** Computes the size of an expression by counting the number of nodes.  Operates in constant time.   Note that it is
     *  possible (even likely) for the 64-bit return value to overflow in expressions when many nodes are shared.  For
     *  instance, the following loop will create an expression that contains more than 2^64 nodes:
     *
     *  @code
     *   InsnSemanticsExpr expr = LeafNode::create_variable(32);
     *   for(size_t i=0; i<64; ++i)
     *       expr = InternalNode::create(32, OP_ADD, expr, expr)
     *  @endcode
     *
     *  When an overflow occurs the result is meaningless.
     *
     *  @sa nnodesUnique */
    virtual uint64_t nnodes() const = 0;

    /** Number of unique nodes in expression. */
    uint64_t nnodesUnique() const;

    /** Returns the variables appearing in the expression. */
    std::set<LeafNodePtr> get_variables() const;

    /** Dynamic cast of this object to an internal node. */
    InternalNodePtr isInternalNode() const {
        return sharedFromThis().dynamicCast<const InternalNode>();
    }

    /** Dynamic cast of this object to a leaf node. */
    LeafNodePtr isLeafNode() const {
        return sharedFromThis().dynamicCast<const LeafNode>();
    }

    /** Returns true if this node has a hash value computed and cached. The hash value zero is reserved to indicate that no
     *  hash has been computed; if a node happens to actually hash to zero, it will not be cached and will be recomputed for
     *  every call to hash(). */
    bool is_hashed() const { return hashval!=0; }

    /** Returns (and caches) the hash value for this node.  If a hash value is not cached in this node, then a new hash value
     *  is computed and cached. */
    uint64_t hash() const;

    /** A node with formatter. See the with_format() method. */
    class WithFormatter {
    private:
        TreeNodePtr node;
        Formatter &formatter;
    public:
        WithFormatter(const TreeNodePtr &node, Formatter &formatter): node(node), formatter(formatter) {}
        void print(std::ostream &stream) const { node->print(stream, formatter); }
    };

    /** Combines a node with a formatter for printing.  This is used for convenient printing with the "<<" operator. For
     *  instance:
     *
     * @code
     *  Formatter fmt;
     *  fmt.show_comments = Formatter::CMT_AFTER; //show comments after the variable
     *  TreeNodePtr expression = ...;
     *  std::cout <<"method 1: "; expression->print(std::cout, fmt); std::cout <<"\n";
     *  std::cout <<"method 2: " <<expression->with_format(fmt) <<"\n";
     *  std::cout <<"method 3: " <<*expression+fmt <<"\n";
     * 
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) const { return WithFormatter(sharedFromThis(), fmt); }
    WithFormatter operator+(Formatter &fmt) const { return with_format(fmt); }
    /** @} */

    /** Print the expression to a stream.  The output is an S-expression with no line-feeds. The format of the output is
     *  controlled by the mutable Formatter argument.
     *  @{ */
    virtual void print(std::ostream&, Formatter&) const = 0;
    void print(std::ostream &o) const { Formatter fmt; print(o, fmt); }
    /** @} */

    /** Asserts that expressions are acyclic. This is intended only for debugging. */
    void assert_acyclic() const;

    /** Find common subexpressions.
     *
     *  Returns a vector of the largest common subexpressions. The list is computed by performing a depth-first search on this
     *  expression and adding expressions to the return vector whenever a subtree is encountered a second time. Therefore the
     *  if a common subexpression A contains another common subexpression B then B will appear earlier in the list than A. */
    std::vector<TreeNodePtr> findCommonSubexpressions() const;

protected:
    void printFlags(std::ostream &o, unsigned flags, char &bracket) const;
};

/** Operator-specific simplification methods. */
class Simplifier {
public:
    virtual ~Simplifier() {}

    /** Constant folding. The range @p begin (inclusive) to @p end (exclusive) must contain at least two nodes and all of
     *  the nodes must be leaf nodes with known values.  This method returns a new folded node if folding is possible, or
     *  the null pointer if folding is not possible. */
    virtual TreeNodePtr fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const {
        return TreeNodePtr();
    }

    /** Rewrite the entire expression to something simpler. Returns the new node if the node can be simplified, otherwise
     *  returns null. */
    virtual TreeNodePtr rewrite(const InternalNode*) const {
        return TreeNodePtr();
    }
};

struct AddSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const ROSE_OVERRIDE;
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct AndSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const ROSE_OVERRIDE;
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct OrSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const ROSE_OVERRIDE;
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct XorSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const ROSE_OVERRIDE;
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct SmulSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const ROSE_OVERRIDE;
};
struct UmulSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const ROSE_OVERRIDE;
};
struct ConcatSimplifier: Simplifier {
    virtual TreeNodePtr fold(TreeNodes::const_iterator, TreeNodes::const_iterator) const ROSE_OVERRIDE;
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct ExtractSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct AsrSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct InvertSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct NegateSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct IteSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct NoopSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct RolSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct RorSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct UextendSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct SextendSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct EqSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct SgeSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct SgtSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct SleSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct SltSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct UgeSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct UgtSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct UleSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct UltSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct ZeropSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct SdivSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct SmodSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct UdivSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct UmodSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct ShiftSimplifier: Simplifier {
    bool newbits;
    ShiftSimplifier(bool newbits): newbits(newbits) {}
    TreeNodePtr combine_strengths(TreeNodePtr strength1, TreeNodePtr strength2, size_t value_width) const;
};
struct ShlSimplifier: ShiftSimplifier {
    ShlSimplifier(bool newbits): ShiftSimplifier(newbits) {}
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct ShrSimplifier: ShiftSimplifier {
    ShrSimplifier(bool newbits): ShiftSimplifier(newbits) {}
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct LssbSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};
struct MssbSimplifier: Simplifier {
    virtual TreeNodePtr rewrite(const InternalNode*) const ROSE_OVERRIDE;
};

/** Internal node of an expression tree for instruction semantics. Each internal node has an operator (constant for the life of
 *  the node and obtainable with get_operator()) and zero or more children. Children are added to the internal node during the
 *  construction phase. Once construction is complete, the children should only change in ways that don't affect the value of
 *  the node as a whole (since this node might be pointed to by any number of expressions). */
class InternalNode: public TreeNode {
private:
    Operator op;
    TreeNodes children;
    uint64_t nnodes_;                                   // total number of nodes; self + children's nnodes

    // Constructors should not be called directly.  Use the create() class method instead. This is to help prevent
    // accidently using pointers to these objects -- all access should be through shared-ownership pointers.
    InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, std::string comment="")
        : TreeNode(comment), op(op), nnodes_(1) {
        add_child(a);
        adjustWidth();
        adjustBitFlags(0);
        ASSERT_require(get_nbits() == nbits);
    }
    InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, std::string comment="")
        : TreeNode(comment), op(op), nnodes_(1) {
        add_child(a);
        add_child(b);
        adjustWidth();
        adjustBitFlags(0);
        ASSERT_require(get_nbits() == nbits);
    }
    InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, const TreeNodePtr &c,
                 std::string comment="")
        : TreeNode(comment), op(op), nnodes_(1) {
        add_child(a);
        add_child(b);
        add_child(c);
        adjustWidth();
        adjustBitFlags(0);
        ASSERT_require(get_nbits() == nbits);
    }
    InternalNode(size_t nbits, Operator op, const TreeNodes &children, std::string comment="", unsigned flags=0)
        : TreeNode(comment), op(op), nnodes_(1) {
        for (size_t i=0; i<children.size(); ++i)
            add_child(children[i]);
        adjustWidth();
        adjustBitFlags(flags);
        ASSERT_require(0 == nbits || get_nbits() == nbits);
    }

public:
    /** Create a new expression node. Although we're creating internal nodes, the simplification process might replace it with
     *  a leaf node. Use these class methods instead of c'tors.
     *
     *  Flags are normally initialized as the union of the flags of the operator arguments subject to various rules in the
     *  expression simplifiers. Flags specified in the constructor are set in addition to those that would normally be set.
     *
     *  @{ */
    static TreeNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const std::string comment="") {
        InternalNodePtr retval(new InternalNode(nbits, op, a, comment));
        return retval->simplifyTop();
    }
    static TreeNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b,
                                  const std::string comment="") {
        InternalNodePtr retval(new InternalNode(nbits, op, a, b, comment));
        return retval->simplifyTop();
    }
    static TreeNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, const TreeNodePtr &c,
                                  const std::string comment="") {
        InternalNodePtr retval(new InternalNode(nbits, op, a, b, c, comment));
        return retval->simplifyTop();
    }
    static TreeNodePtr create(size_t nbits, Operator op, const TreeNodes &children, const std::string comment="",
                              unsigned flags=0) {
        InternalNodePtr retval(new InternalNode(nbits, op, children, comment, flags));
        return retval->simplifyTop();
    }
    /** @} */

    /* see superclass, where these are pure virtual */
    virtual bool must_equal(const TreeNodePtr &other, SMTSolver*) const;
    virtual bool may_equal(const TreeNodePtr &other, SMTSolver*) const;
    virtual bool equivalent_to(const TreeNodePtr &other) const;
    virtual int structural_compare(const TreeNodePtr& other) const;
    virtual TreeNodePtr substitute(const TreeNodePtr &from, const TreeNodePtr &to) const;
    virtual bool is_known() const {
        return false; /*if it's known, then it would have been folded to a leaf*/
    }
    virtual uint64_t get_value() const { ASSERT_forbid2(true, "not a constant value"); return 0;}
    virtual VisitAction depth_first_traversal(Visitor&) const;
    virtual uint64_t nnodes() const { return nnodes_; }

    /** Returns the number of children. */
    size_t nchildren() const { return children.size(); }

    /** Returns the specified child. */
    TreeNodePtr child(size_t idx) const { ASSERT_require(idx<children.size()); return children[idx]; }

    /** Returns all children. */
    TreeNodes get_children() const { return children; }

    /** Returns the operator. */
    Operator get_operator() const { return op; }

    /** Simplifies the specified internal node. Returns a new node if necessary, otherwise returns this. */
    TreeNodePtr simplifyTop() const;

    /** Perform constant folding.  This method returns either a new expression (if changes were mde) or the original
     *  expression. The simplifier is specific to the kind of operation at the node being simplified. */
    TreeNodePtr constant_folding(const Simplifier &simplifier) const;

    /** Simplifies non-associative operators by flattening the specified internal node with its children that are the same
     *  internal node type. Call this only if the top node is a truly non-associative. A new node is returned only if
     *  changed. When calling both nonassociative and commutative, it's usually more appropriate to call nonassociative
     *  first. */
    InternalNodePtr nonassociative() const;

    /** Simplifies commutative operators by sorting arguments. The arguments are sorted so that all the internal nodes come
     *  before the leaf nodes. Call this only if the top node is truly commutative.  A new node is returned only if
     *  changed. When calling both nonassociative and commutative, it's usually more appropriate to call nonassociative
     *  first. */
    InternalNodePtr commutative() const;

    /** Simplifies involutary operators.  An involutary operator is one that is its own inverse.  This method should only be
     *  called if this node is an internal node whose operator has the involutary property (such as invert or negate). Returns
     *  either a new expression that is simplified, or the original expression. */
    TreeNodePtr involutary() const;

    /** Simplifies nested shift-like operators. Simplifies (shift AMT1 (shift AMT2 X)) to (shift (add AMT1 AMT2) X). */
    TreeNodePtr additive_nesting() const;

    /** Removes identity arguments. Returns either a new expression or the original expression. */
    TreeNodePtr identity(uint64_t ident) const;

    /** Replaces a binary operator with its only argument. Returns either a new expression or the original expression. */
    TreeNodePtr unaryNoOp() const;

    /** Simplify an internal node. Returns a new node if this node could be simplified, otherwise returns this node. When
     *  the simplification could result in a leaf node, we return an OP_NOOP internal node instead. */
    TreeNodePtr rewrite(const Simplifier &simplifier) const;

    // documented in super class
    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;

protected:
    /** Appends @p child as a new child of this node. The modification is done in place, so one must be careful that this node
     *  is not part of other expressions.  It is safe to call add_child() on a node that was just created and not used anywhere
     *  yet. If you add a new child, then you probably need to call adjustWidth after the last one is added. */
    void add_child(const TreeNodePtr &child);

    /** Adjust width based on operands. This should only be called from constructors. */
    void adjustWidth();

    /** Adjust user-defined bit flags. This should only be called from constructors.  Flags are the union of the operand flags
     *  subject to simplification rules, unioned with the specified flags. */
    void adjustBitFlags(unsigned extraFlags);
};

/** Leaf node of an expression tree for instruction semantics.
 *
 *  A leaf node is either a known bit vector value, a free bit vector variable, or a memory state. */
class LeafNode: public TreeNode {
private:
    enum LeafType { CONSTANT, BITVECTOR, MEMORY };
    LeafType leaf_type;
    Sawyer::Container::BitVector bits; /**< Value when 'known' is true */
    uint64_t name;                     /**< Variable ID number when 'known' is false. */

    // Private to help prevent creating pointers to leaf nodes.  See create_* methods instead.
    LeafNode()
        : TreeNode(""), leaf_type(CONSTANT), name(0) {}
    explicit LeafNode(const std::string &comment, unsigned flags=0)
        : TreeNode(comment, flags), leaf_type(CONSTANT), name(0) {}

    static uint64_t name_counter;

public:
    /** Construct a new free variable with a specified number of significant bits. */
    static LeafNodePtr create_variable(size_t nbits, std::string comment="", unsigned flags=0);

    /*  Construct another reference to an existing variable.  This method is used internally by the expression parsing
     *  mechanism to produce a new instance of some previously existing variable -- both instances are the same variable and
     *  therefore should be given the same size (although this consistency cannot be checked automatically). */
    static LeafNodePtr create_existing_variable(size_t nbits, uint64_t id, const std::string &comment="", unsigned flags=0);

    /** Construct a new integer with the specified number of significant bits. Any high-order bits beyond the specified size
     *  will be zeroed. */
    static LeafNodePtr create_integer(size_t nbits, uint64_t n, std::string comment="", unsigned flags=0);

    /** Construct a new known value with the specified bits. */
    static LeafNodePtr create_constant(const Sawyer::Container::BitVector &bits, std::string comment="", unsigned flags=0);

    /** Create a new Boolean, a single-bit integer. */
    static LeafNodePtr create_boolean(bool b, std::string comment="", unsigned flags=0) {
        return create_integer(1, (uint64_t)(b?1:0), comment, flags);
    }

    /** Construct a new memory state.  A memory state is a function that maps addresses to values. */
    static LeafNodePtr create_memory(size_t addressWidth, size_t valueWidth, std::string comment="", unsigned flags=0);

    /* see superclass, where these are pure virtual */
    virtual bool is_known() const;
    virtual uint64_t get_value() const;
    virtual const Sawyer::Container::BitVector& get_bits() const;
    virtual bool must_equal(const TreeNodePtr &other, SMTSolver*) const;
    virtual bool may_equal(const TreeNodePtr &other, SMTSolver*) const;
    virtual bool equivalent_to(const TreeNodePtr &other) const;
    virtual int structural_compare(const TreeNodePtr& other) const;
    virtual TreeNodePtr substitute(const TreeNodePtr &from, const TreeNodePtr &to) const;
    virtual VisitAction depth_first_traversal(Visitor&) const;
    virtual uint64_t nnodes() const { return 1; }

    /** Is the node a bitvector variable? */
    virtual bool is_variable() const;

    /** Does the node represent memory? */
    virtual bool is_memory() const;

    /** Returns the name of a free variable.  The output functions print variables as "vN" where N is an integer. It is this N
     *  that this method returns.  It should only be invoked on leaf nodes for which is_known() returns false. */
    uint64_t get_name() const;

    /** Returns a string for the leaf.
     *
     *  Variables are returned as "vN", memory is returned as "mN", and constants are returned as a hexadecimal string, where N
     *  is a variable or memory number. */
    std::string toString() const;

    // documented in super class
    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;

    /** Prints an integer interpreted as a signed value. */
    void print_as_signed(std::ostream&, Formatter&, bool as_signed=true) const;
    void print_as_unsigned(std::ostream &o, Formatter &f) const {
        print_as_signed(o, f, false);
    }
};

std::ostream& operator<<(std::ostream &o, const TreeNode&);
std::ostream& operator<<(std::ostream &o, const TreeNode::WithFormatter&);

/** Counts the number of nodes.
 *
 *  Counts the total number of nodes in multiple expressions.  The return value is a saturated sum, returning MAX_NNODES if an
 *  overflow occurs. */
template<typename InputIterator>
uint64_t
nnodes(InputIterator begin, InputIterator end) {
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
 *  one time, whereas the TreeNode::nnodes virtual method counts shared nodes multiple times. */
template<typename InputIterator>
uint64_t
nnodesUnique(InputIterator begin, InputIterator end)
{
    struct T1: Visitor {
        typedef std::set<const TreeNode*> SeenNodes;

        SeenNodes seen;                                 // nodes that we've already seen, and the subtree size
        uint64_t nUnique;                               // number of unique nodes

        T1(): nUnique(0) {}

        VisitAction preVisit(const TreeNodePtr &node) {
            if (seen.insert(getRawPointer(node)).second) {
                ++nUnique;
                return CONTINUE;                        // this node has not been seen before; traverse into children
            } else {
                return TRUNCATE;                        // this node has been seen already; skip over the children
            }
        }

        VisitAction postVisit(const TreeNodePtr &node) {
            return CONTINUE;
        }
    } visitor;

    VisitAction status = CONTINUE;
    for (InputIterator ii=begin; ii!=end && TERMINATE!=status; ++ii)
        status = (*ii)->depth_first_traversal(visitor);
    return visitor.nUnique;
}

/** Find common subexpressions.
 *
 *  This is similar to @ref TreeNodePtr::findCommonSubexpressions except the analysis is over a collection of expressions
 *  rather than a single expression.
 *
 * @{ */
std::vector<TreeNodePtr> findCommonSubexpressions(const std::vector<TreeNodePtr>&);

template<typename InputIterator>
std::vector<TreeNodePtr>
findCommonSubexpressions(InputIterator begin, InputIterator end) {
    typedef Sawyer::Container::Map<TreeNodePtr, size_t> NodeCounts;
    struct T1: Visitor {
        NodeCounts nodeCounts;
        std::vector<TreeNodePtr> result;

        VisitAction preVisit(const TreeNodePtr &node) ROSE_OVERRIDE {
            size_t &nSeen = nodeCounts.insertMaybe(node, 0);
            if (2 == ++nSeen)
                result.push_back(node);
            return nSeen>1 ? TRUNCATE : CONTINUE;
        }

        VisitAction postVisit(const TreeNodePtr&) ROSE_OVERRIDE {
            return CONTINUE;
        }
    } visitor;

    for (InputIterator ii=begin; ii!=end; ++ii)
        (*ii)->depth_first_traversal(visitor);
    return visitor.result;
}
/** @} */

} // namespace
} // namespace
} // namespace

#endif
