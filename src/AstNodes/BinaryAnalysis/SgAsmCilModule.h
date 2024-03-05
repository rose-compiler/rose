/** CIL Module node (II.22.30). */
class SgAsmCilModule: public SgAsmCilMetadata {

    /** Property: Generation.
     *
     *  A 2-byte value, reserved, shall be zero. */
    [[using Rosebud: rosetta]]
    uint16_t Generation = 0;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

    /** Property: Mvid.
     *
     *  An index into the Guid heap. */
    [[using Rosebud: rosetta]]
    uint32_t Mvid = 0;

    /** Property: Encld.
     *
     *  An index into the Guid heap. */
    [[using Rosebud: rosetta]]
    uint32_t Encld = 0;

    /** Property: EncBaseId.
     *
     *  An index into the Guid heap. */
    [[using Rosebud: rosetta]]
    uint32_t EncBaseId = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_Name_string() const;
    const std::uint32_t* get_Mvid_guid() const;
    const std::uint32_t* get_Encld_guid() const;
    const std::uint32_t* get_EncBaseId_guid() const;
};
