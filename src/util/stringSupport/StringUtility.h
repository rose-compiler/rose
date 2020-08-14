#ifndef ROSE_StringUtility_H
#define ROSE_StringUtility_H

#include "commandline_processing.h"

#include <BitOps.h>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <limits.h>
#include <map>
#include <rose_constants.h>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>
#include <Sawyer/IntervalSet.h>

#if ROSE_MICROSOFT_OS
// This is the boost solution for lack of support for stdint.h (e.g. types such as "uint64_t")
#include <msvc_stdint.h>
#endif

namespace Rose {

/** Functions for operating on strings.
 *
 *  This name space provides functions for operating on strings.  See also, Boost String Algo
 *  [http://http://www.boost.org/doc/libs/1_61_0/doc/html/string_algo.html]. */
namespace StringUtility {




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Character-escaping functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Escapes HTML special characters.
 *
 *  Replaces "<", ">", and "&" with HTML character names and returns the result. */
ROSE_UTIL_API std::string htmlEscape(const std::string&);

/** Escapes characters that are special to C/C++.
 *
 *  Replaces special characters in the input so that it is suitable for the contents of a C string literal (if @p context is a
 *  double quote character) or the contents of a C character constant (if @p context is a single quote). That is, things
 *  like double quotes, line-feeds, tabs, non-printables, etc. are replace by their C backslash escaped versions. Returns the
 *  resulting string.
 *
 *  Note that if the first argument is a string then the context defaults to string literals, and if the first argument is a
 *  single character then the context defaults to character literals. Although this is usually what one wants, it's possible to
 *  change the context in both situations.
 *
 * @{ */
ROSE_UTIL_API std::string cEscape(const std::string&, char context = '"');
ROSE_UTIL_API std::string cEscape(char, char context = '\'');
/** @} */

/** Unescapes C++ string literals.
 *
 *  When given a C++ string literals content, the part between the enclosing quotes, this function will look for escape sequences,
 *  parse them, and replace them in the return value with the actual characters they represent.  For instance, passing in
 *  <code>std::string{"hello\\nworld\\00hidden"}</code> that contains 20 characters including two backslashes, the function replaces
 *  the first backslash+"n" pair with a line feed, and the second backslash+"0"+"0" with a NUL to result in
 *  <code>std::string{"hello\nworld\0hidden"}</code> (18 characters including the LF and NUL). Unicode escapes are not supported
 *  and will be left escaped in the return value. */
ROSE_UTIL_API std::string cUnescape(const std::string&);

/**  Escapes characters that are special to the Bourne shell.
 *
 *   Assumes that the context is outside of any quoting and possibly adds quotes. */
ROSE_UTIL_API std::string bourneEscape(const std::string&);

// [Robb Matzke 2016-05-06]: I am deprecating escapeNewLineCharaters because:
//   1. Its name is spelled wrong: "Charater"
//   2. "newline" in this context is a single word and should be capitalized as "Newline" not "NewLine"
//   3. Its name is misleading because it also escapes double quotes.
//   4. It escapes newlines using "l" rather than the more customary "n".
// I would just remove it, but it seems to be used in some projects and the tutorial.

/** Escapes line feeds and double quotes.
 *
 *  Scans the input string character by character and replaces line-feed characters with a backslash followed by the letter "l"
 *  and replaces double quotes by a backslash followed by a double qoute. */
ROSE_UTIL_API std::string escapeNewLineCharaters(const std::string&);

// DQ (12/8/2016): This is ued in the generation of dot files.
ROSE_UTIL_API std::string escapeNewlineAndDoubleQuoteCharacters(const std::string&);




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
 *  Given a container containing printable objects (such as <code>std::list<std::string></code>, join the objects together
 *  separated from one another by the specified @p separator. The separator does not appear before the first object or after
 *  the final object.  If the container is empty then an empty string is returned.
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
 *  I.e., <code>() => ""</code>
 *
 *  If the input is one phrase, the output is that phrase.
 *  E.g., <code>("foo") => "foo"</code>
 *
 *  If the input is two phrases, the output will be those two phrases separated by "and" (the @p finalIntro).
 *  E.g., <code>("foo", "bar") => "foo and bar"</code>
 *
 *  If the input is three or more phrases, they will be separated from one another by commas (the @p separator) and the last
 *  item will also be introduced with "and" (the @p finalIntro).
 *  E.g., <code>("foo", "bar", "baz") => "foo, bar, and baz"</code>
 *
 *  No transformations are performed on the input phrases. Space characters are inserted after each @p separator and @p
 *  finalIntro. A space is also inserted before the @p finalIntro when the input is two phrases. */
ROSE_UTIL_API std::string joinEnglish(const std::vector<std::string> &phrases,
                                      const std::string &separator = ",",
                                      const std::string &finalIntro = "and");



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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for converting numbers to strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Convert an integer to a string.
 *
 *  These functions are wrappers around <code>boost::lexical_cast<std::string></code>.
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

template<typename T> std::string toHex(T value) { return toHex2((uint64_t)value, 8*sizeof(T)); }
template<typename T> std::string signedToHex(T value) { return signedToHex2((uint64_t)value, 8*sizeof(T)); }
template<typename T> std::string unsignedToHex(T value) { return unsignedToHex2((uint64_t)value, 8*sizeof(T)); }
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      String conversion functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Convert to lower case.
 *
 *  Returns a new string by converting each of the input characters to lower case with @c tolower. */
ROSE_UTIL_API std::string convertToLowerCase(const std::string &inputString);

/** Normalizes line termination.
 *
 *  Changes ASCII-based line termination conventions used by various operating systems into the LF (line-feed) termination used
 *  by Multics, Unix and Unix-like systems (GNU/Linux, Mac OS X, FreeBSD, AIX, Xenix, etc.), BeOS, Amiga, RISC OS and others.
 *  Any occurrance of CR+LF, LF+CR, or CR by itself (in that order of left-to-right matching) is replaced by a single LF
 *  character. */
ROSE_UTIL_API std::string fixLineTermination(const std::string &input);

/** Insert a prefix string before every line.
 *
 *  This function breaks the @p lines string into individual lines, inserts the @p prefix string at the beginning of each line,
 *  then concatenates the lines together into a return value.  If @p prefixAtFront is true (the default) then the prefix is
 *  added to the first line of @p lines, otherwise the first line is unchanged.  An empty @p lines string is considered to be a
 *  single line.  If @p prefixAtBack is false (the default) then the prefix is not appended to the @p lines string if @p lines
 *  ends with a linefeed. */
ROSE_UTIL_API std::string prefixLines(const std::string &lines, const std::string &prefix,
                                      bool prefixAtFront=true, bool prefixAtBack=false);

/** Converts a multi-line string to a single line.
 *
 *  This function converts a multi-line string to a single line by replacing line-feeds and carriage-returns (and their
 *  surrounding white space) with a user-supplied replacement string (that defaults to a single space). Line termination (and
 *  it's surrounding white space) that appears at the front or back of the input string is removed without replacing it.
 *
 *  See roseTests/utilTests/stringTests.C for lots of examples.
 *
 *  A new string is returned. */
ROSE_UTIL_API std::string makeOneLine(const std::string &s, std::string replacement=" ");

/** Trims white space from the beginning and end of a string.
 *
 *  Caller may specify the characters to strip and whether the stripping occurs at the begining, the end, or both. */
ROSE_UTIL_API std::string trim(const std::string &str, const std::string &strip=" \t\r\n",
                               bool at_beginning=true, bool at_end=true);

/** Expand horizontal tab characters. */
ROSE_UTIL_API std::string untab(const std::string &str, size_t tabstops=8, size_t firstcol=0);

/** Remove redundant and blank lines.
 *
 *  Splits the input string into substrings according to @ref listToString, sorts the substrings and removes duplicates and
 *  lines that are empty (a line of only horizontal white space is not considered to be empty), then concatenates the
 *  substrings in their sorted order into the return value using @ref listToString, inserting extra white space at the
 *  beginning of all but the first line.
 *
 *  The original implementation had a bug (ROSE-304) that caused the first substring to be removed from the return value even
 *  if it was non-empty and unique. This happened when it was not followed by a line-feed. */
ROSE_UTIL_API std::string removeRedundantSubstrings(const std::string&);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for string encoding/decoding/hashing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Convert binary data to base-64.
 *
 *  The base64 number system uses the characters A-Z, a-z, 0-9, +, and / (in that order). The returned string does not include
 *  linefeeds.  If @p do_pad is true then '=' characters may appear at the end to make the total length a multiple of four.
 *
 * @{ */
ROSE_UTIL_API std::string encode_base64(const std::vector<uint8_t> &data, bool do_pad=true);
ROSE_UTIL_API std::string encode_base64(const uint8_t *data, size_t nbytes, bool do_padd=true);
/** @} */

/** Convert base-64 to binary. */
ROSE_UTIL_API std::vector<uint8_t> decode_base64(const std::string &encoded);

/** Compute a checkshum.
 *
 *  This function returns a unique checksum from the mangled name used it provides a simple means to obtain a unique value for
 *  any C++  declaration.  At a later date was should use the MD5 Checksum  implementation (but we can do that later).
 *
 *  The declaration is the same under One-time Definition Rule (ODR) if and only if the checksum values for each declaration
 *  are the same. */
ROSE_UTIL_API unsigned long generate_checksum(std::string s);




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Predicates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Determines whether one string contains another.
 *
 *  Returns true if @p longString contains @p shortString as a subsequence. */
inline bool isContainedIn(const std::string &longString, const std::string &shortString) {
    return longString.find(shortString) != std::string::npos;
}

/** Returns true if the string ends with line termination.
 *
 *  Only common ASCII-based line terminations are recognized: CR+LF, LF+CR, CR (only), or LF (only). */
ROSE_UTIL_API bool isLineTerminated(const std::string &s);




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions related to diagnostic messages
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Formatting support for generated code strings. */
ROSE_UTIL_API std::string indentMultilineString(const std::string& inputString, int statementColumnNumber);

/** Append an abbreviation or full name to a string. */
ROSE_UTIL_API void add_to_reason_string(std::string &result, bool isset, bool do_pad,
                                        const std::string &abbr, const std::string &full);


/** Append an assembly comment to a string.  Assembly comments are surrounded by "<" and ">" characters.  If the string
 *  already ends with an assembly comment, then the specified comment is inserted before the final ">" and separated from
 *  the previous comment with a comma.  Assembly comments are usually used for things like printing a decimal representation
 *  of a hexadecimal value, etc.
 *
 *  Example: after executing these statements:
 *
 *  @code
 *   std::string s = "0xff";
 *   s = appendAsmComment(s, "255");
 *   s = appendAsmComment(s, "-1");
 *  @endcode
 *
 *  The variable "s" will contain "0xff<255,-1>" */
ROSE_UTIL_API std::string appendAsmComment(const std::string &s, const std::string &comment);

/** Helpful way to print singular or plural words.
 *
 * @code
 *  size_t n = ...;
 *  std::cout <<"received " <<plural(n, "values") <<"\n";
 * @encode
 *
 *  Output for various values of <em>n</em> will be:
 *
 * @code
 *  received 0 values
 *  received 1 value
 *  received 2 values
 * @endcode
 *
 * This function uses a handful of grade-school rules and common exceptions for converting the supplied plural word to a
 * singular word when necessary.  If these are not enough, then the singular form can be supplied as the third argument.
 *
 * @code
 *  std::cout <<"graph contains " <<plural(nverts, "vertices", "vertex") <<"\n";
 * @endcode
 */
template<typename T>
std::string plural(T n, const std::string &plural_phrase, const std::string &singular_phrase="") {
    assert(!plural_phrase.empty());
    std::string retval = numberToString(n) + " ";
    if (1==n) {
        if (!singular_phrase.empty()) {
            retval += singular_phrase;
        } else if (boost::ends_with(plural_phrase, "vertices")) {
            retval += boost::replace_tail_copy(plural_phrase, 8, "vertex");
        } else if (boost::ends_with(plural_phrase, "indices")) {
            retval += boost::replace_tail_copy(plural_phrase, 7, "index");
        } else if (boost::ends_with(plural_phrase, "ies") && plural_phrase.size() > 3) {
            // string ends with "ies", as in "parties", so emit "party" instead
            retval += boost::replace_tail_copy(plural_phrase, 3, "y");
        } else if (boost::ends_with(plural_phrase, "sses") || boost::ends_with(plural_phrase, "indexes")) {
            // Sometimes we need to drop an "es" rather than just the "s"
            retval += boost::erase_tail_copy(plural_phrase, 2);
        } else if (boost::ends_with(plural_phrase, "s") && plural_phrase.size() > 1) {
            // strings ends with "s", as in "runners", so drop the final "s" to get "runner"
            retval += boost::erase_tail_copy(plural_phrase, 1);
        } else {
            // I give up.  Use the plural and risk being grammatically incorrect.
            retval += plural_phrase;
        }
    } else {
        retval += plural_phrase;
    }
    return retval;
}

// demangledName is defined in rose_support.cpp
/** Compute demangled version of mangled name.
 *
 *  Runs the c++filt command on the input string and returns the result.  If c++filt cannot be run then it prints an error to
 *  standard error and another to standard output. The pipes opened to communicate with the c++filt subcommand might not be
 *  closed if there's an error. */
std::string demangledName(std::string);


} // namespace
} // namespace


#endif
