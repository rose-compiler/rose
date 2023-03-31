/** CIL ManifestResource node. */
class SgAsmCilManifestResource: public SgAsmCilMetadata {
    /** Property: Offset.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t Offset;

    /** Property: Flags.
     *
     *  A 4-byte constant of type ManifestResourceAttributes. */
    [[using Rosebud: rosetta]]
    uint32_t Flags;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name;

    /** Property: Implementation.
     *
     *  An index into a File table, a AssemblyRef table, or null. */
    [[using Rosebud: rosetta]]
    uint32_t Implementation;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_Name_string() const;
    const SgAsmCilMetadata* get_Implementation_object() const;
};
