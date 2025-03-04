#ifndef ROSE_StringUtility_StringToNumber_H
#define ROSE_StringUtility_StringToNumber_H

#include <Rose/StringUtility/NumberToString.h>

#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/Exception.h>
#include <rosedll.h>

#include <Sawyer/Parse.h>

#include <vector>

namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Number parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Convert an ASCII hexadecimal character to an integer.
 *
 *  Converts the characters 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, b, c, d, e, f, A, B, C, D, E, and F into their hexadecimal integer
 *  equivalents. Returns zero if the input character is not in this set. */
ROSE_UTIL_API unsigned hexadecimalToInt(char);

/** Converts a bunch of numbers to strings.
 *
 *  This is convenient when one has a container of numbers and wants to call @ref join to turn it into a single string.  For
 *  instance, here's how to convert a set of integers to a comma-separated list:
 *
 * @code
 *  using namespace Rose::StringUtility;
 *  std::set<int> numbers = ...;
 *  std::string s = join(", ", toStrings(numbers));
 * @endcode
 *
 *  Here's how to convert a vector of addresses to space-separated hexadecimal values:
 * @code
 *  using namespace Rose::StringUtility;
 *  std::vector<Address> addresses = ...;
 *  std::string s = join(" ", toStrings(addresses, addrToString));
 * @endcode
 *
 *  Here's how one could surround each address with angle brackets:
 * @code
 *  using namespace Rose::StringUtility;
 *  struct AngleSurround {
 *      std::string operator()(Address addr) {
 *         return "<" + addrToString(addr) + ">";
 *      }
 *  };
 *  std::string s = join(" ", toStrings(addresses, AngleSurround()));
 * @endcode
 * @{ */
template<class Container, class Stringifier>
std::vector<std::string> toStrings(const Container &numbers, const Stringifier &stringifier=numberToString) {
    return toStrings_range(numbers.begin(), numbers.end(), stringifier);
}
template<class Iterator, class Stringifier>
std::vector<std::string> toStrings_range(Iterator begin, Iterator end, const Stringifier &stringifier=numberToString) {
    std::vector<std::string> retval;
    for (/*void*/; begin!=end; ++begin)
        retval.push_back(stringifier(*begin));
    return retval;
}
/** @} */

/** Convert a character to a numeric digit.
 *
 *  If the character is valid for the specified radix, then its numeric value is returned, otherwise
 *  nothing is returned.  The radix must be less than or equal to 16. */
template<class IntegralType>
Sawyer::Optional<typename std::enable_if<std::is_integral<IntegralType>::value, IntegralType>::type>
toDigit(char ch, IntegralType radix = 10) {
    assert(radix <= 16);
    assert(!std::numeric_limits<IntegralType>::is_signed || radix >= 0); // trait test is to avoid compiler warning
    IntegralType digit;
    if (ch >= '0' && ch <= '9') {
        digit = ch - '0';
    } else if (ch >= 'a' && ch <= 'f') {
        digit = ch - 'a' + 10;
    } else if (ch >= 'A' && ch <= 'F') {
        digit = ch - 'A' + 10;
    } else {
        return Sawyer::Nothing();
    }

    if (digit < radix)
        return digit;
    return Sawyer::Nothing();
}

/** Safely convert a string to a number using C++ style syntax.
 *
 *  The input string must parse as a integer without leading white space and without trailing characters. For signed integer
 *  types, the integer may be immediately preceded by a minus ("-") or plus ("+") sign. The integer part may be preceded by a
 *  radix indicator: "0x" for hexadecimal or "0b" for binary. Octal and other radices are not supported. Digits may be
 *  separated from one another (or from the radix specifier) by a single underscore, as in
 *  "0b_0001_0010_0011_0100_0101_0110_0111_1000".
 *
 *  The @p IntegralType must be a signed or unsigned integer type of any width. The @c uint8_t and @c int8_t types are also accepted
 *  although they're aliases for `unsigned char` and `signed char`.
 *
 *  If a syntax error occurs, or if the magnitude of the value is too large to be represented by the specified @p IntegralType,
 *  then an error message is returned. The text of the message begins with either "syntax error:" or "overflow error:". */
template<class IntegralType>
typename std::enable_if<std::is_integral<IntegralType>::value, Sawyer::Result<IntegralType, std::string>>::type
toNumber(const std::string &s) {
    return Sawyer::parse<IntegralType>(s);
}

/** Safely convert a string to a number using C++ style syntax.
 *
 *  This function is identical to @ref toNumber except in the return type and error situation. If an error occurs, then a
 *  `Rose::Exception` is thrown whose message is the error message. On success, the parsed value is returned. */
template<class IntegralType>
typename std::enable_if<std::is_integral<IntegralType>::value, IntegralType>::type
toNumberOrThrow(const std::string &s) {
    return toNumber<IntegralType>(s).template orThrow<Exception>();
}

} // namespace
} // namespace

#endif
