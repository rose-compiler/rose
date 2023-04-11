#include <generated/TestExpr.h>
#include <Rose/Tree/Base.h>

using namespace Rose;

/** Base class for instructions. */
class TestInsn: public Rose::Tree::Base {
    /** Mnemonic. */
    [[Rosebud::ctor_arg]]
    std::string mnemonic;

    /** List of operand expressions. */
    [[using Rosebud: property, mutators()]]
    Tree::Edge<TestExprList> exprs = TestExprList::instance();

public:
    size_t nInstructions() const {
        return exprs()->size();
    }
};
