#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

static const char *purpose = "ensure SgAsmIntegerValueExpression nodes have base objects";

static const char *description = "Parses the specimen given on the command line and reports how many "
                                 "SgAsmIntegerValueExpressions are found in the AST, and how many of them"
                                 "have non-null baseObject pointers.";

#include <rose.h>
#include <AsmUnparser_compat.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/Map.h>

using namespace Rose;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

// List of integers per heading, where heading is just some arbitrary title string
typedef Sawyer::Container::Map<std::string /*heading*/, std::vector<SgAsmIntegerValueExpression*> > IntegersByHeading;

// Search the AST to find all SgAsmIntegerValueExpressions and organize them into an IntegersByHeading.
struct GatherIntegers: AstSimpleProcessing {
    IntegersByHeading integers;

    void visit(SgNode *node) {
        std::vector<SgAsmIntegerValueExpression*> ivals;
        if (SgAsmBlock *blk = isSgAsmBlock(node)) {
            ivals = blk->get_successors();
        } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
            ivals.push_back(ival);
        }

        BOOST_FOREACH (SgAsmIntegerValueExpression *ival, ivals) {
            std::string heading;
            if (SgAsmInstruction *insn = SageInterface::getEnclosingNode<SgAsmInstruction>(ival)) {
                heading = unparseInstructionWithAddress(insn);
            } else if (SgAsmBlock *blk = SageInterface::getEnclosingNode<SgAsmBlock>(ival)) {
                heading = StringUtility::addrToString(blk->get_address()) + ": basic block";
            } else {
                heading = "other";
            }

            integers.insertMaybeDefault(heading).push_back(ival);
        }
    }
};

// Sort SgAsmIintegerValueExpression since their order in the AST is compiler dependent.
static bool
sortedIntegers(SgAsmIntegerValueExpression *a, SgAsmIntegerValueExpression *b) {
    if (!a)
        return b != NULL;                               // null < non-null
    if (!b)
        return false;                                   // non-null >= null
    if (a->get_absoluteValue() != b->get_absoluteValue())
        return a->get_absoluteValue() < b->get_absoluteValue();
    if (!a->get_baseNode())
        return b->get_baseNode() != NULL;
    if (!b->get_baseNode())
        return false;
    return false;
}

// Print the IntegersByHeading
static std::pair<size_t, size_t>
printIntegersByHeading(const IntegersByHeading &ibh) {
    size_t nWithBase=0, nTotal=0;
    BOOST_FOREACH (const IntegersByHeading::Node &headingInteger, ibh.nodes()) {
        std::cout <<headingInteger.key() <<"\n";
        std::vector<SgAsmIntegerValueExpression*> integers = headingInteger.value();
        std::sort(integers.begin(), integers.end(), sortedIntegers);
        BOOST_FOREACH (SgAsmIntegerValueExpression *ival, integers) {
            ++nTotal;
            if (ival->get_baseNode()) {
                ++nWithBase;
                std::cout <<"      yes";
            } else {
                std::cout <<"  *****no";
            }
            std::cout <<": " <<StringUtility::toHex(ival->get_absoluteValue()) <<ival->get_label() <<"\n";
        }
    }
    return std::make_pair(nTotal, nWithBase);
}

int
main(int argc, char *argv[]) {
    SgAsmBlock *gblock = P2::Engine().frontend(argc, argv, purpose, description);
    GatherIntegers gatherer;
    gatherer.traverse(gblock, preorder);
    std::pair<size_t, size_t> counts = printIntegersByHeading(gatherer.integers);
    std::cout <<"Number of SgAsmIntegerValueExpression nodes: " <<counts.first <<"\n";
    std::cout <<"Number of such nodes having a base object:   " <<counts.second <<"\n";
}

#endif
