#ifndef ROSE_BinaryAnalysis_SRecord_H
#define ROSE_BinaryAnalysis_SRecord_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>

#include <Sawyer/Optional.h>

#include <ostream>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {

/** S-Record hexadecimal data formats.
 *
 *  An S-Record is a simple ASCII-based format for conveying binary information, usually used for loading code and data onto an
 *  embedded processor. Multiple versions of S-Record formats exist, with the most popular being a Motorola syntax and an Intel
 *  syntax.
 *
 *  The Motorola S-Record format is also known as SRECORD, SREC, S19, S28, and S37.  The "S19" name comes from the fact that
 *  record types 1 and 9 are used, allowing for 16-bit addresses; "s28" are record types 2 and 8 which allow 24-bit addresses;
 *  and "s37" are record types 3 and 7 which allow 32-bit addresses.
 *
 *  The Intel HEX format is another variant of S-Records. The sub-formats "I8HEX", "I16HEX", and "I32HEX" describe the address
 *  sizes that are supported, namely 16-bit, 20-bit, and 32-bit (note that the number in the format name doesn't actually match
 *  the address width in the first two cases).
 *
 *  ROSE's S-Record parser doesn't restrict which record types can appear within a file -- it just parses whatever you hand it
 *  and returns a vector of @ref SRecord objects. In fact, it will even allow the input to contain a mixture of Motorola and
 *  Intel syntax, although it will mark the less frequently-occuring syntax as errors.  If any parsing errors are encountered,
 *  they are indicated within the individual @ref SRecord objects. */
class SRecord {
public:
    /** Syntax of the S-Record. */
    enum Syntax {
        SREC_MOTOROLA,                                  /**< Motorola syntax S-Record. */
        SREC_INTEL,                                     /**< Intel syntax S-Record. */
        SREC_UNKNOWN_SYNTAX                             /**< Unknown or no syntax. */
    };

    /** Type of S-Record. */
    enum Type {
        // Motorola
        SREC_M_HEADER,                                  /**< Motorola vendor-specific header, usually NUL-terminated ASCII. */
        SREC_M_DATA16,                                  /**< Motorola data with 16-bit address. */
        SREC_M_DATA24,                                  /**< Motorola data with 24-bit address. */
        SREC_M_DATA32,                                  /**< Motorola data with 32-bit address. */
        SREC_M_RESERVED,                                /**< Motorola reserved. */
        SREC_M_COUNT16,                                 /**< Motorola 16-bit count of data records. */
        SREC_M_COUNT24,                                 /**< Motorola 24-bit count of data records. */
        SREC_M_START32,                                 /**< Motorola 32-bit starting address. */
        SREC_M_START24,                                 /**< Motorola 24-bit starting address. */
        SREC_M_START16,                                 /**< Motorola 16-bit starting address. */

        // Intel
        SREC_I_DATA,                                    /**< Intel data record. */
        SREC_I_END,                                     /**< Intel end record. */
        SREC_I_EXTENDED_SA,                             /**< Intel extended segment address. */
        SREC_I_START_SA,                                /**< Intel starting segment address. */
        SREC_I_EXTENDED_LA,                             /**< Intel extended linear address. */
        SREC_I_START_LA,                                /**< Intel starting linear address. */

        SREC_UNKNOWN_TYPE                               /**< Not a valid S-Record. */
    };

private:
    Type type_;
    Address addr_;
    std::vector<uint8_t> data_;
    std::string error_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors, etc.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Construct an empty S-Record. */
    SRecord(): type_(SREC_UNKNOWN_TYPE), addr_(0) {}

    /** Construct an S-Record with data.
     *
     *  @{ */
    SRecord(Type type, Address addr, const std::vector<uint8_t> data)
        : type_(type), addr_(addr), data_(data) {}
    SRecord(Type type, Address addr, const uint8_t *buffer, size_t bufsz)
        : type_(type), addr_(addr), data_(buffer+0, buffer+bufsz) {}
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // High-level functions.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Construct an S-Record by parsing a string.
     *
     *  This parses either a Motorola or Intel syntax S-Record. */
    static SRecord parse(const std::string&);

    /** Construct S-Records by parsing a file.
     *
     *  Returns a vector with one element per line of input. Lines that did not contain an S-Record or which had trailing
     *  characters other than white space are reported as SRecord objects of type SREC_NONE and an error message. */
    static std::vector<SRecord> parse(std::istream&);

    /** Addresses occupied by all data.
     *
     *  The specified S-Records are scanned in order to create the set of memory addresses that the data will occupy, and this
     *  set is returned. */
    static AddressIntervalSet dataAddresses(const std::vector<SRecord>&);

    /** Create segments for S-Records.
     *
     *  The specified memory map is modified to create mapped regions for the specified S-Records using the minimum possible
     *  number of segments.  First, the S-Records are scanned to obtain a list of addresses. These addresses are expanded by
     *  aligning their beginning and ending locations. Then a temporary memory map is created that contains the minimum number
     *  of aligned segments necessary to represent all the S-Record data. The created segments have the specified access permissions
     *  and names. Finally, the segments are copied into the original @p map by calling @ref MemoryMap::linkTo and passing the
     *  specified @p clobber flag. The temporary map is deleted, but since buffers are reference counted, they will continue to
     *  exist if referenced by the specified @p map.
     *
     *  The return value is the set of addresses that were added to the @p map. */
    static AddressIntervalSet createSegments(const std::vector<SRecord>&, const MemoryMapPtr &map, Address alignment,
                                             unsigned accessPermissions, const std::string &segmentName,
                                             MemoryMap::Clobber clobber);

    /** Load S-Records into a memory map.
     *
     *  The data from the specified S-Records are loaded into the specified memory map. The map must contains segments at all
     *  S-Record addresses or else a @ref MemoryMap::NotMapped exception is thrown. The return value is the execution starting
     *  address contained in the S-Records, if any. */
    static Sawyer::Optional<Address> load(const std::vector<SRecord>&, const MemoryMapPtr&);

    /** Load S-Records into a memory map, creating segments if necessary.
     *
     *  This is a convenience function that calls @ref createSegments in order to create any necessary segments in the memory
     *  map, and then calls the two-argument version of @ref load in order to load the S-Record data into those segments. Therefore,
     *  the arguments are the union of the arguments for those two functions. */
    static Sawyer::Optional<Address> load(const std::vector<SRecord>&, const MemoryMapPtr&, Address alignment,
                                               unsigned accessPerms, const std::string &name, MemoryMap::Clobber);

    /** Create S-Records from a memory map. */
    static std::vector<SRecord> create(const MemoryMapPtr&, Syntax, size_t bytesPerRecord=28, size_t preferredAddrSize=4);

    /** Convert an S-Record to a string. */
    std::string toString() const;

    /** Print an S-Record to a stream.
     *
     *  @{ */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const SRecord&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // S-Record properties.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: syntax family.
     *
     *  This is a read-only property computed from the record type. */
    Syntax syntax() const;

    /** Property: record type.
     *
     *  @{ */
    Type type() const { return type_; }
    SRecord& type(Type type) { type_ = type; return *this; }
    /** @} */

    /** Property: record starting address.
     *
     *  Note that Intel S-Record addresses are only partially specified in the individual S-Records; the actual full address
     *  depends on the S-Records that came before it. This property is only the partial address for Intel S-Records.
     *
     *  See also, @ref RunLengthEncoding::dataAddress to get the fully specified address.
     *
     *  @{ */
    Address address() const { return addr_; }
    SRecord& address(Address addr) { addr_ = addr; return *this; }
    /** @} */

    /** Property: record data.
     *
     *  @{ */
    const std::vector<uint8_t>& data() const { return data_; }
    std::vector<uint8_t>& data() { return data_; }
    SRecord& data(const std::vector<uint8_t>& data) { data_ = data; return *this; }
    /** @} */

    /** Property: record error messsage.
     *
     *  @{ */
    const std::string& error() const { return error_; }
    SRecord& error(const std::string &error) { error_ = error; return *this; }
    /** @} */



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Low-level functions.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Run-length encoding information. */
    class RunLengthEncoding {
        Sawyer::Optional<Address> dataVa_, executionVa_;
        size_t nDataRecords_;
    public:
        /** Default constructor. */
        RunLengthEncoding()
            : nDataRecords_(0) {}

        /** Insert an S-Record into the data sumarized by this class.
         *
         *  This should be called after parsing or emitting each S-Record. */
        void insert(const SRecord&);

        /** Returns the memory address for the start of a data record, or nothing. */
        Sawyer::Optional<Address> dataAddress(const SRecord&) const;

        /** Base data address.
         *
         *  This is the amount which to add to individual S-Record addresses. */
        Address baseDataAddress() const;

        /** Returns the execution starting address if one was specified. */
        Sawyer::Optional<Address> executionAddress() const;

        /** Number of data records seen. */
        size_t nDataRecords() const { return nDataRecords_; }
    };

    /** Construct a Motorola S-Record by parsing a string. */
    static SRecord parseMotorola(const std::string&);

    /** Construct an Intel S-Record by parsing a string. */
    static SRecord parseIntel(const std::string&);

    /** Parse a hexadecimal string to an unsigned integer.
     *
     *  The parsing looks at characters of the string starting at the specified offset and continuing for the specified
     *  number of characters. Each character in that range must be a valid hexadecimal digit representing four bits of the
     *  result in big-endian order.  Returns the unsigned value if successful, or nothing if a syntax error, short read,
     *  or overflow is encountered. */
    static Sawyer::Optional<Address> parseBigEndianInteger(const std::string&, size_t start, size_t nChars);

    /** Convert the first few bytes of the buffer to a big-endian integer. */
    static unsigned bigEndian(const std::vector<uint8_t> &data, size_t nBytes);

    /** Size of address in bytes.
     *
     *  The address is encoded in twice as many ASCII characters. */
    static size_t addressNBytes(Type);

    /** Compute a checksum for part of a record.
     *
     *  Given a string, compute the checksum for the hexadecimal characters starting with the specified @p start offset and
     *  continuing for @p n characters. All characters in the range must be hexadecimal digits, and @p n must be even. */
    static uint8_t checksum(Syntax, const std::string&, size_t start, size_t n);
};

} // namespace
} // namespace

#endif
#endif
