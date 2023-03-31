/** CIL Assembly node. */
class SgAsmCilAssembly: public SgAsmCilMetadata {
    /** Property: HashAlgId.
     *
     *  A 4-byte constant of type AssemblyHashAlgorithm */
    [[using Rosebud: rosetta]]
    uint32_t HashAlgId;

    /** Property: MajorVersion.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t MajorVersion;

    /** Property: MinorVersion.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t MinorVersion;

    /** Property: BuildNumber.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t BuildNumber;

    /** Property: RevisionNumber.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t RevisionNumber;

    /** Property: Flags.
     *
     *  A 4-byte bitmask of type AssemblyFlags. */
    [[using Rosebud: rosetta]]
    uint32_t Flags;

    /** Property: PublicKey.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t PublicKey;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name;

    /** Property: Culture.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Culture;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_PublicKey_blob() const;
    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Culture_string() const;
};
