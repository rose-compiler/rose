static const char *purpose = "ensure SgAsmIntegerValueExpression nodes have base objects";

static const char *description = "Parses the specimen given on the command line and reports how many "
                                 "SgAsmIntegerValueExpressions are found in the AST, and how many of them"
                                 "have non-null baseObject pointers.";

#include <rose.h>
#include <AsmUnparser_compat.h>
#include <Partitioner2/Engine.h>

namespace P2 = rose::BinaryAnalysis::Partitioner2;

struct Counter: AstSimpleProcessing {
    size_t nWithBase, nTotal;
    std::string prevHeading;

    Counter(): nWithBase(0), nTotal(0) {}

    void visit(SgNode *node) {
        std::vector<SgAsmIntegerValueExpression*> ivals;
        if (SgAsmBlock *blk = isSgAsmBlock(node)) {
            ivals = blk->get_successors();
        } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
            ivals.push_back(ival);
        }

        BOOST_FOREACH (SgAsmIntegerValueExpression *ival, ivals) {
            ++nTotal;

            std::string heading;
            if (SgAsmInstruction *insn = SageInterface::getEnclosingNode<SgAsmInstruction>(ival)) {
                heading = unparseInstructionWithAddress(insn);
            } else if (SgAsmBlock *blk = SageInterface::getEnclosingNode<SgAsmBlock>(ival)) {
                heading = StringUtility::addrToString(blk->get_address()) + ": basic block";
            } else {
                heading = "other";
            }

            if (heading != prevHeading) {
                std::cout <<heading <<"\n";
                prevHeading = heading;
            }

            if (ival->get_baseNode()) {
                ++nWithBase;
                std::cout <<"      yes";
            } else {
                std::cout <<"  *****no";
            }
            std::cout <<": " <<StringUtility::toHex(ival->get_absoluteValue()) <<ival->get_label() <<"\n";
        }
    }
};

int
main(int argc, char *argv[]) {
    Counter counts;
    counts.traverse(P2::Engine().frontend(argc, argv, purpose, description), preorder);
    std::cout <<"Number of SgAsmIntegerValueExpression nodes: " <<counts.nTotal <<"\n";
    std::cout <<"Number of such nodes having a base object:   " <<counts.nWithBase <<"\n";
}
