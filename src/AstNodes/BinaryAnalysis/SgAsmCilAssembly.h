/** CIL Assembly node (II.22.2). */
class SgAsmCilAssembly: public SgAsmCilMetadata {

    /** Property: HashAlgId.
     *
     *  A 4-byte constant of type AssemblyHashAlgorithm. */
    [[using Rosebud: rosetta]]
    uint32_t HashAlgId = 0;

    /** Property: MajorVersion.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t MajorVersion = 0;

    /** Property: MinorVersion.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t MinorVersion = 0;

    /** Property: BuildNumber.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t BuildNumber = 0;

    /** Property: RevisionNumber.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t RevisionNumber = 0;

    /** Property: Flags.
     *
     *  A 4-byte bitmask of type AssemblyFlags. */
    [[using Rosebud: rosetta]]
    uint32_t Flags = 0;

    /** Property: PublicKey.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t PublicKey = 0;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

    /** Property: Culture.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Culture = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_PublicKey_blob() const;
    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Culture_string() const;
};
