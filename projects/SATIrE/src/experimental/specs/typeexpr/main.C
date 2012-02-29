#include <config.h>

#include "CFGTraversal.h"
#include "ProcTraversal.h"

#undef NDEBUG
#include "testcfg.h"

#define doit(analysis) xdoit(analysis)
#define xdoit(analysis) analysis##_doit

#define str(analysis) xstr(analysis)
#define xstr(analysis) #analysis

extern "C" void *doit(ANALYSIS)(void *);
extern "C" void gdl_create(char *, int);

int main(int argc, char **argv)
{
    SgProject *root = frontend(argc, argv);
    std::cout << "collecting functions... ";
    ProcTraversal s;
    s.traverseInputFiles(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "generating cfg... ";
    CFGTraversal t(s.get_procedures());
    t.traverseInputFiles(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "testing cfg... " << std::endl;
    int test_result = kfg_testit(t.getCFG(), 0, "cfg_dump.gdl");
    if (test_result == 0)
    {
        std::cout << std::endl
            << "Warning: There are problems in the CFG."
            << std::endl
            << "Do not rely on the analysis results." << std::endl;
    }
    else
        std::cout << "no problems found" << std::endl;
    
    std::cout << "performing analysis " str(ANALYSIS) "... ";
    std::cout.flush();
    doit(ANALYSIS)(t.getCFG());
    std::cout << "done" << std::endl;
    std::cout << "generating visualization... ";
    gdl_create(str(ANALYSIS) "_result.gdl", 0);
    std::cout << "done" << std::endl;

    return 0;
}
