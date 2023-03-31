/** CIL ClassLayout node. */
class SgAsmCilClassLayout: public SgAsmCilMetadata {
    /** Property: PackingSize.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t PackingSize;

    /** Property: ClassSize.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t ClassSize;

    /** Property: Parent.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Parent;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_Parent_object() const;
};
