/** CIL MethodImpl node (II.22.27). */
class SgAsmCilMethodImpl: public SgAsmCilMetadata {

    /** Property: Class.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Class = {};

    /** Property: MethodBody.
     *
     *  An index into the MethodDef or MemberRef table. */
    [[using Rosebud: rosetta]]
    uint32_t MethodBody = {};

    /** Property: MethodDeclaration.
     *
     *  An index into the MethodDef or MemberRef table. */
    [[using Rosebud: rosetta]]
    uint32_t MethodDeclaration = {};

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_Class_object() const;
    const SgAsmCilMetadata* get_MethodBody_object() const;
    const SgAsmCilMetadata* get_MethodDeclaration_object() const;
};
