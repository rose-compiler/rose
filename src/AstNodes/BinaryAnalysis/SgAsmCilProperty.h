/** CIL Property node (II.22.34). */
class SgAsmCilProperty: public SgAsmCilMetadata {

    /** Property: Flags.
     *
     *  A 2-byte bitmask of type PropertyAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Flags = {};

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = {};

    /** Property: Type.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Type = {};

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Type_blob() const;
};
