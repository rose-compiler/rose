/** CIL NestedClass node (II.22.32). */
class SgAsmCilNestedClass: public SgAsmCilMetadata {

    /** Property: NestedClass.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t NestedClass = 0;

    /** Property: EnclosingClass.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t EnclosingClass = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_NestedClass_object() const;
    const SgAsmCilMetadata* get_EnclosingClass_object() const;
};
