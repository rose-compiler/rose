#ifndef ROSE_BinaryFormats_SRecord_H
#define ROSE_BinaryFormats_SRecord_H

#include "sage3basic.h"

namespace rose {
namespace BinaryAnalysis {

/** Motorola S-Record.
 *
 *  Motorola S-record is a file format, created by Motorola, that conveys binary information in ASCII hexadecimal text
 *  form. This file format may also be known as SRECORD, SREC, S19, S28, S37. It is commonly used for programming
 *  microcontrollers, EPROMs, and other types of programmable logic devices. In a typical application, a compiler or assembler
 *  converts a program's source code (such as C or assembly language) to machine code and outputs it into a HEX file. The HEX
 *  file is then imported by a programmer to "burn" the machine code into a ROM, or is transferred to the target system for
 *  loading and execution. */
class SRecord {
public:
    /** Type of S-Record. */
    enum Type {
        // The values listed explicitly are important as they are part of a standard.
        SREC_HEADER     = 0,                            /**< Vendor-specific header, usually NUL-terminated ASCII. */
        SREC_DATA16     = 1,                            /**< Data with 16-bit address. */
        SREC_DATA24     = 2,                            /**< Data with 24-bit address. */
        SREC_DATA32     = 3,                            /**< Data with 32-bit address. */
        SREC_RESERVED   = 4,                            /**< Reserved. */
        SREC_COUNT16    = 5,                            /**< 16-bit count of data records. */
        SREC_COUNT24    = 6,                            /**< 24-bit count of data records. */
        SREC_START32    = 7,                            /**< 32-bit starting address. */
        SREC_START24    = 8,                            /**< 24-bit starting address. */
        SREC_START16    = 9,                            /**< 16-bit starting address. */
        SREC_NONE                                       /**< Not a valid S-Record. */
    };

private:
    Type type_;
    rose_addr_t addr_;
    std::vector<uint8_t> data_;
    std::string error_;

public:
    /** Construct an empty S-Record. */
    SRecord(): type_(SREC_NONE), addr_(0) {}

    /** Construct an S-Record with data.
     *
     *  @{ */
    SRecord(Type type, rose_addr_t addr, const std::vector<uint8_t> data)
        : type_(type), addr_(addr), data_(data) {}
    SRecord(Type type, rose_addr_t addr, const uint8_t *buffer, size_t bufsz)
        : type_(type), addr_(addr), data_(buffer+0, buffer+bufsz) {}
    /** @} */
    

    /** Construct an S-Record by parsing a string. */
    static SRecord parse(const std::string&);

    /** Construct S-Records by parsing a file.
     *
     *  Returns a vector with one element per line of input. Lines that did not contain an S-Record or which had trailing
     *  characters other than white space are reported as SRecord objects of type SREC_NONE and an error message. */
    static std::vector<SRecord> parse(std::istream&);

    /** Accessor for the S-Record type.
     *
     *  @{ */
    Type type() const { return type_; }
    SRecord& type(Type type) { type_ = type; return *this; }
    /** @} */

    /** Accessor for the S-Record address.
     *
     *  @{ */
    rose_addr_t address() const { return addr_; }
    SRecord& address(rose_addr_t addr) { addr_ = addr; return *this; }
    /** @} */

    /** Accessor for the S-Record data.
     *
     *  @{ */
    const std::vector<uint8_t>& data() const { return data_; }
    std::vector<uint8_t>& data() { return data_; }
    SRecord& data(const std::vector<uint8_t>& data) { data_ = data; return *this; }
    /** @} */

    /** Accessor for the S-Record error message.
     *
     *  @{ */
    const std::string& error() const { return error_; }
    SRecord& error(const std::string &error) { error_ = error; return *this; }
    /** @} */

    /** Size of address in bytes.
     *
     *  The address is encoded in twice as many ASCII characters. */
    static size_t addressNBytes(Type);

    /** Size of encoded S-Record in characters.
     *
     *  If an error was encountered when parsing this S-Record (<code>!error().empty()</code>) then the size reported by this
     *  method will likely be only the size up to the point where the error was encountered. */
    size_t encodedSize() const;

    /** Load S-Records into a memory map.
     *
     *  The specified S-Records are loaded into the memory map.  If @p createSegments is true then a minimal number of segments
     *  are created to hold the S-Record data, otherwise the destinations must already be mapped or a MemoryMap::NotMapped
     *  exception is thrown.  When new segments are created they are given access permissions specified by @p
     *  accessPerms. Access permissions are not consulted and do not cause errors when writing S-Record data into the map. */
    static rose_addr_t load(const std::vector<SRecord>&, MemoryMap&, bool createSegments=true,
                            unsigned accessPerms=MemoryMap::READABLE|MemoryMap::WRITABLE|MemoryMap::EXECUTABLE);

    /** Compute the checksum for the data.
     *
     *  An S-Record checksum is the 1's complement of the sum of address and data bytes modulo 256. */
    uint8_t checksum() const;

    /** Print an S-Record to a stream.
     *
     *  @{ */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const SRecord&);
    /** @} */


    /** Dump a memory map as S-Records.
     *
     *  All data in the specified memory map is dumped to the specified output stream. The @p addrSize should be 2, 3, or 4 to
     *  indicate 16-, 24-, or 32-bit addresses. The output is not prefixed with an SREC_HEADER record nor terminated with
     *  SREC_COUNT or SREC_START records.  An <code>std::runtime_error</code> is thrown if an address in the memory map cannot
     *  be encoded in the specified @p addrSize number of bytes. Returns the number of records emitted. */
    static size_t dump(const MemoryMap&, std::ostream&, size_t addrSize=4);
};

} // namespace
} // namespace

#endif
