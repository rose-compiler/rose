// Virtual CFG generator
// Used as a standalone tool for user
// Installed under rose_ins/bin
// This program will find all function definitions within input files
// and generate virtual control flow graph's dot files for each of them.
// 
// filename_mangledFunctionName.dot
// Liao 1/27/2009

#include "rose.h"
//#include <algorithm>
using namespace std;
using namespace VirtualCFG;

int main(int argc, char *argv[]) 
{
  SgProject* sageProject = frontend(argc,argv);
  AstTests::runAllTests(sageProject);

  Rose_STL_Container <SgNode*> functions = NodeQuery::querySubTree(sageProject, V_SgFunctionDefinition);

  for (Rose_STL_Container <SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) 
  {
    SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
    ROSE_ASSERT (proc);
    string file_name = StringUtility::stripPathFromFileName(proc->get_file_info()->get_filename());
    string file_func_name= file_name+ "_"+proc->get_mangled_name().getString();
    
    string full_output = file_func_name +"_vcfg.dot";
    std::ofstream graph(full_output.c_str());
    //std::ofstream graph("graph.dot");
    // Full CFG graph
    cfgToDotForDebugging(graph, "dotGraph", proc->cfgForBeginning());

#if 0 // not ready
    string simple_output = file_func_name +"_simple_vcfg.dot";
    std::ofstream simplegraph(simple_output.c_str());
    // Simplified CFG suitable for most analyses
    // This will cause assertion failure
    //VirtualCFG::cfgToDot(simplegraph, proc->get_declaration()->get_name(), VirtualCFG::makeInterestingCfg(proc)); 
    // This will generate identical graph as cfgToDotForDebugging ()
    VirtualCFG::cfgToDot(simplegraph, proc->get_declaration()->get_name(), proc->cfgForBeginning());
#endif    
  }
  return 0;
}

