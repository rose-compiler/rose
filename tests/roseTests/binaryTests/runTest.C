#include <rose.h>
#include <string>

/* NOTE: This test is *very* sensitive to changes in the AST!  If you get a failed assertion then it's probably just because
 *       the arrangement of nodes changed. */

using namespace std;

/*******************************************
 * print Error
 *******************************************/
void printSet(std::set<uint64_t> res) {
  bool debug =false;
  std::set<uint64_t>::iterator it = res.begin();
  int i=0;
  for (;it!=res.end();++it) {
    i++;
    uint64_t nr  = *it;
    if (debug)
    cerr << "    Set " << RoseBin_support::ToString(i) << " = " << RoseBin_support::HexToString(nr) << " (" <<RoseBin_support::ToString(nr) << ")"<<endl;
  }
}

/*******************************************
 * check if definition of a node is correct (dataflow test)
 *******************************************/
void checkNode(RoseBin_DataFlowAnalysis* dfanalysis,
               uint64_t address, 
	       set<uint64_t>& result, 
               X86RegisterClass regclass,
               int regnum) {
  string registerName = unparseX86Register(regclass, regnum, x86_regpos_qword);
  SgGraphNode* node = dfanalysis->getNodeFor(address);
  SgAsmInstruction* inst = NULL;
  string unparsed ="";
  if (node) {
    inst = isSgAsmInstruction(node->get_SgNode());
    unparsed = unparseInstructionWithAddress(inst);
  }
  set<uint64_t> def_nodes = dfanalysis->getDefForInst(address,std::make_pair(regclass, regnum));
  //cerr << " testing address : " << RoseBin_support::HexToString(address) << " ("<<RoseBin_support::ToString(address)<<")" << 
  //  "  reg: " << registerName << "   .. " << unparsed << endl;

  if (def_nodes!=result) {
    cerr << " ERROR :: Test failed on " << RoseBin_support::HexToString(address) << endl;
    cerr << " Problem at : " << unparsed << endl;
    cerr << " Def Nodes : " << endl;
    printSet(def_nodes);
    cerr << " Result Nodes : " << endl;
    printSet(result);
  }
  ROSE_ASSERT(def_nodes == result);
  result.clear();
}


int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s executableName\n", argv[0]);
    return 1;
  }
  string execName = argv[1];

  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  SgBinaryComposite* binary = isSgBinaryComposite(project->get_fileList()[0]);

  const SgAsmInterpretationPtrList& interps = binary->get_interpretations()->get_interpretations();
  ROSE_ASSERT (interps.size() == 1);
  SgAsmInterpretation* interp = interps[0];
  unparseAsmStatementToFile("unparsed.s", interp->get_global_block());

  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  // objdumpToRoseBinaryAst(execName, file, project);

  //RoseBin_unparse* unparser = new RoseBin_unparse();
  //RoseBin_support::setUnparseVisitor(unparser->getVisitor());

  //cerr << " writing _binary_tree ... " << endl;
  string filename="_binary_tree.dot";
  //  set<SgNode*> skippedNodeSet;
  //SimpleColorFilesTraversal::generateGraph(globalBlock,filename,skippedNodeSet);
  AST_BIN_Traversal* trav = new AST_BIN_Traversal();
  trav->run(interp->get_global_block(), filename);



  // control flow analysis  *******************************************************
  bool forward = true;
  bool edges = true;
  bool mergedEdges = false;
  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(project);
  RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph();
  RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph();
  const char* cfgFileName = "cfg.dot";
  GraphAlgorithms* graphalgo = new GraphAlgorithms(info);
  RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(interp->get_global_block(), forward, 
                                                                             new RoseObj(), edges, graphalgo);
  cfganalysis->run(dotGraph, cfgFileName, mergedEdges);
  //cerr << " Number of nodes == " << cfganalysis->nodesVisited() << endl;
  //cerr << " Number of edges == " << cfganalysis->edgesVisited() << endl;

  // call graph analysis  *******************************************************
  //cerr << " creating call graph ... " << endl;
  const char* callFileName = "callgraph.gml";
  forward = true;
  RoseBin_CallGraphAnalysis* callanalysis = new RoseBin_CallGraphAnalysis(interp->get_global_block(), new RoseObj(), graphalgo);
  callanalysis->run(gmlGraph, callFileName, !mergedEdges);
  cerr << " Number of nodes == " << callanalysis->nodesVisited() << endl;
  cerr << " Number of edges == " << callanalysis->edgesVisited() << endl;
  cerr <<endl;

  ROSE_ASSERT(callanalysis->nodesVisited()==20 || callanalysis->nodesVisited()==18 ||
	      callanalysis->nodesVisited()==16 || callanalysis->nodesVisited()==10 || callanalysis->nodesVisited()==13 ||
              callanalysis->nodesVisited()==14 || callanalysis->nodesVisited()==21);
  ROSE_ASSERT(callanalysis->edgesVisited()==20 || callanalysis->edgesVisited()==8  || callanalysis->edgesVisited()==13 ||
              callanalysis->edgesVisited()==14 || callanalysis->edgesVisited()==18 || callanalysis->edgesVisited()==21 ||
              callanalysis->edgesVisited()==22);


    //cerr << " creating dataflow graph ... " << endl;
  string dfgFileName = "dfg.dot";
  forward = true;
  bool printEdges = true;
  bool interprocedural = true;
  RoseBin_DataFlowAnalysis* dfanalysis = new RoseBin_DataFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), graphalgo);
  dfanalysis->init(interprocedural, printEdges);
  dfanalysis->run(dotGraph, dfgFileName, mergedEdges);
#if 1
  cerr << " Number of nodes == " << dfanalysis->nodesVisited() << endl;
  cerr << " Number of edges == " << dfanalysis->edgesVisited() << endl;
  cerr << " Number of memWrites == " << dfanalysis->nrOfMemoryWrites() << endl;
  cerr << " Number of regWrites == " << dfanalysis->nrOfRegisterWrites() << endl;
  cerr << " Number of definitions == " << dfanalysis->nrOfDefinitions() << endl;
  cerr << " Number of uses == " << dfanalysis->nrOfUses() << endl;
#endif
  // values for old implementation -- using objdump


  // These assertions are very sensitive to the heuristics used to find instructions and functions.
  ROSE_ASSERT(dfanalysis->nodesVisited()==218 || dfanalysis->nodesVisited()==211 || dfanalysis->nodesVisited()==200 ||
              dfanalysis->nodesVisited()==209 || dfanalysis->nodesVisited()==210 || dfanalysis->nodesVisited()==240 ||
              dfanalysis->nodesVisited()==246 || dfanalysis->nodesVisited()==247 || dfanalysis->nodesVisited()==244 ||
              dfanalysis->nodesVisited()==235);
  ROSE_ASSERT(dfanalysis->edgesVisited()==233 || dfanalysis->edgesVisited()==252 || dfanalysis->edgesVisited()==240 ||
              dfanalysis->edgesVisited()==253 || dfanalysis->edgesVisited()==251 || dfanalysis->edgesVisited()==281 ||
              dfanalysis->edgesVisited()==282 || dfanalysis->edgesVisited()==288 || dfanalysis->edgesVisited()==289 ||
              dfanalysis->edgesVisited()==272);
  ROSE_ASSERT(dfanalysis->nrOfMemoryWrites()==14 || dfanalysis->nrOfMemoryWrites()==8 || dfanalysis->nrOfMemoryWrites()==12 ||
              dfanalysis->nrOfMemoryWrites()==18);
  ROSE_ASSERT(dfanalysis->nrOfRegisterWrites()==23 ||dfanalysis->nrOfRegisterWrites()==56 ||
              dfanalysis->nrOfRegisterWrites()==33 || dfanalysis->nrOfRegisterWrites()==37 ||
              dfanalysis->nrOfRegisterWrites()==38);
  ROSE_ASSERT(dfanalysis->nrOfDefinitions()==105 || dfanalysis->nrOfDefinitions()==161 || dfanalysis->nrOfDefinitions()==147 ||
              dfanalysis->nrOfDefinitions()==152 || dfanalysis->nrOfDefinitions()==150 || dfanalysis->nrOfDefinitions()==176 ||
              dfanalysis->nrOfDefinitions()==180 || dfanalysis->nrOfDefinitions()==159);
  ROSE_ASSERT(dfanalysis->nrOfUses()==15 || dfanalysis->nrOfUses()==24 || dfanalysis->nrOfUses()==23 ||
              dfanalysis->nrOfUses()==25);

  // detailed dfa test
  set<uint64_t> result;
  result.insert(RoseBin_support::HexToDec("8048364"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048364"), result, x86_regclass_gpr, x86_gpr_cx);
  result.insert(RoseBin_support::HexToDec("8048364"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("804836e"), result, x86_regclass_gpr, x86_gpr_cx);
  result.insert(RoseBin_support::HexToDec("804836e"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("804836e"), result, x86_regclass_gpr, x86_gpr_sp);

  // does not work anymore after symbol table info added
#if 0
  result.insert(RoseBin_support::HexToDec("80482c3"));
  result.insert(RoseBin_support::HexToDec("8048364"));
    checkNode(dfanalysis, RoseBin_support::HexToDec("8048290"), result, x86_regclass_gpr, x86_gpr_cx);
  result.insert(RoseBin_support::HexToDec("8048290"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048290"), result, x86_regclass_gpr, x86_gpr_sp);
  result.insert(RoseBin_support::HexToDec("80482dc"));
  result.insert(RoseBin_support::HexToDec("804837c"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048290"), result, x86_regclass_gpr, x86_gpr_bp);
#endif

  // (tps 05/23/08)  these cannot be tested in the new version because malloc analysis wont work until functions have names
  // comment back in once the symbol information is available for new disassembler
#define BUFFER_OVERFLOW 0
#if BUFFER_OVERFLOW
  result.insert(RoseBin_support::HexToDec("80483b2"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("80483bc"), result, x86_regclass_gpr, x86_gpr_ax);
  result.insert(RoseBin_support::HexToDec("80483bc"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("80483bc"), result, x86_regclass_gpr, x86_gpr_sp);
#endif

  result.insert(RoseBin_support::HexToDec("8048346"));
  result.insert(RoseBin_support::HexToDec("804834f"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048361"), result, x86_regclass_gpr, x86_gpr_ax);
  result.insert(RoseBin_support::HexToDec("8048361"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048361"), result, x86_regclass_gpr, x86_gpr_sp);

  result.insert(RoseBin_support::HexToDec("804846c"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("804828d"), result, x86_regclass_gpr, x86_gpr_ax);
  result.insert(RoseBin_support::HexToDec("8048464"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("804828d"), result, x86_regclass_gpr, x86_gpr_bx);
  result.insert(RoseBin_support::HexToDec("804828d"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("804828d"), result, x86_regclass_gpr, x86_gpr_sp);
  result.insert(RoseBin_support::HexToDec("804828d"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("804828d"), result, x86_regclass_gpr, x86_gpr_bp);

  // (tps 05/23/08)  these cannot be tested in the new version because malloc analysis wont work until functions have names
  // comment back in once the symbol information is available for new disassembler
#if BUFFER_OVERFLOW
  // buffer overflow analysis
  RoseBin_DataFlowAbstract* variableA =  dfanalysis->getVariableAnalysis();
  RoseBin_Variable* var = variableA->getVariable(RoseBin_support::HexToDec("8048381"));
  ROSE_ASSERT(var);
  //cerr << "malloc variable found : '" << var->getName() << "'  length: " << 
  //  RoseBin_support::ToString(var->getLength()) << " ... toString:: " << var->toString() << endl;
  ROSE_ASSERT(var->getLength()==40);
  ROSE_ASSERT(var->getName()==" 804837c:_malloc");
#endif

  unparseAsmStatementToFile("unparsed.s", interp->get_global_block());
  return 0;
}
