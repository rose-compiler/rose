#include "rose.h"
#include <CallGraph.h>
#include <iostream>
using namespace std;

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

int main( int argc, char * argv[] ) 
{
  SgProject* project = new SgProject(argc, argv);
  ROSE_ASSERT (project != NULL);

  if (project->get_fileList().size() >=1)
  {
    //Construct a call Graph
    CallGraphBuilder CGBuilder( project);
    CGBuilder.buildCallGraph(keepFunction());

    // Output to a dot file
    AstDOTGeneration dotgen;
    SgFilePtrList file_list = project->get_fileList();
    std::string firstFileName = StringUtility::stripPathFromFileName(file_list[0]->getFileName());
    dotgen.writeIncidenceGraphToDOTFile( CGBuilder.getGraph(), firstFileName+"_callGraph.dot");
  }

  return 0; // backend(project);
}
