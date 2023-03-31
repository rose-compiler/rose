/** CIL InterfaceImpl node. */
class SgAsmCilInterfaceImpl: public SgAsmCilMetadata {
    /** Property: Class.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Class;

    /** Property: Interface.
     *
     *  An index into the TypeDef, TypeRef, or TypeSpec table. */
    [[using Rosebud: rosetta]]
    uint32_t Interface;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_Class_object() const;
    const SgAsmCilMetadata* get_Interface_object() const;
};
