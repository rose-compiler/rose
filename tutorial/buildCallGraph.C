#include "rose.h"
#include <CallGraph.h>
#include <iostream>
using namespace std;
using namespace StringUtility;

// A Function object used as a predicate that determines which functions are 
// to be represented in the call graph.
struct keepFunction : public unary_function<bool,SgFunctionDeclaration*>
{
  bool operator()(SgFunctionDeclaration* funcDecl)
  {
    bool returnValue = true;
    ROSE_ASSERT(funcDecl != NULL);
    string filename = funcDecl->get_file_info()->get_filename();
    std::string func_name = funcDecl->get_name().getString();
    string stripped_file_name = stripPathFromFileName(filename);
    //string::size_type loc;

    //Filter out functions from the ROSE preinclude header file
    if(filename.find("rose_edg_required_macros_and_functions")!=string::npos)
      returnValue = false;
    //Filter out compiler generated functions
    else if(funcDecl->get_file_info()->isCompilerGenerated()==true)
      returnValue=false;
    //Filter out compiler generated functions
    else if(funcDecl->get_file_info()->isFrontendSpecific()==true)
      returnValue=false;
    // filter out other built in functions
    //      else if( func_name.find ("__",0)== 0);
    //         returnValue = false;
    // _IO_getc _IO_putc _IO_feof, etc.	
    //loc = func_name.find ("_IO_",0);
    //if (loc == 0 ) returnValue = false;

    // skip functions from standard system headers
    // TODO Need more rigid check
    else  if (
              stripped_file_name==string("stdio.h")||
              stripped_file_name==string("libio.h")||
              stripped_file_name==string("math.h")||
              stripped_file_name==string("time.h")||
              stripped_file_name==string("select.h")||
              stripped_file_name==string("mathcalls.h")
          )
      returnValue=false;
    if (returnValue)
      cout<<"Debug:"<<func_name << " from file:"<<stripped_file_name<<" Keep: "<<returnValue<<endl;
    return returnValue;
  }
};
int main( int argc, char * argv[] ) 
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  SgProject* project = new SgProject(argc, argv);
  ROSE_ASSERT (project != NULL);

  if (project->get_fileList().size() >=1)
  {
    //Construct a call Graph
    CallGraphBuilder CGBuilder(project);
//    CGBuilder.buildCallGraph(keepFunction());
    CGBuilder.buildCallGraph(builtinFilter());

    // Output to a dot file
    AstDOTGeneration dotgen;
    SgFilePtrList file_list = project->get_fileList();
    std::string firstFileName = StringUtility::stripPathFromFileName(file_list[0]->getFileName());
    dotgen.writeIncidenceGraphToDOTFile( CGBuilder.getGraph(), firstFileName+"_callGraph.dot");
  }

  return 0; // backend(project);
}
