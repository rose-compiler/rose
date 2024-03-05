/** CIL MethodSpec node (II.22.29). */
class SgAsmCilMethodSpec: public SgAsmCilMetadata {

    /** Property: Method.
     *
     *  An index into the MethodDef or MemberRef table. */
    [[using Rosebud: rosetta]]
    uint32_t Method = 0;

    /** Property: Instantiation.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Instantiation = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const SgAsmCilMetadata* get_Method_object() const;
    const std::uint8_t* get_Instantiation_blob() const;
};
