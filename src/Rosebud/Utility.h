#ifndef Rosebud_Utility_H
#define Rosebud_Utility_H
#include <Rosebud/Ast.h>

#include <Sawyer/Graph.h>
#include <Sawyer/Message.h>
#include <Sawyer/Optional.h>

namespace Rosebud {

enum class Backend {
    ROSETTA_BINARY,
    YAML,
    NONE
};

/** Command-line settings for the rosebud tool. */
struct Settings {
    Backend backend = Backend::YAML;
    bool showingWarnings = true;
    bool showingCppLineDirectives = false;
    bool debugging = false;
    When usingColor = When::AUTO;
};

/** Command-line settings for the rosebud tool. */
extern Settings settings;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Return the matching opening or closing construct.
 *
 *  E.g., if input is "{" then output is "}" and vice versa. */
std::string matching(const std::string&);

// Split a string into one string per line, removing the LF line termination. ROSE code does not have CR line termination so we
// don't have to worry about that.
std::vector<std::string> splitIntoLines(const std::string&);

// Remove lines that are blank or contain only white space
void eraseBlankLines(std::vector<std::string>&);

/** Trim leading, trailing, and internal blank lines and trailing white space.
 *
 *  + Trailing white space is removed from each line.
 *  + Leading blank lines are removed.
 *  + Trailing blank lines are removed.
 *  + Two or more consecutive blank lines are replaced by a single blank line */
void trimBlankLines(std::vector<std::string>&);

/** Add a prefix to every line.
 *
 * @{ */
std::string prefixLines(const std::string &s, const std::string &prefix);
void prefixLines(std::vector<std::string> &lines, const std::string &prefix);
/** @} */

// Damerau-Levenshtein edit distance
size_t editDistance(const std::string &src, const std::string &tgt);

// Relative difference between two strings. Returns a value between zero and one.
double relativeDifference(const std::string &src, const std::string &tgt);

// Return the best matching string from a set.
std::string bestMatch(const std::vector<std::string> &candidates, const std::string &sample);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Comment utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Make a block comment.
 *
 *  The string is split into lines. The first line is prefixed with the opening text and the prefix for the following lines is
 *  created automatically from the opening text. This works for both C-style and C++-style comments. If the opening contains neither
 *  a C-style nor C++-style comment opening, then C-style is assumed and the opening is used as-is for the prefix for all lines. The
 *  closing token for C-style comments hangs on the last line of text. If there is no text, then the return value is similarly
 *  empty.
 *
 * @{ */
std::string makeBlockComment(const std::string &text, const std::string &opening);
std::vector<std::string> makeBlockComment(const std::vector<std::string> &textLines, const std::string &opening);
/** @} */

/** Make a title comment that spans the entire width of the source code.
 *
 *  This is similar to the wide //////... comments in this header file.
 *
 * @{ */
std::string makeTitleComment(const std::string &multiLine, const std::string &prefix, char bar, size_t width);
std::vector<std::string> makeTitleComment(const std::vector<std::string> &lines, const std::string &prefix, char bar, size_t width);
/** @} */

/** Append text to a Doxygen comment.
 *
 * Given a Doxygen block comment (or nothing), append the specified multi-line text to the end of the comment. The new text should
 * not include start or end with C-style comment delimiters or contain C++-style or box decorations at the start of each line of new
 * text; these will be added automatically. */
std::string appendToDoxygen(const std::string &existingComment, const std::string &newText);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Diagnostic messages
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Number of error messages reported. */
extern size_t nErrors;

/** True if we're using color output for diagnostics. */
bool usingColor();

/** Convert an importance level and message to an error output string.
 *
 *  The returned string may have ANSI color codes and will have line termination. */
std::string messageString(Sawyer::Message::Importance, const std::string &mesg);

/** Print a diagnostic message to standard error.
 *
 *  Messages contain the following parts:
 *    + The multi-line message
 *    + The importance
 *    + The file name if a file is provided, otherwise the program name
 *    + The line and column number if a token is provided
 *    + The lines from the source file if a token is provided
 *    + An indication of the important part of the line if a token is provided and a source line is emitted
 *
 *  The message will include the name of the input file, the position in the input file, and a copy of the relevant part of the
 *  input file
 *
 * @{ */
void message(Sawyer::Message::Importance, const Ast::FilePtr&, const Token&, const std::string &mesg);
void message(Sawyer::Message::Importance, const Ast::FilePtr&, const Token &begin, const Token &focus,
             const Token &end, const std::string &mesg);
void message(Sawyer::Message::Importance, const Ast::FilePtr&, const std::vector<Token>&, const std::string &mesg);
void message(Sawyer::Message::Importance, const std::string &mesg);
void message(Sawyer::Message::Importance, const Ast::FilePtr&, const std::string &mesg);
/** @} */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class hierarchy utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Key for ordering classes in the class hierarchy. */
struct HierarchyKey {
    std::string s;
    HierarchyKey(const Ast::ClassPtr &c) /*implicit*/
        : s(c->name) {}
    HierarchyKey(const std::string &s) /*implicit*/
        : s(s) {}
    bool operator<(const HierarchyKey &other) const {
        return s < other.s;
    }
};

/** Class hierarchy.
 *
 *  The vertices are pointers to AST class nodes. The edges point from base classes to derived classes. */
using Hierarchy = Sawyer::Container::Graph<Ast::ClassPtr, Sawyer::Nothing, HierarchyKey>;

/** Ordered sequence of classes. */
using Classes = std::vector<Ast::ClassPtr>;

/** Generate the class hierarchy from the specified class definitions.
 *
 *  The edges in the graph point from base classes to derived classes. */
Hierarchy classHierarchy(const Classes&);

/** Check for problems such as cycles in the class hiearchy and report them as errors. */
void checkClassHierarchy(Hierarchy&);

/** Return all the class definitions so that base classes are before derived classes. */
Classes topDown(Hierarchy&);

/** Return all the class definitions so that derived classes appear before base classes. */
Classes bottomUp(Hierarchy&);

/** Return all known subclasses. */
Classes derivedClasses(const Ast::ClassPtr&, const Hierarchy&);

/** True if the class is a base class of some other class. */
bool isBaseClass(const Ast::ClassPtr&, const Hierarchy&);

/** Properties that form constructor arguments. */
std::vector<Ast::PropertyPtr> allConstructorArguments(const Ast::ClassPtr&, const Hierarchy&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Turn a type into a const reference to the type.
 *
 * @code
 *  Input           Output
 *  --------------- ----------
 *  char            char const&
 *  char*           char* const&
 *  const char*     const char* const&
 * @endcode */
std::string constRef(const std::string &type);

/** Rmove "volatile" and "mutable" from the beginning of a type string. */
std::string removeVolatileMutable(const std::string &type);

} // namespace
#endif
