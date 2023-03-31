/** CIL CustomAttribute node. */
class SgAsmCilCustomAttribute: public SgAsmCilMetadata {
    /** Property: Parent.
     *
     *  An index into a metadata table that has an associated HasCustomAttribute index. */
    [[using Rosebud: rosetta]]
    uint32_t Parent;

    /** Property: Type.
     *
     *  An index into the MethodDef or MemberRef table (more precisely, a CustomAttributeType coded index). */
    [[using Rosebud: rosetta]]
    uint32_t Type;

    /** Property: Value.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Value;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_Parent_object() const;
    const SgAsmCilMetadata* get_Type_object() const;
    const std::uint8_t* get_Value_blob() const;
};
