#include "rose.h"
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
BufferOverflow::run(string& name, SgGraphNode* node,
		    SgGraphNode* previous){
  // check known function calls and resolve variables
  ROSE_ASSERT(node);
  ROSE_ASSERT(node->get_SgNode());
  //cerr << " bufferoverflow->run " << node->get_name() << endl;
  SgAsmx86Instruction* asmNode = isSgAsmx86Instruction(node->get_SgNode());
  if (asmNode==NULL)
    return false;

  // ANALYSIS 2 : BUFFER OVERFLOW DETECTION -------------------------------------------
  string callName = asmNode->get_comment();
  if (asmNode->get_kind() == x86_call && (
					  callName=="malloc" || callName=="calloc")) {
    // DEFINITION OF BUFFER OVERFLOW
    if (debug) 
      cerr << "    " << name << " : found malloc function call " << endl;
    // find the size of the malloc, = backward search within this function
    bool foundMov=false;
    SgGraphNode* pre = node;
    uint64_t value=0;
    while (foundMov!=true && sameParents(node, pre)) {
      pre = getPredecessor(pre);
      if (pre==NULL)
	break;
      SgAsmx86Instruction* asmPre = isSgAsmx86Instruction(pre->get_SgNode());
      if (asmPre && (asmPre->get_kind() == x86_mov || asmPre->get_kind() == x86_push)) {
	foundMov = true;
	if (asmPre->get_kind() == x86_mov) {
	  // make sure we are moving to the top of the stack, i.e. esp
	  bool memRef = false;
	  std::pair<X86RegisterClass, int> code;
	  code = check_isRegister(pre, asmPre, false, memRef);
	  string codeStr = unparseX86Register(RegisterDescriptor(code.first, code.second, 0, 64));
	  if (codeStr=="rdi")
	    value = getValueOfInstr(asmPre, true);
	  else if (debug)
	    cerr << " Error :: found a mov before a call that does not point to rdi but ::: " << codeStr << endl;
	  if (debug && asmPre->get_kind() == x86_mov) 
	    cerr << "   malloc: found mov size of " << codeStr << " in " << value 
                 << " for malloc call : " << unparseInstruction(asmPre) <<endl;
        } else if (asmPre->get_kind() == x86_push) {
	  value = getValueOfInstr(asmPre, false);
	  if (debug && asmPre->get_kind() == x86_push) 
	    cerr << "   malloc: found push size " << value << " for malloc call : " << unparseInstruction(asmPre) <<endl;
	}
      }
    } // while
    
    if (value==0)
      return false;

    // result of malloc (variable) is in eax, we need to see what the variable is and store it
    // forward search in the same function
    SgGraphNode* aft = node;
    ROSE_ASSERT(aft);
    ROSE_ASSERT(aft->get_SgNode());
    while (foundMov==true && sameParents(node, aft)) {
      ROSE_ASSERT(aft);
      //cerr << " ----------- Getting successor for " << aft->get_name() << endl;
      aft = getSuccessor(aft);
      ROSE_ASSERT(aft);
      SgAsmx86Instruction* asmAft = isSgAsmx86Instruction(aft->get_SgNode());
      if (asmAft->get_kind() == x86_mov) {
	foundMov = true;
	uint64_t address_of_var=0;
	bool memRef = false;
	std::pair<X86RegisterClass, int> code;
	code = check_isRegister(aft, asmAft, true, memRef);
	if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
	  if (debug && asmAft->get_kind() == x86_mov) 
	    cerr << "    found mov of eax of malloc call : " << unparseInstruction(asmAft) <<endl;
	  SgAsmMemoryReferenceExpression* memExpr = 
	    isSgAsmMemoryReferenceExpression(getOperand(asmAft,false));
	  if (memExpr) {
	    address_of_var = getValueInMemoryRefExp( memExpr->get_address());
	    if (debug) 
	      cerr << " The address of the malloc variable is : " << RoseBin_support::HexToString(address_of_var) << endl;
	    string functionName = "func";
	    functionName = RoseBin_support::HexToString(asmNode->get_address())+":";
	    uint64_t pos = asmAft->get_address();
	    vector<uint64_t> val_v ;
	    val_v.push_back(address_of_var);
	    vector<uint64_t> pos_v ;
	    pos_v.push_back(address_of_var);
	    if (debug)
	      cerr <<" Creating Variable : " << functionName << "malloc    address:" 
                   << RoseBin_support::HexToString(address_of_var) << endl;
            createVariable(pos, pos_v, functionName+"malloc", d_array, "Memory allocation", value, val_v,true); 		  
	    // done for now; 
	    return false;
	  }
	} else if (debug) 
	  cerr << " Error :: foud a mov after a call that is not rax." << endl;
      } // mov
    } // while
  } // if ==call



  // BUFFER OVERFLOW : Once we have the buffer overflow variable,
  // we are looking for the mov instruction, that moves the variable into eax
  // in order to access it.
  bool isDataTransfer = x86InstructionIsDataTransfer(asmNode);
  if (isDataTransfer ) {
    // USAGE OF BUFFER OVERFLOW
    bool memRef = false;
    uint64_t address_of_var=0;
    std::pair<X86RegisterClass, int> code;
    //    if (debug)
    // cerr << "  DataTransfer instr : " << RoseBin_support::HexToString(asmNode->get_address()) << " "<<unparseInstruction(asmNode) <<endl;
    // check if eax register on the left hand side
    code = check_isRegister(node, asmNode, false, memRef);
    if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
      // right hand side is Register Reg / MemoryRef
      SgAsmMemoryReferenceExpression* memExpr = 
	isSgAsmMemoryReferenceExpression(getOperand(asmNode,true));
      if (memExpr) {
	address_of_var = getValueInMemoryRefExp( memExpr->get_address());
	if (debug && isDataTransfer) {
	  cout << "  >>> malloc after:  found mov to eax  " << RoseBin_support::HexToString(asmNode->get_address()) 
               << " "<<unparseInstruction(asmNode) ;
          cout << "  LOOKING FOR malloc address :: " << RoseBin_support::HexToString(address_of_var) << endl;
	}
	RoseBin_Variable* var = getVariable(address_of_var);
	if (var) {
	  string varName = var->toString();
	  RoseBin_DataTypes::DataTypes type = var->getType();
	  bool array = false;
	  if (type==d_array)
	    array = true;
	  if (debug ) 
	    cerr << "  >>>>>> malloc after:  variable found :  " << varName << " array? " << RoseBin_support::resBool(array) 
                 << "    instr : " << unparseInstruction(asmNode) <<endl;
          // now that we have found the usage of an array, we check 
	  // in a forward analysis, whether we access a value that is greater than
	  // the length of the array
	  if (array) {
	    int length = var->getLength();
	    int arrayLength = 0;
	    bool foundMov=false;
	    SgGraphNode* aft = node;
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
		  if (debug && asmAft->get_kind() == x86_mov) {
		    cout << "   malloc - access to eax : " << unparseInstruction(asmAft) 
                         << "   length array (var) " << length << "  access array point: " << arrayLength  <<endl;
                  }
		  if (arrayLength> array) {
		    if (asmAft->get_kind() == x86_mov) {
		      if (debug)
			cerr << "  WARNING:: MALLOC - Buffer Overflow at : " << unparseInstruction(asmAft) 
                             <<  "  Length of array is " << length << "  but access at : " << arrayLength << endl;
                      string res = "possible buffer overflow : ";
		      res +=RoseBin_support::ToString(arrayLength)+">="+RoseBin_support::ToString(length);
		      string funcname="";
		      SgAsmBlock* b = isSgAsmBlock(asmAft->get_parent());
		      SgAsmFunctionDeclaration* func = NULL;
		      if (b)
			func=isSgAsmFunctionDeclaration(b->get_parent()); 
		      if (func)
			funcname = func->get_name();
		      res+=" ("+RoseBin_support::HexToString(asmAft->get_address())+") : "+unparseInstruction(asmAft)+
			" <"+asmAft->get_comment()+">  in function: "+funcname;
		      result[asmAft]= res;
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
      
  

  return false;
}


void
BufferOverflow::run(SgNode* fileA, SgNode* fileB) {
  instance = NULL;
  if (!testFlag)
    instance = QROSE::cbData<BinQGUI *>();
  if (isSgProject(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!" << endl;
    QString res = QString("This is not a valid file for this analysis");
    if (instance)
      instance->analysisResult->append(res);  
    return;
  }


  ROSE_ASSERT(isSgProject(fileA));
  SgBinaryComposite* binary = isSgBinaryComposite(isSgProject(fileA)->get_fileList()[0]);
  SgAsmGenericFile* file = binary != NULL ? binary->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  //  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  QString res = QString("Creating dataflow graph ");
  if (instance) {
    ROSE_ASSERT(instance);
    ROSE_ASSERT(instance->analysisTab);
    instance->analysisTab->setCurrentIndex(1);
    instance->analysisResult->append(res);  
  }

  bool dot=true;
  bool forward=true;
  bool edges=true;
  bool mergedEdges=true;
  bool interprocedural=false;
  string dfgFileName = "dfg.dot";
  if (graph==NULL) {
    //cerr << "No graph found yet .. creating graph " << endl;
    graph= new RoseBin_DotGraph();
    if (dot==false) {
      dfgFileName = "dfg.gml";
      graph= new RoseBin_GMLGraph();
    }
  } 


  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);
  GraphAlgorithms* algo = new GraphAlgorithms(info);
  SgAsmInterpretation* interp = SageInterface::getMainInterpretation(file);
  if (dfanalysis==NULL) {
    //cerr << "No dataflow analysis run yet ... running ... " << endl;
    dfanalysis = 
      new RoseBin_DataFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), algo);
    ROSE_ASSERT(dfanalysis);
    dfanalysis->writeToFile(false);
    dfanalysis->init(interprocedural, edges);
    dfanalysis->run(graph, dfgFileName, mergedEdges);
  } else {
    //cerr << "Dataflow analysis run before ... " << endl;
  }

  if (instance) {
    res = QString("nr of nodes visited %1. nr of edges visited %2. ")
      .arg(dfanalysis->nodesVisited())
      .arg(dfanalysis->edgesVisited());
    
    instance->analysisResult->append(res);  
    res = QString("Running BufferOverflow detection... ");
    instance->analysisResult->append(res);  
  }

  vector<SgGraphNode*> rootNodes;
  dfanalysis->getRootNodes(rootNodes);

  if (instance) {
    res = QString("Graph has Rootnodes : %1 ")
      .arg(rootNodes.size());
    instance->analysisResult->append(res);  
  }

  dfanalysis->init();
  if (instance) {
    res = QString("Initializing ... ");
    instance->analysisResult->append(res);  
  }
  init(graph);
  if (instance) {
    res = QString("Traversing ... ");
    instance->analysisResult->append(res);  
  }
  dfanalysis->traverseGraph(rootNodes, this, interprocedural);
  
  if (instance) {
    res = QString("Done. ");
    instance->analysisResult->append(res);  
  }  

  
}




void
BufferOverflow::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
