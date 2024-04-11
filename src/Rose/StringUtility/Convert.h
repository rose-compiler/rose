#ifndef ROSE_StringUtility_Convert_H
#define ROSE_StringUtility_Convert_H
#include <RoseFirst.h>
#include <rosedll.h>

#include <cstdint>
#include <string>
#include <vector>

namespace Rose {
namespace StringUtility {

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

/** Left justify a string to specified width.
 *
 *  The given string is either truncated or extended to make it the specified number of characters. New @p fill characters are
 *  added to the end of the string if necessary. */
ROSE_UTIL_API std::string leftJustify(const std::string &input, size_t width, char fill = ' ');

/** Right justify a string to specified width.
 *
 *  The given string is either truncated or extended to make it the specified number of characters. New @p fill characters are
 *  added to the beginning of the string if necessary. */
ROSE_UTIL_API std::string rightJustify(const std::string &input, size_t width, char fill = ' ');

/** Center a string in a field.
 *
 *  The given string is either truncated or extended to make it the specified number of characters. New @p fill characters are
 *  added to the beginning and end of the string if necessary. */
ROSE_UTIL_API std::string centerJustify(const std::string &input, size_t width, char fill = ' ');

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

/** Remove ANSI escape characters.
 *
 *  Currently handles only the "Control Sequence Introducer" commands, but these are the most common and most useful commands
 *  anyway since they include such things as cursor movement, erasing, scrolling, colors, and other graphic renditions. */
ROSE_UTIL_API std::string removeAnsiEscapes(const std::string&);

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

} // namespace
} // namespace

#endif
