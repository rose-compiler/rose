/** Instructions defined at runtime.
 *
 *  This class describes instructions whose form is not known until runtime. Most architectures have their own instruction class
 *  derived from @ref SgAsmInstruction, but for architectures intended to be defined outside the ROSE library, we need some way
 *  to represent their decoded machine instructions. This class serves that purpose. */
class SgAsmUserInstruction: public SgAsmInstruction {
public:
    /** Property: Instruction kind.
     *
     *  The instruction kind distinguishes between different kinds of instructions within a single architecture. */
    [[using Rosebud: rosetta, ctor_arg]]
    unsigned kind = 0;

public:
    // Overrides are documented in the base class
    virtual unsigned get_anyKind() const override;
};
