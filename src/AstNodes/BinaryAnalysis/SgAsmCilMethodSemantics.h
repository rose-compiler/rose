/** CIL MethodSemantics node (II.22.28). */
class SgAsmCilMethodSemantics: public SgAsmCilMetadata {

    /** Property: Semantics.
     *
     *  A 2-byte bitmask of type MethodSemanticsAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Semantics = 0;

    /** Property: Method.
     *
     *  An index into the MethodDef table. */
    [[using Rosebud: rosetta]]
    uint32_t Method = 0;

    /** Property: Association.
     *
     *  An index into the Event or Property table. */
    [[using Rosebud: rosetta]]
    uint32_t Association = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Method_object() const;
    const SgAsmCilMetadata* get_Association_object() const;
};
