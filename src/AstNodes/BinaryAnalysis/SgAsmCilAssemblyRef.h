/** CIL AssemblyRef node (II.22.5). */
class SgAsmCilAssemblyRef: public SgAsmCilMetadata {

    /** Property: MajorVersion.
     *
     *  Major version number. */
    [[using Rosebud: rosetta]]
    uint16_t MajorVersion = 0;

    /** Property: MinorVersion.
     *
     *  Minor version number. */
    [[using Rosebud: rosetta]]
    uint16_t MinorVersion = 0;

    /** Property: BuildNumber.
     *
     *  Build number. */
    [[using Rosebud: rosetta]]
    uint16_t BuildNumber = 0;

    /** Property: RevisionNumber.
     *
     *  Revision number. */
    [[using Rosebud: rosetta]]
    uint16_t RevisionNumber = 0;

    /** Property: Flags.
     *
     *  Shall have only one bit set, the PublicKey bit. */
    [[using Rosebud: rosetta]]
    uint32_t Flags = 0;

    /** Property: PublicKeyOrToken.
     *
     *  An index into the Blob heap, can be null, or non-null. */
    [[using Rosebud: rosetta]]
    uint32_t PublicKeyOrToken = 0;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

    /** Property: Culture.
     *
     *  An index into the String heap, null or non-null. */
    [[using Rosebud: rosetta]]
    uint32_t Culture = 0;

    /** Property: HashValue.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t HashValue = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_PublicKeyOrToken_blob() const;
    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Culture_string() const;
    const std::uint8_t* get_HashValue_blob() const;
};
