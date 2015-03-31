#include <sage3basic.h>

#include <BinaryString.h>

namespace rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StringFinder::String
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
StringFinder::String::isValid() const {
    switch (lengthEncoding_) {
        case MAP_TERMINATED:
            return true;

        case NUL_TERMINATED:
        case SEQUENCE_TERMINATED:
        case BYTE_LENGTH:
            return nBytes_ >= 1;

        case LE16_LENGTH:
        case BE16_LENGTH:
            return nBytes_ >= 2;

        case LE32_LENGTH:
        case BE32_LENGTH:
            return nBytes_ >= 4;
    }
    ASSERT_not_reachable("unexpected length encoding");
}

bool
StringFinder::String::isRunLengthEncoded() const {
    switch (lengthEncoding_) {
        case MAP_TERMINATED:
        case NUL_TERMINATED:
        case SEQUENCE_TERMINATED:
            return true;
        case BYTE_LENGTH:
        case LE16_LENGTH:
        case BE16_LENGTH:
        case LE32_LENGTH:
        case BE32_LENGTH:
            switch (characterEncoding_) {
                case ASCII:
                    return false;
            }
            break;
    }
    ASSERT_not_reachable("unexpected encoding");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StringFinder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
StringFinder::isAsciiCharacter(uint8_t ch) const {
    return isgraph(ch) || isspace(ch);
}

struct AsciiSequenceLength {
    const StringFinder *self;
    size_t nBytes;
    AsciiSequenceLength(const StringFinder *self): self(self), nBytes(0) {}
    bool operator()(const MemoryMap::Super &map, const AddressInterval &interval) {
        rose_addr_t va = interval.least();
        while (va <= interval.greatest()) {
            uint8_t byte;
            map.at(va).limit(1).read(&byte);
            if (!self->isAsciiCharacter(byte))
                return false;
            ++nBytes;
            if (va == interval.greatest())
                return true;                            // prevent overflow
            ++va;
        }
        return true;
    }
};

// Given a memory location, find the length of the longest sequence of ASCII characters.
size_t
StringFinder::asciiSequenceLength(MemoryMap::ConstConstraints where) const {
    AsciiSequenceLength visitor(this);
    where.traverse(visitor, Sawyer::Container::MATCH_CONTIGUOUS);
    return visitor.nBytes;
}

struct AsciiSequenceLocation {
    const StringFinder *self;
    rose_addr_t startVa;
    size_t nChars, minChars;
    AsciiSequenceLocation(const StringFinder *self, size_t minChars): self(self), startVa(0), nChars(0), minChars(minChars) {}
    bool operator()(const MemoryMap::Super &map, const AddressInterval &interval) {
        rose_addr_t va = interval.least();
        if (startVa + nChars != va)
            nChars = 0;
        while (va <= interval.greatest()) {
            uint8_t byte;
            map.at(va).limit(1).read(&byte);
            if (self->isAsciiCharacter(byte)) {
                if (1 == ++nChars)
                    startVa = va;
                if (nChars >= minChars)
                    return false;
            } else {
                nChars = 0;
            }
            if (va == interval.greatest())
                return true;                            // prevent overflow
            ++va;
        }
        return true;
    }
};

// Find the starting address for the next sequence of ASCII characters.
Sawyer::Optional<rose_addr_t>
StringFinder::findAsciiSequence(MemoryMap::ConstConstraints where, size_t minChars) const {
    AsciiSequenceLocation visitor(this, minChars);
    where.traverse(visitor);
    if (visitor.nChars >= minChars)
        return visitor.startVa;
    return Sawyer::Nothing();
}

// Find first string
Sawyer::Optional<StringFinder::String>
StringFinder::findString(MemoryMap::ConstConstraints where) const {
    static const size_t minChars = 5;
    rose_addr_t stringVa = 0;
    while (findAsciiSequence(where, minChars).assignTo(stringVa)) {
        uint8_t byte;
        size_t nBytes = minChars + asciiSequenceLength(where.at(stringVa+minChars));

        // Read the byte following the ASCII sequence if possible
        if (stringVa + nBytes < stringVa) {             // overflow
            return String(stringVa, nBytes, nBytes, MAP_TERMINATED, ASCII);
        } else if (!where.at(stringVa+nBytes).limit(1).read(&byte)) {
            return String(stringVa, nBytes, nBytes, MAP_TERMINATED, ASCII);
        }

        // Is the following byte a NUL character?
        if (0 == byte)
            return String(stringVa, nBytes+1, nBytes, NUL_TERMINATED, ASCII);

        // FIXME[Robb P. Matzke 2015-01-17]: Other encoding methods are defined but not implemented yet.

        // Catch-all for ASCII sequences that we found but which don't seem to have a length or NUL termination.
        return String(stringVa, nBytes, nBytes, SEQUENCE_TERMINATED, ASCII);
    }
    return Sawyer::Nothing();
}

// Find all strings
StringFinder::Strings
StringFinder::findAllStrings(MemoryMap::ConstConstraints where) const {
    Strings retval;
    while (Sawyer::Optional<String> string = findString(where)) {
        retval.insert(string->address(), *string);
        where = where.atOrAfter(string->address() + string->nBytes());
    }
    return retval;
}

// Read a string from memory
std::string
StringFinder::decode(const MemoryMap &map, const String &string) const {
    ASSERT_require(string.isValid());

    struct Resources {
        uint8_t *buffer;
        Resources(): buffer(NULL) {}
        ~Resources() { delete buffer; }
    } r;

    // Read the data for the string
    r.buffer = new uint8_t[string.nBytes()];
    size_t nRead = map.at(string.address()).limit(string.nBytes()).read(r.buffer).size();
    if (nRead < string.nBytes()) {
        throw MemoryMap::NotMapped("short read for " + StringUtility::numberToString(string.nBytes()) + "-byte string at " + 
                                   StringUtility::addrToString(string.address()),
                                   &map, string.address() + nRead);
    }

    // Decode the string length
    uint8_t *data = r.buffer;
    size_t dataSize = string.nBytes();
    ASSERT_require(string.isValid());                   // checks string length for encoding
    switch (string.lengthEncoding()) {
        case MAP_TERMINATED:
        case SEQUENCE_TERMINATED:
            break;
        case NUL_TERMINATED:
            --dataSize;
            break;
        case BYTE_LENGTH: {
            size_t n = *data++;
            --dataSize;
            ASSERT_require2(n == dataSize, "mismatched lengths in byte-length encoded string");
            break;
        }
        case LE16_LENGTH: {
            size_t n = ByteOrder::le_to_host(*(uint16_t*)data);
            data += 2;
            dataSize -= 2;
            ASSERT_require2(n == dataSize, "mismatched lengths in le16-length encoded string");
            break;
        }
        case BE16_LENGTH: {
            size_t n = ByteOrder::be_to_host(*(uint16_t*)data);
            data += 2;
            dataSize -= 2;
            ASSERT_require2(n == dataSize, "mismatched lengths in be16-length encoded string");
            break;
        }
        case LE32_LENGTH: {
            size_t n = ByteOrder::le_to_host(*(uint32_t*)data);
            data += 4;
            dataSize -= 4;
            ASSERT_require2(n == dataSize, "mismatched lengths in le32-length encoded string");
            break;
        }
        case BE32_LENGTH: {
            size_t n = ByteOrder::be_to_host(*(uint32_t*)data);
            data += 4;
            dataSize -= 4;
            ASSERT_require2(n == dataSize, "mismatched lengths in be32-length encoded string");
            break;
        }
    }

    // Decode the string
    std::string s;
    switch (string.characterEncoding()) {
        case ASCII:
            s = std::string((const char*)data, dataSize);
            break;
    }

    return s;
}

} // namespace
} // namespace
