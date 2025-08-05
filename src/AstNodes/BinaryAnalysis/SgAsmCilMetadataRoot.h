/** CIL SgAsmCilMetadataRoot. */
class SgAsmCilMetadataRoot: public SgAsmCilNode {


    /** Property: Magic signature for physical metadata (always 0x424A5342) */
    [[using Rosebud: rosetta]]
    uint32_t Signature = 0;

    /** Property: Major version number. */
    [[using Rosebud: rosetta]]
    uint16_t MajorVersion = 0;

    /** Property: Minor version number. */
    [[using Rosebud: rosetta]]
    uint16_t MinorVersion = 0;

    /** Property: Reserved space. */
    [[using Rosebud: rosetta]]
    uint32_t Reserved0 = 0;

    /** Property: Number of bytes allocated to hold version string (including null terminator). */
    [[using Rosebud: rosetta]]
    uint32_t Length = 0;

    /** Property: Version string. */
    [[using Rosebud: rosetta]]
    std::string Version;

    /** Property: Padding to next 4-byte boundary. */
    [[using Rosebud: rosetta]]
    uint32_t VersionPadding = 0;

    /** Property: Reserved space. */
    [[using Rosebud: rosetta]]
    uint16_t Flags = 0;

    /** Property: Reserved space. */
    [[using Rosebud: rosetta]]
    uint16_t NumberOfStreams = 0;

    /** Property: Metadata streams. */
    [[using Rosebud: rosetta, traverse, accessors(get_Streams), mutators(), large]]
    std::vector<SgAsmCilDataStream*> Streams;
    
    /** Property: Flag indicating whether the entire CIL metadata section was parsed without errors. */
    [[using Rosebud: rosetta]]
    bool ErrorFree = true;

private:
    /** indices to efficiently access the different heaps
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

    /** Declares the parse function as called from the Cli section header object.
     *  @pre isSgAsmCliHeader(this->get_parent()) != nullptr
     */
    void parse();

    /** Declares the parse function.
     *  @param buf the binary buffer.
     *  @param index the start location of the metadata root object.
     *         buf[index] is the location of the magic number.
     */
    void parse(const std::vector<uint8_t>& buf, size_t index);

    /** Declares the unparse function as called from the Cli section header object. */
    void unparse(std::ostream& f) const;

    /** Unparses the metadata objects into a buffer starting at index @p idx. */
    void unparse(std::vector<uint8_t>&, std::size_t) const;

    /** Prints the object's data in a human readable form to @p os. */
    void dump(std::ostream& os) const;

    /** Accessor function to return a specific heap.
     *
     * @{ */
    SgAsmCilUint8Heap*    get_StringHeap() const;
    SgAsmCilUint8Heap*    get_BlobHeap() const;
    SgAsmCilUint8Heap*    get_UsHeap() const;
    SgAsmCilUint32Heap*   get_GuidHeap() const;
    SgAsmCilMetadataHeap* get_MetadataHeap() const;
    /** }@ */
};
