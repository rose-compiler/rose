#ifndef ROSE_BinaryAnalysis_String_H
#define ROSE_BinaryAnalysis_String_H

#include <MemoryMap.h>
#include <sawyer/Optional.h>

namespace rose {
namespace BinaryAnalysis {

/** Analysis for finding strings in memory.
 *
 *  This analysis looks for various kinds of strings in specimen memory.  A string is a sequence of characters encoded in one
 *  of a variety of ways in memory.  For instance, NUL-terminated ASCII is a common encoding from C compilers.  The characters
 *  within the string must all satisfy some valid-character predicate. */
class StringFinder {
public:
    /** How string length is represented. */
    enum LengthEncoding {
        MAP_TERMINATED,                                 /**< String is terminated by a hole in memory. */
        NUL_TERMINATED,                                 /**< Terminated by an ASCII NUL character. */
        SEQUENCE_TERMINATED,                            /**< Terminated by an invalid character. */
        BYTE_LENGTH,                                    /**< String is preceded by a length byte. */
        LE16_LENGTH,                                    /**< String is preceded by a little-endian 16-bit length. */
        LE32_LENGTH,                                    /**< String is preceded by a little-endian 32-bit length. */
        BE16_LENGTH,                                    /**< String is preceded by a big-endian 16-bit length. */
        BE32_LENGTH,                                    /**< String is preceded by a big-endian 32-bit length. */
    };

    /** How string characters are represented. */
    enum CharacterEncoding {
        ASCII,                                          /**< One byte per character. */
    };

    /** A string of characters.  This type holds all the information that's necessary to decode a string from some interval of
     *  memory. */
    class String {
        rose_addr_t va_;                                // Starting address
        size_t nBytes_;                                 // Number of bytes to encode the string
        size_t nCharacters_;                            // Length of string; excludes size bytes and termination
        LengthEncoding lengthEncoding_;                 // How string length is indicated
        CharacterEncoding characterEncoding_;           // How characters are encoded

    public:
        /** Construct an invalid string. An invalid string is a zero-length string for which no address has been specified.
         *  Internally, the encoding is set to BYTE_LENGTH but the number of bytes is zero, which is an impossible valid
         *  combination. This constructor is mostly present for those STL containers that need a default constructor. */
        String()
            : va_(0), nBytes_(0), nCharacters_(0), lengthEncoding_(BYTE_LENGTH), characterEncoding_(ASCII) {}

        /** Construct a string with specified parameters. The @p nCharacters represents the number of actual characters in the
         *  string, not counting termination characters. */
        String(rose_addr_t va, size_t nBytes, size_t nCharacters,
               LengthEncoding lengthEncoding, CharacterEncoding characterEncoding)
            : va_(va), nBytes_(nBytes), nCharacters_(nCharacters),
              lengthEncoding_(lengthEncoding), characterEncoding_(characterEncoding) {}

        /** Construct a NUL-terminated ASCII string.  The @p nCharactes does not include the terminating NUL character. In
         *  other words, its the same as the value returned if the string had been passed to the C @c strlen function. */
        static String createCString(rose_addr_t va, size_t nCharacters) {
            return String(va, nCharacters+1, nCharacters, NUL_TERMINATED, ASCII);
        }

        /** Check whether the string is valid.  In particular, a default-constructed string is invalid. */
        bool isValid() const;

        /** Check whether the string is empty. */
        bool isEmpty() const { return 0 == nCharacters_; }

        /** Length encoding.  Returns information about how the length of the string is encoded in memory. */
        LengthEncoding lengthEncoding() const { return lengthEncoding_; }

        /** Character encoding. Returns information about how the characters of the string are encoded in memory. */
        CharacterEncoding characterEncoding() const { return characterEncoding_; }

        /** Starting address. */
        rose_addr_t address() const { return va_; }

        /** Number of characters. The number of characters in a string never includes any length bytes or termination
         *  characters--only the actual data of the string.  The number of characters will usually be different than the number
         *  of bytes required to store the string in memory since most strings will need at least one extra byte to represent
         *  the string length and perhaps multiple bytes per character, or even less than one byte per character depending on
         *  the encoding.
         *
         *  @sa nBytes */
        size_t nCharacters() const { return nCharacters_; }

        /** Number of bytes used to represent the string in its encoded form.  The number of bytes includes all bytes used to
         *  encode the string: those use to represent length, and those used to represent the characters.
         *
         *  @sa nCharacters */
        size_t nBytes() const { return nBytes_; }

        /** Check whether string is run-length encoded.  A run-length encoded string is a string whose number of characters can
         *  only be determined by reading the encoded string data.  For instance, NUL-terminated strings are run-length encoded
         *  because one must read the bytes of the string in order to find the first NUL character.  Compressed strings are
         *  also run-length-encoded since one must decompress the encoded bytes to get the characters. */
        bool isRunLengthEncoded() const;
    };

public:
    /** Map of all strings by starting address. */
    typedef Sawyer::Container::Map<rose_addr_t, String> Strings;

public:
    /** Predicate determining valid ASCII character.
     *
     *  This predicate returns true if the specified byte is considered to be a valid character for an ASCII string.  The base
     *  implementation returns true for white space or characters having a graphic. */
    virtual bool isAsciiCharacter(uint8_t) const;
    
    /** Find the starting address for the next sequence of ASCII characters.  If a sequence of at least @p minChars characters
     *  can be found in the memory map then return the address for the starting character, otherwise return nothing.
     *
     *  The following example shows how to find a string of at least five characters in readable memory and starting the search
     *  at a particular address.
     *
     * @code
     *  StringFinder stringFinder = ...; // the string-finding analyzer
     *  const MemoryMap &map = ...;      // the memory to be searched
     *  rose_addr_t va = ...;            // where to start searching for ASCII strings
     *  size_t minChars = 5;             // minimum number of characters to find
     *  if (stringFinder.findAsciiSequence(map.require(READABLE).atOrAfter(va), minChars).assignTo(va))
     *      std::cout <<"found an ASCII string starting at " <<StringUtility::addrToString(va) <<"\n";
     * @endcode
     *
     * @{ */
    Sawyer::Optional<rose_addr_t> findAsciiSequence(MemoryMap::ConstConstraints where, size_t minChars) const;
    /** @} */

    /** Length of longest sequence of ASCII characters.  Returns the length of the longest sequence of ASCII characters
     *  beginning at the specified address. Here's an example sans error checking:
     *
     * @code
     *  MemoryMap map = ...;
     *  rose_addr_t stringVa = stringFinder.findAsciiSequence(map.atOrAfter(startVa), 5).orElse(-1);
     *  size_t nChars = stringFinder.asciiSequenceLength(map.at(stringVa));
     * @endcode
     *
     * @{ */
    size_t asciiSequenceLength(MemoryMap::ConstConstraints where) const;
    /** @} */

    /** Find the first string in the specified memory.
     *
     *  Scans the constrained memory locations to find the first string and returns it.  If no string can be found then nothing
     *  is returned.
     *
     * @code
     *  if (Sawyer::Optional<String> s = stringFinder.findString(map.atOrAfter(startVa).require(READABLE)))
     *      std::cout <<"Found a string: " <<*s <<"\n";
     * @endcode */
    Sawyer::Optional<String> findString(MemoryMap::ConstConstraints where) const;
    /** @} */

    /** Find all strings in memory. */
    Strings findAllStrings(MemoryMap::ConstConstraints where) const;

    /** Read a string from memory. */
    std::string decode(const MemoryMap&, const String&) const;
};

} // namespace
} // namespace

#endif
