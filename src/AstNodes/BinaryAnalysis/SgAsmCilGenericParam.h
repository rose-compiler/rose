/** CIL GenericParam node (II.22.20). */
class SgAsmCilGenericParam: public SgAsmCilMetadata {

    /** Property: Number.
     *
     *  The 2-byte index of the generic parameter. */
    [[using Rosebud: rosetta]]
    uint16_t Number = 0;

    /** Property: Flags.
     *
     *  A 2-byte bitmask of type GenericParamAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Flags = 0;

    /** Property: Owner.
     *
     *  An index into the TypeDef or MethodDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Owner = 0;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Owner_object() const;
    const std::uint8_t* get_Name_string() const;
};
