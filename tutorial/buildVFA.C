#include "sage3basic.h"

#include <string>
#include <iostream>

#include "VirtualFunctionAnalysis.h"
using namespace boost;


using namespace std;
using namespace boost;
// A Function object used as a predicate that determines which functions are 
// to be represented in the call graph.
struct keepFunction : public unary_function<bool,SgFunctionDeclaration*>{
  public:
    bool operator()(SgFunctionDeclaration* funcDecl){
      bool returnValue = true;
      ROSE_ASSERT(funcDecl != NULL);
      string filename = funcDecl->get_file_info()->get_filename();

      //Filter out functions from the ROSE preinclude header file
      if(filename.find("rose_edg_required_macros_and_functions")!=string::npos)
        returnValue = false;

      //Filter out compiler generated functions
      if(funcDecl->get_file_info()->isCompilerGenerated()==true)
        returnValue=false;

      return returnValue;
    }
};

int main(int argc, char * argv[]) {

    SgProject* project = frontend(argc, argv);
    ROSE_ASSERT(project != NULL);

    CallGraphBuilder builder(project);
    builder.buildCallGraph(keepFunction());
    // Generate call graph in dot format
    AstDOTGeneration dotgen;
    dotgen.writeIncidenceGraphToDOTFile(builder.getGraph(), "original_call_graph.dot");

    SgFunctionDeclaration *mainDecl = SageInterface::findMain(project);
    if(mainDecl == NULL) {
            std::cerr<< "Can't execute Virtual Function Analysis without main function\n";
            return 0;
     }
    
    VirtualFunctionAnalysis *anal = new VirtualFunctionAnalysis(project);
    anal->run();
    
    anal->pruneCallGraph(builder);
     
     AstDOTGeneration dotgen2;
     dotgen2.writeIncidenceGraphToDOTFile(builder.getGraph(), "pruned_call_graph.dot");

    delete anal;
    
    
    return 0;
}


