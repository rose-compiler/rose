/** CIL TypeDef node (II.22.37). */
class SgAsmCilTypeDef: public SgAsmCilMetadata {

    /** Property: Flags.
     *
     *  A 4-byte bitmask of type TypeAttributes. */
    [[using Rosebud: rosetta]]
    uint32_t Flags = 0;

    /** Property: TypeName.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeName = 0;

    /** Property: TypeNamespace.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeNamespace = 0;

    /** Property: Extends.
     *
     *  An index into the TypeDef, TypeRef, or TypeSpec table. */
    [[using Rosebud: rosetta]]
    uint32_t Extends = 0;

    /** Property: FieldList.
     *
     *  An index into the Field table. */
    [[using Rosebud: rosetta]]
    uint32_t FieldList = 0;

    /** Property: MethodList.
     *
     *  An index into the MethodDef table. */
    [[using Rosebud: rosetta]]
    uint32_t MethodList = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_TypeName_string() const;
    const std::uint8_t* get_TypeNamespace_string() const;
    const SgAsmCilMetadata* get_Extends_object() const;
    const SgAsmCilMetadata* get_FieldList_object() const;
    const SgAsmCilMetadata* get_MethodList_object() const;
};
