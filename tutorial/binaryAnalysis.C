#include <rose.h>

using namespace std;

int main(int argc, char** argv)
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT

     RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;

#if 1
     SgAsmFile* file = project->get_file(0).get_binaryFile();
     ROSE_ASSERT (file != NULL);

     ROSE_ASSERT (file->get_global_block() != NULL);

  // RoseBin_unparse* unparser = new RoseBin_unparse();
  // RoseBin_support::setUnparseVisitor(unparser->getVisitor());

  // Build the DOT file representing the AST (in color)
     cout << " writing _binary_tree ... " << endl;
     string filename="_binary_tree.dot";
     AST_BIN_Traversal* trav = new AST_BIN_Traversal();
     trav->run(file->get_global_block(), filename);

  // control flow analysis  *******************************************************
  cout << " creating control flow graph ... " << endl;
  bool forward = true;
  bool edges = true;
  bool mergedEdges = false;
  RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph();
  RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph();
  char* cfgFileName = "cfg.dot";
  RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(file->get_global_block(), forward, new RoseObj(), edges);
  cfganalysis->run(dotGraph, cfgFileName, mergedEdges);


  // call graph analysis  *******************************************************
  cout << " creating call graph ... " << endl;
  char* callFileName = "callgraph.gml";
  forward = true;
  RoseBin_CallGraphAnalysis* callanalysis = new RoseBin_CallGraphAnalysis(file->get_global_block(), new RoseObj());

  // Building a GML file for the call graph
     callanalysis->run(gmlGraph, callFileName, !mergedEdges);

  // Building a DOT file for the call graph
  // callFileName = "callgraph.dot";
  // callanalysis->run(dotGraph, callFileName);


  // dataflow analysis  *******************************************************
  cout << " creating dataflow graph ... " << endl;
  string dfgFileName = "dfg.dot";
  forward = true;
  bool printEdges = true;
  bool interprocedural = true;
  RoseBin_DataFlowAnalysis* dfanalysis = new RoseBin_DataFlowAnalysis(file->get_global_block(), forward, new RoseObj());
  dfanalysis->init(interprocedural, printEdges);

  // Building a DOT file for the data-flow graph
     dfanalysis->run(dotGraph, dfgFileName, mergedEdges);
#endif

#else
  // Output an a message to make it clear that binary support is not available...
     cerr << "Binary Support in ROSE has not be configured (see configure options)" << endl;
#endif

  // Unparse the output to test the unparser...
     return backend(project);
   }
