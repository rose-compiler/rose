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
        SyntaxError(const std::string &mesg, const std::string &filename, unsigned lineNumber, unsigned columnNumber);
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

    

} // namespace
} // namespace

#endif
