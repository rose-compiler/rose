#include <rose.h>
#include <BinarySymbolicExpr.h>
#include <cstdlib>

using namespace Rose::BinaryAnalysis::SymbolicExpr;

int main() {
    LeafPtr a = makeIntegerConstant(32, 1);
    LeafPtr b = makeIntegerConstant(32, 1);
    int cmp = a->compareStructure(b);
    return 0==cmp ? EXIT_SUCCESS : EXIT_FAILURE;
}
