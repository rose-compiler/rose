/** CIL MemberRef node (II.22.25). */
class SgAsmCilMemberRef: public SgAsmCilMetadata {

    /** Property: Class.
     *
     *  An index into the MethodDef, ModuleRef, TypeDef, TypeRef, or TypeSpec tables. */
    [[using Rosebud: rosetta]]
    uint32_t Class = 0;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

    /** Property: Signature.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Signature = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Class_object() const;
    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Signature_blob() const;
};
