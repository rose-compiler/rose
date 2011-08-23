#include <rose.h>

using namespace std;

int main(int argc, char** argv)
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;

#if 1
     SgBinaryComposite* binary = isSgBinaryComposite(project->get_fileList()[0]);
     SgAsmGenericFile* file = binary != NULL ? binary->get_binaryFile() : NULL;

     ROSE_ASSERT (file != NULL);
     const SgAsmInterpretationPtrList& interps = binary->get_interpretations()->get_interpretations();
     ROSE_ASSERT (!interps.empty());
     SgAsmInterpretation* interp = interps.back();
     SgAsmBlock* global_block = interp->get_global_block();

     ROSE_ASSERT (global_block != NULL);

  // RoseBin_unparse* unparser = new RoseBin_unparse();
  // RoseBin_support::setUnparseVisitor(unparser->getVisitor());

  // Build the DOT file representing the AST (in color)
     cout << " writing _binary_tree ... " << endl;
     string filename="_binary_tree.dot";
     AST_BIN_Traversal* trav = new AST_BIN_Traversal();
     trav->run(global_block, filename);

  // control flow analysis  *******************************************************
  cout << " creating control flow graph ... " << endl;
  bool forward = true;
  bool edges = true;
  bool mergedEdges = false;
  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(project);
  RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph();
  RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph();
  const char* cfgFileName = "cfg.dot";
  GraphAlgorithms* algo = new GraphAlgorithms(info);
  RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(global_block, forward, new RoseObj(), edges, algo);
  cfganalysis->run(dotGraph, cfgFileName, mergedEdges);


  // call graph analysis  *******************************************************
  cout << " creating call graph ... " << endl;
  const char* callFileName = "callgraph.gml";
  forward = true;
  RoseBin_CallGraphAnalysis* callanalysis = new RoseBin_CallGraphAnalysis(global_block, new RoseObj(), algo);

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
  RoseBin_DataFlowAnalysis* dfanalysis = new RoseBin_DataFlowAnalysis(global_block, forward, new RoseObj(), algo);
  dfanalysis->init(interprocedural, printEdges);

  // Building a DOT file for the data-flow graph
     dfanalysis->run(dotGraph, dfgFileName, mergedEdges);
#endif

  // Unparse the output to test the unparser...
     return backend(project);
   }
