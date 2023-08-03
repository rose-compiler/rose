/** CIL MethodDef node (II.22.26). */
class SgAsmCilMethodDef: public SgAsmCilMetadata {

    /** Property: RVA.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t RVA = {};

    /** Property: ImplFlags.
     *
     *  A 2-byte bitmask of type MethodImplAttributes. */
    [[using Rosebud: rosetta]]
    uint32_t ImplFlags = {};

    /** Property: Flags.
     *
     *  A 2-byte bitmask of type MethodAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Flags = {};

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = {};

    /** Property: Signature.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Signature = {};

    /** Property: ParamList.
     *
     *  An index into the Param table. */
    [[using Rosebud: rosetta]]
    uint32_t ParamList = {};


    /** Property: body.
     *
     *  A block containing an instruction sequence (either x86 or CIL). */
    [[using Rosebud: rosetta]]
    SgAsmBlock* body = {};

    /** Property: initLocals.
     *
     *  Returns true if locals are initialized. */
    [[using Rosebud: rosetta]]
    bool initLocals = {};

    /** Property: stackSize.
     *
     *  Returns the max stack size required for this method. */
    [[using Rosebud: rosetta]]
    uint32_t stackSize = {};

    /** Property: hasMoreSections.
     *
     *  Returns true if this method has more sections in addition to the body. */
    [[using Rosebud: rosetta]]
    bool hasMoreSections = {};
public:
    void parse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);

    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Signature_blob() const;
    const SgAsmCilMetadata* get_ParamList_object() const;
};
