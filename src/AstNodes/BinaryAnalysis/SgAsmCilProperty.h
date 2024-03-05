/** CIL Property node (II.22.34). */
class SgAsmCilProperty: public SgAsmCilMetadata {

    /** Property: Flags.
     *
     *  A 2-byte bitmask of type PropertyAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Flags = 0;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

    /** Property: Type.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Type = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Type_blob() const;
};
