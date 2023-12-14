/** Registers accessed indirectly.
 *
 *  An indirect register is a register whose descriptor is computed dynamically.  The dynamic descriptor is computed by reading
 *  "offset" register to obtain an integral value and adding it to the integral "index" modulo the specified "modulus" to obtain an
 *  integer @em i. (These are all properties of this node.)  Then, the final register descriptor is calculated by adding @em i times
 *  "stride" to the base register @ref SgAsmRegisterReferenceExpression::descriptor. The @em i times "stride" multiplies each member
 *  of "stride" by @em i, and addition of two register descriptors is defined as the pair-wise addition of their elements.
 *
 *  An example of an indirect register is x86 ST(1) which has base register "st", stride={0,1,0,0}, offset register "fpstatus_top",
 *  index is 1, and modulus is 8. Thus, the dynamically-computed register is:
 *
 *  @code
 *   i = (read("fpstatus_top") + index) % modulo;
 *   result = descriptor("st") + {0,1,0,0} * i;
 *  @endcode */
class SgAsmIndirectRegisterExpression: public SgAsmRegisterReferenceExpression {
public:
   /** Property: Register descriptor stride.
     *
     *  This is the "stride" referred to in the documentation for this class.  This is not an actual register. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::RegisterDescriptor stride;

    /** Property: Register descriptor offset.
     *
     *  This is the "offset" referred to in the documentation for this class. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::RegisterDescriptor offset;

    /** Property: Register descriptor index.
     *
     *  This is the "index" referred to in the documentation for this class. */
    [[using Rosebud: rosetta, ctor_arg]]
    size_t index = 0;

    /** Property: Register descriptor modulus.
     *
     *  This is the "modulus" referred to in the documentation for this class. */
    [[using Rosebud: rosetta, ctor_arg]]
    size_t modulus = 0;
};
