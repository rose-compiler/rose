/** CIL AssemblyRef node (II.22.5). */
class SgAsmCilAssemblyRef: public SgAsmCilMetadata {

    /** Property: MajorVersion.
     *
     *  Major version number. */
    [[using Rosebud: rosetta]]
    uint16_t MajorVersion = {};

    /** Property: MinorVersion.
     *
     *  Minor version number. */
    [[using Rosebud: rosetta]]
    uint16_t MinorVersion = {};

    /** Property: BuildNumber.
     *
     *  Build number. */
    [[using Rosebud: rosetta]]
    uint16_t BuildNumber = {};

    /** Property: RevisionNumber.
     *
     *  Revision number. */
    [[using Rosebud: rosetta]]
    uint16_t RevisionNumber = {};

    /** Property: Flags.
     *
     *  Shall have only one bit set, the PublicKey bit. */
    [[using Rosebud: rosetta]]
    uint32_t Flags = {};

    /** Property: PublicKeyOrToken.
     *
     *  An index into the Blob heap, can be null, or non-null. */
    [[using Rosebud: rosetta]]
    uint32_t PublicKeyOrToken = {};

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = {};

    /** Property: Culture.
     *
     *  An index into the String heap, null or non-null. */
    [[using Rosebud: rosetta]]
    uint32_t Culture = {};

    /** Property: HashValue.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t HashValue = {};

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_PublicKeyOrToken_blob() const;
    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Culture_string() const;
    const std::uint8_t* get_HashValue_blob() const;
};
