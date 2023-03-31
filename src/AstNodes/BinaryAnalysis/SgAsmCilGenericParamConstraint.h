/** CIL GenericParamConstraint node. */
class SgAsmCilGenericParamConstraint: public SgAsmCilMetadata {
    /** Property: Owner.
     *
     *  An index into the GenericParam table. */
    [[using Rosebud: rosetta]]
    uint32_t Owner;

    /** Property: Constraint.
     *
     *  An index into the TypeDef, TypeRef, or TypeSpec table. */
    [[using Rosebud: rosetta]]
    uint32_t Constraint;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_Owner_object() const;
    const SgAsmCilMetadata* get_Constraint_object() const;
};
