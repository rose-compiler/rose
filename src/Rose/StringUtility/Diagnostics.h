#ifndef ROSE_StringUtility_Diagnostics_H
#define ROSE_StringUtility_Diagnostics_H
#include <RoseFirst.h>
#include <rosedll.h>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <string>

namespace Rose {
namespace StringUtility {

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
    std::string retval = boost::lexical_cast<std::string>(n) + " ";
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
