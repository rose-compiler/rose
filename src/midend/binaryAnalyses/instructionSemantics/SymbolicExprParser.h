#ifndef ROSE_BinaryAnalysis_SymbolicExprParser_H
#define ROSE_BinaryAnalysis_SymbolicExprParser_H

#include <InsnSemanticsExpr.h>
#include <Sawyer/Map.h>

namespace rose {
namespace BinaryAnalysis {

/** Parses symbolic expressions from text.
 *
 *  Parses symbolic expressions using a simple LISP-like syntax, the same syntax used by the unparser.  An expression can be a
 *  numeric constant with an optional negative sign, a hexadecimal bit vector, a numbered variable like "v123", a named
 *  variable, or an operator with arguments.  Operators are expressed as an operator name and space-separated arguments; the
 *  operator and arguments are both inside the parentheses like in LISP.  A width in bits can be appended to any constant,
 *  variable, or operator name and is a positive number inside square brackets.  Inline comments are enclosed in angle brackets
 *  (less than and greater than signs) and can be nested. Backslashes are used to escape parentheses, square brackets, and
 *  angle brackets to remove their special meaning. The normal C/C++ backslash escapes are also understood.
 *
 *  @todo Symbolic expressions were originally intended to be used only within ROSE and therefore many of the
 *        operations assert that their arguments are correct. Now that users can easily construct their own symbolic
 *        expressions from text, we need to make the failure modes less extreme. [Robb P. Matzke 2015-09-22]. */
class SymbolicExprParser {
    Sawyer::Container::Map<std::string, InsnSemanticsExpr::Operator> ops_;

public:
    /** Syntax errors that are thrown. */
    class SyntaxError: public std::runtime_error {
    public:
        std::string inputName;                          /**< Name of input, usually a file name. */
        unsigned lineNumber;                            /**< Line number (1-origin). Zero if position info is not available. */
        unsigned columnNumber;                          /**< Column number (0-origin). Unused when @c lineNumber is zero. */
        void print(std::ostream&) const;                /**< Print error message to stream. */
        SyntaxError(const std::string &mesg, const std::string &inputName, unsigned lineNumber, unsigned columnNumber);
        ~SyntaxError() throw () {}
    };

public:
    /** Default constructor. */
    SymbolicExprParser() { init(); }

    /** Create a symbolic expression by parsing a string.
     *
     *  Parses the string and returns the first expression in the string. Throws a @ref SyntaxError if problems are
     *  encountered. */
    InsnSemanticsExpr::TreeNodePtr parse(const std::string&);

    /** Create a symbolic expression by parsing a file.
     *
     *  Parses the file and returns the first expression in the file. Throws a @ref SyntaxError if problems are encountered. */
    InsnSemanticsExpr::TreeNodePtr parse(std::istream &input, const std::string &filename="-",
                                         unsigned lineNumber=1, unsigned columnNumber=0);

private:
    void init();
};
    
std::ostream& operator<<(std::ostream&, const SymbolicExprParser::SyntaxError&);

} // namespace
} // namespace

#endif
