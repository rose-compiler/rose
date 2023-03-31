/** Expression representing unsigned extending.
 *
 *  The size of the result is based on the sizes of the types for the operand and the result. There is no second argument
 *  that says how large the result should be since this would be redundant and possibly inconsistent with the type for the
 *  resulting expression. */
class SgAsmUnaryUnsignedExtend: public SgAsmUnaryExpression {};
