/** CIL TypeRef node. */
class SgAsmCilTypeRef: public SgAsmCilMetadata {
    /** Property: ResolutionScope.
     *
     *  An index into a Module, ModuleRef, AssemblyRef or TypeRef table, or null. */
    [[using Rosebud: rosetta]]
    uint16_t ResolutionScope;

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

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_ResolutionScope_object() const;
    const std::uint8_t* get_TypeName_string() const;
    const std::uint8_t* get_TypeNamespace_string() const;
};
