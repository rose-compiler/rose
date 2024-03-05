/** CIL ImplMap node (II.22.22). */
class SgAsmCilImplMap: public SgAsmCilMetadata {

    /** Property: MappingFlags.
     *
     *  A 2-byte bitmask of type PInvokeAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t MappingFlags = 0;

    /** Property: MemberForwarded.
     *
     *  An index into the Field or MethodDef table. */
    [[using Rosebud: rosetta]]
    uint32_t MemberForwarded = 0;

    /** Property: ImportName.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t ImportName = 0;

    /** Property: ImportScope.
     *
     *  An index into the ModuleRef table. */
    [[using Rosebud: rosetta]]
    uint32_t ImportScope = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_MemberForwarded_object() const;
    const std::uint8_t* get_ImportName_string() const;
    const SgAsmCilMetadata* get_ImportScope_object() const;
};
