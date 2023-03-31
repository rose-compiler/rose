/** CIL Module node. */
class SgAsmCilModule: public SgAsmCilMetadata {
    /** Property: Generation.
     *
     *  A 2-byte value, reserved, shall be zero. */
    [[using Rosebud: rosetta]]
    uint16_t Generation;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name;

    /** Property: Mvid.
     *
     *  An index into the Guid heap. */
    [[using Rosebud: rosetta]]
    uint32_t Mvid;

    /** Property: Encld.
     *
     *  An index into the Guid heap. */
    [[using Rosebud: rosetta]]
    uint32_t Encld;

    /** Property: EncBaseId.
     *
     *  An index into the Guid heap. */
    [[using Rosebud: rosetta]]
    uint32_t EncBaseId;

public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_Name_string() const;
    const std::uint32_t* get_Mvid_guid() const;
    const std::uint32_t* get_Encld_guid() const;
    const std::uint32_t* get_EncBaseId_guid() const;
};
