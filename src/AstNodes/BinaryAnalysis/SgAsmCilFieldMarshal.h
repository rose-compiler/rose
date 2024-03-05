/** CIL FieldMarshal node (II.22.17). */
class SgAsmCilFieldMarshal: public SgAsmCilMetadata {

    /** Property: Parent.
     *
     *  An index into Field or Param table. */
    [[using Rosebud: rosetta]]
    uint32_t Parent = 0;

    /** Property: NativeType.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t NativeType = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Parent_object() const;
    const std::uint8_t* get_NativeType_blob() const;
};
