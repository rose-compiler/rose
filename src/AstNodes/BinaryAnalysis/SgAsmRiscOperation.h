#ifdef ROSE_IMPL
#include <SgAsmExprListExp.h>
#endif

/** Static representation of instruction semantics.
 *
 *  Instruction semantics are not added to the AST by default since this would make it very, very large.  Instead, ROSE has
 *  a non-traditional approach: instead of ROSE having C++ code to generate a data-centric representation of semantics (a
 *  tree like data structure similar to a syntax tree) and then users writing analyses on that data structure, ROSE's C++
 *  code can be hooked into directly by users via C++ class derivation. If a user really wants a data-centric view they can
 *  either have ROSE create @ref SgAsmRiscOperation nodes in the AST, or they can hook into ROSE's instruction semantics
 *  API and build whatever kind of data-centric representation that suites their need. */
class SgAsmRiscOperation: public SgAsmExpression {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** One enum per RISC operator.
     *
     * The names are the same as the RISC operator.  Note that some of these operators aren't normally part of the base
     * set. For instance, "subtract" and comparison operations are usually implemented in terms of more basic operations
     * like add_, invert, and equalToZero. */
    enum RiscOperator {
        OP_NONE,
        OP_bottom,
        OP_undefined,
        OP_unspecified,
        OP_filterCallTarget,
        OP_filterReturnTarget,
        OP_filterIndirectJumpTarget,
        OP_hlt,
        OP_cpuid,
        OP_rdtsc,
        OP_and_,
        OP_or_,
        OP_xor_,
        OP_invert,
        OP_extract,
        OP_concat,
        OP_leastSignificantSetBit,
        OP_mostSignificantSetBit,
        OP_rotateLeft,
        OP_rotateRight,
        OP_shiftLeft,
        OP_shiftRight,
        OP_shiftRightArithmetic,
        OP_equalToZero,
        OP_ite,
        OP_isEqual,
        OP_isNotEqual,
        OP_isUnsignedLessThan,
        OP_isUnsignedLessThanOrEqual,
        OP_isUnsignedGreaterThan,
        OP_isUnsignedGreaterThanOrEqual,
        OP_isSignedLessThan,
        OP_isSignedLessThanOrEqual,
        OP_isSignedGreaterThan,
        OP_isSignedGreaterThanOrEqual,
        OP_unsignedExtend,
        OP_signExtend,
        OP_add,                                     /**< Two args + optional carry bit. */
        OP_addCarries,                              /**< Carries from a 3-arg add operation. */
        OP_subtract,
        OP_negate,
        OP_signedDivide,
        OP_signedModulo,
        OP_signedMultiply,
        OP_unsignedDivide,
        OP_unsignedModulo,
        OP_unsignedMultiply,
        OP_interrupt,
        OP_readRegister,
        OP_peekRegister,
        OP_writeRegister,
        OP_readMemory,                              /**< Three or four args depending on whether segment reg is present. */
        OP_peekMemory,
        OP_writeMemory,                             /**< Three or four args depending on whether segment reg is present. */
        OP_N_OPERATORS                              /**< Number of operators in this enum. */ // MUST BE LAST!
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    // Note that the qualification for the type is necessary only for ROSETTA. Remove it when ROSETTA finally goes away.
    /** Property: Low-level semantic operation.
     *
     *  This property is an enum constant that represents an operation in @ref
     *  Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RiscOperators "RiscOperators". */
    [[using Rosebud: rosetta, ctor_arg]]
    SgAsmRiscOperation::RiscOperator riscOperator = OP_NONE;

    /** Property: Operands for the low-level operation.
     *
     *  These are the operands used by the low-level operation. The actual list is stored in a separate @ref SgAsmExprListExp AST
     *  node instead of directly in this node due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmExprListExp* operands = nullptr;
};
