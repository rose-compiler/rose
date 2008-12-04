#include "BinQGui.h"

#include <iostream>


#include "BinQSupport.h"
#include "slide.h"

#include <qtabwidget.h>
#include "BufferOverflow.h"


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;
using namespace  RoseBin_DataTypes;

std::string BufferOverflow::name() {
  return "Buffer Overflow Analysis";
}

std::string BufferOverflow::getDescription() {
  return "Runs Buffer Overflow analysis on binary.";
}


bool 
BufferOverflow::run(string& name, SgDirectedGraphNode* node,
			      SgDirectedGraphNode* previous){
  // check known function calls and resolve variables
  ROSE_ASSERT(node);

  //cerr << " bufferoverflow->run " << node->get_name() << endl;
  SgAsmx86Instruction* asmNode = isSgAsmx86Instruction(node->get_SgNode());
  if (asmNode) {

    // ANALYSIS 2 : BUFFER OVERFLOW DETECTION -------------------------------------------
      if (asmNode->get_kind() == x86_call) {
	// DEFINITION OF BUFFER OVERFLOW
	uint64_t malloc_pos = asmNode->get_address();
	SgAsmOperandList* opList = asmNode->get_operandList();
	ROSE_ASSERT(opList);
	SgAsmExpressionPtrList ptrList = opList->get_operands();
	// get the first (and only) element 
	if (ptrList.size()!=0) {
	SgAsmExpression* expr = *(ptrList.begin());
	string replace = expr->get_replacement();
	string op = unparseX86Expression(expr);

	// we can detect malloc with the help of ida.
	if (replace=="_malloc" || replace=="malloc@plt") {
	  if (RoseBin_support::DEBUG_MODE()) 
	    cerr << "    " << name << " : found malloc function call " << endl;
	  
	  // find the size of the malloc, = backward search within this function
	  bool foundMov=false;
	  SgDirectedGraphNode* pre = node;
	  uint64_t value=0;
	  while (foundMov!=true && sameParents(node, pre)) {
	    pre = getPredecessor(pre);
	    SgAsmx86Instruction* asmPre = isSgAsmx86Instruction(pre->get_SgNode());
	    if (asmPre->get_kind() == x86_mov || asmPre->get_kind() == x86_push) {
	      foundMov = true;
	      if (asmPre->get_kind() == x86_mov) {
		// make sure we are moving to the top of the stack, i.e. esp
		bool memRef = false;
		std::pair<X86RegisterClass, int> code;
		code = check_isRegister(pre, asmPre, false, memRef);
		string codeStr = unparseX86Register(code.first, code.second, x86_regpos_qword);
		if (codeStr=="rsp")
		  value = getValueOfInstr(asmPre, true);
		else 
		  cerr << " Error :: foud a mov before a call that does not point to rsp but ::: " << codeStr << endl;
		if (RoseBin_support::DEBUG_MODE() && asmPre->get_kind() == x86_mov) 
		  cerr << "   malloc: found mov size of " << codeStr << " in " << value << " for malloc call : " << unparseInstruction(asmPre) <<endl;
	      } else if (asmPre->get_kind() == x86_push) {
		value = getValueOfInstr(asmPre, false);
		if (RoseBin_support::DEBUG_MODE() && asmPre->get_kind() == x86_push) 
		  cerr << "   malloc: found push size " << value << " for malloc call : " << unparseInstruction(asmPre) <<endl;
	      }
	    }
	  }


	  // result of malloc (variable) is in eax, we need to see what the variable is and store it
	  // forward search in the same function
	  foundMov=false;
	  SgDirectedGraphNode* aft = node;
	  while (foundMov!=true && sameParents(node, aft)) {
	    aft = getSuccessor(aft);
	    SgAsmx86Instruction* asmAft = isSgAsmx86Instruction(aft->get_SgNode());
	    if (asmAft->get_kind() == x86_mov) {
	      foundMov = true;
	      uint64_t address_of_var=0;
		bool memRef = false;
		std::pair<X86RegisterClass, int> code;
		code = check_isRegister(aft, asmAft, true, memRef);
		if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
		  if (RoseBin_support::DEBUG_MODE() && asmAft->get_kind() == x86_mov) 
		    cerr << "    found mov of eax of malloc call : " << unparseInstruction(asmAft) <<endl;
		  SgAsmMemoryReferenceExpression* memExpr = 
		    isSgAsmMemoryReferenceExpression(getOperand(asmAft,false));
		  if (memExpr) {
		    //SgAsmRegisterReferenceExpression* refLeft = getRegister(memref->get_segment(),false);
		    
		    //SgAsmMemoryReferenceExpression* memExpr = 
		    //  isSgAsmMemoryReferenceExpression(refLeft->get_offset());
		    //if (memExpr)
		      address_of_var = getValueInMemoryRefExp( memExpr->get_address());
		    if (RoseBin_support::DEBUG_MODE()) 
		    cerr << " The address of the malloc variable is : " << RoseBin_support::HexToString(address_of_var) << endl;
		    string functionName = "func";
		    //SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(mov->get_parent());
		    //if (func)
		    //  functionName = func->get_name();
		    functionName = RoseBin_support::HexToString(malloc_pos)+":";
		    uint64_t pos = asmAft->get_address();
		    vector<uint64_t> val_v ;
		    val_v.push_back(address_of_var);
		    vector<uint64_t> pos_v ;
		    pos_v.push_back(address_of_var);
		    //RoseBin_Variable* var = 
		    createVariable(pos, pos_v, functionName+"_malloc", d_array, "Memory allocation", value, val_v,true); 		  
		    //string varStr = var->toString();
		    //aft->append_properties(RoseBin_Def::dfa_variable,varStr);		  
		  }
		} else 
		  cerr << " Error :: foud a mov after a call that is not rax." << endl;
	    } // mov
	  } // while
	}	  
	}
      }

    // BUFFER OVERFLOW : Once we have the buffer overflow variable,
    // we are looking for the mov instruction, that moves the variable into eax
    // in order to access it.
    //      if (isSgAsmMov(asmNode) ) {
    bool isDataTransfer = x86InstructionIsDataTransfer(asmNode);
    if (isDataTransfer ) {
      // USAGE OF BUFFER OVERFLOW
      // Should this be only for mov instructions?
	if (isDataTransfer) {
	bool memRef = false;
	uint64_t address_of_var=0;
	std::pair<X86RegisterClass, int> code;
	// check if eax register on the left hand side
	code = check_isRegister(node, asmNode, false, memRef);
	if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
	  // right hand side is Register Reg / MemoryRef
	  //	  SgAsmRegisterReferenceExpression* refRight = getRegister(mov,true);
	  //if (refRight) {
	    SgAsmMemoryReferenceExpression* memExpr = 
	      isSgAsmMemoryReferenceExpression(getOperand(asmNode,true));
	    if (memExpr) {
	      address_of_var = getValueInMemoryRefExp( memExpr->get_address());
	    if (RoseBin_support::DEBUG_MODE() && isDataTransfer) {
	      cout << "  malloc:  found mov to eax  " << unparseInstruction(asmNode) ;
	      cout << "  malloc address ::  : " << RoseBin_support::HexToString(address_of_var) << endl;
	    }
	    RoseBin_Variable* var = getVariable(address_of_var);
	    if (var) {
	      string varName = var->toString();
	      RoseBin_DataTypes::DataTypes type = var->getType();
	      bool array = false;
	      if (type==d_array)
		array = true;
	      if (RoseBin_support::DEBUG_MODE() ) 
		cerr << "  malloc:  variable found :  " << varName << " array? " << RoseBin_support::resBool(array) 
	             << "    instr : " << unparseInstruction(asmNode) <<endl;
	      // now that we have found the usage of an array, we check 
	      // in a forward analysis, whether we access a value that is greater than
	      // the length of the array
	      if (array) {
		int length = var->getLength();
		int arrayLength = 0;
		bool foundMov=false;
		SgDirectedGraphNode* aft = node;
		while (foundMov!=true && sameParents(node, aft)) {
		  aft = getSuccessor(aft);
		  SgAsmx86Instruction* asmAft = isSgAsmx86Instruction(aft->get_SgNode());
		  if (asmAft->get_kind() == x86_add) {
		    bool memRef = false;
		    std::pair<X86RegisterClass, int> code;
		    code = check_isRegister(aft, asmAft, false, memRef);
		    if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
		      uint64_t val = getValueOfInstr(asmAft, true);
		      arrayLength += val;
		    }
		  }		  
		  if (asmAft->get_kind() == x86_mov) {
		    foundMov = true;
		    bool memRef = false;
		    std::pair<X86RegisterClass, int> code;
		    code = check_isRegister(aft, asmAft, true, memRef);
		    if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
		      if (RoseBin_support::DEBUG_MODE() && asmAft->get_kind() == x86_mov) {
			cout << "   malloc - access to eax : " << unparseInstruction(asmAft) 
			     << "   length array (var) " << length << "  access array point: " << arrayLength  <<endl;
		      }
		      if (arrayLength> array) {
			if (RoseBin_support::DEBUG_MODE() && asmAft->get_kind() == x86_mov) {
			  cerr << "  WARNING:: MALLOC - Buffer Overflow at : " << unparseInstruction(asmAft) 
			       <<  "  Length of array is " << length << "  but access at : " << arrayLength << endl;
			  aft->append_properties(RoseBin_Def::dfa_bufferoverflow,varName);		  
			}
		      }
		    }
		  } 
		} // while
		
	      }
	    }
	  } // refRight
	}
	} // mov
    }

  }
  return false;
}


void
BufferOverflow::run(SgNode* fileA, SgNode* fileB) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  if (isSgProject(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!" << endl;
    QString res = QString("This is not a valid file for this analysis");
    instance->analysisResult->append(res);  
    return;
  }

  RoseBin_Graph* graph=NULL;
  ROSE_ASSERT(isSgProject(fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  ROSE_ASSERT(instance);
  ROSE_ASSERT(instance->analysisTab);
  instance->analysisTab->setCurrentIndex(1);
  QString res = QString("Creating dataflow graph ");
  instance->analysisResult->append(res);  
  
  bool dot=true;
  bool forward=true;
  bool edges=true;
  bool mergedEdges=true;
  bool interprocedural=false;
  string dfgFileName = "dfg.dot";
  graph= new RoseBin_DotGraph(info);
  if (dot==false) {
    dfgFileName = "dfg.gml";
    graph= new RoseBin_GMLGraph(info);
  }

  SgAsmInterpretation* interp = SageInterface::getMainInterpretation(file);
  RoseBin_DataFlowAnalysis* dfanalysis = 
    new RoseBin_DataFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), info);
  ROSE_ASSERT(dfanalysis);
  dfanalysis->init(interprocedural, edges);
  dfanalysis->run(graph, dfgFileName, mergedEdges);

  res = QString("nr of nodes visited %1. nr of edges visited %2. ")
    .arg(dfanalysis->nodesVisited())
    .arg(dfanalysis->edgesVisited());
    
  instance->analysisResult->append(res);  

  res = QString("Running BufferOverflow detection... ");
  instance->analysisResult->append(res);  

  vector<SgDirectedGraphNode*> rootNodes;
  dfanalysis->getRootNodes(rootNodes);

  res = QString("Graph has Rootnodes : %1 ")
    .arg(rootNodes.size());
  instance->analysisResult->append(res);  

  dfanalysis->init();
  res = QString("Initializing ... ");
  instance->analysisResult->append(res);  
  init(graph);
  res = QString("Traversing ... ");
  instance->analysisResult->append(res);  
  dfanalysis->traverseGraph(rootNodes, this, interprocedural);
  
  res = QString("Done. ");
  instance->analysisResult->append(res);  
  

  
}




void
BufferOverflow::test(SgNode* fileA, SgNode* fileB) {

  RoseBin_Graph* graph=NULL;
  ROSE_ASSERT(isSgProject(fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  bool dot=true;
  bool forward=true;
  bool edges=true;
  bool mergedEdges=true;
  bool interprocedural=false;
  string dfgFileName = "dfg.dot";
  graph= new RoseBin_DotGraph(info);
  if (dot==false) {
    dfgFileName = "dfg.gml";
    graph= new RoseBin_GMLGraph(info);
  }

  SgAsmInterpretation* interp = SageInterface::getMainInterpretation(file);
  RoseBin_DataFlowAnalysis* dfanalysis = 
    new RoseBin_DataFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), info);
  ROSE_ASSERT(dfanalysis);
  dfanalysis->init(interprocedural, edges);
  dfanalysis->run(graph, dfgFileName, mergedEdges);


  vector<SgDirectedGraphNode*> rootNodes;
  dfanalysis->getRootNodes(rootNodes);


  dfanalysis->init();
  init(graph);
  dfanalysis->traverseGraph(rootNodes, this, interprocedural);

}
