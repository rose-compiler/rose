#ifndef ROSE_StringUtility_StringToNumber_H
#define ROSE_StringUtility_StringToNumber_H

#include <Rose/StringUtility/NumberToString.h>
#include <rosedll.h>

#include <string>
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
 *  std::vector<rose_addr_t> addresses = ...;
 *  std::string s = join(" ", toStrings(addresses, addrToString));
 * @endcode
 *
 *  Here's how one could surround each address with angle brackets:
 * @code
 *  using namespace Rose::StringUtility;
 *  struct AngleSurround {
 *      std::string operator()(rose_addr_t addr) {
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

} // namespace
} // namespace

#endif
