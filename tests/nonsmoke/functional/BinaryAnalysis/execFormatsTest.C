// This simple test code is used to read binary files for testing 
// of large collections of binaries using the runExecFormatsTest
// bash script.  This test code take any binary as input (technically
// it takes any source code (C, C++, Fortran, etc.) as well, since 
// nothing here is specific to binaries).
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "rose.h"
#include "stringify.h"

using namespace Rose;

/* Check that all nodes have the correct parent.  Not thread safe. */
struct AstChecker: public AstPrePostProcessing {
    std::vector<SgNode*> stack;                 // current path within the AST
    std::ostream &output;                       // where to emit warning/error messages
    size_t nproblems;                           // number of problems detected
    size_t limit;                               // number of errors to allow before exit
    std::string prefix;                         // line prefix

    explicit AstChecker(std::ostream &output, const std::string &prefix="")
        : output(output), nproblems(0), limit(0), prefix(prefix) {}

    bool check(SgNode *ast, size_t limit=0) {
        this->limit = limit;
        nproblems = 0;
        try {
            traverse(ast);
        } catch (const AstChecker*) {
            return false;
        }
        return 0==nproblems;
    }

    void preOrderVisit(SgNode *node) {
        if (!stack.empty()) {
            if (NULL==node->get_parent()) {
                output <<prefix <<"node has null parent property but was reached by AST traversal\n";
                show_details_and_maybe_fail(node);
            } else if (node->get_parent()!=stack.back()) {
                output <<prefix <<"node's parent property does not match traversal parent\n";
                show_details_and_maybe_fail(node);
            }
        }
        stack.push_back(node);
    }

    void postOrderVisit(SgNode *node) {
        assert(!stack.empty());
        assert(node==stack.back());
        stack.pop_back();
    }

    void show_details_and_maybe_fail(SgNode *node) {
        output <<prefix <<"AST path (including node) when inconsistency was detected:\n";
        for (size_t i=0; i<stack.size(); ++i)
            output <<prefix
                   <<"    #" <<std::setw(4) <<std::left <<i <<" " <<stringifyVariantT(stack[i]->variantT(), "V_")
                   <<" " <<stack[i] <<"; parent=" <<stack[i]->get_parent()
                   <<"\n";
        output <<prefix
               <<"    #" <<std::setw(4) <<std::left <<stack.size() <<" " <<stringifyVariantT(node->variantT(), "V_")
               <<" " <<node <<"; parent=" <<node->get_parent()
               <<"\n";
        if (++nproblems>=limit)
            throw this;
    }
};

class UnparseFileName: public AstSimpleProcessing {
    std::string name;

public:
    explicit UnparseFileName(const std::string &name)
        : name(name) {}

    void visit(SgNode *node) {
        if (SgFile *file = isSgFile(node))
            file->set_unparse_output_filename(name);
        if (SgAsmGenericFile *asmfile = isSgAsmGenericFile(node))
            asmfile->set_name(name);
    }
};

void
setUnparseFileName(SgProject *project, const std::string &unparseFileName) {
    UnparseFileName visitor(unparseFileName);
    visitor.traverse(project, preorder);
}

int
main(int argc, char *argv[])
{
    std::string unparseFileName;
    for (int i=1; i<argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i+1 < argc) {
            unparseFileName = argv[i+1];
            memmove(argv+i, argv+i+2, (argc-(i+1))*sizeof(argv[0]));// don't forget to move argv[argc]
            argc -= 2;
            break;
        }
    }
    
    SgProject *project= frontend(argc,argv);

    if (!unparseFileName.empty()) {
        setUnparseFileName(project, unparseFileName);
    }
    
    // Check AST parent/child consistency straight out of the parser
    {
        std::ostringstream ss;
        if (!AstChecker(ss, "    ").check(project, 10))
            std::cerr <<"Detected AST parent/child relationship problems straight from the parser:\n" <<ss.str();
    }

    // Try to fix any problems
    AstPostProcessing(project);
    AstTests::runAllTests(project);

    // Check AST again, and this time fail if there are still problems
    {
        std::ostringstream ss;
        if (!AstChecker(ss, "    ").check(project, 10)) {
            std::cerr <<"Detected AST parent/child relationship problems after AST post processing:\n" <<ss.str();
            abort();
        }
    }

#if 0 /*debugging; don't leave this enabled unless you fix "make distcheck" to clean up these files!*/
    generateDOT(*project);
    generateAstGraph(project, 4000);
#endif

 // Previous details to regenerate the binary from the AST for testing (*.new files) 
 // and to output a dump of the binary executable file format (*.dump files) has
 // been combined with the output of the disassembled instructions in the "backend()".

    return backend(project);
}

#endif
