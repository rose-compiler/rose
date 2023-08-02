/** CIL AssemblyRefOS node (II.22.6). */
class SgAsmCilAssemblyRefOS: public SgAsmCilMetadata {

    /** Property: OSPlatformID.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t OSPlatformID = {};

    /** Property: OSMajorVersion.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t OSMajorVersion = {};

    /** Property: OSMinorVersion.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t OSMinorVersion = {};

    /** Property: AssemblyRefOS.
     *
     *  An index into the AssemblyRef table. */
    [[using Rosebud: rosetta]]
    uint32_t AssemblyRefOS = {};

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_AssemblyRefOS_object() const;
};
