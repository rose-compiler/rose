#ifndef Rose_InsnSemanticsExpr_H
#define Rose_InsnSemanticsExpr_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class SMTSolver;

/** Namespace supplying types and functions for symbolic expressions. These are used by certain instruction semantics policies
 *  and satisfiability modulo theory (SMT) solvers. These expressions are tailored to bit-vector and integer difference logics,
 *  whereas the expression nodes in other parts of ROSE have different goals. */
namespace InsnSemanticsExpr {

    /** Operators for internal nodes of the expression tree. Commutative operators generally take one or more operands.
     *  Operators such as shifting, extending, and truncating have the size operand appearing before the bit vector on which
     *  to operate (this makes the output more human-readable since the size operand is often a constant). */
    enum Operator 
        {
        OP_ADD,                 /**< Addition. One or more operands, all the same width. */
        OP_AND,                 /**< Boolean AND. Operands are all Boolean (1-bit) values. See also OP_BV_AND. */
        OP_ASR,                 /**< Arithmetic shift right. Operand B shifted by A bits; 0 <= A < width(B). */
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
        OP_ROL,                 /**< Rotate left. Rotate bits of B left by A bits.  0 <= A < width(B). */
        OP_ROR,                 /**< Rotate right. Rotate bits of B right by A bits. 0 <= B < width(B).  */
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

    typedef std::map<uint64_t, uint64_t> RenameMap;

    class TreeNode;
    class InternalNode;
    class LeafNode;

    typedef boost::shared_ptr<const TreeNode> TreeNodePtr;
    typedef boost::shared_ptr<const InternalNode> InternalNodePtr;
    typedef boost::shared_ptr<const LeafNode> LeafNodePtr;

    class Visitor {
    public:
        virtual ~Visitor() {}
        virtual void operator()(const TreeNodePtr&) = 0;
    };

    /** Any node of an expression tree for instruction semantics, from which the InternalNode and LeafNode classes are
     *  derived.  Every node has a specified number of significant bits that is constant over the life of the node.
     *
     *  In order that subtrees can be freely assigned as children of other nodes (provided the structure as a whole remains a
     *  lattice and not a graph with cycles), tree nodes are always referenced through boost::shared_ptr<const T> where T is
     *  one of the tree node types: TreeNode, InternalNode, or LeafNode.  For convenience, we define TreeNodePtr,
     *  InternalNodePtr, and LeafNodePtr typedefs.  The shared_ptr owns the pointer to the tree node and thus the tree node
     *  pointer should never be deleted explicitly. */
    class TreeNode: public boost::enable_shared_from_this<TreeNode> {
    protected:
        size_t nbits;           /**< Number of significant bits. Constant over the life of the node. */
        std::string comment;    /**< Optional comment. */
    public:
        TreeNode(size_t nbits, std::string comment=""): nbits(nbits), comment(comment) { assert(nbits>0); }

        /** Print the expression to a stream.  The output is an S-expression with no line-feeds.  If @p rmap is non-null then
         *  it will be used to rename free variables for readability.  If the expression contains N variables, then the new
         *  names will be numbered from the set M = (i | 0 <= i <= N-1). */
        virtual void print(std::ostream&, RenameMap *rmap=NULL) const = 0;

        /** Tests two expressions for equality using an optional satisfiability modulo theory (SMT) solver. Returns true if
         *  the inequality is not satisfiable.  Two expressions can be equal without necessarily being the same width (e.g., a
         *  32-bit constant zero is equal to a 16-bit constant zero). */
        virtual bool equal_to(const TreeNodePtr& other, SMTSolver*) const = 0;

        /** Tests two expressions for structural equivalence.  Two leaf nodes are equivalent if they are the same width and
         *  have equal values or are the same variable. Two internal nodes are equivalent if they are the same width, the same
         *  operation, have the same number of children, and those children are all pairwise equivalent. */
        virtual bool equivalent_to(const TreeNodePtr& other) const = 0;

        /** Returns true if the expression is a known value.
         *
         *  FIXME: The current implementation returns true only when @p this node is leaf node with a known value. Since
         *         InsnSemanticsExpr does not do constant folding, this is of limited use. [RPM 2010-06-08]. */
        virtual bool is_known() const = 0;

        /** Returns the integer value of a node for which is_known() returns true.  The high-order bits, those beyond the
         *  number of significant bits returned by get_nbits(), are guaranteed to be zero. */
        virtual uint64_t get_value() const = 0;

        /** Accessors for the comment string associated with a node.
         * @{ */
        const std::string& get_comment() const { return comment; }
        void set_comment(const std::string &s) { comment=s; }
        /** @} */

        /** Returns the number of significant bits.  An expression with a known value is guaranteed to have all higher-order
         *  bits cleared. */
        size_t get_nbits() const { return nbits; }

        /** Traverse the expression.  The expression is traversed in a depth-first visit, invoking the functor at each node of
         *  the expression tree. */
        virtual void depth_first_visit(Visitor*) const = 0;

        /** Returns the variables appearing in the expression. */
        std::set<LeafNodePtr> get_variables() const;

        /** Dynamic cast of this object to an internal node. */
        InternalNodePtr isInternalNode() const {
            return boost::dynamic_pointer_cast<const InternalNode>(shared_from_this());
        }

        /** Dynamic cast of this object to a leaf node. */
        LeafNodePtr isLeafNode() const {
            return boost::dynamic_pointer_cast<const LeafNode>(shared_from_this());
        }
    };

    /** Internal node of an expression tree for instruction semantics. Each internal node has an operator (constant for the
     *  life of the node and obtainable with get_operator()) and zero or more children. Children are added to the internal
     *  node during the construction phase. Once construction is complete, the children should only change in ways that don't
     *  affect the value of the node as a whole (since this node might be pointed to by any number of expressions). */
    class InternalNode: public TreeNode {
    private:
        Operator op;
        std::vector<TreeNodePtr> children;

        // Constructors should not be called directly.  Use the create() class method instead. This is to help prevent
        // accidently using pointers to these objects -- all access should be through boost::shared_ptr<>.
        InternalNode(size_t nbits, Operator op, const std::string comment="")
            : TreeNode(nbits, comment), op(op) {}
        InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, std::string comment="")
            : TreeNode(nbits, comment), op(op) {
            add_child(a);
        }
        InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, std::string comment="")
            : TreeNode(nbits, comment), op(op) {
            add_child(a);
            add_child(b);
        }
        InternalNode(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, const TreeNodePtr &c,
                     std::string comment="")
            : TreeNode(nbits, comment), op(op) {
            add_child(a);
            add_child(b);
            add_child(c);
        }
        InternalNode(size_t nbits, Operator op, const std::vector<TreeNodePtr> &children, std::string comment="")
            : TreeNode(nbits, comment), op(op) {
            for (size_t i=0; i<children.size(); ++i)
                add_child(children[i]);
        }

    public:
        /** Create a new expression node. Use these class methods instead of c'tors. */
        static InternalNodePtr create(size_t nbits, Operator op, const std::string comment="") {
            InternalNodePtr retval(new InternalNode(nbits, op, comment));
            return retval;
        }
        static InternalNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const std::string comment="") {
            InternalNodePtr retval(new InternalNode(nbits, op, a, comment));
            return retval;
        }
        static InternalNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b,
                                      const std::string comment="") {
            InternalNodePtr retval(new InternalNode(nbits, op, a, b, comment));
            return retval;
        }
        static InternalNodePtr create(size_t nbits, Operator op, const TreeNodePtr &a, const TreeNodePtr &b, const TreeNodePtr &c,
                                      const std::string comment="") {
            InternalNodePtr retval(new InternalNode(nbits, op, a, b, c, comment));
            return retval;
        }
        static InternalNodePtr create(size_t nbits, Operator op, const std::vector<TreeNodePtr> &children,
                                      const std::string comment="") {
            InternalNodePtr retval(new InternalNode(nbits, op, children, comment));
            return retval;
        }
        /** @} */

        /* see superclass, where this is pure virtual */
        virtual void print(std::ostream &o, RenameMap *rmap=NULL) const;

        /* see superclass, where this is pure virtual */
        virtual bool equal_to(const TreeNodePtr &other, SMTSolver*) const;

        /* see superclass, where this is pure virtual */
        virtual bool equivalent_to(const TreeNodePtr &other) const;

        /* see superclass, where this is pure virtual */
        virtual bool is_known() const {
            return false; /*if it's known, then it would have been folded to a leaf*/
        }

        /* see superclass, where this is pure virtual */
        virtual uint64_t get_value() const { ROSE_ASSERT(!"not a constant value"); return 0;}

        /* see superclass, where this is pure virtual */
        virtual void depth_first_visit(Visitor*) const;

        /** Returns the number of children. */
        size_t size() const { return children.size(); }

        /** Returns the specified child. */
        TreeNodePtr child(size_t idx) const { assert(idx<children.size()); return children[idx]; }

        /** Returns the operator. */
        Operator get_operator() const { return op; }

    protected:
        /** Appends @p child as a new child of this node. The modification is done in place, so one must be careful that this
         *  node is not part of other expressions.  It is safe to call add_child() on a node that was just created and not used
         *  anywhere yet. */
        void add_child(const TreeNodePtr &child);
    };

    /** Leaf node of an expression tree for instruction semantics.
     *
     *  A leaf node is either a known bit vector value, a free bit vector variable, or a memory state. */
    class LeafNode: public TreeNode {
    private:
        enum LeafType { CONSTANT, BITVECTOR, MEMORY };
        LeafType leaf_type;
        union {
            uint64_t ival;                  /**< Integer (unsigned) value when 'known' is true; unused msb are zero */
            uint64_t name;                  /**< Variable ID number when 'known' is false. */
        };

        // Private to help prevent creating pointers to leaf nodes.  See create_* methods instead.
        LeafNode(std::string comment=""): TreeNode(32, comment), leaf_type(CONSTANT), ival(0) {}

        static uint64_t name_counter;

    public:
        /** Construct a new free variable with a specified number of significant bits. */
        static LeafNodePtr create_variable(size_t nbits, std::string comment="");

        /** Construct a new integer with the specified number of significant bits. Any high-order bits beyond the specified
         *  size will be zeroed. */
        static LeafNodePtr create_integer(size_t nbits, uint64_t n, std::string comment="");

        /** Construct a new memory state.  A memory state is a function that maps a 32-bit address to a value of
         *  specified size. */
        static LeafNodePtr create_memory(size_t nbits, std::string comment="");

        /* see superclass, where this is pure virtual */
        virtual bool is_known() const;

        /* see superclass, where this is pure virtual */
        virtual uint64_t get_value() const;

        /** Is the node a bitvector variable? */
        virtual bool is_variable() const;

        /** Does the node represent memory? */
        virtual bool is_memory() const;

        /** Returns the name of a free variable.  The output functions print variables as "vN" where N is an integer. It is
         *  this N that this method returns.  It should only be invoked on leaf nodes for which is_known() returns false. */
        uint64_t get_name() const;

        /* see superclass, where this is pure virtual */
        virtual void print(std::ostream &o, RenameMap *rmap=NULL) const;

        /* see superclass, where this is pure virtual */
        virtual bool equal_to(const TreeNodePtr &other, SMTSolver*) const;

        /* see superclass, where this is pure virtual */
        virtual bool equivalent_to(const TreeNodePtr &other) const;

        /* see superclass, where this is pure virtual */
        virtual void depth_first_visit(Visitor*) const;
    };

    std::ostream& operator<<(std::ostream &o, const InsnSemanticsExpr::TreeNode &node);
};


#endif
