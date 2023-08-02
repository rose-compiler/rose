/** CIL GenericParam node (II.22.20). */
class SgAsmCilGenericParam: public SgAsmCilMetadata {

    /** Property: Number.
     *
     *  The 2-byte index of the generic parameter. */
    [[using Rosebud: rosetta]]
    uint16_t Number = {};

    /** Property: Flags.
     *
     *  A 2-byte bitmask of type GenericParamAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Flags = {};

    /** Property: Owner.
     *
     *  An index into the TypeDef or MethodDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Owner = {};

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = {};

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_Owner_object() const;
    const std::uint8_t* get_Name_string() const;
};
