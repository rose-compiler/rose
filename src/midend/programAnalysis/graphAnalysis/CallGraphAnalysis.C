// tps : Switching from rose.h to sage3 changed size from 19,4 MB to 10MB
#include "sage3basic.h"
#include "CallGraphAnalysis.h"

using namespace std;

void
SB_CallGraph::createCallGraph(SgProject* project) {


  const char* callFileNameGML = "callgraph.gml";
  const char* callFileNameDOT = "callgraph.dot";
  bool mergedEdges = false;

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  SgBinaryComposite* binary = isSgBinaryComposite(project->get_fileList()[0]);
  SgAsmGenericFile* file = binary != NULL ? binary->get_binaryFile() : NULL;
  if (file) {
    // binary analysis
    const SgAsmInterpretationPtrList& interps = binary->get_interpretations()->get_interpretations();
    ROSE_ASSERT (interps.size() == 1);
    SgAsmInterpretation* interp = interps[0];
    SgAsmBlock* global_block = interp->get_global_block();
    ROSE_ASSERT (global_block != NULL);
    VirtualBinCFG::AuxiliaryInformation* info= new VirtualBinCFG::AuxiliaryInformation(project);
    RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph();
    RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph();
    cout << " creating call graph ... " << endl;
    GraphAlgorithms* algo = new GraphAlgorithms(info);
    bin_callanalysis = new RoseBin_CallGraphAnalysis(global_block, new RoseObj(), algo);
    // Building a GML file for the call graph
    bin_callanalysis->run(gmlGraph, callFileNameGML, !mergedEdges);
    // Building a DOT file for the call graph
    bin_callanalysis->run(dotGraph, callFileNameDOT, !mergedEdges);

  } else {
    // source code
    //VirtualCFG::AuxiliaryInformation* info= new VirtualCFG::AuxiliaryInformation(project);
#if 0
    RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph(info);
    RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph(info);
    cout << " creating call graph ... " << endl;
    RoseBin_CallGraphAnalysis* callanalysis = new RoseBin_CallGraphAnalysis(global_block, new RoseObj(), info);
    // Building a GML file for the call graph
    callanalysis->run(gmlGraph, callFileNameGML, !mergedEdges);
    // Building a DOT file for the call graph
    callanalysis->run(dotGraph, callFileNameDOT);


#endif
  }

#else
     printf ("Error: ROSE not configured for binary analysis (this is a language specific build) \n");
     ROSE_ASSERT(false);
#endif


}

