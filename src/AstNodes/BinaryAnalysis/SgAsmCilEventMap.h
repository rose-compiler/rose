/** CIL EventMap node (II.22.12). */
class SgAsmCilEventMap: public SgAsmCilMetadata {

    /** Property: Parent.
     *
     *  An index into the TypeDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Parent = 0;

    /** Property: EventList.
     *
     *  An index into the Event table. */
    [[using Rosebud: rosetta]]
    uint32_t EventList = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Parent_object() const;
    const SgAsmCilMetadata* get_EventList_object() const;
};
