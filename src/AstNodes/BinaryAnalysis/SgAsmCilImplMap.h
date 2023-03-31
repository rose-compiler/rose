/** CIL ImplMap node. */
class SgAsmCilImplMap: public SgAsmCilMetadata {
    /** Property: MappingFlags.
     *
     *  A 2-byte bitmask of type PInvokeAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t MappingFlags;

    /** Property: MemberForwarded.
     *
     *  An index into the Field or MethodDef table. */
    [[using Rosebud: rosetta]]
    uint32_t MemberForwarded;

    /** Property: ImportName.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t ImportName;

    /** Property: ImportScope.
     *
     *  an index into the ModuleRef table. */
    [[using Rosebud: rosetta]]
    uint32_t ImportScope;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_MemberForwarded_object() const;
    const std::uint8_t* get_ImportName_string() const;
    const SgAsmCilMetadata* get_ImportScope_object() const;
};
