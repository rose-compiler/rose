/** Expression that represents an update to a storage location.
 *
 *  The left-hand-side must be a register or memory location, and the right-hand-side is an expression that will be
 *  computed and stored at that location. Pre-updates happen before an instruction is evaluated. */
class SgAsmBinaryPreupdate: public SgAsmBinaryExpression {};
