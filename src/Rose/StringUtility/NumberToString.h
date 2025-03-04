#ifndef ROSE_StringUtility_NumberToString_H
#define ROSE_StringUtility_NumberToString_H

#include <Rose/BitOps.h>
#include <rosedll.h>

#include <Sawyer/IntervalSet.h>
#include <string>

namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for converting numbers to strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Convert an integer to a string.
 *
 *  These functions are wrappers around `boost::lexical_cast<std::string>`.
 *
 *  @{ */
ROSE_UTIL_API std::string numberToString(long long);
ROSE_UTIL_API std::string numberToString(unsigned long long);
ROSE_UTIL_API std::string numberToString(long);
ROSE_UTIL_API std::string numberToString(unsigned long);
ROSE_UTIL_API std::string numberToString(int);
ROSE_UTIL_API std::string numberToString(unsigned int);

#if !defined(_MSC_VER) &&                                                                                                      \
    defined(BACKEND_CXX_IS_GNU_COMPILER) &&                                                                                    \
    ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER > 6) ||                      \
     BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4) &&                                                                         \
    __WORDSIZE == 64
ROSE_UTIL_API std::string numberToString(__int128 x);
ROSE_UTIL_API std::string numberToString(unsigned __int128 x);
#endif

/** @} */

/** Convert a pointer to a string. */
ROSE_UTIL_API std::string numberToString(const void*);

/** Convert a floating-point number to a string.
 *
 *  The returned string uses @c printf with "%2.2f" format. */
ROSE_UTIL_API std::string numberToString(double);

/** Convert an integer to a hexadecimal string. */
ROSE_UTIL_API std::string intToHex(uint64_t);

/** Convert a number to a hexadecimal and decimal string.
 *
 *  The returned string starts with the hexadecimal representation of the number and an optional decimal representation
 *  in angle brackets.  The decimal portion will contain a signed and/or unsigned value depending on whether the value
 *  is interpretted as signed and whether the sign bit is set.  The signedToHex versions print the decimal value for only
 *  the signed interpretation; the unsignedToHex versions print only the decimal unsigned interpretation, and the toHex
 *  versions print both (but not redunantly).
 *
 *  @{ */
ROSE_UTIL_API std::string toHex2(uint64_t value, size_t nbits,
                                 bool show_unsigned_decimal=true, bool show_signed_decimal=true,
                                 uint64_t decimal_threshold=256);
ROSE_UTIL_API std::string signedToHex2(uint64_t value, size_t nbits);
ROSE_UTIL_API std::string unsignedToHex2(uint64_t value, size_t nbits);

template<typename T>
std::string toHex(T value) {
    return toHex2((uint64_t)value, 8*sizeof(T));
}

template<typename T>
std::string signedToHex(T value) {
    return signedToHex2((uint64_t)value, 8*sizeof(T));
}

template<typename T>
std::string unsignedToHex(T value) {
    return unsignedToHex2((uint64_t)value, 8*sizeof(T));
}
/** @} */

/** Convert a virtual address to a string.
 *
 *  Converts a virtual address to a hexadecimal string with a leading "0x". The string is zero-padded so that it explicitly
 *  represents at least @p nbits bits (four bits per hexadecimal digits). If @p nbits is zero then the function uses 32 bits
 *  for values that fit in 32 bits, otherwise 64 bits. */
ROSE_UTIL_API std::string addrToString(uint64_t value, size_t nbits = 0);

/** Convert an interval of virtual addresses to a string.
 *
 *  Converts an interval to a string by converting each address to a string, separating them with a comma, and enclosing the
 *  whole string in square brackets. */
ROSE_UTIL_API std::string addrToString(const Sawyer::Container::Interval<uint64_t> &interval, size_t nbits = 0);

/** Convert an interval set of virtual addresses to a string.
 *
 *  Converts the interval-set to a string by converting each interval to a string, separating the intervals with commas, and
 *  enclosing the whole string in curly braces. */
ROSE_UTIL_API std::string addrToString(const Sawyer::Container::IntervalSet<Sawyer::Container::Interval<uint64_t> > &iset,
                                       size_t nbits = 0);

/** Convert an optional virtual address to a string.
 *
 *  Same as the non-optional version of this function, but prints the word "none" if the argument is @ref Sawyer::Nothing. */
ROSE_UTIL_API std::string addrToString(const Sawyer::Optional<uint64_t>&, size_t nbits = 0);

/** Convert a number to a binary string. */
template<typename Unsigned>
std::string toBinary(Unsigned value, size_t nBits = 0, size_t groupSize = 4, const std::string groupSeparator = "_") {
    if (0 == nBits)
        nBits = BitOps::nBits(value);
    std::string retval;
    for (size_t i = nBits; i > 0; --i) {
        retval += BitOps::bit(value, i-1) ? '1' : '0';
        if (groupSize > 0 && i > 1 && (i-1) % groupSize == 0)
            retval += groupSeparator;
    }
    return retval;
}

} // namespace
} // namespace

#endif
