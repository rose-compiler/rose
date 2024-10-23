#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <AsmUnparser.h>

#include <Rose/AST/Traversal.h>

#include <SgAsmInstruction.h>

namespace Rose {
namespace BinaryAnalysis {

// [Robb Matzke 2024-10-23]: Deprecated
size_t
AsmUnparser::unparse(std::ostream &output, SgNode *ast) {
    size_t count = 0;
    AST::Traversal::forwardPre<SgAsmInstruction>(ast, [&output, &count](SgAsmInstruction *insn) {
        output <<insn->toString() <<"\n";
        ++count;
    });
    return count;
}

} // namespace
} // namespace

#endif
