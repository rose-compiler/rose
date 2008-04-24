

#include "BufferOverflow.h"
#include "string.h"


using namespace std;
using namespace RoseBin_DataTypes;
using namespace RoseBin_OS_VER;
using namespace RoseBin_Arch;



bool 
BufferOverflow::run(string& name, SgDirectedGraphNode* node,
			      SgDirectedGraphNode* previous){
  // check known function calls and resolve variables
  ROSE_ASSERT(unparser);
  ROSE_ASSERT(node);

  cerr << " bufferoverflow->run " << node->get_name() << endl;
  SgAsmInstruction* asmNode = isSgAsmInstruction(node->get_SgNode());
  if (asmNode) {

    // ANALYSIS 2 : BUFFER OVERFLOW DETECTION -------------------------------------------
      if (isSgAsmx86Call(asmNode)) {
	// DEFINITION OF BUFFER OVERFLOW
	uint64_t malloc_pos = asmNode->get_address();
	SgAsmOperandList* opList = asmNode->get_operandList();
	ROSE_ASSERT(opList);
	SgAsmExpressionPtrList ptrList = opList->get_operands();
	// get the first (and only) element 
	if (ptrList.size()!=0) {
	SgAsmExpression* expr = *(ptrList.begin());
	string replace = "";
	string op = unparser->resolveOperand(expr, &replace);

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
	    SgAsmInstruction* asmPre = isSgAsmInstruction(pre->get_SgNode());
	    SgAsmx86Mov* mov = isSgAsmx86Mov(asmPre);
	    SgAsmx86Push* push = isSgAsmx86Push(asmPre);
	    if (mov || push) {
	      foundMov = true;
	      if (mov) {
		// make sure we are moving to the top of the stack, i.e. esp
		bool memRef = false;
		SgAsmRegisterReferenceExpression::x86_register_enum code;
		code = check_isRegister(pre, mov, false, memRef);
		string codeStr = unparser->resolveRegister(code,SgAsmRegisterReferenceExpression::qword);
		if (codeStr=="rsp")
		  value = getValueOfInstr(mov, true);
		else 
		  cerr << " Error :: foud a mov before a call that does not point to rsp but ::: " << codeStr << endl;
		if (RoseBin_support::DEBUG_MODE() && mov) 
		  cerr << "   malloc: found mov size of " << codeStr << " in " << value << " for malloc call : " << unparser->unparseInstruction(mov) <<endl;
	      } else if (push) {
		value = getValueOfInstr(push, false);
		if (RoseBin_support::DEBUG_MODE() && push) 
		  cerr << "   malloc: found push size " << value << " for malloc call : " << unparser->unparseInstruction(push) <<endl;
	      }
	    }
	  }


	  // result of malloc (variable) is in eax, we need to see what the variable is and store it
	  // forward search in the same function
	  foundMov=false;
	  SgDirectedGraphNode* aft = node;
	  while (foundMov!=true && sameParents(node, aft)) {
	    aft = getSuccessor(aft);
	    SgAsmInstruction* asmAft = isSgAsmInstruction(aft->get_SgNode());
	    SgAsmx86Mov* mov = isSgAsmx86Mov(asmAft);
	    if (mov) {
	      foundMov = true;
	      uint64_t address_of_var=0;
		bool memRef = false;
		SgAsmRegisterReferenceExpression::x86_register_enum code;
		code = check_isRegister(aft, mov, true, memRef);
		if (code == SgAsmRegisterReferenceExpression::rAX) {
		  if (RoseBin_support::DEBUG_MODE() && mov) 
		    cerr << "    found mov of eax of malloc call : " << unparser->unparseInstruction(mov) <<endl;
		  SgAsmMemoryReferenceExpression* memExpr = 
		    isSgAsmMemoryReferenceExpression(getOperand(mov,false));
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
		    uint64_t pos = mov->get_address();
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
    if (isSgAsmx86DataTransferInstruction(asmNode) ) {
      // USAGE OF BUFFER OVERFLOW
	SgAsmx86DataTransferInstruction* mov = isSgAsmx86DataTransferInstruction(asmNode);
	if (mov) {
	bool memRef = false;
	uint64_t address_of_var=0;
	SgAsmRegisterReferenceExpression::x86_register_enum code;
	// check if eax register on the left hand side
	code = check_isRegister(node, mov, false, memRef);
	if (code == SgAsmRegisterReferenceExpression::rAX) {
	  // right hand side is Register Reg / MemoryRef
	  //	  SgAsmRegisterReferenceExpression* refRight = getRegister(mov,true);
	  //if (refRight) {
	    SgAsmMemoryReferenceExpression* memExpr = 
	      isSgAsmMemoryReferenceExpression(getOperand(mov,true));
	    if (memExpr) {
	      address_of_var = getValueInMemoryRefExp( memExpr->get_address());
	    if (RoseBin_support::DEBUG_MODE() && mov) {
	      cout << "  malloc:  found mov to eax  " << unparser->unparseInstruction(mov) ;
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
	             << "    instr : " << unparser->unparseInstruction(mov) <<endl;
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
		  SgAsmInstruction* asmAft = isSgAsmInstruction(aft->get_SgNode());
		  SgAsmx86Add* add = isSgAsmx86Add(asmAft);
		  if (add) {
		    bool memRef = false;
		    SgAsmRegisterReferenceExpression::x86_register_enum code;
		    code = check_isRegister(aft, add, false, memRef);
		    if (code == SgAsmRegisterReferenceExpression::rAX) {
		      uint64_t val = getValueOfInstr(add, true);
		      arrayLength += val;
		    }
		  }		  
		  SgAsmx86Mov* mov = isSgAsmx86Mov(asmAft);
		  if (mov) {
		    foundMov = true;
		    bool memRef = false;
		    SgAsmRegisterReferenceExpression::x86_register_enum code;
		    code = check_isRegister(aft, mov, true, memRef);
		    if (code == SgAsmRegisterReferenceExpression::rAX) {
		      if (RoseBin_support::DEBUG_MODE() && mov) {
			cout << "   malloc - access to eax : " << unparser->unparseInstruction(mov) 
			     << "   length array (var) " << length << "  access array point: " << arrayLength  <<endl;
		      }
		      if (arrayLength> array) {
			if (RoseBin_support::DEBUG_MODE() && mov) {
			  cerr << "  WARNING:: MALLOC - Buffer Overflow at : " << unparser->unparseInstruction(mov) 
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
  return true;
}


extern "C" BC_GraphAnalysisInterface* create() {
  return new BufferOverflow();
}

