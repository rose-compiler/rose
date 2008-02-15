#include <rose.h>
#include <string>
#include "objdumpToRoseBinaryAst.h"
#include "RoseBin_unparse.h"
#include "RoseBin_unparse.h"
#include "AST_BIN_Traversal.h"
#include "analyses/RoseBin_ControlFlowAnalysis.h"
//#include "graph/RosebGraph.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s executableName\n", argv[0]);
    return 1;
  }
  string execName = argv[1];

  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  SgAsmFile* file = objdumpToRoseBinaryAst(execName);
  ROSE_ASSERT (file);

  RoseBin_unparse* unparser = new RoseBin_unparse();
  RoseBin_support::setUnparseVisitor(unparser->getVisitor());

  cerr << " writing _binary_tree ... " << endl;
  string filename="_binary_tree.dot";
  //  set<SgNode*> skippedNodeSet;
  //SimpleColorFilesTraversal::generateGraph(globalBlock,filename,skippedNodeSet);
  AST_BIN_Traversal* trav = new AST_BIN_Traversal();
  trav->run(file->get_global_block(), filename);


  // control flow analysis  *******************************************************
  bool forward = true;
  bool edges = true;
  RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph();
  char* cfgFileName = "cfg.dot";
  RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(file->get_global_block(), forward, NULL, edges);
  cfganalysis->run(dotGraph, cfgFileName);

  trav->run(file->get_global_block(), filename);

  RoseBin_unparse up;
  up.init(file->get_global_block(), "unparsed.s");
  up.unparse();
  return 0;
}
