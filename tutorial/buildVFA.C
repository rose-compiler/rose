#include "sage3basic.h"

#include <string>
#include <iostream>

#include "VirtualFunctionAnalysis.h"
using namespace boost;


using namespace std;
using namespace boost;
// A Function object used as a predicate that determines which functions are 
// to be represented in the call graph.
// Liao 1/23/2013. It turns out there is another FunctionFilter used in src/midend/programAnalysis/VirtualFunctionAnalysis/PtrAliasAnalysis.C
// We have to make two filters consistent or there will be mismatch!
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
#if 0
      //Filter out prototypes when defining function declarations exist at the same time
      // This is now necessary since we always generate the first nondefining declaration in ROSE using EDG 4.4.
      //  We cannot do this since the call graph generator always tries to use first nondefining decl whenenver possible.
      //  This non-defining decl filter will essentially zero out the call graph.
      //Liao 1/23/2013
      if (funcDecl->get_definingDeclaration () != NULL)
        if (funcDecl->get_firstNondefiningDeclaration() == funcDecl)
          returnValue = false;
#endif
      return returnValue;
    }
};

int main(int argc, char * argv[]) {
    // Initialize and check compatibility. See rose::initialize
    ROSE_INITIALIZE;

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


