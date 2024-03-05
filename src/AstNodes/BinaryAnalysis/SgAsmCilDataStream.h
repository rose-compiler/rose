/** Base class for CIL branch of binary analysis IR nodes. */
[[Rosebud::abstract]]
class SgAsmCilDataStream : public SgAsmCilNode {
    /** Property: Start offset of the byte stream. */
    [[using Rosebud: rosetta, ctor_arg]]
    uint32_t Offset;

    /** Property: Length of the byte stream. */
    [[using Rosebud: rosetta, ctor_arg]]
    uint32_t Size;

    /** Property: Name of the byte stream. */
    [[using Rosebud: rosetta, ctor_arg]]
    std::string Name;

    /** Property: Name of the byte stream. */
    [[using Rosebud: rosetta, ctor_arg]]
    uint32_t NamePadding;

public:
    /** Define constant strings for the known stream types
     *  @{
     */
    static const std::string ID_STRING_HEAP;
    static const std::string ID_BLOB_HEAP;
    static const std::string ID_US_HEAP;
    static const std::string ID_GUID_HEAP;
    static const std::string ID_METADATA_HEAP;
    /** }@ */

    /**
     * Defines the parse function.
     * @param buf the binary buffer.
     * @param startOfMetaData the location in buf where the metadata root (CIL ECMA II.24.2.1)
     *        is located.
     * @details
     *   buf[startOfMetaData] is the location of the metadata root magic number.
     *   parse will add get_Offset to startOfMetaData to find the first byte of a
     *   stream.
     */
    virtual void parse(const std::vector<uint8_t>& buf, size_t startOfMetaData); /* FIXME: should be abstract */

    /** unparses the metadata objects into the buffer */
    virtual void unparse(std::vector<uint8_t>& buf, size_t startOfMetaData) const; /* FIXME: should be abstract */

    /** Prints the object's data in a human readable form to @ref os. */
    virtual void dump(std::ostream& os) const; /* FIXME: should be abstract */
};

