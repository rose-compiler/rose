/** CIL GenericParamConstraint node (II.22.21). */
class SgAsmCilGenericParamConstraint: public SgAsmCilMetadata {

    /** Property: Owner.
     *
     *  An index into the GenericParam table. */
    [[using Rosebud: rosetta]]
    uint32_t Owner = 0;

    /** Property: Constraint.
     *
     *  An index into the TypeDef, TypeRef, or TypeSpec table. */
    [[using Rosebud: rosetta]]
    uint32_t Constraint = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Owner_object() const;
    const SgAsmCilMetadata* get_Constraint_object() const;
};
