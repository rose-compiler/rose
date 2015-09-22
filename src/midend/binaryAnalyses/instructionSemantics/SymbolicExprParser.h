#ifndef ROSE_BinaryAnalysis_SymbolicExprParser_H
#define ROSE_BinaryAnalysis_SymbolicExprParser_H

#include <InsnSemanticsExpr.h>
#include <Sawyer/Map.h>

namespace rose {
namespace BinaryAnalysis {

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
    SymbolicExprParser() { init(); }

    /** Create a symbolic expression by parsing a string. */
    InsnSemanticsExpr::TreeNodePtr parse(const std::string&);

    /** Create a symbolic expression by parsing a file. */
    InsnSemanticsExpr::TreeNodePtr parse(std::istream &input, const std::string &filename="-",
                                         unsigned lineNumber=1, unsigned columnNumber=0);

private:
    void init();
};
    
std::ostream& operator<<(std::ostream&, const SymbolicExprParser::SyntaxError&);

} // namespace
} // namespace

#endif
