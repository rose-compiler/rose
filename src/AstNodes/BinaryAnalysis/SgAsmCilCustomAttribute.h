/** CIL CustomAttribute node (II.22.10). */
class SgAsmCilCustomAttribute: public SgAsmCilMetadata {

    /** Property: Parent.
     *
     *  An index into a metadata table that has an associated HasCustomAttribute index. */
    [[using Rosebud: rosetta]]
    uint32_t Parent = 0;

    /** Property: Type.
     *
     *  An index into the MethodDef or MemberRef table (more precisely, a CustomAttributeType coded index). */
    [[using Rosebud: rosetta]]
    uint32_t Type = 0;

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
    const SgAsmCilMetadata* get_Type_object() const;
    const std::uint8_t* get_Value_blob() const;
};
