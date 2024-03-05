/** CIL AssemblyProcessor node (II.22.4). */
class SgAsmCilAssemblyProcessor: public SgAsmCilMetadata {

    /** Property: Processor.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t Processor = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

};
