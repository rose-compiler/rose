/** CIL Event node (II.22.13). */
class SgAsmCilEvent: public SgAsmCilMetadata {

    /** Property: EventFlags.
     *
     *  A 2-byte bitmask of type EventAttribute. */
    [[using Rosebud: rosetta]]
    uint16_t EventFlags = 0;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

    /** Property: EventType.
     *
     *  An index into a TypeDef, a TypeRef, or TypeSpec table. */
    [[using Rosebud: rosetta]]
    uint32_t EventType = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_Name_string() const;
    const SgAsmCilMetadata* get_EventType_object() const;
};
