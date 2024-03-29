/** CIL TypeSpec node (II.22.39). */
class SgAsmCilTypeSpec: public SgAsmCilMetadata {

    /** Property: Signature.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Signature = 0;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_Signature_blob() const;
};
