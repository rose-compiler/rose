#include <config.h>

#include "CFGTraversal.h"
#include "ProcTraversal.h"

#undef NDEBUG
#include "testcfg.h"

int main(int argc, char **argv)
{
    SgProject *root = frontend(argc, argv);
    std::cout << "collecting functions... ";
    ProcTraversal s;
    s.traverseInputFiles(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "generating cfg..." << std::endl;
    CFGTraversal t(s.get_procedures());
    t.traverseInputFiles(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "testing cfg" << std::endl;
    if (kfg_testit(t.getCFG(), 0, "cfg_dump.gdl") == 0)
        std::cerr << "oops" << std::endl;
    std::cout << "done" << std::endl;

    return 0;
}
