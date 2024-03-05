/** CIL Constant node (II.22.9). */
class SgAsmCilConstant: public SgAsmCilMetadata {

    /** Property: Type.
     *
     *  A 1-byte constant. */
    [[using Rosebud: rosetta]]
    uint8_t Type = 0;

    /** Property: Padding.
     *
     *  A 1-byte zero padding. */
    [[using Rosebud: rosetta]]
    uint8_t Padding = 0;

    /** Property: Parent.
     *
     *  An index into the Param, Field, or Property table. */
    [[using Rosebud: rosetta]]
    uint32_t Parent = 0;

    /** Property: Value.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Value = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Parent_object() const;
    const std::uint8_t* get_Value_blob() const;
};
