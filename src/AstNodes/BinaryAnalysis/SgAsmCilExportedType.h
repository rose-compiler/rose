/** CIL ExportedType node. */
class SgAsmCilExportedType: public SgAsmCilMetadata {
    /** Property: EventFlags.
     *
     *  A 4-byte bitmask of type TypeAttribute. */
    [[using Rosebud: rosetta]]
    uint32_t EventFlags;

    /** Property: TypeDefIdName.
     *
     *  A 4-byte index into a TypeDef table of another module in this Assembly. */
    [[using Rosebud: rosetta]]
    uint32_t TypeDefIdName;

    /** Property: TypeName.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeName;

    /** Property: TypeNamespace.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeNamespace;

    /** Property: Implementation.
     *
     *  An index into the File table, ExportedType table, or the AssemblyRef table. */
    [[using Rosebud: rosetta]]
    uint32_t Implementation;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_TypeName_string() const;
    const std::uint8_t* get_TypeNamespace_string() const;
    const SgAsmCilMetadata* get_Implementation_object() const;
};
