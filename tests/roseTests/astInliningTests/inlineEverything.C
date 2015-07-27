// This test attempts to inline function calls until I cannot inline anything else or some limit is reached.
#include "rose.h"

// Finds needle in haystack and returns true if found.  Needle is a single node (possibly an invalid pointer and will not be
// dereferenced) and haystack is the root of an abstract syntax (sub)tree.
static bool
isAstContaining(SgNode *haystack, SgNode *needle) {
    struct T1: AstSimpleProcessing {
        SgNode *needle;
        T1(SgNode *needle): needle(needle) {}
        void visit(SgNode *node) {
            if (node == needle)
                throw this;
        }
    } t1(needle);
    try {
        t1.traverse(haystack, preorder);
        return false;
    } catch (const T1*) {
        return true;
    }
}

int
main (int argc, char* argv[]) {
    // Build the project object (AST) which we will fill up with multiple files and use as a
    // handle for all processing of the AST(s) associated with one or more source files.
    SgProject* sageProject = frontend(argc,argv);
    AstTests::runAllTests(sageProject);

    // Inline one call at a time until all have been inlined.  Loops on recursive code.
    size_t nInlined = 0;
    for (int count=0; count<10; ++count) {
        bool changed = false;
        BOOST_FOREACH (SgFunctionCallExp *call, SageInterface::querySubTree<SgFunctionCallExp>(sageProject)) {
            if (doInline(call)) {
                ASSERT_always_forbid2(isAstContaining(sageProject, call),
                                      "Inliner says it inlined, but the call expression is still present in the AST.");
                ++nInlined;
                changed = true;
                break;
            }
        }
        if (!changed)
            break;
    }
    std::cout <<"Test inlined " <<StringUtility::plural(nInlined, "function calls") <<"\n";

 // Post-inline AST normalizations

 // DQ (6/12/2015): These functions first renames all variable (a bit heavy handed for my tastes)
 // and then (second) removes the blocks that are otherwise added to support the inlining.  The removal
 // of the blocks is the motivation for renaming the variables, but the variable renaming is 
 // done evarywhere instead of just where the functions are inlined.  I think the addition of
 // the blocks is a better solution than the overly agressive renaming of variables in the whole
 // program.  So the best solution is to comment out both of these functions.  All test codes
 // pass (including the token-based unparsing tests).
 // renameVariables(sageProject);
 // flattenBlocks(sageProject);

    cleanupInlinedCode(sageProject);
    changeAllMembersToPublic(sageProject);

    AstTests::runAllTests(sageProject);

    return backend(sageProject);
}
