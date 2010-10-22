// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Binary Buffer Overflow Analysis
// Author: Thomas Panas
// Date: 18-August-2008

#include "rose.h"
#include "compass.h"

using namespace std;

extern const Compass::CheckerUsingGraphProcessing* const binaryBufferOverflowChecker;

namespace CompassAnalyses
{ 
  namespace BinaryBufferOverflow
  { 
    /*! \brief Binary Buffer Overflow: Add your description here 
     */

    // Specification of Checker Output Implementation
    class CheckerOutput: public Compass::OutputViolationBase
    { 
    public:
      CheckerOutput(SgNode* node);
    };

    // Specification of Checker Traversal Implementation

    class Traversal
      : public Compass::GraphProcessingWithRunFunction //RoseBin_DataFlowAbstract
    {
      Compass::OutputObject* output;
      // Checker specific parameters should be allocated here.

    public:
      Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

      // Change the implementation of this function if you are using inherited attributes.
      void *initialInheritedAttribute() const { return NULL; }

      // The implementation of the run function has to match the traversal being called.
      bool run(string& name, SgGraphNode* node, SgGraphNode* previous);

      bool runEdge(SgGraphNode* node, SgGraphNode* next) {
        return false;
      }
      bool checkIfValidCycle(SgGraphNode* node, SgGraphNode* next);
               
      void init(RoseBin_Graph* vg) {
        vizzGraph = vg;
      }
      
      void run(SgNode*);

    };
  }
}

CompassAnalyses::BinaryBufferOverflow::
CheckerOutput::CheckerOutput ( SgNode* node )
  : OutputViolationBase(node,::binaryBufferOverflowChecker->checkerName,::binaryBufferOverflowChecker->shortDescription)
{}

CompassAnalyses::BinaryBufferOverflow::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BinaryBufferOverflow.YourParameter"]);

}

void
CompassAnalyses::BinaryBufferOverflow::
Traversal::run ( SgNode* node )
{
  /*
  SgAsmGenericFile* file = Compass::projectPrerequisite.getProject()->get_file(0).get_binaryFile();
  if (file==NULL)
    return;

  CompassAnalyses::BinaryBufferOverflow::Traversal checker(params, output);
  checker.init(Compass::binDataFlowPrerequisite.getGraph());

  bool interprocedural = false;
  RoseBin_DataFlowAnalysis* dfanalysis = Compass::binDataFlowPrerequisite.getBinDataFlowInfo();
  vector<SgGraphNode*> rootNodes;
  dfanalysis->getRootNodes(rootNodes);
  dfanalysis->init();
  
  dfanalysis->traverseGraph(rootNodes, &checker, interprocedural);
  */
}


static void run(Compass::Parameters params, Compass::OutputObject* output) {
  //  CompassAnalyses::BinaryBufferOverflow::Traversal(params, output).run(Compass::projectPrerequisite.getProject());

  SgBinaryComposite* binary = isSgBinaryComposite(Compass::projectPrerequisite.getProject()->get_fileList()[0]);
  SgAsmGenericFile* file = binary != NULL ? binary->get_binaryFile() : NULL;

  if (file==NULL)
    return;

  CompassAnalyses::BinaryBufferOverflow::Traversal checker(params, output);
  checker.init(Compass::binDataFlowPrerequisite.getGraph());

  bool interprocedural = false;
  RoseBin_DataFlowAnalysis* dfanalysis = Compass::binDataFlowPrerequisite.getBinDataFlowInfo();
  vector<SgGraphNode*> rootNodes;
  dfanalysis->getRootNodes(rootNodes);
  dfanalysis->init();
  
  dfanalysis->traverseGraph(rootNodes, &checker, interprocedural);

}

static Compass::PrerequisiteList getPrerequisites() {
  Compass::PrerequisiteList defusePre;
  defusePre.push_back(&Compass::projectPrerequisite);
  defusePre.push_back(&Compass::binDataFlowPrerequisite);
  return defusePre;
}


// Remove this function if your checker is not an AST traversal
static Compass::GraphProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  std::cerr << " creating bufferOverflow traversal " << std::endl;
  return new CompassAnalyses::BinaryBufferOverflow::Traversal(params, output);
}

extern const Compass::CheckerUsingGraphProcessing* const binaryBufferOverflowChecker =
  new Compass::CheckerUsingGraphProcessing(
                       "BinaryBufferOverflow",
                       // Descriptions should not include the newline character "\n".
                       "Short description not written yet!",
                       "Long description not written yet!",
                       Compass::X86Assembly,
                       getPrerequisites(),
                       run,
                       createTraversal);
   




using namespace std;
using namespace RoseBin_DataTypes;
using namespace RoseBin_OS_VER;
using namespace RoseBin_Arch;



bool 
CompassAnalyses::BinaryBufferOverflow::Traversal::run(string& name, SgGraphNode* node,
                                                      SgGraphNode* previous){
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
	string op = unparseExpression(expr);

	// we can detect malloc with the help of ida.
	if (replace=="_malloc" || replace=="malloc@plt") {
	  if (RoseBin_support::DEBUG_MODE()) 
	    cerr << "    " << name << " : found malloc function call " << endl;
	  
	  // find the size of the malloc, = backward search within this function
	  bool foundMov=false;
	  SgGraphNode* pre = node;
	  uint64_t value=0;
	  while (foundMov!=true && sameParents(node, pre)) {
	    pre = getPredecessor(pre);
	    SgAsmx86Instruction* asmPre = isSgAsmx86Instruction(pre->get_SgNode());
	    if (asmPre->get_kind() == x86_mov || asmPre->get_kind() == x86_push) {
	      foundMov = true;
	      if (asmPre->get_kind() == x86_mov) {
		// make sure we are moving to the top of the stack, i.e. esp
		bool memRef = false, regRef = false;
		std::pair<X86RegisterClass, int> code;
		code = check_isRegister(pre, asmPre, false, memRef, regRef);
		string codeStr = unparseX86Register(RegisterDescriptor(code.first, code.second, 0, 64));
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
	  SgGraphNode* aft = node;
	  while (foundMov!=true && sameParents(node, aft)) {
	    aft = getSuccessor(aft);
	    SgAsmx86Instruction* asmAft = isSgAsmx86Instruction(aft->get_SgNode());
	    if (asmAft->get_kind() == x86_mov) {
	      foundMov = true;
	      uint64_t address_of_var=0;
              bool memRef = false, regRef = false;
              std::pair<X86RegisterClass, int> code;
              code = check_isRegister(aft, asmAft, true, memRef, regRef);
              if (regRef && code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
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
	bool memRef = false, regRef = false;
	uint64_t address_of_var=0;
	std::pair<X86RegisterClass, int> code;
	// check if eax register on the left hand side
	code = check_isRegister(node, asmNode, false, memRef, regRef);
	if (regRef && code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
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
		SgGraphNode* aft = node;
		while (foundMov!=true && sameParents(node, aft)) {
		  aft = getSuccessor(aft);
		  SgAsmx86Instruction* asmAft = isSgAsmx86Instruction(aft->get_SgNode());
		  if (asmAft->get_kind() == x86_add) {
		    bool memRef = false, regRef = false;
		    std::pair<X86RegisterClass, int> code;
		    code = check_isRegister(aft, asmAft, false, memRef, regRef);
		    if (regRef && code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
		      uint64_t val = getValueOfInstr(asmAft, true);
		      arrayLength += val;
		    }
		  }		  
		  if (asmAft->get_kind() == x86_mov) {
		    foundMov = true;
		    bool memRef = false, regRef = false;
		    std::pair<X86RegisterClass, int> code;
		    code = check_isRegister(aft, asmAft, true, memRef, regRef);
		    if (regRef && code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
		      if (RoseBin_support::DEBUG_MODE() && asmAft->get_kind() == x86_mov) {
			cout << "   malloc - access to eax : " << unparseInstruction(asmAft) 
                             << "   length array (var) " << length << "  access array point: " << arrayLength  <<endl;
                      }
		      if (arrayLength> array) {
			if (RoseBin_support::DEBUG_MODE() && asmAft->get_kind() == x86_mov) {
			  cerr << "  WARNING:: MALLOC - Buffer Overflow at : " << unparseInstruction(asmAft) 
                               <<  "  Length of array is " << length << "  but access at : " << arrayLength << endl;
                          aft->append_properties(SgGraph::dfa_bufferoverflow,varName);		  
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

