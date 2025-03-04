#ifndef ROSE_StringUtility_SplitJoin_H
#define ROSE_StringUtility_SplitJoin_H

#include <Rose/Constants.h>
#include <rosedll.h>

#include <list>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Splitting and joining strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Splits strings into parts.
 *
 *  Find all non-overlapping occurrences the specified @p separator string by greedily scanning from left to right in the input
 *  string, @p str.  The input string is then logically chopped into parts at each separator position and the parts are
 *  assembled into the return value. Only up to @p maxparts-1 occurrences of the @p separator string are found, and any
 *  remaining occurrences are not treated specially.  For instance, if @p maxparts is two then at most one separator is found
 *  and at most two substrings are returned. Separators at positions that would result in empty substrings being returned are
 *  not treated specially--empty substrings can be returned. This occurs when a separator is found at the beginning or end of a
 *  string or two separators are adjacent. The C++ library already has other functions for removing empty strings from a list.
 *  If @p trim_white_space is true then white space is removed from the beginning and end of each returned substring and
 *  resulting empty substrings are not removed from the return value. The first few arguments are in the same order as for
 *  Perl's "split" operator.
 *
 * @{ */
ROSE_UTIL_API std::vector<std::string> split(const std::string &separator, const std::string &str, size_t maxparts = UNLIMITED,
                                             bool trim_white_space = false);
ROSE_UTIL_API std::vector<std::string> split(char separator, const std::string &str, size_t maxparts = UNLIMITED,
                                             bool trim_white_space = false);
/** @} */

/** Split a string into a list based on a separator character.
 *
 *  Scans the input string for delimiter characters and splits the input into substrings at each delimiter positions. The
 *  delimiter is not included in the substring. Consecutive delimiter characters will result in an empty substring. */
ROSE_UTIL_API std::list<std::string> tokenize(const std::string&, char delim);

/** Join individual items to form a single string.
 *
 *  Given a container containing printable objects (such as `std::list<std::string>`, join the objects together separated from one
 *  another by the specified @p separator. The separator does not appear before the first object or after the final object.  If the
 *  container is empty then an empty string is returned.
 *
 * @{ */
template<class Iterator>
std::string join_range(const std::string &separator, Iterator begin, Iterator end) {
    std::ostringstream retval;
    for (Iterator i=begin; i!=end; ++i)
        retval <<(i==begin ? std::string() : separator) <<*i;
    return retval.str();
}

template<class Container>
std::string join(const std::string &separator, const Container &container) {
    return join_range(separator, container.begin(), container.end());
}

template<class Container>
std::string join(char separator, const Container &container) {
    return join_range(std::string(1, separator), container.begin(), container.end());
}

ROSE_UTIL_API std::string join(const std::string &separator, char *strings[], size_t nstrings);
ROSE_UTIL_API std::string join(const std::string &separator, const char *strings[], size_t nstrings);
ROSE_UTIL_API std::string join(char separator, char *strings[], size_t nstrings);
ROSE_UTIL_API std::string join(char separator, const char *strings[], size_t nstrings);

/** @} */

/** Join strings as if they were English prose.
 *
 *  This is useful when generating documentation strings.
 *
 *  If the input is empty, the output is the empty string.
 *  I.e., `() => ""`
 *
 *  If the input is one phrase, the output is that phrase.
 *  E.g., `("foo") => "foo"`
 *
 *  If the input is two phrases, the output will be those two phrases separated by "and" (the @p finalIntro).
 *  E.g., `("foo", "bar") => "foo and bar"`
 *
 *  If the input is three or more phrases, they will be separated from one another by commas (the @p separator) and the last
 *  item will also be introduced with "and" (the @p finalIntro).
 *  E.g., `("foo", "bar", "baz") => "foo, bar, and baz"`
 *
 *  No transformations are performed on the input phrases. Space characters are inserted after each @p separator and @p
 *  finalIntro. A space is also inserted before the @p finalIntro when the input is two phrases.
 *
 * @{ */
ROSE_UTIL_API std::string joinEnglish(const std::vector<std::string> &phrases,
                                      const std::string &separator = ",",
                                      const std::string &finalIntro = "and");
ROSE_UTIL_API std::string joinEnglish(const std::set<std::string> &phrases,
                                      const std::string &separator = ",",
                                      const std::string &finalIntro = "and");
/** @} */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Container versus scalar functions
//
// Functions that convert containers of things to a string and vice versa.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Generate a string from a list of integers.
 *
 *  The return value is the concatenation of substrings. Each substring is formed by converting the corresponding integer from
 *  the list into a string via @ref numberToString and then adding a single space character and an optional line feed. The line
 *  feeds are added only if @p separateStrings is true. */
ROSE_UTIL_API std::string listToString(const std::list<int>&, bool separateStrings = false);

/** Generate a string from a container of strings.
 *
 *  The return value is the concatenation of substrings. Each substring is formed by adding a single space to the corresponding
 *  list element and an optional line feed. The line feeds are added only if @p separateStrings is true.
 *
 *  @{ */
ROSE_UTIL_API std::string listToString(const std::list<std::string>&, bool separateStrings = false);
ROSE_UTIL_API std::string listToString(const std::vector<std::string>&, bool separateStrings = false);
/** @} */

/** Split a string into substrings at line feeds.
 *
 *  Splits the input string into substrings at the linefeed characters to construct a list, then removes empty strings from the
 *  list.
 *
 *  The original implementation (pre-2016) had a bug (ROSE-304) that caused the last substring to not be returned if it was not
 *  followed by a linefeed.  That implementation was also slow for large inputs (ROSE-305). Both of these are now fixed since
 *  stringToList is now implemented in terms of @ref split. */
ROSE_UTIL_API std::list<std::string> stringToList(const std::string&);

/** Splits string into substring based on a separator character.
 *
 *  Empty strings are removed from the result, which is returned in the @p stringList argument. The return argument is cleared
 *  before the splitting begins. */
ROSE_UTIL_API void splitStringIntoStrings(const std::string& inputString, char separator, std::vector<std::string>& stringList);

} // namespace
} // namespace

#endif
