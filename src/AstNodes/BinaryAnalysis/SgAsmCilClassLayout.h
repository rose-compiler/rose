/** CIL ClassLayout node (II.22.8). */
class SgAsmCilClassLayout: public SgAsmCilMetadata {

    /** Property: PackingSize.
     *
     *  A 2-byte constant. */
    [[using Rosebud: rosetta]]
    uint16_t PackingSize = 0;

    /** Property: ClassSize.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t ClassSize = 0;

    /** Property: Parent.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Parent = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Parent_object() const;
};
