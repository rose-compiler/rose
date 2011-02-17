#ifndef Rose_InsnSemanticsExpr_H
#define Rose_InsnSemanticsExpr_H

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
        OP_ROL,                 /**< Rotate left. Rotate bits of B left by A bits.  0 <= A < width(B). */
        OP_ROR,                 /**< Rotate right. Rotate bits of B right by A bits. 0 <= B < width(B).  */
        OP_SDIV,                /**< Signed division. Two operands, A/B. Result width is width(A). */
        OP_SEXTEND,             /**< Signed extension at msb. Extend B to A bits by replicating B's most significant bit. */
        OP_SGE,                 /**< Signed greater-than-or-equal. Two operands of equal width. Result is Boolean. */
        OP_SGT,                 /**< Signed greater-than. Two operands of equal width. Result is Boolean. */
        OP_SHL0,                /**< Shift left, introducing zeros at lsb. Bits of B are shifted by A, where 0 <= A < width(B). */
        OP_SHL1,                /**< Shift left, introducing ones at lsb. Bits of B are shifted by A, where 0 <= A < width(B). */
        OP_SHR0,                /**< Shift right, introducing zeros at msb. Bits of B are shifted by A, where 0 <= A < width(B). */
        OP_SHR1,                /**< Shift right, introducing ones at msb. Bits of B are shifted by A, where 0 <= A < width(B). */
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
        OP_ZEROP,               /**< Equal to zero. One operand. Result is a single bit, set iff A is equal to zero. */
    };

    const char *to_str(Operator o);

    typedef std::map<uint64_t, uint64_t> RenameMap;

    /** Any node of an expression tree for instruction semantics, from which the InternalNode and LeafNode classes are
     *  derived.  Every node has a specified number of significant bits that is constant over the life of the node.
     *
     *  In order that subtrees can be freely assigned as children of other nodes (provided the structure as a whole remains a
     *  lattice and not a graph with cycles), each tree node maintains a counter (nrefs) of the number of times it is
     *  referenced.  Each time a subtree is added as a child to another node, the reference count for the root node of the
     *  subtree is incremented. Likewise, when a child is removed the reference count for the root of the child is
     *  decremented. The delete operator must only be invoked on nodes with a zero reference count. */
    class TreeNode {
    protected:
        size_t nbits;           /**< Number of significant bits. Constant over the life of the node. */
        mutable size_t nrefs;   /**< Number of parent nodes. Zero implies this node can be deleted. */
    public:
        TreeNode(size_t nbits): nbits(nbits), nrefs(0) { assert(nbits>0); }

        /** Shallow delete. Deletes only this node, decrementing reference counts in any children. It is an error to delete a
         *  node whose reference count is nonzero. */
        virtual ~TreeNode() { assert(0==nrefs); }

        /** Deletes this node and all children, provided the children aren't also children of other nodes.  If the reference
         *  count of the root node (@p this) is nonzero then nothing happens. */
        virtual void deleteDeeply();

        /** Print the expression to a stream.  The output is an S-expression with no line-feeds.  If @p rmap is non-null then
         *  it will be used to rename free variables for readability.  If the expression contains N variables, then the new
         *  names will be numbered from the set M = (i | 0 <= i <= N-1). */
        virtual void print(std::ostream&, RenameMap *rmap=NULL) const = 0;

        /** Tests two expressions for equality using an optional satisfiability modulo theory (SMT) solver. Returns true if
         *  the inequality is not satisfiable. */
        virtual bool equal_to(const TreeNode *other, SMTSolver*) const = 0;

        /** Returns true if the expression is a known value.
         *
         *  FIXME: The current implementation returns true only when @p this node is leaf node with a known value. Since
         *         InsnSemanticsExpr does not do constant folding, this is of limited use. [RPM 2010-06-08]. */
        virtual bool is_known() const = 0;

        /** Returns the integer value of a node for which is_known() returns true.  The high-order bits, those beyond the
         *  number of significant bits returned by get_nbits(), are guaranteed to be zero. */
        virtual uint64_t get_value() const = 0;

        /** Returns the number of significant bits.  An expression with a known value is guaranteed to have all higher-order
         *  bits cleared. */
        size_t get_nbits() const { return nbits; }

        /** Returns the number of times this node is referenced.  Reference counting happens automatically when a node is used
         *  as a child of an internal node, but users may also forcibly increment and decrement the reference counter for
         *  other reasons. See also, documentation for the class. */
        size_t get_nrefs() const { return nrefs; }

        /** Increments the reference count by one and returns the new reference count. It is not necessary to increment the
         *  reference count when a node is used as a child of an InternalNode since this happens automatically.  However, the
         *  user may want to increment the reference count for other reasons. See also, documentation for the class. */
        size_t inc_nrefs() const { return ++nrefs; }

        /** Decrements the reference count by one and returns the new reference count. It is not necessary to decrement the
         *  reference count when a node is removed as a child of an InternalNode since this happens automatically. However,
         *  the user may want to decrement the reference count for other reasons.  The dec_nrefs() method should be called the
         *  same number of times that inc_nrefs() was called. See also, documentation for the class. */
        size_t dec_nrefs() const { ROSE_ASSERT(nrefs>0); return --nrefs; }
    };

    /** Internal node of an expression tree for instruction semantics. Each internal node has an operator (constant for the
     *  life of the node and obtainable with get_operator()) and zero or more children. Children are added to the internal
     *  node during the construction phase. Once construction is complete, the children should only change in ways that don't
     *  affect the value of the node as a whole (since this node might be pointed to by any number of expressions). It is safe
     *  to deleteDeeply() a node that participates in more than one expression since that method is a no-op when the node's
     *  reference count is nonzero. */
    class InternalNode: public TreeNode {
    private:
        Operator op;
        std::vector<const TreeNode*> children;
    public:
        /** Constructs an internal node with no children. It is expected that the appropriate number and bit sized children
         *  will be added before the node is used. The number and sizes depend on the operator chosen. */
        InternalNode(size_t nbits, Operator op)
            : TreeNode(nbits), op(op) {}
        InternalNode(size_t nbits, Operator op, const TreeNode *a)
            : TreeNode(nbits), op(op) {
            add_child(a);
        }
        InternalNode(size_t nbits, Operator op, const TreeNode *a, const TreeNode *b)
            : TreeNode(nbits), op(op) {
            add_child(a);
            add_child(b);
        }
        InternalNode(size_t nbits, Operator op, const TreeNode *a, const TreeNode *b, const TreeNode *c)
            : TreeNode(nbits), op(op) {
            add_child(a);
            add_child(b);
            add_child(c);
        }

        /** Shallow delete. Deletes only this node, unlinking all its children (decrementing their reference counts) but not
         *  deleting them. This node's reference count must be zero. */
        virtual ~InternalNode();

        /** Deep delete. If the reference count of this node is zero, then this node's children are unlinked (their reference
         *  counts deleted) and this node is deleted. The deleteDeeply() method is recursively invoked for any former children
         *  that have zero for their new reference counts. */
        virtual void deleteDeeply();

        /* see superclass, where this is pure virtual */
        virtual void print(std::ostream &o, RenameMap *rmap=NULL) const;

        /* see superclass, where this is pure virtual */
        virtual bool equal_to(const TreeNode *other, SMTSolver*) const;

        /* see superclass, where this is pure virtual */
        virtual bool is_known() const {
            return false; /*if it's known, then it would have been folded to a leaf*/
        }

        /* see superclass, where this is pure virtual */
        virtual uint64_t get_value() const { ROSE_ASSERT(!"not a constant value"); return 0;}

        /** Returns the number of children. */
        size_t size() const { return children.size(); }

        /** Returns the specified child. */
        const TreeNode *child(size_t idx) const { return children[idx]; }

        /** Returns the operator. */
        Operator get_operator() const { return op; }

        /** Appends @p child as a new child of this node. The reference count for the root of the child is incremented; the
         *  child expression is not copied. */
        void add_child(const TreeNode* child);
    };

    /** Leaf node of an expression tree for instruction semantics. A leaf node is either a known value or a free variable. */
    class LeafNode: public TreeNode {
    private:
        bool known;                         /**< True if the value is known; false for variables. */
        union {
            uint64_t ival;                  /**< Integer (unsigned) value when 'known' is true; unused msb are zero */
            uint64_t name;                  /**< Variable ID number when 'known' is false. */
        };
    public:
        LeafNode(): TreeNode(32), known(true), ival(0) {}

        /** Construct a new free variable with a specified number of significant bits. */
        static LeafNode *create_variable(size_t nbits);

        /** Construct a new integer with the specified number of significant bits. Any high-order bits beyond the specified
         *  size will be zeroed. */
        static LeafNode *create_integer(size_t nbits, uint64_t n);

        /* see superclass, where this is pure virtual */
        virtual bool is_known() const;

        /* see superclass, where this is pure virtual */
        virtual uint64_t get_value() const;

        /** Returns the name of a free variable.  The output functions print variables as "vN" where N is an integer. It is
         *  this N that this method returns.  It should only be invoked on leaf nodes for which is_known() returns false. */
        uint64_t get_name() const;

        /* see superclass, where this is pure virtual */
        virtual void print(std::ostream &o, RenameMap *rmap=NULL) const;

        /* see superclass, where this is pure virtual */
        virtual bool equal_to(const TreeNode *other, SMTSolver*) const;
    };

};

#endif
