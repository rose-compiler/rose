/** CIL InterfaceImpl node (II.22.23). */
class SgAsmCilInterfaceImpl: public SgAsmCilMetadata {

    /** Property: Class.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Class = 0;

    /** Property: Interface.
     *
     *  An index into the TypeDef, TypeRef, or TypeSpec table. */
    [[using Rosebud: rosetta]]
    uint32_t Interface = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Class_object() const;
    const SgAsmCilMetadata* get_Interface_object() const;
};
