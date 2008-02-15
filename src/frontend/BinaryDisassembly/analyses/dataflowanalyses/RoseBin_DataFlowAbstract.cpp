/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 7Sep07
 * Decription : Dataflow analysis
 ****************************************************/
#include "rose.h"
#include "RoseBin_DataFlowAbstract.h"

using namespace std;
using namespace RoseBin_DataTypes;
using namespace RoseBin_OS_VER;
using namespace RoseBin_Arch;


bool
RoseBin_DataFlowAbstract::isInstructionAlteringOneRegister(SgAsmInstruction* inst) {
  bool alters = false;
  if (isSgAsmx86DataTransferInstruction(inst) ||
      isSgAsmx86ArithmeticInstruction(inst) ||
      isSgAsmx86ShiftAndRotateInstruction(inst) ||
      isSgAsmx86BitAndByteInstruction(inst) ||
      isSgAsmx86LogicalInstruction(inst) ||
      isSgAsmx86SegmentRegisterInstruction(inst))
    alters =true;

  // additional instructions from ControlTransfer
  else if (isSgAsmx86Loop(inst) || // ControlTransfer
      isSgAsmx86Loopz(inst) || // ControlTransfer
      isSgAsmx86Loopnz(inst)) // ControlTransfer
    alters = true;

  // additional instructions from StringInstruction
  else if (isSgAsmx86Cmps(inst) || // StringInstruction
      isSgAsmx86Scas(inst)) // StringInstruction
    alters = true;

  // additional instructions from FlagControl
  else if (isSgAsmx86Lahf(inst)) // FlagControl
    alters = true;

  // additional instructions from Misc
  else if (isSgAsmx86Lea(inst)) // Misc
    alters = true;

  // check exceptions
  if (isSgAsmx86Cmp(inst) || // arithmetic
      isSgAsmx86Test(inst) || // bitAndByte
      isSgAsmx86Bsf(inst) || // bitAndByte
      isSgAsmx86Bsr(inst) || // bitAndByte
      isSgAsmx86Bt(inst) // bitAndByte
      )
    alters=false;
  return alters;
}




bool
RoseBin_DataFlowAbstract::altersMultipleRegisters(vector<SgAsmRegisterReferenceExpression::x86_register_enum>& codes,
						SgAsmInstruction* inst) {
  bool alters=false;
  if (isSgAsmx86Push(inst) || // datatransfer
      isSgAsmx86Pusha(inst)) {  // datatransfer
    codes.push_back(SgAsmRegisterReferenceExpression::rSP);
    alters = true;
  }

  else if (isSgAsmx86Pop(inst) || // datatransfer
      isSgAsmx86Popa(inst)) {  // datatransfer
    codes.push_back(SgAsmRegisterReferenceExpression::rSP);
    alters = true;
  }

  else if (isSgAsmx86Call(inst) || 
	   isSgAsmx86Ret(inst) ||
	   isSgAsmx86IRet(inst) ||
	   isSgAsmx86Leave(inst) ||
	   isSgAsmx86Enter(inst)) {  
    codes.push_back(SgAsmRegisterReferenceExpression::rSP);
    codes.push_back(SgAsmRegisterReferenceExpression::rBP);
    alters = true;
  }

  else if (isSgAsmx86Cpuid(inst) ||
      isSgAsmx86Rdtsc(inst)
      ) {
    codes.push_back(SgAsmRegisterReferenceExpression::rAX);
    codes.push_back(SgAsmRegisterReferenceExpression::rBX);
    codes.push_back(SgAsmRegisterReferenceExpression::rCX);
    codes.push_back(SgAsmRegisterReferenceExpression::rDX);
    alters = true;
  }

  return alters;
}


/******************************************
 * return vector to user
 * for any given node and initName, return all definitions
 *****************************************/
std::set < SgDirectedGraphNode* > 
RoseBin_DataFlowAbstract::getDefFor(SgDirectedGraphNode* node, SgAsmRegisterReferenceExpression::x86_register_enum initName) {
  multitype multi = getDefMultiMapFor(node);
  return getAnyFor( &multi, initName);
}

/******************************************
 * return vector to user
 * for any given node and initName, return all definitions
 *****************************************/
std::set < SgDirectedGraphNode* > 
RoseBin_DataFlowAbstract::getUseFor(SgDirectedGraphNode* node, SgAsmRegisterReferenceExpression::x86_register_enum initName) {
  multitype multi = getUseMultiMapFor(node);
  return getAnyFor(&multi, initName);
}

/******************************************
 * return vector to user
 * for any given node and initName, return all definitions
 *****************************************/
std::set < SgDirectedGraphNode* > 
RoseBin_DataFlowAbstract::getAnyFor(const multitype* multi, SgAsmRegisterReferenceExpression::x86_register_enum initName) {
  set < SgDirectedGraphNode*> defNodes;
  defNodes.clear();
  //multitype multi = getDefUseFor(node);
  if (multi->size()>0) {
    multitype::const_iterator i = multi->begin();
    for (; i!=multi->end();++i) {
      SgAsmRegisterReferenceExpression::x86_register_enum initNameMM = i->first;
      SgDirectedGraphNode* thenode = i->second;
      if (initName==initNameMM) {
        // we have found the right node and right initName
        //defNodes.push_back(thenode);
	defNodes.insert(thenode);
      }
    }
  }
  return defNodes;
}

/******************************************
 * return multimap to user
 * for any given node, return all definitions
 *****************************************/
//__gnu_cxx::hash_multimap 
const std::multimap< SgAsmRegisterReferenceExpression::x86_register_enum , SgDirectedGraphNode* >& 
RoseBin_DataFlowAbstract::getDefMultiMapFor(SgDirectedGraphNode* node) {
  static std::multimap< SgAsmRegisterReferenceExpression::x86_register_enum , SgDirectedGraphNode* >
    mymap;
  if (deftable.find(node)!=deftable.end())
    return deftable[node];
  else 
    return mymap;
}
 
/******************************************
 * return multimap to user
 * for any given node, return all definitions
 *****************************************/
//__gnu_cxx::hash_multimap 
const std::multimap< SgAsmRegisterReferenceExpression::x86_register_enum , SgDirectedGraphNode* >& 
RoseBin_DataFlowAbstract::getUseMultiMapFor(SgDirectedGraphNode* node) {
  static std::multimap< SgAsmRegisterReferenceExpression::x86_register_enum , SgDirectedGraphNode* >
    mymap;
  if (usetable.find(node)!=usetable.end())
    return usetable[node];
  else 
    return mymap;
}


void 
RoseBin_DataFlowAbstract::printDefTableToFile(
					      std::string fileName) {
  std::ofstream myfile;
  myfile.open(fileName.c_str());

  vector<SgDirectedGraphNode*> sorted;
  tabletype::iterator it2 = deftable.begin();
  for (;it2!=deftable.end();++it2) {
    SgDirectedGraphNode* node = it2->first;
    sorted.push_back(node);
  }
  std::sort(sorted.begin(), sorted.end());
  
  //  tabletype::iterator it = deftable.begin();
  //for (;it!=deftable.end();++it) {
  vector<SgDirectedGraphNode*>::iterator it = sorted.begin();
  for (;it!=sorted.end();++it) {
    //    SgDirectedGraphNode* node = it->first;
    SgDirectedGraphNode* node = *it;
    //    const multitype& type = getDefinitionsFor(node);
    const multitype& type = getDefMultiMapFor(node);
    multitype::const_iterator itm = type.begin();
    if (node) {
      string line = ""+node->get_name()+" : \n";
      for (;itm!=type.end();++itm) {
	SgAsmRegisterReferenceExpression::x86_register_enum code = itm->first;
	SgDirectedGraphNode* nodeDef = itm->second;
	string registerName = unparser->resolveRegister(code, 
							SgAsmRegisterReferenceExpression::qword);
	
	string def = registerName+" - "+nodeDef->get_name();
	line+="   "+def+"\n";
      }
      myfile << line ;
    }
  }
  
  myfile.close();  
}

bool 
RoseBin_DataFlowAbstract::sameParents(SgDirectedGraphNode* node, SgDirectedGraphNode* next) {
  bool same=false;
  if (isSgAsmFunctionDeclaration(node->get_SgNode())) {
    return true;
  }

  SgAsmInstruction* thisNode = isSgAsmInstruction(node->get_SgNode());
  SgAsmInstruction* nextNode = isSgAsmInstruction(next->get_SgNode());
  if (thisNode && nextNode) {
    SgAsmFunctionDeclaration* func1 = isSgAsmFunctionDeclaration(thisNode->get_parent());
    SgAsmFunctionDeclaration* func2 = isSgAsmFunctionDeclaration(nextNode->get_parent());
    if (func1==func2)
      same=true;
  }
  return same;
}

uint64_t 
RoseBin_DataFlowAbstract::getValueInExpression(SgAsmValueExpression* valExp) {

  uint8_t b_val=0xF;
  uint16_t w_val=0xFF;
  uint32_t dw_val=0xFFFF;
  uint64_t qw_val=0xFFFFFFFF;
  
  RoseBin_support::resolveValue(valExp, false,
				b_val,
				w_val,
				dw_val,
				qw_val);
  uint64_t val = 0xFFFFFFFF;
  if (b_val!=0xF)
    val = uint64_t(b_val);
  else if (w_val!=0xFF)
    val = uint64_t(w_val);
  else if (dw_val!=0xFFFF)
    val = uint64_t(dw_val);
  else if (qw_val!=0xFFFFFFFF)
    val = uint64_t(qw_val);
  
  return val;
}


/************************************************************
 * checks if an instruction has a RegisterReference on the
 * right (bool) or left side
 ***********************************************************/
int64_t
RoseBin_DataFlowAbstract::check_isRegister(SgDirectedGraphNode* node,
					   SgAsmInstruction* inst,
					   SgAsmRegisterReferenceExpression::x86_register_enum codeSearch,
					   bool rightSide,
					   vector<SgAsmRegisterReferenceExpression::x86_register_enum>& regsOfInterest,
					   bool& cantTrack
					   ) {
  // bool rightSide specifies 
  // true = checking the right side (of instruction operands) for a registerReference
  // false = checking the left side (of instruction operands) for a registerReference
  int64_t res = 0xFFFFFFFF;
  SgAsmOperandList* opList = inst->get_operandList();
  ROSE_ASSERT(opList);

  SgAsmRegisterReferenceExpression* refExpr =NULL;
  SgAsmRegisterReferenceExpression::x86_register_enum code ;
  //SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos ;
  //SgAsmMemoryReferenceExpression* memRef = NULL;
  int counter=0;
  if (rightSide) 
    counter =-1;
  else
    counter = 2;

  string operands="";
  bool foundECX = false;
  int64_t newVal = 0xFFFFFFFF;
  // iterate through the operands (for x86 = 2 operands)
  SgAsmExpressionPtrList ptrList = opList->get_operands();
  for (SgAsmExpressionPtrList::iterator it=ptrList.begin(); it!=ptrList.end(); ++it) {
    SgAsmExpression* expr = *it;
    ROSE_ASSERT(expr);
    if (rightSide)
      counter++;
    else 
      counter--;
    if (counter==0) {
      // left hand side if rightSide=true *************************************************************
      // right hand side if rightSide=false *************************************************************
      SgAsmValueExpression* valExp = isSgAsmValueExpression(expr);
      refExpr = isSgAsmRegisterReferenceExpression(expr);

      // ****** 1. valueExpression
      if (valExp) {
	// value expr
	newVal = getValueInExpression(valExp);
	operands += " <<left :: value " +RoseBin_support::HexToString(newVal)+">>";
      } 

      // ****** 2. referenceExpression
      else if (refExpr) {
        // ****** 2. referenceExpression
	// the right hand side is also a register or memory location
	code = refExpr->get_x86_register_code();
	operands += " << left ::  refExpr >> ";


	// we need to track the value of the register in order to find out its value
	vector<SgAsmRegisterReferenceExpression::x86_register_enum>::iterator it = regsOfInterest.begin();
	for (;it!=regsOfInterest.end();++it) {
	  SgAsmRegisterReferenceExpression::x86_register_enum codeI = *it;
	  if (codeI==codeSearch) {
	    newVal = trackValueForRegister(node, codeSearch, cantTrack, refExpr);
	    break;
	  }
	}


      }	else {
	cerr << " unhandled case in checkVariables " << expr->class_name() << endl;
	exit(0);
      }


    } else {
      // right hand side if rightSide=true  ************************************************************
      // left hand side if rightSide=false  ************************************************************
      SgAsmRegisterReferenceExpression* refExprR = isSgAsmRegisterReferenceExpression(expr);
      if (refExprR) {
	code = refExprR->get_x86_register_code();
	operands += " <<right :: refExpr>> ";
	if (code==codeSearch)
	  foundECX=true;
      }
    }
  }
  if (foundECX)
    res = newVal;
  /*
  if (RoseBin_support::DEBUG_MODE()) 
    cout << "   >> checkRegister : " << RoseBin_support::HexToString(res) << 
      " -- " << operands << "  foundReg: " << RoseBin_support::resBool(foundECX) << endl; 
  */
  return res;
}

/************************************************************
 * checks if an instruction has a RegisterReference on the
 * right (bool) or left side
 ***********************************************************/
SgAsmRegisterReferenceExpression::x86_register_enum 
RoseBin_DataFlowAbstract::check_isRegister(SgDirectedGraphNode* node,
					   SgAsmInstruction* inst,
					   bool rightSide, bool& memoryReference ) {
  // bool rightSide specifies 
  // true = checking the right side (of instruction operands) for a registerReference
  // false = checking the left side (of instruction operands) for a registerReference
  SgAsmOperandList* opList = inst->get_operandList();
  ROSE_ASSERT(opList);


  SgAsmRegisterReferenceExpression* refExpr =NULL;
  SgAsmRegisterReferenceExpression::x86_register_enum code = SgAsmRegisterReferenceExpression::undefined_general_register;
  int counter=0;
  int endCounter=0;
  if (rightSide)
    endCounter=1;

  string operands="";
  // iterate through the operands (for x86 = 2 operands)
  SgAsmExpressionPtrList ptrList = opList->get_operands();
  for (SgAsmExpressionPtrList::iterator it=ptrList.begin(); it!=ptrList.end(); ++it) {
    SgAsmExpression* expr = *it;
    ROSE_ASSERT(expr);
    // skip the iteration if we do not evaluate the correct side
    // counter ==0 (left side) counter==1 (right side)
    if (counter!=endCounter) {
      counter++;
      continue;
    } else
      counter++;

    SgAsmMemoryReferenceExpression* mem = isSgAsmMemoryReferenceExpression(expr);
    if (mem) {
      memoryReference = true;
      SgAsmExpression* memOff = mem->get_address();
	if (isSgAsmRegisterReferenceExpression(memOff)) {
	  SgAsmRegisterReferenceExpression* memRegRef = isSgAsmRegisterReferenceExpression(memOff);
	  code = memRegRef->get_x86_register_code();
	}
    }
    refExpr = isSgAsmRegisterReferenceExpression(expr);
    if (refExpr) {
	code = refExpr->get_x86_register_code();
    }
  }

#if 0
    refExpr = isSgAsmRegisterReferenceExpression(expr);
    if (refExpr) {
      // check if it could be a memory reference
      // changed by tps on 16Jan08

      SgAsmExpression* offset = refExpr->get_offset();
      if (isSgAsmMemoryReferenceExpression(offset)) {
	// is a memory reference
	memoryReference = true;
	SgAsmMemoryReferenceExpression* mem = isSgAsmMemoryReferenceExpression(offset);
	SgAsmExpression* memOff = mem->get_address();
	if (isSgAsmRegisterReferenceExpression(memOff)) {
	  SgAsmRegisterReferenceExpression* memRegRef = isSgAsmRegisterReferenceExpression(memOff);
	  code = memRegRef->get_x86_register_code();
	}
      } else {
	// is a register reference
	code = refExpr->get_x86_register_code();
	//      }
      }
    }
#endif

  return code;
}

/************************************************************
 * checks if an instruction has a RegisterReference on the
 * right (bool) or left side
 ***********************************************************/
SgAsmExpression*
RoseBin_DataFlowAbstract::getOperand(
				      SgAsmInstruction* inst,
				      bool rightSide ) {
  // bool rightSide specifies 
  // true = checking the right side (of instruction operands) for a registerReference
  // false = checking the left side (of instruction operands) for a registerReference
  SgAsmOperandList* opList = inst->get_operandList();
  ROSE_ASSERT(opList);

  SgAsmExpression* refExpr =NULL;

  int counter=0;
  int endCounter=0;
  if (rightSide)
    endCounter=1;

  // iterate through the operands (for x86 = 2 operands)
  SgAsmExpressionPtrList ptrList = opList->get_operands();
  for (SgAsmExpressionPtrList::iterator it=ptrList.begin(); it!=ptrList.end(); ++it) {
    SgAsmExpression* expr = *it;
    ROSE_ASSERT(expr);
    // skip the iteration if we do not evaluate the correct side
    // counter ==0 (left side) counter==1 (right side)
    if (counter!=endCounter) {
      counter++;
      continue;
    } else
      counter++;

    refExpr = isSgAsmRegisterReferenceExpression(expr);
    if (refExpr==NULL)
    refExpr = isSgAsmMemoryReferenceExpression(expr);
  }
  return refExpr;
}

uint64_t 
RoseBin_DataFlowAbstract::getValueInMemoryRefExp(SgAsmExpression* expr) {
  uint64_t res = 0;
  if (isSgAsmValueExpression(expr)) {
    res = getValueInExpression(isSgAsmValueExpression(expr));
  } else if (isSgAsmRegisterReferenceExpression(expr)) {
    SgAsmRegisterReferenceExpression* refexp = isSgAsmRegisterReferenceExpression(expr);
    SgAsmRegisterReferenceExpression::x86_register_enum code;
    if (refexp) {
      code = refexp->get_x86_register_code();    
      // we have to add up this value, currently we assign 5000 to the register
      // fixme later
      if (code == SgAsmRegisterReferenceExpression::rBP)
	res = 0;
      else
	res = 5000;
    }
  } else if (isSgAsmBinaryAdd(expr)) {
    // add up the left and right side
    SgAsmBinaryAdd* binadd = isSgAsmBinaryAdd(expr);
    SgAsmExpression* left = binadd->get_lhs();
    SgAsmExpression* right = binadd->get_rhs();
    res = getValueInMemoryRefExp(left) + getValueInMemoryRefExp(right);
  } else {
    cerr << " ERROR :: getValueInMemoryRefExp - no such condition specified" 
	 << expr->class_name() << endl;
  }
  return res;
}

/************************************************************
 * gets the value of the right (bool=true) hand side 
 * or left hand side of the instruction
 ***********************************************************/
uint64_t
RoseBin_DataFlowAbstract::getValueOfInstr( SgAsmInstruction* inst,
					   bool rightSide ) {
  // bool rightSide specifies 
  // true = checking the right side (of instruction operands) for a registerReference
  // false = checking the left side (of instruction operands) for a registerReference
  SgAsmOperandList* opList = inst->get_operandList();
  ROSE_ASSERT(opList);

  uint64_t val = 0xffffffff;
  int counter=0;
  int endCounter=0;
  if (rightSide)
    endCounter=1;

  // iterate through the operands (for x86 = 2 operands)
  SgAsmExpressionPtrList ptrList = opList->get_operands();
  for (SgAsmExpressionPtrList::iterator it=ptrList.begin(); it!=ptrList.end(); ++it) {
    SgAsmExpression* expr = *it;
    ROSE_ASSERT(expr);
    // skip the iteration if we do not evaluate the correct side
    // counter ==0 (left side) counter==1 (right side)
    if (counter!=endCounter) {
      counter++;
      continue;
    } else
      counter++;
    
    SgAsmValueExpression* valExpr = isSgAsmValueExpression(expr);
    if (valExpr) {
      val = getValueInExpression(valExpr);
    }
  }
  return val;
}

int64_t 
RoseBin_DataFlowAbstract::trackValueForRegister(
					       SgDirectedGraphNode* node,
					       SgAsmRegisterReferenceExpression::x86_register_enum codeSearch,
					       bool& cantTrack,
					       SgAsmRegisterReferenceExpression* refExpr_rightHand) {
  int64_t value = 0xffffffff;
  if (RoseBin_support::DEBUG_MODE())
    cout << "    ........ trying to resolve value for register :: " << RoseBin_support::ToString(codeSearch) << endl;

  SgAsmInstruction* inst = isSgAsmInstruction(node->get_SgNode());
  ROSE_ASSERT(inst);
  SgAsmRegisterReferenceExpression::x86_register_enum code = refExpr_rightHand->get_x86_register_code();
  // iterate up and find an assignment to this register codeSearch i.e. instr codeSearch, esi

  bool condInst = RoseBin_support::isConditionalInstruction(inst);
  bool condInstFlag = RoseBin_support::isConditionalFlagInstruction(inst);
  if (condInstFlag==false) {
    // the instruction is not dependent on a flag
    if (condInst==false) {
      // the instruction is not dependent on a value in one of its operands
      // easiest track
      SgDirectedGraphNode* previous = getPredecessor(node);
	/*
      vector <SgDirectedGraphNode*> vec;
      vizzGraph->getPredecessors(node, vec);
      if (vec.size()==1) {
	// found one predecessor
	SgDirectedGraphNode* previous = vec.back();
	ROSE_ASSERT(previous);
	string name = vizzGraph->getProperty(RoseBin_Def::name, previous);
	if (RoseBin_support::DEBUG_MODE()) 
	  cout << "    tracking recursive var " << name << endl;

	value = trackValueForRegister(previous, code, cantTrack, refExpr_rightHand);

      } else if (vec.size()>1) {
	cerr << " Tracking:: Problem, we have more than one predecessor for a node... cant track this " << endl;
	exit(0);
      }
	*/
	value = trackValueForRegister(previous, code, cantTrack, refExpr_rightHand);
    } else {
      // the instruction is dependent on a value in one of its operands
      // e.g. cmovz eax, esi (moved only if esi=0);
      // need to track the value of esi to track the value of eax .. more complicated!
      int addr = inst->get_address();
      if (RoseBin_support::DEBUG_MODE()) {
	cout << " ERROR ------------------------------------------ " << endl;
	      cout << RoseBin_support::HexToString(addr) << "  " << inst->class_name() << 
		" -- CANT resolve the value of the register because it depends on CONDITION -- code " << 
		RoseBin_support::ToString(code) << endl;
      }
      cantTrack =true;
	  }
    
  } else {
    // the instruction is dependent on a flag
    
    int addr = inst->get_address();
    if (RoseBin_support::DEBUG_MODE()) {
      cout << " ERROR ------------------------------------------ " << endl;
      cout << RoseBin_support::HexToString(addr) << "  " << inst->class_name() << 
	" -- CANT resolve the value of the register because it depends on FLAGS -- code " << 
	RoseBin_support::ToString(code) << endl;
    }
    cantTrack =true;
  }


  return value;
}

SgDirectedGraphNode*  
RoseBin_DataFlowAbstract::getPredecessor(SgDirectedGraphNode* node){
  vector <SgDirectedGraphNode*> vec;
  vizzGraph->getDirectCFGPredecessors(node, vec);
  SgDirectedGraphNode* previous = NULL;
  if (vec.size()==1) {
    // found one predecessor
    previous = vec.back();
    ROSE_ASSERT(previous);
    string name = vizzGraph->getProperty(RoseBin_Def::name, previous);
    if (RoseBin_support::DEBUG_MODE()) 
      cout << "    tracking recursive var " << name << endl;
    
  } else if (vec.size()>1) {
    cerr << " Tracking:: Problem, we have more than one predecessor for a node... cant track this " << endl;
    exit(0);
  }
  return previous;
}

SgDirectedGraphNode*  
RoseBin_DataFlowAbstract::getSuccessor(SgDirectedGraphNode* node){
  vector <SgDirectedGraphNode*> vec;
  vizzGraph->getDirectCFGSuccessors(node, vec);
  SgDirectedGraphNode* after = NULL;
  if (vec.size()==1) {
    // found one predecessor
    after = vec.back();
    ROSE_ASSERT(after);
    string name = vizzGraph->getProperty(RoseBin_Def::name, after);
    if (RoseBin_support::DEBUG_MODE()) 
      cout << "    tracking recursive var " << name << endl;
    
  } else if (vec.size()>1) {
    cerr << " Tracking:: Problem, we have more than one successor for a node... cant track this " << endl;
    exit(0);
  }
  return after;
}




RoseBin_Variable* 
RoseBin_DataFlowAbstract::createVariable(uint64_t position,
					 vector<uint64_t> pos, 
					 string name,  
					 DataTypes type, string description,
					 int length, 
					 vector<uint64_t> value,
					 bool memoryRef) {
  RoseBin_Variable* var_hashed =NULL;
  if (name=="") {
    cerr << " ERROR: no variable name . " << endl;
    exit(0);
  }
  string var_name = RoseBin_support::HexToString(position)+":"+name;

  __gnu_cxx::hash_map <uint64_t, RoseBin_Variable*>::iterator it = variables.find(position);
  if (it==variables.end()) {
    var_hashed = new RoseBin_Variable(position, name, type, description, length, value);
    variables[position] = var_hashed;
    if (memoryRef) {
      memory[position] = var_hashed;
    }
    vector<uint64_t>::iterator p = pos.begin();
    for (;p!=pos.end();++p) {
      uint64_t var_ref = *p;
      variables[var_ref] = var_hashed;
    }
    variablesReverse[var_name] = position;
  } else {
    var_hashed = it->second;
    cout << " Variable already exists. " << name << " - " << 
      RoseBin_support::HexToString(position) << " - " << description << endl;
  }
  ROSE_ASSERT(var_hashed);
  return var_hashed;
}

RoseBin_Variable*
RoseBin_DataFlowAbstract::getVariable(std::string var) {
  uint64_t val = 0;
  __gnu_cxx::hash_map <std::string,uint64_t>::iterator it = variablesReverse.find(var);
  if (it!=variablesReverse.end()) {
    val = it->second;
  }
  
  return getVariable(val);
}

RoseBin_Variable*
RoseBin_DataFlowAbstract::getVariable(uint64_t pos) {
  RoseBin_Variable* var=NULL;
  __gnu_cxx::hash_map <uint64_t, RoseBin_Variable*>::iterator it = variables.find(pos);
  if (it!=variables.end()) {
    var = it->second;
  }
  return var;
}

