// This simple test code is used to read binary files for testing 
// of large collections of binaries using the runExecFormatsTest
// bash script.  This test code take any binary as input (technically
// it takes any source code (C, C++, Fortran, etc.) as well, since 
// nothing here is specific to binaries).

#include "rose.h"
#include "stringify.h"


/* Check that all nodes have a parent */
struct T1: public AstPrePostProcessing {
    std::vector<SgNode*> stack;
    void preOrderVisit(SgNode *node) {
        if (!stack.empty()) {
            if (NULL==node->get_parent()) {
                std::cerr <<"node has null parent property\n";
                show_stack_and_fail();
            }
            if (node->get_parent()!=stack.back()) {
                std::cerr <<"node's parent property does not match traversal parent\n"
                          <<"    node is an " <<stringifyVariantT(node->variantT(), "V_") <<"\n"
                          <<"    node's parent is an " <<stringifyVariantT(node->variantT(), "V_") <<"\n";
                show_stack_and_fail();
            }
        }
        stack.push_back(node);
    }

    void postOrderVisit(SgNode *node) {
        assert(!stack.empty());
        assert(node==stack.back());
        stack.pop_back();
    }

    void show_stack_and_fail() {
        for (size_t i=0; i<stack.size(); ++i)
            std::cerr <<"    " <<std::setw(4) <<std::left <<i <<" " <<stringifyVariantT(stack[i]->variantT(), "V_") <<"\n";
        exit(1);
    }
};


int
main(int argc, char *argv[])
{

    SgProject *project= frontend(argc,argv);
    
    T1().traverse(project);
    //AstPostProcessing(project);
    AstTests::runAllTests(project);

#if 0 /*debugging; don't leave this enabled unless you fix "make distcheck" to clean up these files!*/
    generateDOT(*project);
    generateAstGraph(project, 4000);
#endif

 // Previous details to regenerate the binary from the AST for testing (*.new files) 
 // and to output a dump of the binary executable file format (*.dump files) has
 // been combined with the output of the disassembled instructions in the "backend()".

    return backend(project);
}

