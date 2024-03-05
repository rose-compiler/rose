/** CIL AssemblyOS node (II.22.3). */
class SgAsmCilAssemblyOS: public SgAsmCilMetadata {

    /** Property: OSPlatformID.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t OSPlatformID = 0;

    /** Property: OSMajorVersion.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t OSMajorVersion = 0;

    /** Property: OSMinorVersion.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t OSMinorVersion = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

};
