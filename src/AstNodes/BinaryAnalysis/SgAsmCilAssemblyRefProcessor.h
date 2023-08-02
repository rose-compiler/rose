/** CIL AssemblyRefProcessor node (II.22.7). */
class SgAsmCilAssemblyRefProcessor: public SgAsmCilMetadata {

    /** Property: Processor.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t Processor = {};

    /** Property: AssemblyRef.
     *
     *  An index into the AssemblyRef table. */
    [[using Rosebud: rosetta]]
    uint32_t AssemblyRef = {};

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_AssemblyRef_object() const;
};
