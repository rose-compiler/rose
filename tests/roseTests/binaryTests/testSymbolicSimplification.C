#include <rose.h>
#include <InsnSemanticsExpr.h>
#include <SymbolicSemantics2.h>
#include <YicesSolver.h>

using namespace rose;
using namespace rose::BinaryAnalysis;

#if 0 // [Robb P. Matzke 2015-06-25]: cannot be tested automatically since Jenkins might not have Yices
static void
test_yices_linkage() {
    if (unsigned avail = YicesSolver::available_linkage()) {
        std::cout <<"Yices is available as";
        if (avail & YicesSolver::LM_EXECUTABLE)
            std::cout <<" an executable";
        if (avail & YicesSolver::LM_LIBRARY)
            std::cout <<" a library";
        std::cout <<"\n";
    } else {
        std::cout <<"Yices is not available";
    }
    
    YicesSolver yices1;
    switch (yices1.get_linkage()) {
        case YicesSolver::LM_NONE:
            std::cout <<"Yices is not available.\n";
            break;
        case YicesSolver::LM_LIBRARY:
            std::cout <<"Yices is linked as a library.\n";
            break;
        case YicesSolver::LM_EXECUTABLE:
            std::cout <<"Yices is used as a separate executable.\n";
            break;
        default:
            ASSERT_not_reachable("invalid yices linkage");
    }

    if ((YicesSolver::available_linkage() & YicesSolver::LM_LIBRARY) != 0) {
        YicesSolver yices2;
        yices2.set_linkage(YicesSolver::LM_LIBRARY);
        switch (yices2.get_linkage()) {
            case YicesSolver::LM_NONE:
                std::cout <<"Yices is not available.\n";
                break;
            case YicesSolver::LM_LIBRARY:
                std::cout <<"Yices is linked as a library.\n";
                break;
            case YicesSolver::LM_EXECUTABLE:
                std::cout <<"Yices is used as a separate executable.\n";
                break;
            default:
                ASSERT_not_reachable("invalid yices linkage");
        }
    }
}
#endif

static void
test_add_simplifications() {
    using namespace InsnSemanticsExpr;

    TreeNodePtr reg = LeafNode::create_variable(32, "esp_0");
    TreeNodePtr nreg = InternalNode::create(32, OP_NEGATE, reg);
    TreeNodePtr number = LeafNode::create_integer(32, 0xfffffffc);
    TreeNodePtr t1 = InternalNode::create(32, OP_ADD, reg, nreg, number);
    std::cout <<"(add esp_0 (negate esp_0) 0xfffffffc) = " <<*t1 <<"\n";

    TreeNodePtr n1 = LeafNode::create_variable(32, "esp_0");
    TreeNodePtr n2 = LeafNode::create_integer(32, 4);
    TreeNodePtr n3 = LeafNode::create_integer(32, 8);
    TreeNodePtr n4 = InternalNode::create(32, OP_ADD, n1, n2);
    TreeNodePtr n5 = InternalNode::create(32, OP_ADD, n4, n3);
    TreeNodePtr n6 = InternalNode::create(32, OP_NEGATE, n1);
    TreeNodePtr n7 = InternalNode::create(32, OP_ADD, n5, n6);
    std::cout <<"(add esp_0 4 8 (negate esp_0)) = " <<*n7 <<"\n";
}

static void
test_svalues() {
    using namespace InstructionSemantics2;

    BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(SymbolicSemantics::SValue::instance(), NULL);

    BaseSemantics::SValuePtr a1 = ops->undefined_(32);                 // v1
    BaseSemantics::SValuePtr a2 = ops->add(a1, ops->number_(32, 4));   // (add v1 4)
    BaseSemantics::SValuePtr a3 = ops->add(a2, ops->number_(32, 8));   // (add v1 12)
    BaseSemantics::SValuePtr a4 = ops->subtract(a3, a2);               // (add v1 12 (negate (add v1 4)))
    std::cout <<"(add v1 4 8 (negate (add v1 4))) = " <<*a4 <<"\n";
}

int
main() {
#if 0 // [Robb P. Matzke 2015-06-25]: cannot be tested automatically since Jenkins might not have Yices
    test_yices_linkage();
#endif
    test_add_simplifications();
    test_svalues();
}
