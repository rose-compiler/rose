#include <sageContainer.h>

/** An ordered list of registers. */
class SgAsmRegisterNames: public SgAsmExpression {
public:
    /** Property: List of registers.
     *
     *  These are the actual registers contained in this node.  Registers are stored in this node instead of directly in a
     *  @ref SgAsmRegisterNames node because of limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmRegisterReferenceExpressionPtrList registers;

    /** Property: Bit mask.
     *
     *  This is an optional bit mask representing the registers. It's used by the m68k disassembler and represents the mask
     *  that appeared in the encoded instruction. */
    [[using Rosebud: rosetta]]
    unsigned mask = 0;
};
