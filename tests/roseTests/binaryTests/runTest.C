#include <rose.h>
#include <string>

using namespace std;

/*******************************************
 * print Error
 *******************************************/
void printSet(std::set<uint64_t> res) {
  std::set<uint64_t>::iterator it = res.begin();
  int i=0;
  for (;it!=res.end();++it) {
    i++;
    uint64_t nr  = *it;
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
  SgDirectedGraphNode* node = dfanalysis->getNodeFor(address);
  SgAsmInstruction* inst = NULL;
  string unparsed ="";
  if (node) {
    inst = isSgAsmInstruction(node->get_SgNode());
    unparsed = unparseInstructionWithAddress(inst);
  }
  set<uint64_t> def_nodes = dfanalysis->getDefForInst(address,std::make_pair(regclass, regnum));
  cerr << " testing address : " << RoseBin_support::HexToString(address) << " ("<<RoseBin_support::ToString(address)<<")" << 
    "  reg: " << registerName << "   .. " << unparsed << endl;

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
  SgAsmFile* file = project->get_file(0).get_binaryFile();

  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  // objdumpToRoseBinaryAst(execName, file, project);
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
  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(project);
  RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph(info);
  RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph(info);
  char* cfgFileName = "cfg.dot";
  RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(file->get_global_block(), forward, new RoseObj(), edges, info);
  cfganalysis->run(dotGraph, cfgFileName, mergedEdges);
  cout << " Number of nodes == " << cfganalysis->nodesVisited() << endl;
  cout << " Number of edges == " << cfganalysis->edgesVisited() << endl;
  ROSE_ASSERT(cfganalysis->nodesVisited()==237);
  //ROSE_ASSERT(cfganalysis->nodesVisited()==209); // -- old implementation
  ROSE_ASSERT(cfganalysis->edgesVisited()==261);
  //ROSE_ASSERT(cfganalysis->edgesVisited()==232); // -- old implementation

  // call graph analysis  *******************************************************
  cerr << " creating call graph ... " << endl;
  char* callFileName = "callgraph.gml";
  forward = true;
  RoseBin_CallGraphAnalysis* callanalysis = new RoseBin_CallGraphAnalysis(file->get_global_block(), new RoseObj(), info);
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
  RoseBin_DataFlowAnalysis* dfanalysis = new RoseBin_DataFlowAnalysis(file->get_global_block(), forward, new RoseObj(), info);
  dfanalysis->init(interprocedural, printEdges);
  dfanalysis->run(dotGraph, dfgFileName, mergedEdges);
  cout << " Number of nodes == " << dfanalysis->nodesVisited() << endl;
  cout << " Number of edges == " << dfanalysis->edgesVisited() << endl;
  cout << " Number of memWrites == " << dfanalysis->nrOfMemoryWrites() << endl;
  cout << " Number of regWrites == " << dfanalysis->nrOfRegisterWrites() << endl;
  cout << " Number of definitions == " << dfanalysis->nrOfDefinitions() << endl;
  cout << " Number of uses == " << dfanalysis->nrOfUses() << endl;
  // values for old implementation -- using objdump
  //ROSE_ASSERT(dfanalysis->nodesVisited()==209);
  //ROSE_ASSERT(dfanalysis->edgesVisited()==255);
  //ROSE_ASSERT(dfanalysis->nrOfMemoryWrites()==18);
  //ROSE_ASSERT(dfanalysis->nrOfRegisterWrites()==45);
  //ROSE_ASSERT(dfanalysis->nrOfDefinitions()==176);
  //ROSE_ASSERT(dfanalysis->nrOfUses()==26);

  ROSE_ASSERT(dfanalysis->nodesVisited()==237);
  ROSE_ASSERT(dfanalysis->edgesVisited()==284);
  ROSE_ASSERT(dfanalysis->nrOfMemoryWrites()==12);
  ROSE_ASSERT(dfanalysis->nrOfRegisterWrites()==36);
  ROSE_ASSERT(dfanalysis->nrOfDefinitions()==183);
  ROSE_ASSERT(dfanalysis->nrOfUses()==25);


  // detailed dfa test
  set<uint64_t> result;
  result.insert(RoseBin_support::HexToDec("8048364"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048364"), result, x86_regclass_gpr, x86_gpr_cx);
  result.insert(RoseBin_support::HexToDec("8048364"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("804836e"), result, x86_regclass_gpr, x86_gpr_cx);
  result.insert(RoseBin_support::HexToDec("804836e"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("804836e"), result, x86_regclass_gpr, x86_gpr_sp);

  result.insert(RoseBin_support::HexToDec("80482c3"));
  result.insert(RoseBin_support::HexToDec("8048364"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048290"), result, x86_regclass_gpr, x86_gpr_cx);
  result.insert(RoseBin_support::HexToDec("8048290"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048290"), result, x86_regclass_gpr, x86_gpr_sp);
  result.insert(RoseBin_support::HexToDec("80482dc"));
  result.insert(RoseBin_support::HexToDec("804837c"));
  checkNode(dfanalysis, RoseBin_support::HexToDec("8048290"), result, x86_regclass_gpr, x86_gpr_bp);

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
  cerr << "malloc variable found : '" << var->getName() << "'  length: " << 
    RoseBin_support::ToString(var->getLength()) << " ... toString:: " << var->toString() << endl;
  ROSE_ASSERT(var->getLength()==40);
  ROSE_ASSERT(var->getName()==" 804837c:_malloc");
#endif

  unparseAsmStatementToFile("unparsed.s", file->get_global_block());
  return 0;
}
