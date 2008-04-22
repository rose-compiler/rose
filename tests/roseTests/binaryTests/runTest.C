#include <rose.h>
#include <string>

using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s executableName\n", argv[0]);
    return 1;
  }
  string execName = argv[1];

  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  SgAsmFile* file = project->get_file(0).get_binaryFile();

  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  objdumpToRoseBinaryAst(execName, file, project);
  ROSE_ASSERT (file);

  //RoseBin_unparse* unparser = new RoseBin_unparse();
  //RoseBin_support::setUnparseVisitor(unparser->getVisitor());

  cerr << " writing _binary_tree ... " << endl;
  string filename="_binary_tree.dot";
  //  set<SgNode*> skippedNodeSet;
  //SimpleColorFilesTraversal::generateGraph(globalBlock,filename,skippedNodeSet);
  AST_BIN_Traversal* trav = new AST_BIN_Traversal();
  trav->run(file->get_global_block(), filename);



  // control flow analysis  *******************************************************
  bool forward = true;
  bool edges = true;
  bool mergedEdges = false;
  RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph();
  RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph();
  char* cfgFileName = "cfg.dot";
  RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(file->get_global_block(), forward, new RoseObj(), edges);
  cfganalysis->run(dotGraph, cfgFileName, mergedEdges);
  cout << " Number of nodes == " << cfganalysis->nodesVisited() << endl;
  cout << " Number of edges == " << cfganalysis->edgesVisited() << endl;
  ROSE_ASSERT(cfganalysis->nodesVisited()==209);
  ROSE_ASSERT(cfganalysis->edgesVisited()==232);

  // call graph analysis  *******************************************************
  cerr << " creating call graph ... " << endl;
  char* callFileName = "callgraph.gml";
  forward = true;
  RoseBin_CallGraphAnalysis* callanalysis = new RoseBin_CallGraphAnalysis(file->get_global_block(), new RoseObj());
  callanalysis->run(gmlGraph, callFileName, !mergedEdges);
  cout << " Number of nodes == " << callanalysis->nodesVisited() << endl;
  cout << " Number of edges == " << callanalysis->edgesVisited() << endl;
  ROSE_ASSERT(callanalysis->nodesVisited()==10);
  ROSE_ASSERT(callanalysis->edgesVisited()==7);


  cerr << " creating dataflow graph ... " << endl;
  string dfgFileName = "dfg.dot";
  forward = true;
  bool printEdges = true;
  bool interprocedural = true;
  RoseBin_DataFlowAnalysis* dfanalysis = new RoseBin_DataFlowAnalysis(file->get_global_block(), forward, new RoseObj());
  dfanalysis->init(interprocedural, printEdges);
  dfanalysis->run(dotGraph, dfgFileName, mergedEdges);
  cout << " Number of nodes == " << dfanalysis->nodesVisited() << endl;
  cout << " Number of edges == " << dfanalysis->edgesVisited() << endl;
  cout << " Number of memWrites == " << dfanalysis->nrOfMemoryWrites() << endl;
  cout << " Number of regWrites == " << dfanalysis->nrOfRegisterWrites() << endl;
  cout << " Number of definitions == " << dfanalysis->nrOfDefinitions() << endl;
  cout << " Number of uses == " << dfanalysis->nrOfUses() << endl;
  ROSE_ASSERT(dfanalysis->nodesVisited()==209);
  ROSE_ASSERT(dfanalysis->edgesVisited()==255);
  ROSE_ASSERT(dfanalysis->nrOfMemoryWrites()==18);
  ROSE_ASSERT(dfanalysis->nrOfRegisterWrites()==45);
  ROSE_ASSERT(dfanalysis->nrOfDefinitions()==176);
  ROSE_ASSERT(dfanalysis->nrOfUses()==26);



  RoseBin_unparse up;
  up.init(file->get_global_block(), "unparsed.s");
  up.unparse();
  return 0;
}
