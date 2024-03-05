/** CIL FieldLayout node (II.22.16). */
class SgAsmCilFieldLayout: public SgAsmCilMetadata {

    /** Property: Offset.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t Offset = 0;

    /** Property: Field.
     *
     *  An index into the Field table. */
    [[using Rosebud: rosetta]]
    uint32_t Field = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Field_object() const;
};
