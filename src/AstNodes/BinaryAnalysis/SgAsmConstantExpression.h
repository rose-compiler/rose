#include <Sawyer/BitVector.h>

/** Base class for constants.
 *
 *  Represents integer values, floating-point values, etc. This class holds the actual bits for the constant value.
 *  Subclasses provide the intepretation of those bits. */
[[Rosebud::abstract]]
class SgAsmConstantExpression: public SgAsmValueExpression {
public:
    /** Property: Bits for constant.
     *
     *  This is the canonical value of an AST node; subclasses may cache native representations of this value. */
    [[using Rosebud: rosetta, large]]
    Sawyer::Container::BitVector bitVector;
};
