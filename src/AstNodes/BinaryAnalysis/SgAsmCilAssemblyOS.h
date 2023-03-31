/** CIL AssemblyOS node. */
class SgAsmCilAssemblyOS: public SgAsmCilMetadata {
    /** Property: OSPlatformID.
     *
     *  A 4-byte constant */
    [[using Rosebud: rosetta]]
    uint32_t OSPlatformID;

    /** Property: OSMajorVersion.
     *
     *  A 4-byte constant */
    [[using Rosebud: rosetta]]
    uint32_t OSMajorVersion;

    /** Property: OSMinorVersion.
     *
     *  A 4-byte constant */
    [[using Rosebud: rosetta]]
    uint32_t OSMinorVersion;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
};
