#include "SRecord.h"
#include "Diagnostics.h"
#include "StringUtility.h"
#include "integerOps.h"
#include <boost/foreach.hpp>

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {

std::ostream&
operator<<(std::ostream &stream, const SRecord &x) {
    x.print(stream);
    return stream;
}

// class method
size_t
SRecord::addressNBytes(Type type) {
    switch (type) {
        case SREC_HEADER:
        case SREC_DATA16:
        case SREC_COUNT16:
        case SREC_START16:
            return 2;
        case SREC_DATA24:
        case SREC_COUNT24:
        case SREC_START24:
            return 3;
        case SREC_DATA32:
        case SREC_START32:
            return 4;
        default:
            return 0;
    }
}

// class method
SRecord
SRecord::parse(const std::string &input)
{
    using namespace StringUtility;

    // Header: 'S' + type
    if (input.size() < 1+1+2)                           // 'S' + type + payloadsize
        return SRecord().error("short input");
    if (input[0]!='S')
        return SRecord().error("not an S-Record");
    if (!isdigit(input[1]))
        return SRecord().error("invalid type");
    SRecord srec;
    srec.type((Type)hexadecimalToInt(input[1]));

    // Payload size: two hexadecimal characters
    if (!isxdigit(input[2]) || !isxdigit(input[3]))
        return SRecord().error("size syntax error");
    size_t payloadNBytes = hexadecimalToInt(input[2])*16 + hexadecimalToInt(input[3]);
    if (payloadNBytes < addressNBytes(srec.type()) + 1) // sizeof(address) + sizeof(checksum)
        return SRecord().error("invalid size");
    if (input.size() < 2 + 2 + 2*payloadNBytes)         // sizeof(header)+sizeof(payloadsize)+sizeof(payload)
        return SRecord().error("short input");

    // Address
    rose_addr_t addr = 0;
    size_t at = 1+1+2;                                  // 'S' type payloadsize
    for (size_t i=2*addressNBytes(srec.type()); i>0; --i, ++at) {
        if (!isxdigit(input[at]))
            return SRecord().error("address syntax error");
        addr = addr*16 + hexadecimalToInt(input[at]);
    }
    srec.address(addr);

    // Data
    ASSERT_require(payloadNBytes >= addressNBytes(srec.type()) + 1 /*checksum*/);
    size_t dataNBytes = payloadNBytes - addressNBytes(srec.type()) - 1 /*checksum*/;
    for (size_t i=0; i<dataNBytes; ++i, at+=2) {
        if (!isxdigit(input[at]) || !isxdigit(input[at+1]))
            return SRecord().error("data syntax error");
        uint8_t byte = hexadecimalToInt(input[at])*16 + hexadecimalToInt(input[at+1]);
        srec.data().push_back(byte);
    }

    // Checksum
    uint8_t checksum = hexadecimalToInt(input[at])*16 + hexadecimalToInt(input[at+1]);
    if (srec.checksum() != checksum)
        return SRecord().error("invalid checksum");

    return srec;
}

// class method
std::vector<SRecord>
SRecord::parse(std::istream &input)
{
    std::vector<SRecord> srecs;
    static const size_t maxLineLength = 1+1+2+255+16+1;   // 'S' + type + payloadsize + payload + arbitrary padding + NUL
    char line[maxLineLength];
    while (input.good()) {
        input.getline(line, sizeof line);
        if (line[0])
            srecs.push_back(SRecord::parse(line));
        if (srecs.back().error().empty()) {
            for (size_t at=srecs.back().encodedSize(); line[at]; ++at) {
                if (!isspace(line[at])) {
                    srecs.back().error("trailing garbage: \"" + StringUtility::cEscape(line+at) + "\"");
                    break;
                }
            }
        }
    }
    return srecs;
}

// class method
rose_addr_t
SRecord::load(const std::vector<SRecord> &srecs, MemoryMap &map, bool createSegments, unsigned accessPerms)
{
    if (createSegments) {
        // We want to minimize the number of buffers in the map, so the first step is to discover what addresses are covered by
        // the data S-records
        Sawyer::Container::IntervalSet<AddressInterval> addressesUsed;
        BOOST_FOREACH (const SRecord &srec, srecs) {
            switch (srec.type()) {
                case SREC_DATA16:
                case SREC_DATA24:
                case SREC_DATA32:
                    addressesUsed.insert(AddressInterval::baseSize(srec.address(), srec.data().size()));
                    break;
                default:
                    break;
            }
        }

        // Create buffers for the data and insert them into the memory map
        BOOST_FOREACH (const AddressInterval &interval, addressesUsed.intervals()) {
            ASSERT_forbid(interval.isWhole());              // not practically possible since S-Record file would be >2^65 bytes
            map.insert(interval, MemoryMap::Segment::anonymousInstance(interval.size(), accessPerms, "S-Records"));
        }
    }

    // Populate the map by writing the S-Record data into it.
    rose_addr_t startingAddr = 0;
    BOOST_FOREACH (const SRecord &srec, srecs) {
        switch (srec.type()) {
            case SREC_DATA16:
            case SREC_DATA24:
            case SREC_DATA32: {
                if (!srec.data().empty()) {
                    size_t nwritten = map.at(srec.address()).write(srec.data()).size();
                    if (nwritten != srec.data().size())
                        throw MemoryMap::NotMapped("S-Record destination is not mapped for " +
                                                   StringUtility::plural(srec.data().size(), "bytes"),
                                                   &map, srec.address());
                }
                break;
            }
            case SREC_START16:
            case SREC_START24:
            case SREC_START32:
                startingAddr = srec.address();
                break;
            default:
                break;
        }
    }
    return startingAddr;
}

// class method
size_t
SRecord::dump(const MemoryMap &map, std::ostream &out, size_t addrSize) {
    ASSERT_require(2==addrSize || 3==addrSize || 4==addrSize);
    SRecord::Type type = SREC_NONE;
    switch (addrSize) {
        case 2: type = SREC_DATA16; break;
        case 3: type = SREC_DATA24; break;
        case 4: type = SREC_DATA32; break;
    }

    size_t nRecords = 0;
    rose_addr_t va = 0;
    static const size_t maxBytesPerRecord = 28;         // common value so each S-Record fits on an 80-character screen
    uint8_t buffer[maxBytesPerRecord];
    while (map.atOrAfter(va).next().assignTo(va)) {
        size_t nread = map.at(va).limit(maxBytesPerRecord).read(buffer).size();
        ASSERT_require(nread>0);                        // since map.next() returned true
        SRecord srec(type, va, buffer, nread);
        out <<srec <<"\n";
        va += nread;
        ++nRecords;
    }
    return nRecords;
}

void
SRecord::print(std::ostream &out) const {
    // Header
    switch (type_) {
        case SREC_HEADER:   out <<"S0"; break;
        case SREC_DATA16:   out <<"S1"; break;
        case SREC_DATA24:   out <<"S2"; break;
        case SREC_DATA32:   out <<"S3"; break;
        case SREC_RESERVED: out <<"S4"; break;
        case SREC_COUNT16:  out <<"S5"; break;
        case SREC_COUNT24:  out <<"S6"; break;
        case SREC_START32:  out <<"S7"; break;
        case SREC_START24:  out <<"S8"; break;
        case SREC_START16:  out <<"S9"; break;
        default:
            ASSERT_not_reachable("invalid S-Record type " + StringUtility::numberToString(type_));
    }

    // Payload size
    size_t size = addressNBytes(type_) + data_.size() + 1 /*checksum*/;
    ASSERT_forbid2(size > 255, "S-Record size is too large");
    mfprintf(out)("%02zX", size);

    // Address
    rose_addr_t addrMask = IntegerOps::genMask<rose_addr_t>(8*addressNBytes(type_));
    if ((addr_ & ~addrMask) != 0) {
        throw std::runtime_error("S-Record address " + StringUtility::addrToString(addr_) +
                                 " needs more than " + StringUtility::numberToString(addressNBytes(type_)) + " bytes");
    }
    mfprintf(out)("%0*"PRIX64, 2*(int)addressNBytes(type_), addr_);

    // Data
    BOOST_FOREACH (uint8_t byte, data_)
        mfprintf(out)("%02X", (unsigned)byte);

    // Checksum
    mfprintf(out)("%02X", (unsigned)checksum());
}

size_t
SRecord::encodedSize() const {
    return 1/*'S'*/ + 1/*type*/ + 2/*payloadSize*/ + 2*addressNBytes(type_) + 2*data_.size() + 2/*checksum*/;
}

uint8_t
SRecord::checksum() const {
    unsigned sum = addressNBytes(type_) + data_.size() + 1 /*checksum*/;
    for (size_t i=0; i<sizeof addr_; ++i)
        sum += (addr_ >> (8*i)) & 0xff;
    BOOST_FOREACH (uint8_t byte, data_)
        sum += byte;
    return ~sum & 0xff;
}

} // namespace
} // namespace
