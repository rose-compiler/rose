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
    string output_file_name = StringUtility::stripPathFromFileName(proc->get_file_info()->get_filename());
    cout<<"Processing a function named "<<proc->get_mangled_name().getString()<<endl;
    output_file_name+="_"+proc->get_mangled_name().getString()+"_vcfg.dot";
    cout<<"dot file name is "<<output_file_name<<endl;
    std::ofstream graph(output_file_name.c_str());
    //std::ofstream graph("graph.dot");
    cfgToDotForDebugging(graph, "dotGraph", proc->cfgForBeginning());
  }
  return 0;
}

