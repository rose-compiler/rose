/** CIL MethodImpl node (II.22.27). */
class SgAsmCilMethodImpl: public SgAsmCilMetadata {

    /** Property: Class.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Class = 0;

    /** Property: MethodBody.
     *
     *  An index into the MethodDef or MemberRef table. */
    [[using Rosebud: rosetta]]
    uint32_t MethodBody = 0;

    /** Property: MethodDeclaration.
     *
     *  An index into the MethodDef or MemberRef table. */
    [[using Rosebud: rosetta]]
    uint32_t MethodDeclaration = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Class_object() const;
    const SgAsmCilMetadata* get_MethodBody_object() const;
    const SgAsmCilMetadata* get_MethodDeclaration_object() const;
};
