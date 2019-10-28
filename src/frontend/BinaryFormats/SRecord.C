#include "sage3basic.h"

#include "SRecord.h"
#include "Diagnostics.h"
#include "StringUtility.h"
#include "integerOps.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <rose_getline.h>

using namespace Rose::Diagnostics;

namespace Rose {
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
        case SREC_M_HEADER:
        case SREC_M_DATA16:
        case SREC_M_COUNT16:
        case SREC_M_START16:
            return 2;
        case SREC_M_DATA24:
        case SREC_M_COUNT24:
        case SREC_M_START24:
            return 3;
        case SREC_M_DATA32:
        case SREC_M_START32:
            return 4;
        default:
            return 0;
    }
}

// class method
Sawyer::Optional<rose_addr_t>
SRecord::parseBigEndianInteger(const std::string &s, size_t start, size_t nChars) {
    rose_addr_t retval = 0;
    if (start + nChars > s.size())
        return Sawyer::Nothing();
    for (size_t i=0; i < nChars; ++i) {
        if (!isxdigit(s[start+i]))
            return Sawyer::Nothing();
        if ((retval >> (8*sizeof(retval) - 4)) & 0xf)
            return Sawyer::Nothing();               // overflow
        retval = (retval << 4) | StringUtility::hexadecimalToInt(s[start+i]);
    }
    return retval;
}

// class method
uint8_t
SRecord::checksum(Syntax syntax, const std::string &input, size_t start, size_t nChars) {
    ASSERT_require(nChars % 2 == 0);
    unsigned sum = 0;
    for (size_t i = 0; i < nChars; i += 2)
        sum += parseBigEndianInteger(input, start+i, 2).orElse(0);
    switch (syntax) {
        case SREC_MOTOROLA:
            return ~sum & 0xff;
        case SREC_INTEL:
            return (~sum + 1) & 0xff;
        default:
            ASSERT_not_reachable("invalid syntax family");
    }
}

// class method
SRecord
SRecord::parseMotorola(const std::string &input) {
    using namespace StringUtility;

    // Header: 'S' + type
    if (input.size() < 1+1+2)                           // 'S' + type + payloadsize
        return SRecord().error("short input; Motorola S-Records must be at least 4 characters");
    if (input[0]!='S')
        return SRecord().error("not a Motorola S-Record since the line doesn't start with 'S'");
    SRecord srec;
    switch (unsigned t = parseBigEndianInteger(input, 1, 1).orElse(256)) {
        case 0: srec.type_ = SREC_M_HEADER; break;
        case 1: srec.type_ = SREC_M_DATA16; break;
        case 2: srec.type_ = SREC_M_DATA24; break;
        case 3: srec.type_ = SREC_M_DATA32; break;
        case 4: srec.type_ = SREC_M_RESERVED; break;
        case 5: srec.type_ = SREC_M_COUNT16; break;
        case 6: srec.type_ = SREC_M_COUNT24; break;
        case 7: srec.type_ = SREC_M_START32; break;
        case 8: srec.type_ = SREC_M_START24; break;
        case 9: srec.type_ = SREC_M_START16; break;
        case 256:
            return srec.error("type syntax error; expected a hexadecimal character");
        default:
            return srec.error("invalid record type " + numberToString(t));
    }

    // Payload size: two hexadecimal characters
    size_t payloadNBytes = 0;
    if (!parseBigEndianInteger(input, 2, 2).assignTo(payloadNBytes))
        return srec.error("payload size syntax error; expected two hexadecimal characters");
    if (payloadNBytes < addressNBytes(srec.type()) + 1) // sizeof(address) + sizeof(checksum)
        return srec.error("invalid record size; payload is too small for record type");
    if (input.size() < 2 + 2 + 2*payloadNBytes)         // sizeof(header)+sizeof(payloadsize)+sizeof(payload)
        return srec.error("short input; line must contain at least " + plural(2+2+2*payloadNBytes, "characters"));

    // Address
    if (!parseBigEndianInteger(input, 4, 2*addressNBytes(srec.type())).assignTo(srec.addr_))
        return srec.error("address syntax error; expected four hexadecimal characters");

    // Data
    ASSERT_require(payloadNBytes >= addressNBytes(srec.type()) + 1 /*checksum*/);
    size_t dataNBytes = payloadNBytes - addressNBytes(srec.type()) - 1 /*checksum*/;
    size_t at = 1 + 1 + 2 + 2*addressNBytes(srec.type());
    for (size_t i = 0; i < dataNBytes; ++i, at += 2) {
        if (!isxdigit(input[at]) || !isxdigit(input[at+1]))
            return srec.error("data syntax error; data must contain only hexadecimal characters");
        uint8_t byte = parseBigEndianInteger(input, at, 2).get();
        srec.data_.push_back(byte);
    }

    // Checksum
    uint8_t parsedChecksum = 0;
    if (!parseBigEndianInteger(input, 2 + 2 + 2*payloadNBytes - 2, 2).assignTo(parsedChecksum))
        return srec.error("checksum syntax error; expected two hexadecimal characters");
    uint8_t computedChecksum = checksum(SREC_MOTOROLA, input, 2, 2 + 2*payloadNBytes - 2); // sum address and data but not checksum
    if (computedChecksum != parsedChecksum) {
        return srec.error("invalid checksum; parsed " + numberToString(parsedChecksum) +
                          ", computed " + numberToString(computedChecksum));
    }

    // Trailing garbage?
    at = 2 + 2 + 2*payloadNBytes;
    for (size_t eof = at; at < input.size(); ++at) {
        if (!isspace(input[at]))
            return srec.error("trailing garbage: \"" + cEscape(input.substr(eof)) + "\"");
    }


    return srec;
}

// class method
SRecord
SRecord::parseIntel(const std::string &input) {
    using namespace StringUtility;
    static const size_t headerNChars = 1 + 2 + 4 + 2;   // ':' + payload size + address + record type

    if (input.size() < headerNChars)
        return SRecord().error("short input; Intel S-Records must be at least " + plural(headerNChars, "characters"));
    if (input[0] != ':')
        return SRecord().error("not an Intel S-Record ince the line doesn't start with ':'");

    // Payload size
    size_t payloadNBytes = 0;
    if (!parseBigEndianInteger(input, 1, 2).assignTo(payloadNBytes))
        return SRecord().error("size syntax error; expected two hexadecimal characters");
    size_t recordNChars = headerNChars + 2*payloadNBytes + 2;
    if (input.size() < recordNChars)
        return SRecord().error("short input; record needs at least " + plural(recordNChars, "characters"));

    // Address
    SRecord srec;
    if (!parseBigEndianInteger(input, 3, 4).assignTo(srec.addr_))
        return srec.error("address syntax error; expected four hexadecimal characters");

    // Record type
    switch (unsigned t = parseBigEndianInteger(input, 7, 2).orElse(65536)) {
        case 0:
            srec.type_ = SREC_I_DATA;
            break;
        case 1:
            srec.type_ = SREC_I_END;
            if (payloadNBytes != 0)
                return srec.error("Intel end record should have a zero payload size");
            if (srec.address() != 0)
                return srec.error("Intel end record should have a zero address");
            break;
        case 2:
            srec.type_ = SREC_I_EXTENDED_SA;
            if (payloadNBytes != 2)
                return srec.error("Intel extended segment address record should have a two-byte payload");
            if (srec.address() != 0)
                return srec.error("Intel extended segment address record should have a zero address");
            break;
        case 3:
            srec.type_ = SREC_I_START_SA;
            if (payloadNBytes != 2)
                return srec.error("Intel start segment address record should have a two-byte payload");
            if (srec.address() != 0)
                return srec.error("Intel start segment address record should have a zero address");
            break;
        case 4:
            srec.type_ = SREC_I_EXTENDED_LA;
            if (payloadNBytes != 2)
                return srec.error("Intel extended linear address record should have a two-byte payload");
            if (srec.address() != 0)
                return srec.error("Intel extended linear address record should have a zero address");
            break;
        case 5:
            srec.type_ = SREC_I_START_LA;
            if (payloadNBytes != 2)
                return srec.error("Intel start linear address record should have a two-byte payload");
            if (srec.address() != 0)
                return srec.error("Intel start linear address record should have a zero address");
            break;
        case 65536:
            return srec.error("type syntax error; expected two hexadecimal characters");
        default:
            return srec.error("invalid record type " + numberToString(t));
    }

    // Payload
    for (size_t i=0; i < payloadNBytes; ++i) {
        uint8_t byte = 0;
        if (!parseBigEndianInteger(input, headerNChars + 2*i, 2).assignTo(byte)) {
            return srec.error("payload syntax error; payload must contain only hexadecimal characters");
        } else {
            srec.data_.push_back(byte);
        }
    }

    // Checksum
    uint8_t parsedChecksum = 0;
    if (!parseBigEndianInteger(input, recordNChars-2, 2).assignTo(parsedChecksum))
        return srec.error("checksum syntax error; expected two hexadecimal characters");
    uint8_t computedChecksum = checksum(SREC_INTEL, input, 1, recordNChars-3);
    if (computedChecksum != parsedChecksum)
        return srec.error("invalid checksum; parsed " + numberToString(parsedChecksum) +
                          ", computed " + numberToString(computedChecksum));

    // Trailing garbage?
    for (size_t i = recordNChars; i<input.size(); ++i) {
        if (!isspace(input[i]))
            return srec.error("trailing garbage: \"" + cEscape(input.substr(recordNChars)) + "\"");
    }

    return srec;
}

// class method
SRecord
SRecord::parse(const std::string &input) {
    if (boost::starts_with(input, "S")) {
        return parseMotorola(input);
    } else if (boost::starts_with(input, ":")) {
        return parseIntel(input);
    } else {
        return SRecord().error("not an S-Record");
    }
}

// class method
unsigned
SRecord::bigEndian(const std::vector<uint8_t> &data, size_t nBytes) {
    unsigned retval = 0;
    ASSERT_require(nBytes <= data.size());
    ASSERT_require(nBytes <= sizeof(retval));
    for (size_t i = 0; i < nBytes; ++i)
        retval = (retval << 8) | data[i];
    return retval;
}

// class method
std::vector<SRecord>
SRecord::parse(std::istream &input)
{
    std::vector<SRecord> srecs;

    // Parse the input
    RunLengthEncoding rle;
    size_t nMotorola = 0, nIntel = 0;
    while (input.good()) {
        std::string line = rose_getline(input);
        if (line.empty())
            break;

        // Count syntax families
        SRecord srec = SRecord::parse(line);
        switch (srec.syntax()) {
            case SREC_MOTOROLA: ++nMotorola; break;
            case SREC_INTEL: ++nIntel; break;
            default: break;
        }

        // Check data record counts
        if (srec.error().empty()) {
            Sawyer::Optional<size_t> n;
            switch (srec.type()) {
                case SREC_M_COUNT16:
                case SREC_M_COUNT24:
                    n = srec.address();
                    break;
                default:
                    break;
            }
            if (n && *n != rle.nDataRecords()) {
                srec.error("wrong data record count; record says " + StringUtility::numberToString(*n) +
                           " but expected " + StringUtility::numberToString(rle.nDataRecords()));
            }
        }

        // Save record
        srecs.push_back(srec);
        rle.insert(srec);
    }

    // Input should consist of only one syntax style: either Motorola or Intel syntax
    if (nMotorola > 0 && nIntel > 0) {
        Syntax expected = nMotorola >= nIntel ? SREC_MOTOROLA : SREC_INTEL;
        BOOST_FOREACH (SRecord &srec, srecs) {
            if (srec.syntax() != expected)
                srec.error("wrong syntax family for S-Record collection");
        }
    }

    return srecs;
}

SRecord::Syntax
SRecord::syntax() const {
    switch (type_) {
        case SREC_M_HEADER:
        case SREC_M_DATA16:
        case SREC_M_DATA24:
        case SREC_M_DATA32:
        case SREC_M_RESERVED:
        case SREC_M_COUNT16:
        case SREC_M_COUNT24:
        case SREC_M_START32:
        case SREC_M_START24:
        case SREC_M_START16:
            return SREC_MOTOROLA;

        case SREC_I_DATA:
        case SREC_I_END:
        case SREC_I_EXTENDED_SA:
        case SREC_I_START_SA:
        case SREC_I_EXTENDED_LA:
        case SREC_I_START_LA:
            return SREC_INTEL;

        case SREC_UNKNOWN_TYPE:
            return SREC_UNKNOWN_SYNTAX;
    }
    ASSERT_not_reachable("invalid S-Record type");
}

void
SRecord::RunLengthEncoding::insert(const SRecord &srec) {
    switch (srec.type()) {
        case SREC_M_START16:
        case SREC_M_START24:
        case SREC_M_START32:
            executionVa_ = srec.address();
            break;
        case SREC_I_EXTENDED_SA:
            dataVa_ = bigEndian(srec.data(), 2) << 4;
            break;
        case SREC_I_START_SA:
            executionVa_ = bigEndian(srec.data(), 2) << 4;
            break;
        case SREC_I_EXTENDED_LA:
            dataVa_ = bigEndian(srec.data(), 2) << 16;
            break;
        case SREC_I_START_LA:
            executionVa_ = bigEndian(srec.data(), 2) << 16;
            break;
        case SREC_M_DATA16:
        case SREC_M_DATA24:
        case SREC_M_DATA32:
        case SREC_I_DATA:
            ++nDataRecords_;
            break;
        default:
            break;
    }
}

Sawyer::Optional<rose_addr_t>
SRecord::RunLengthEncoding::executionAddress() const {
    return executionVa_;
}

rose_addr_t
SRecord::RunLengthEncoding::baseDataAddress() const {
    return dataVa_.orElse(0);
}

Sawyer::Optional<rose_addr_t>
SRecord::RunLengthEncoding::dataAddress(const SRecord &srec) const {
    switch (srec.type()) {
        case SREC_M_DATA16:
        case SREC_M_DATA24:
        case SREC_M_DATA32:
            return srec.address();
        case SREC_I_DATA:
            return baseDataAddress() + srec.address();
        default:
            return Sawyer::Nothing();                   // not a data record
    }
}

// class method
AddressIntervalSet
SRecord::dataAddresses(const std::vector<SRecord> &srecs) {
    AddressIntervalSet retval;
    RunLengthEncoding rle;
    BOOST_FOREACH (const SRecord &srec, srecs) {
        rose_addr_t va = 0;
        if (rle.dataAddress(srec).assignTo(va))
            retval.insert(AddressInterval::baseSize(va, srec.data().size()));
        rle.insert(srec);
    }
    return retval;
}

// class method
Sawyer::Optional<rose_addr_t>
SRecord::load(const std::vector<SRecord> &srecs, const MemoryMap::Ptr &map, bool createSegments, unsigned accessPerms,
              const std::string &newSegmentNames)
{
    if (createSegments) {
        // We want to minimize the number of buffers in the map, so the first step is to discover what addresses are covered by
        // the data S-records
        AddressIntervalSet addresses = dataAddresses(srecs);
        BOOST_FOREACH (const AddressInterval &interval, addresses.intervals()) {
            ASSERT_forbid(interval.isWhole());          // not practically possible since S-Record file would be >2^65 bytes
            map->insert(interval, MemoryMap::Segment::anonymousInstance(interval.size(), accessPerms, newSegmentNames));
        }
    }

    // Populate the map by writing the S-Record data into it.
    RunLengthEncoding rle;
    BOOST_FOREACH (const SRecord &srec, srecs) {
        rose_addr_t va = 0;
        if (rle.dataAddress(srec).assignTo(va)) {
            size_t nWritten = map->at(va).write(srec.data()).size();
            if (nWritten != srec.data().size()) {
                throw MemoryMap::NotMapped("S-Record destination is not mapped for " +
                                           StringUtility::plural(srec.data().size(), "bytes"),
                                           map, va);
            }
        }
        rle.insert(srec);
    }

    return rle.executionAddress();
}

// class method
std::vector<SRecord>
SRecord::create(const MemoryMap::Ptr &map, Syntax syntax, size_t bytesPerRecord, size_t preferredAddrSize) {
    std::vector<SRecord> retval;
    uint8_t buffer[255];
    ASSERT_require(bytesPerRecord > 0);
    ASSERT_require(bytesPerRecord <= sizeof buffer);
    Type dataRecordType = SREC_UNKNOWN_TYPE;
    switch (syntax) {
        case SREC_MOTOROLA: {
            if (preferredAddrSize <= 2) {
                preferredAddrSize = 2;
                dataRecordType = SREC_M_DATA16;
            } else if (preferredAddrSize <= 3) {
                preferredAddrSize = 3;
                dataRecordType = SREC_M_DATA24;
            } else {
                preferredAddrSize = 4;
                dataRecordType = SREC_M_DATA32;
            }
            size_t maxBytesPerRecord = 255 - addressNBytes(dataRecordType);
            bytesPerRecord = std::min(bytesPerRecord, maxBytesPerRecord);
            break;
        }
        case SREC_INTEL: {
            preferredAddrSize = 2;
            dataRecordType = SREC_I_DATA;
            bytesPerRecord = std::min(bytesPerRecord, size_t(255));
            break;
        }
        default:
            ASSERT_not_reachable("invalid syntax family");
    }

    rose_addr_t va = 0;
    RunLengthEncoding rle;
    while (map->atOrAfter(va).next().assignTo(va)) {
        size_t nRead = map->at(va).limit(bytesPerRecord).read(buffer).size();
        ASSERT_require(nRead > 0);

        switch (syntax) {
            case SREC_MOTOROLA:
                retval.push_back(SRecord(dataRecordType, va, buffer, nRead));
                rle.insert(retval.back());
                break;

            case SREC_INTEL: {
                ASSERT_require(va >= rle.baseDataAddress()); // because va is increasing
                rose_addr_t offset = va - rle.baseDataAddress();
                if (0 == rle.nDataRecords()) {
                    static uint8_t zeros[2];
                    retval.push_back(SRecord(SREC_I_EXTENDED_LA, 0, zeros, 2)); // good form to initialize the linear addresses
                    rle.insert(retval.back());
                } else if (offset > 0xffff) {
                    uint8_t linear[2];
                    linear[0] = (va >> 24) & 0xff;
                    linear[1] = (va >> 16) & 0xff;
                    retval.push_back(SRecord(SREC_I_EXTENDED_LA, 0, linear, 2));
                    rle.insert(retval.back());
                    offset = va & 0xffff;
                }
                retval.push_back(SRecord(dataRecordType, offset, buffer, nRead));
                rle.insert(retval.back());
                break;
            }

            default:
                ASSERT_not_reachable("invalid syntax family");
        }

        // Increment va safely
        va += nRead - 1;
        if (va == map->hull().greatest())
            break;                                      // avoid possible overflow
        ++va;
    }

    return retval;
}

std::string
SRecord::toString() const {
    std::string s;
    size_t checksumOffset = 0;
    switch (syntax()) {
        case SREC_MOTOROLA: {
            switch (type_) {
                case SREC_M_HEADER:      s += "S0"; break;
                case SREC_M_DATA16:      s += "S1"; break;
                case SREC_M_DATA24:      s += "S2"; break;
                case SREC_M_DATA32:      s += "S3"; break;
                case SREC_M_RESERVED:    s += "S4"; break;
                case SREC_M_COUNT16:     s += "S5"; break;
                case SREC_M_COUNT24:     s += "S6"; break;
                case SREC_M_START32:     s += "S7"; break;
                case SREC_M_START24:     s += "S8"; break;
                case SREC_M_START16:     s += "S9"; break;
                default:
                    ASSERT_not_reachable("invalid Motorola S-Record type " + StringUtility::numberToString(type_));
            }

            size_t size = addressNBytes(type_) + data_.size() + 1 /*checksum*/;
            if (size > 255)
                throw Exception("S-Record size is too large");
            s += (boost::format("%|02X|") % size).str();

            rose_addr_t addrMask = IntegerOps::genMask<rose_addr_t>(8*addressNBytes(type_));
            if ((addr_ & ~addrMask) != 0) {
                throw Exception("S-Record address " + StringUtility::addrToString(addr_) +
                                " needs more than " + StringUtility::numberToString(addressNBytes(type_)) + " bytes");
            }

            std::string addrFmt = "%|0" + StringUtility::numberToString(2*addressNBytes(type_)) + "x|";
            s += (boost::format(addrFmt) % addr_).str();
            checksumOffset = 2;                         // skip 'S' and the type
            break;
        }

        case SREC_INTEL: {
            if (data_.size() > 255)
                throw Exception("S-Record size is too large");
            if (address() > 0xffff)
                throw Exception("S-Record address is too large");
            s = (boost::format(":%|02X|%|04X|") % data_.size() % (unsigned)address()).str();
            switch (type_) {
                case SREC_I_DATA:        s += "00"; break;
                case SREC_I_END:         s += "01"; break;
                case SREC_I_EXTENDED_SA: s += "02"; break;
                case SREC_I_START_SA:    s += "03"; break;
                case SREC_I_EXTENDED_LA: s += "04"; break;
                case SREC_I_START_LA:    s += "05"; break;
                default:
                    ASSERT_not_reachable("invalid Intel S-Record type " + StringUtility::numberToString(type_));
            }
            checksumOffset = 1;                         // skip only ':'
            break;
        }

        default:
            ASSERT_not_reachable("invalid syntax family");
    }

    // Append the data and checksum
    BOOST_FOREACH (uint8_t byte, data_)
        s += (boost::format("%|02X|") % (unsigned)byte).str();
    ASSERT_require(checksumOffset <= s.size());
    s += (boost::format("%|02X|") % (unsigned)checksum(syntax(), s, checksumOffset, s.size()-checksumOffset)).str();
    return s;
}

void
SRecord::print(std::ostream &out) const {
    out <<toString();
}

} // namespace
} // namespace
