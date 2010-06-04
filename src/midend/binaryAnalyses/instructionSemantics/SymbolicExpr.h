#ifndef Rose_SymbolicExpr_H
#define Rose_SymbolicExpr_H

/** Namespace supplying types and functions for symbolic expressions. These are used by certain instruction semantics policies
 *  and satisfiability modulo theory (SMT) solvers. */
namespace SymbolicExpr {

    /** Operators for internal nodes of the expression tree. Commutative operators generally take one or more operands.
     *  Operators such as shifting, extending, and truncating have the size operand appearing before the bit vector on which
     *  to operate (this makes the output more human-readable since the size operand is often a constant). */
    enum Operator {
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
        OP_SHL0,                /**< Shift left, introducing zeros at lsb. Bits of B are shifted by A, where 0 <= A < width(B). */
        OP_SHL1,                /**< Shift left, introducing ones at lsb. Bits of B are shifted by A, where 0 <= A < width(B). */
        OP_SHR0,                /**< Shift right, introducing zeros at msb. Bits of B are shifted by A, where 0 <= A < width(B). */
        OP_SHR1,                /**< Shift right, introducing ones at msb. Bits of B are shifted by A, where 0 <= A < width(B). */
        OP_SMOD,                /**< Signed modulus. Two operands, A%B. Result width is width(B). */
        OP_SMUL,                /**< Signed multiplication. Two operands A*B. Result width is width(A)+width(B). */
        OP_UDIV,                /**< Signed division. Two operands, A/B. Result width is width(A). */
        OP_UEXTEND,             /**< Unsigned extention at msb. Extend B to A bits by introducing zeros at the msb of B. */
        OP_UMOD,                /**< Unsigned modulus. Two operands, A%B. Result width is width(B). */
        OP_UMUL,                /**< Unsigned multiplication. Two operands, A*B. Result width is width(A)+width(B). */
        OP_ZEROP,               /**< Equal to zero. One operand. Result is a single bit, set iff A is equal to zero. */
    };

    const char *to_str(Operator o);

    typedef std::map<uint64_t, uint64_t> RenameMap;

    class TreeNode {
    protected:
        size_t nbits;                       /**< Number of significant bits. */
    public:
        TreeNode(size_t nbits): nbits(nbits) { assert(nbits>0); }
        virtual ~TreeNode() {}
        virtual void print(std::ostream&, RenameMap *rmap=NULL) const = 0;
        virtual bool equal_to(const TreeNode *other) const { return other && nbits==other->nbits; }
        virtual bool is_known() const = 0;
        virtual uint64_t get_value() const = 0;
        size_t get_nbits() const { return nbits; }
    };

    class InternalNode: public TreeNode {
    private:
        Operator op;
        std::vector<TreeNode*> children;
    public:
        InternalNode(size_t nbits, Operator op)
            : TreeNode(nbits), op(op) {}
        InternalNode(size_t nbits, Operator op, TreeNode *a)
            : TreeNode(nbits), op(op) {
            assert(a!=NULL);
            children.push_back(a);
        }
        InternalNode(size_t nbits, Operator op, TreeNode *a, TreeNode *b)
            : TreeNode(nbits), op(op) {
            assert(a!=NULL && b!=NULL);
            children.push_back(a);
            children.push_back(b);
        }
        InternalNode(size_t nbits, Operator op, TreeNode *a, TreeNode *b, TreeNode *c)
            : TreeNode(nbits), op(op) {
            assert(a!=NULL && b!=NULL && c!=NULL);
            children.push_back(a);
            children.push_back(b);
            children.push_back(c);
        }
        virtual void print(std::ostream &o, RenameMap *rmap=NULL) const;
        virtual bool equal_to(const TreeNode *other_) const;
        virtual bool is_known() const {
            return false; /*if it's known, then it would have been folded to a leaf*/
        }
        virtual uint64_t get_value() const { ROSE_ASSERT(!"not a constant value"); }
        size_t size() const { return children.size(); }
        TreeNode *child(size_t idx) { return children[idx]; }
        const TreeNode *child(size_t idx) const { return children[idx]; }
        Operator get_operator() const { return op; }
        void add_child(TreeNode* child) { children.push_back(child); }
    };

    class LeafNode: public TreeNode {
    private:
        bool known;                         /**< True if the value is known; false for variables. */
        union {
            uint64_t ival;                  /**< Integer (unsigned) value when 'known' is true; unused msb are zero */
            uint64_t name;                  /**< Variable ID number when 'known' is false. */
        };
    public:
        LeafNode(): TreeNode(32), known(true), ival(0) {}
        static LeafNode *create_variable(size_t nbits);
        static LeafNode *create_integer(size_t nbits, uint64_t n);
        virtual bool is_known() const;
        virtual uint64_t get_value() const;
        uint64_t get_name() const;
        virtual void print(std::ostream &o, RenameMap *rmap=NULL) const;
        virtual bool equal_to(const TreeNode *other_) const;
    };
    
};

#endif
