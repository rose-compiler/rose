/** CIL SgAsmCilMetadataRoot. */
class SgAsmCilMetadataRoot: public SgAsmCilNode {
    /** Property: Magic signature for physical metadata (always 0x424A5342). */
    [[using Rosebud: rosetta]]
    uint32_t Signature;

    /** Property: Major version number. */
    [[using Rosebud: rosetta]]
    uint16_t MajorVersion;

    /** Property: Minor version number. */
    [[using Rosebud: rosetta]]
    uint16_t MinorVersion;

    /** Property: Reserved space. */
    [[using Rosebud: rosetta]]
    uint32_t Reserved0;

    /** Property: Number of bytes allocated to hold version string (including null terminator). */
    [[using Rosebud: rosetta]]
    uint32_t Length;

    /** Property: Version string. */
    [[using Rosebud: rosetta]]
    std::string Version;

    /** Property: Padding to next 4-byte boundary. */
    [[using Rosebud: rosetta]]
    uint32_t VersionPadding;

    /** Property: Reserved space. */
    [[using Rosebud: rosetta]]
    uint16_t Flags;

    /** Property: Reserved space. */
    [[using Rosebud: rosetta]]
    uint16_t NumberOfStreams;

    // FIXME[Robb Matzke 2023-03-22]: these were implemented elsewhere, but could be auto generated
    [[using Rosebud: rosetta, traverse, accessors(), mutators(), large]]
    std::vector<SgAsmCilDataStream*> Streams;
    const std::vector<SgAsmCilDataStream*>& get_Streams() const;
    std::vector<SgAsmCilDataStream*>& get_Streams();

private:
    /** indices to efficienty access the different heaps
     * @{ */
    mutable int8_t idxStringHeap = -1;
    mutable int8_t idxBlobHeap = -1;
    mutable int8_t idxUsHeap = -1;
    mutable int8_t idxGuidHeap = -1;
    mutable int8_t idxMetadataHeap = -1;
    /** }@ */

public:
    /** 4-byte indicating the start of the metadata root in the .text section.
     */
    static constexpr uint32_t MAGIC_SIGNATURE = 0x424A5342;

    /** Declares the parse function as called from the Cil section header object
     *  @pre isSgAsmCliHeader(this->get_parent()) != nullptr
     */
    void parse();

    /** Declares the parse function.
     *  @param buf the binary buffer
     *  @param index the start location of the metadata root object.
     *         buf[index] is the location of the magic number.
     */
    void parse(std::vector<uint8_t>& buf, size_t index);

    /** accessor functions to return specific heaps
     * @{ */
    SgAsmCilUint8Heap*    get_StringHeap() const;
    SgAsmCilUint8Heap*    get_BlobHeap() const;
    SgAsmCilUint8Heap*    get_UsHeap() const;
    SgAsmCilUint32Heap*   get_GuidHeap() const;
    SgAsmCilMetadataHeap* get_MetadataHeap() const;
    /** }@ */
};
