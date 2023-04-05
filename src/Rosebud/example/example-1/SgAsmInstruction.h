#include <generated/SgAsmExpression.h>

/** Base class for instructions. */
[[Rosebud::abstract]]
class SgAsmInstruction: public TreeNode {
    /** Mnemonic. */
    [[Rosebud::ctor_arg]]
    std::string mnemonic;

    /** List of operand expressions. */
    [[using Rosebud: property, mutators()]]
    TreeEdge<SgAsmExpressionList> exprs = SgAsmExpressionList::instance();

public:
    size_t nInstructions() const {
        return exprs()->size();
    }
};
