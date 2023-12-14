/** CIL TypeRef node (II.22.38). */
class SgAsmCilTypeRef: public SgAsmCilMetadata {
public:
    /** Property: ResolutionScope.
     *
     *  An index into a Module, ModuleRef, AssemblyRef or TypeRef table, or null. */
    [[using Rosebud: rosetta]]
    uint16_t ResolutionScope = 0;

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

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const SgAsmCilMetadata* get_ResolutionScope_object() const;
    const std::uint8_t* get_TypeName_string() const;
    const std::uint8_t* get_TypeNamespace_string() const;
};
