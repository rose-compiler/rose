/** Reference to memory locations. */
class SgAsmMemoryReferenceExpression: public SgAsmExpression {
    /** Property: Memory address expression.
     *
     *  This property stores the address of the memory reference.  If the reference is for multiple bytes of memory, then
     *  only one address is stored and the instruction semantics determine which address it is. Usually multi-byte
     *  references store the lowest address. */
    [[using Rosebud: rosetta, ctor_arg, traverse]]
    SgAsmExpression* address = nullptr;

    /** Property: Optional memory segment register.
     *
     *  If a segment register is specified then the actual memory address is formed by adding the stored memory address to
     *  the current value of the segment register.  The x86 architecture is the only ROSE architecture that uses segment
     *  registers, and most of the time they are initialized to zero by the instruction semantics. */
    [[using Rosebud: rosetta, ctor_arg, traverse]]
    SgAsmExpression* segment = nullptr;
};
