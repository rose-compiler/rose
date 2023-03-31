/** CIL Param node. */
class SgAsmCilParam: public SgAsmCilMetadata {
    /** Property: Flags.
     *
     *  A 2-byte bitmask of type ParamAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Flags;

    /** Property: Sequence.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t Sequence;

    /** Property: Name.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_Name_blob() const;
};
