/** CIL Param node (II.22.33). */
class SgAsmCilParam: public SgAsmCilMetadata {
public:
    /** Property: Flags.
     *
     *  A 2-byte bitmask of type ParamAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Flags = 0;

    /** Property: Sequence.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t Sequence = 0;

    /** Property: Name.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_Name_blob() const;
};
