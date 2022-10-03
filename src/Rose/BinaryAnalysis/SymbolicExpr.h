#ifndef ROSE_BinaryAnalysis_SymbolicExpr_H
#define ROSE_BinaryAnalysis_SymbolicExpr_H

// [Robb Matzke 2022-09-20]: deprecated
#include <rose_pragma_message.h>
ROSE_PRAGMA_MESSAGE("This header is deprecated; use Rose/BinaryAnalysis/SymbolicExpression.h instead");
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

// [Robb Matzke 2022-09-20]: deprecated
namespace Rose {
    namespace BinaryAnalysis {
        namespace SymbolicExpr = Rose::BinaryAnalysis::SymbolicExpression;
    }
    using SymbolicExprPtr = Rose::BinaryAnalysis::SymbolicExpressionPtr;
}

#endif
