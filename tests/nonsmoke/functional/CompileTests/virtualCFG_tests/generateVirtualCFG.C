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
using namespace Rose;
using namespace VirtualCFG;

int main(int argc, char *argv[]) 
{
  bool generateInterestingGraph = false; 
  vector <string> argvList (argv, argv+argc);
  generateInterestingGraph = CommandlineProcessing::isOption (argvList,"-rose:", "interesting",true);
  
  bool showHelp = CommandlineProcessing::isOption (argvList,"", "--help",false)|| 
                CommandlineProcessing::isOption (argvList,"", "-h",false) ||
                CommandlineProcessing::isOption (argvList,"", "-help",false);
  if (showHelp)              
   {
     printf ("To show full virtual control flow graph of each function within an input file:\n");
     printf ("\t %s your_input_file\n",argv[0]);
     printf ("To show both full and interesting virtual control flow graphs of each function:\n");
     printf ("\t %s -rose:interesting your_input_file\n",argv[0]);
   } 

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

    if (generateInterestingGraph)
    {
      // not ready, use a command line option to control it, Liao 12/15/2011
      string simple_output = file_func_name +"_simple_vcfg.dot";
      std::ofstream simplegraph(simple_output.c_str());
      // Simplified CFG suitable for most analyses
      // This may cause assertion failure
      VirtualCFG::cfgToDot(simplegraph, proc->get_declaration()->get_name(), VirtualCFG::makeInterestingCfg(proc)); 
    }
  }
  return 0;
}

