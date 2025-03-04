#ifndef ROSE_StringUtility_Escape_H
#define ROSE_StringUtility_Escape_H
#include <RoseFirst.h>
#include <rosedll.h>

#include <string>

namespace Rose {
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
 *  `std::string{"hello\\nworld\\00hidden"}` that contains 20 characters including two backslashes, the function replaces the first
 *  backslash+"n" pair with a line feed, and the second backslash+"0"+"0" with a NUL to result in
 *  `std::string{"hello\nworld\0hidden"}` (18 characters including the LF and NUL). Unicode escapes are not supported and will be
 *  left escaped in the return value. */
ROSE_UTIL_API std::string cUnescape(const std::string&);

/** Escapes characters that are special to the Bourne shell.
 *
 *  Assumes that the context is outside of any quoting and possibly adds quotes. */
ROSE_UTIL_API std::string bourneEscape(const std::string&);

/** Escapes characters that are special to YAML strings.
 *
 *  Assumes that the context is outside of any quoting and possibly adds quotes. */
ROSE_UTIL_API std::string yamlEscape(const std::string&);

/** Escapes characters that are special inside a JSON string.
 *
 *  Assumes that the provided string is already inside double quotes; no additional quotes are added to the return value. */
ROSE_UTIL_API std::string jsonEscape(const std::string&);

/** Escapes characters that are special in CSV tables.
 *
 *  Assumes that the context is outside of any quoting and possib9 adds quotes. See RFC 4180 for details. */
ROSE_UTIL_API std::string csvEscape(const std::string&);

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

} // namespace
} // namespace

#endif
