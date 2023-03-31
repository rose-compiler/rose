/** CIL PropertyMap node. */
class SgAsmCilPropertyMap: public SgAsmCilMetadata {
    /** Property: Parent.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Parent;

    /** Property: PropertyList.
     *
     *  An index into the Property table. */
    [[using Rosebud: rosetta]]
    uint32_t PropertyList;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_Parent_object() const;
    const SgAsmCilMetadata* get_PropertyList_object() const;
};
