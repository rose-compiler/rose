/** CIL Param node (II.22.33). */
class SgAsmCilParam: public SgAsmCilMetadata {

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
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_Name_string() const;
};
