/** CIL File node. */
class SgAsmCilFile: public SgAsmCilMetadata {
    /** Property: Flags.
     *
     *  A 4-byte bitmask of type FileAttributes. */
    [[using Rosebud: rosetta]]
    uint32_t Flags;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name;

    /** Property: HashValue.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t HashValue;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_HashValue_blob() const;
};
