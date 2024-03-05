/** CIL PropertyMap node (II.22.35). */
class SgAsmCilPropertyMap: public SgAsmCilMetadata {

    /** Property: Parent.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Parent = 0;

    /** Property: PropertyList.
     *
     *  An index into the Property table. */
    [[using Rosebud: rosetta]]
    uint32_t PropertyList = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Parent_object() const;
    const SgAsmCilMetadata* get_PropertyList_object() const;
};
