/** CIL FieldLayout node. */
class SgAsmCilFieldLayout: public SgAsmCilMetadata {
    /** Property: Offset.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t Offset;

    /** Property: Field.
     *
     *  An index into the Field table. */
    [[using Rosebud: rosetta]]
    uint32_t Field;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_Field_object() const;
};
