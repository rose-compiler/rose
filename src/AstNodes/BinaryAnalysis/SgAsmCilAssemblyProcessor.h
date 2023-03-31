/** CIL AssemblyProcessor node. */
class SgAsmCilAssemblyProcessor: public SgAsmCilMetadata {
    /** Property: Processor.
     *
     *  A 4-byte constant */
    [[using Rosebud: rosetta]]
    uint32_t Processor;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
};
