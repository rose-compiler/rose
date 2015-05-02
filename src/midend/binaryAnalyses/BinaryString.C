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
                case UTF8:
                case UTF16:
                case UTF32:
                    return false;
            }
            break;
    }
    ASSERT_not_reachable("unexpected encoding");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StringFinder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static size_t
bytesPerChar(StringFinder::CharacterEncoding e) {
    switch (e) {
        case StringFinder::UTF8: return 1;
        case StringFinder::UTF16: return 2;
        case StringFinder::UTF32: return 4;
    }
    ASSERT_not_implemented("invalid character encoding");
}

bool
StringFinder::isAsciiCharacter(const std::vector<uint8_t> &character) const {
    ASSERT_forbid2(character.empty(), "character encoding must be at least one byte");

    // FIXME[Robb P. Matzke 2015-05-01]: Assuming little-endian
    for (size_t i=1; i<character.size(); ++i) {
        if (character[i]!=0)
            return false;
    }

    uint8_t ch = character[0];
    return isgraph(ch) || isspace(ch);
}

struct AsciiSequenceLength {
    const StringFinder *self;
    size_t nChars, bytesPerChar;

    AsciiSequenceLength(const StringFinder *self, size_t bytesPerChar): self(self), nChars(0), bytesPerChar(bytesPerChar) {}

    bool operator()(const MemoryMap::Super &map, const AddressInterval &interval) {
        rose_addr_t va = interval.least();
        while (va <= interval.greatest()) {
            std::vector<uint8_t> bytes(bytesPerChar);
            if (bytesPerChar != map.at(va).read(bytes).size() || !self->isAsciiCharacter(bytes))
                return false;
            ++nChars;
            if (va+(bytesPerChar-1) == interval.greatest())
                return true;                            // prevent overflow
            va += bytesPerChar;
        }
        return true;
    }
};

// Given a memory location, find the length of the longest sequence of ASCII characters.
size_t
StringFinder::asciiSequenceLength(MemoryMap::ConstConstraints where, CharacterEncoding characterEncoding) const {
    AsciiSequenceLength visitor(this, bytesPerChar(characterEncoding));
    where.traverse(visitor, Sawyer::Container::MATCH_CONTIGUOUS);
    return visitor.nChars;
}

struct AsciiSequenceLocation {
    const StringFinder *self;
    rose_addr_t startVa;
    size_t nChars, minChars, bytesPerChar;

    AsciiSequenceLocation(const StringFinder *self, size_t minChars, size_t bytesPerChar)
        : self(self), startVa(0), nChars(0), minChars(minChars), bytesPerChar(bytesPerChar) {}

    bool operator()(const MemoryMap::Super &map, const AddressInterval &interval) {
        rose_addr_t va = interval.least();
        if (startVa + nChars*bytesPerChar != va)
            nChars = 0;
        while (va <= interval.greatest()) {
            std::vector<uint8_t> bytes(bytesPerChar);
            if (bytesPerChar == map.at(va).read(bytes).size() && self->isAsciiCharacter(bytes)) {
                if (1 == ++nChars)
                    startVa = va;
                if (nChars >= minChars)
                    return false;
            } else {
                nChars = 0;
            }
            if (va+(bytesPerChar-1) == interval.greatest())
                return true;                            // prevent overflow
            va += bytesPerChar;;
        }
        return true;
    }
};

// Find the starting address for the next sequence of ASCII characters.
Sawyer::Optional<rose_addr_t>
StringFinder::findAsciiSequence(MemoryMap::ConstConstraints where, size_t minChars, CharacterEncoding characterEncoding) const {
    AsciiSequenceLocation visitor(this, minChars, bytesPerChar(characterEncoding));
    where.traverse(visitor);
    if (visitor.nChars >= minChars)
        return visitor.startVa;
    return Sawyer::Nothing();
}

// Find first string
Sawyer::Optional<StringFinder::String>
StringFinder::findString(MemoryMap::ConstConstraints where) const {
    static const size_t minChars = 5;                   // arbitrary
    static const CharacterEncoding charEncodings[] = {UTF32, UTF16, UTF8};
    static const size_t nTries = sizeof(charEncodings)/sizeof(charEncodings[0]);
    static const rose_addr_t NO_ADDRESS(-1);

    CharacterEncoding charEncoding;
    rose_addr_t stringVa = NO_ADDRESS;
    for (size_t i=0; i<nTries; ++i) {
        rose_addr_t x = findAsciiSequence(where, minChars, charEncodings[i]).orElse(NO_ADDRESS);
        if (x < stringVa) {
            stringVa = x;
            charEncoding = charEncodings[i];
        }
    }
    if (stringVa == NO_ADDRESS)
        return Sawyer::Nothing();

    if (where.isAnchored() && stringVa != where.anchored().least())
        return Sawyer::Nothing();

    size_t nChars = asciiSequenceLength(where.at(stringVa), charEncoding);
    size_t bpc = bytesPerChar(charEncoding);
    size_t nBytes = nChars * bpc;
            
    // Read the (multi-byte) character following the sequence if possible
    if (stringVa + nBytes < stringVa) // overflow
        return String(stringVa, nBytes, nChars, MAP_TERMINATED, charEncoding);
    std::vector<uint8_t> charBytes(bpc);
    if (bpc != where.at(stringVa+nBytes).read(charBytes).size())
        return String(stringVa, nBytes, nChars, MAP_TERMINATED, charEncoding);

    // Is the following character a NUL character?
    bool isNul = true;
    for (size_t i=0; i<bpc && isNul; ++i)
        isNul = charBytes[i] == 0;
    if (isNul)
        return String(stringVa, nBytes+bpc, nChars, NUL_TERMINATED, charEncoding);

    // FIXME[Robb P. Matzke 2015-01-17]: Other encoding methods are defined but not implemented yet.

    // Catch-all for ASCII sequences that we found but which don't seem to have a length or NUL termination.
    return String(stringVa, nBytes, nChars, SEQUENCE_TERMINATED, charEncoding);
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
            ASSERT_require(dataSize >= bytesPerChar(string.characterEncoding()));
            dataSize -= bytesPerChar(string.characterEncoding());
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
        case UTF8:
            s = std::string((const char*)data, dataSize);
            break;
        case UTF16:
            // Store only the low-order bytes since ROSE is not supporting UTF encodings yet [Robb P. Matzke 2015-05-01]
            ASSERT_require(dataSize % 2 == 0);
            for (size_t i=0; i<dataSize; i+=2)
                s += (char)data[i];
            break;
        case UTF32:
            // Store only the low-order bytes since ROSE is not supporting UTF encodings yet [Robb P. Matzke 2015-05-01]
            ASSERT_require(dataSize % 4 == 0);
            for (size_t i=0; i<dataSize; i+=4)
                s += (char)data[i];
            break;
    }

    return s;
}

} // namespace
} // namespace
