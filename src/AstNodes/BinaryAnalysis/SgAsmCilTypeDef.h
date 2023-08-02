/** CIL TypeDef node (II.22.37). */
class SgAsmCilTypeDef: public SgAsmCilMetadata {

    /** Property: Flags.
     *
     *  A 4-byte bitmask of type TypeAttributes. */
    [[using Rosebud: rosetta]]
    uint32_t Flags = {};

    /** Property: TypeName.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeName = {};

    /** Property: TypeNamespace.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeNamespace = {};

    /** Property: Extends.
     *
     *  An index into the TypeDef, TypeRef, or TypeSpec table. */
    [[using Rosebud: rosetta]]
    uint32_t Extends = {};

    /** Property: FieldList.
     *
     *  An index into the Field table. */
    [[using Rosebud: rosetta]]
    uint32_t FieldList = {};

    /** Property: MethodList.
     *
     *  An index into the MethodDef table. */
    [[using Rosebud: rosetta]]
    uint32_t MethodList = {};

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_TypeName_string() const;
    const std::uint8_t* get_TypeNamespace_string() const;
    const SgAsmCilMetadata* get_Extends_object() const;
    const SgAsmCilMetadata* get_FieldList_object() const;
    const SgAsmCilMetadata* get_MethodList_object() const;
};
