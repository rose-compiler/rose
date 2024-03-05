/** CIL ExportedType node (II.22.14). */
class SgAsmCilExportedType: public SgAsmCilMetadata {

    /** Property: EventFlags.
     *
     *  A 4-byte bitmask of type TypeAttribute. */
    [[using Rosebud: rosetta]]
    uint32_t EventFlags = 0;

    /** Property: TypeDefIdName.
     *
     *  A 4-byte index into a TypeDef table of another module in this Assembly. */
    [[using Rosebud: rosetta]]
    uint32_t TypeDefIdName = 0;

    /** Property: TypeName.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeName = 0;

    /** Property: TypeNamespace.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeNamespace = 0;

    /** Property: Implementation.
     *
     *  An index into the File table, ExportedType table, or the AssemblyRef table. */
    [[using Rosebud: rosetta]]
    uint32_t Implementation = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_TypeName_string() const;
    const std::uint8_t* get_TypeNamespace_string() const;
    const SgAsmCilMetadata* get_Implementation_object() const;
};
