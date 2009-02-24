// This is a small example of how to use the distributed memory traversal mechanism. It computes a list of function
// definitions in a program and outputs their names, their depth in the AST, and the ID of the process that found it.

#include <mpi.h>
#include <rose.h>
#include "functionNames.h"


int main(int argc, char **argv)
{
 // Read the AST.
    SgProject *project = frontend(argc, argv);

 // This function must be called before any distributed memory analyses are run.
    initializeDistributedMemoryProcessing(&argc, &argv);

 // Instances of the pre- and post-traversal classes defined above.
    FunctionNamesPreTraversal preTraversal;
    FunctionNamesPostTraversal postTraversal;

 // Instance of the analyzer object.
    FunctionNames functionNames;

 // The initial inherited attribute passed to the pre-traversal.
    int initialDepth = 0;

 // Run the analysis: This invokes the pre-traversal, calls the analyzeSubtree() method for every defining function
 // declaration in the AST, and then collects the computed data using the post-traversal.
    functionNames.performAnalysis(project, initialDepth, &preTraversal, &postTraversal);

 // The isRootProcess() method will return true in exactly one of the processes; the getFinalResults() method in
 // its instance of the analyzer can be used to access the analysis results.
    if (functionNames.isRootProcess()) {
        std::string results = functionNames.getFinalResults();
        std::cout << "----- found the following functions: ------" << std::endl;
        std::cout << results;
        std::cout << "-------------------------------------------" << std::endl;
    }

 // This function must be called after all distributed memory analyses have finished.
    finalizeDistributedMemoryProcessing();
}
