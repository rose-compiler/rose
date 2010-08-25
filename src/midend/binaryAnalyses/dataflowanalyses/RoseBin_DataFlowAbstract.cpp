/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 7Sep07
 * Decription : Dataflow analysis
 ****************************************************/
// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"
#include "RoseBin_DataFlowAbstract.h"
#include "AsmUnparser_compat.h"

using namespace std;
using namespace RoseBin_DataTypes;
using namespace RoseBin_OS_VER;
using namespace RoseBin_Arch;


bool
RoseBin_DataFlowAbstract::isInstructionAlteringOneRegister(SgAsmx86Instruction* inst) {
  bool alters = false;
  switch (inst->get_kind()) {
    case x86_aaa:
    case x86_aad:
    case x86_aam:
    case x86_aas:
    case x86_adc:
    case x86_add:
    case x86_addsd:
    case x86_addss:
    case x86_and:
    case x86_andnpd:
    case x86_andpd:
    case x86_arpl:
    case x86_bsf:
    case x86_bsr:
    case x86_bswap:
    case x86_btc:
    case x86_btr:
    case x86_bts:
    case x86_cbw:
    case x86_cmova:
    case x86_cmovae:
    case x86_cmovb:
    case x86_cmovbe:
    case x86_cmove:
    case x86_cmovg:
    case x86_cmovge:
    case x86_cmovl:
    case x86_cmovle:
    case x86_cmovne:
    case x86_cmovno:
    case x86_cmovns:
    case x86_cmovo:
    case x86_cmovpe:
    case x86_cmovpo:
    case x86_cmovs:
    case x86_cmpsb:
    case x86_cmpsw:
    case x86_cmpsd:
    case x86_cmpsq:
    case x86_cmpxchg:
    case x86_cmpxchg8b:
    case x86_cwd:
    case x86_daa:
    case x86_das:
    case x86_dec:
    case x86_div:
    case x86_divsd:
    case x86_emms:
    case x86_enter:
    case x86_f2xm1:
    case x86_fabs:
    case x86_fadd:
    case x86_faddp:
    case x86_fbld:
    case x86_fbstp:
    case x86_fchs:
    case x86_fcmovb:
    case x86_fcmovbe:
    case x86_fcmove:
    case x86_fcmovnb:
    case x86_fcmovnbe:
    case x86_fcmovne:
    case x86_fcmovnu:
    case x86_fcmovu:
    case x86_fcom:
    case x86_fcomi:
    case x86_fcomip:
    case x86_fcomp:
    case x86_fcompp:
    case x86_fcos:
    case x86_fdecstp:
    case x86_fdiv:
    case x86_fdivp:
    case x86_fdivr:
    case x86_fdivrp:
    case x86_femms:
    case x86_ffree:
    case x86_fiadd:
    case x86_ficom:
    case x86_ficomp:
    case x86_fidiv:
    case x86_fidivr:
    case x86_fild:
    case x86_fimul:
    case x86_fincstp:
    case x86_fist:
    case x86_fistp:
    case x86_fisttp:
    case x86_fisub:
    case x86_fisubr:
    case x86_fld:
    case x86_fld1:
    case x86_fldcw:
    case x86_fldenv:
    case x86_fldl2e:
    case x86_fldl2t:
    case x86_fldlg2:
    case x86_fldln2:
    case x86_fldpi:
    case x86_fldz:
    case x86_fmul:
    case x86_fmulp:
    case x86_fnclex:
    case x86_fninit:
    case x86_fnop:
    case x86_fnsave:
    case x86_fnstcw:
    case x86_fnstenv:
    case x86_fnstsw:
    case x86_fpatan:
    case x86_fprem:
    case x86_fprem1:
    case x86_fptan:
    case x86_frndint:
    case x86_frstor:
    case x86_fscale:
    case x86_fsin:
    case x86_fsincos:
    case x86_fsqrt:
    case x86_fst:
    case x86_fstp:
    case x86_fsub:
    case x86_fsubp:
    case x86_fsubr:
    case x86_fsubrp:
    case x86_ftst:
    case x86_fucom:
    case x86_fucomi:
    case x86_fucomip:
    case x86_fucomp:
    case x86_fucompp:
    case x86_fwait:
    case x86_fxam:
    case x86_fxch:
    case x86_fxsave:
    case x86_fxtract:
    case x86_fyl2x:
    case x86_fyl2xp1:
    case x86_idiv:
    case x86_imul:
    case x86_in:
    case x86_inc:
    case x86_insb:
    case x86_insw:
    case x86_insd:
    case x86_int:
    case x86_int1:
    case x86_int3:
    case x86_into:
    case x86_iret:
    case x86_lahf:
    case x86_lar:
    case x86_ldmxcsr:
    case x86_lds:
    case x86_lea:
    case x86_les:
    case x86_lfs:
    case x86_lgs:
    case x86_lodsb:
    case x86_lodsw:
    case x86_lodsd:
    case x86_lodsq:
    case x86_loop:
    case x86_loopnz:
    case x86_loopz:
    case x86_lss:
    case x86_lzcnt:
    case x86_mov:
    case x86_movaps:
    case x86_movdqu:
    case x86_movlpd:
    case x86_movntq:
    case x86_movsb:
    case x86_movsw:
    case x86_movsd:
    case x86_movsq:
    case x86_movss:
    case x86_movsx:
    case x86_movsxd:
    case x86_movzx:
    case x86_mul:
    case x86_neg:
    case x86_nop:
    case x86_not:
    case x86_or:
    case x86_outs:
    case x86_paddb:
    case x86_paddw:
    case x86_paddd:
    case x86_paddq:
    case x86_paddusb:
    case x86_paddusw:
    case x86_pand:
    case x86_pandn:
    case x86_pause:
    case x86_pcmpeqb:
    case x86_pcmpeqw:
    case x86_pcmpeqd:
    case x86_pcmpeqq:
    case x86_popcnt:
    case x86_psllw:
    case x86_pslld:
    case x86_psllq:
    case x86_psrlw:
    case x86_psrld:
    case x86_psrlq:
    case x86_psubb:
    case x86_psubw:
    case x86_psubd:
    case x86_psubq:
    case x86_psubusb:
    case x86_psubusw:
    case x86_pxor:
    case x86_rcl:
    case x86_rcr:
    case x86_rol:
    case x86_ror:
    case x86_sahf:
    case x86_salc:
    case x86_sar:
    case x86_sbb:
    case x86_scasb:
    case x86_scasw:
    case x86_scasd:
    case x86_scasq:
    case x86_seta:
    case x86_setae:
    case x86_setb:
    case x86_setbe:
    case x86_sete:
    case x86_setg:
    case x86_setge:
    case x86_setl:
    case x86_setle:
    case x86_setne:
    case x86_setno:
    case x86_setns:
    case x86_seto:
    case x86_setpe:
    case x86_setpo:
    case x86_sets:
    case x86_shl:
    case x86_shld:
    case x86_shr:
    case x86_shrd:
    case x86_sldt:
    case x86_stc:
    case x86_std:
    case x86_sti:
    case x86_stmxcsr:
    case x86_stos:
    case x86_str:
    case x86_sub:
    case x86_subsd:
    case x86_subss:
    case x86_syscall:
    case x86_sysenter:
    case x86_sysexit:
    case x86_sysret:
    case x86_ucomisd:
    case x86_ucomiss:
    case x86_verr:
    case x86_verw:
    case x86_xadd:
    case x86_xchg:
    case x86_xlatb:
    case x86_xor:
    case x86_xorpd:
    case x86_xorps:
      alters = true;
      break;

    default: break;
  }
  return alters;
}




bool
RoseBin_DataFlowAbstract::altersMultipleRegisters(vector<std::pair<X86RegisterClass, int> >& codes,
						SgAsmx86Instruction* inst) {
  bool alters=false;
  switch (inst->get_kind()) {
    case x86_push:
    case x86_pusha:
      codes.push_back(std::make_pair(x86_regclass_gpr, x86_gpr_sp));
      alters = true;
      break;

    case x86_pop:
    case x86_popa:
      codes.push_back(std::make_pair(x86_regclass_gpr, x86_gpr_sp));
      alters = true;
      break;

    case x86_call:
    case x86_ret:
    case x86_iret:
    case x86_leave:
    case x86_enter:
      codes.push_back(std::make_pair(x86_regclass_gpr, x86_gpr_sp));
      codes.push_back(std::make_pair(x86_regclass_gpr, x86_gpr_bp));
      alters = true;
      break;

    case x86_cpuid:
    case x86_rdtsc:
      codes.push_back(std::make_pair(x86_regclass_gpr, x86_gpr_ax));
      codes.push_back(std::make_pair(x86_regclass_gpr, x86_gpr_bx));
      codes.push_back(std::make_pair(x86_regclass_gpr, x86_gpr_cx));
      codes.push_back(std::make_pair(x86_regclass_gpr, x86_gpr_dx));
      alters = true;
      break;

    default: break;
  }

  return alters;
}


/******************************************
 * return vector to user
 * for any given node and initName, return all definitions
 *****************************************/
std::set < SgGraphNode* >
RoseBin_DataFlowAbstract::getDefFor(SgGraphNode* node, std::pair<X86RegisterClass, int>  initName) {
  multitype multi = getDefMultiMapFor(node);
  return getAnyFor( &multi, initName);
}

/******************************************
 * return vector to user
 * for any given node and initName, return all definitions
 *****************************************/
std::set < SgGraphNode* >
RoseBin_DataFlowAbstract::getUseFor(SgGraphNode* node, std::pair<X86RegisterClass, int>  initName) {
  multitype multi = getUseMultiMapFor(node);
  return getAnyFor(&multi, initName);
}

/******************************************
 * return vector to user
 * for any given node and initName, return all definitions
 *****************************************/
std::set < SgGraphNode* >
RoseBin_DataFlowAbstract::getAnyFor(const multitype* multi, std::pair<X86RegisterClass, int>  initName) {
  set < SgGraphNode*> defNodes;
  defNodes.clear();
  //multitype multi = getDefUseFor(node);
  if (multi->size()>0) {
    multitype::const_iterator i = multi->begin();
    for (; i!=multi->end();++i) {
      std::pair<X86RegisterClass, int>  initNameMM = i->first;
      SgGraphNode* thenode = i->second;
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
const std::multimap< std::pair<X86RegisterClass, int>  , SgGraphNode* >&
RoseBin_DataFlowAbstract::getDefMultiMapFor(SgGraphNode* node) {
  static std::multimap< std::pair<X86RegisterClass, int>  , SgGraphNode* >
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
const std::multimap< std::pair<X86RegisterClass, int>  , SgGraphNode* >&
RoseBin_DataFlowAbstract::getUseMultiMapFor(SgGraphNode* node) {
  static std::multimap< std::pair<X86RegisterClass, int>  , SgGraphNode* >
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

  vector<SgGraphNode*> sorted;
  tabletype::iterator it2 = deftable.begin();
  for (;it2!=deftable.end();++it2) {
    SgGraphNode* node = it2->first;
    sorted.push_back(node);
  }
  std::sort(sorted.begin(), sorted.end());

  //  tabletype::iterator it = deftable.begin();
  //for (;it!=deftable.end();++it) {
  vector<SgGraphNode*>::iterator it = sorted.begin();
  for (;it!=sorted.end();++it) {
    //    SgGraphNode* node = it->first;
    SgGraphNode* node = *it;
    //    const multitype& type = getDefinitionsFor(node);
    const multitype& type = getDefMultiMapFor(node);
    multitype::const_iterator itm = type.begin();
    if (node) {
      string line = ""+node->get_name()+" : \n";
      for (;itm!=type.end();++itm) {
	std::pair<X86RegisterClass, int>  code = itm->first;
	SgGraphNode* nodeDef = itm->second;
	string registerName = unparseX86Register(code.first, code.second,
                                                 x86_regpos_qword);

	string def = registerName+" - "+nodeDef->get_name();
	line+="   "+def+"\n";
      }
      myfile << line ;
    }
  }

  myfile.close();
}

bool
RoseBin_DataFlowAbstract::sameParents(SgGraphNode* node, SgGraphNode* next) {
  bool same=false;
  if (isSgAsmFunctionDeclaration(node->get_SgNode())) {
    return true;
  }

  SgAsmx86Instruction* thisNode = isSgAsmx86Instruction(node->get_SgNode());
  SgAsmx86Instruction* nextNode = isSgAsmx86Instruction(next->get_SgNode());
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
RoseBin_DataFlowAbstract::check_isRegister(SgGraphNode* node,
                                           SgAsmx86Instruction* inst,
					   std::pair<X86RegisterClass, int>  codeSearch,
					   bool rightSide,
					   vector<std::pair<X86RegisterClass, int> >& regsOfInterest,
					   bool& cantTrack
					   ) {
  // bool rightSide specifies
  // true = checking the right side (of instruction operands) for a registerReference
  // false = checking the left side (of instruction operands) for a registerReference
  int64_t res = 0xFFFFFFFF;
  SgAsmOperandList* opList = inst->get_operandList();
  ROSE_ASSERT(opList);

  SgAsmx86RegisterReferenceExpression* refExpr =NULL;
  std::pair<X86RegisterClass, int>  code ;
  //SgAsmx86RegisterReferenceExpression::x86_position_in_register_enum pos ;
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
      refExpr = isSgAsmx86RegisterReferenceExpression(expr);

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
	code = refExpr->get_identifier();
	operands += " << left ::  refExpr >> ";


	// we need to track the value of the register in order to find out its value
	vector<std::pair<X86RegisterClass, int> >::iterator it = regsOfInterest.begin();
	for (;it!=regsOfInterest.end();++it) {
	  std::pair<X86RegisterClass, int>  codeI = *it;
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
      SgAsmx86RegisterReferenceExpression* refExprR = isSgAsmx86RegisterReferenceExpression(expr);
      if (refExprR) {
	code = refExprR->get_identifier();
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
std::pair<X86RegisterClass, int>
RoseBin_DataFlowAbstract::check_isRegister(SgGraphNode* node,
                                           SgAsmx86Instruction* inst,
					   bool rightSide, bool& memoryReference ) {
  // bool rightSide specifies
  // true = checking the right side (of instruction operands) for a registerReference
  // false = checking the left side (of instruction operands) for a registerReference
  SgAsmOperandList* opList = inst->get_operandList();
  ROSE_ASSERT(opList);


  SgAsmx86RegisterReferenceExpression* refExpr =NULL;
  std::pair<X86RegisterClass, int>  code = std::make_pair(x86_regclass_unknown, 0);
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
	if (isSgAsmx86RegisterReferenceExpression(memOff)) {
	  SgAsmx86RegisterReferenceExpression* memRegRef = isSgAsmx86RegisterReferenceExpression(memOff);
	  code = memRegRef->get_identifier();
	}
    }
    refExpr = isSgAsmx86RegisterReferenceExpression(expr);
    if (refExpr) {
	code = refExpr->get_identifier();
    }
  }

#if 0
    refExpr = isSgAsmx86RegisterReferenceExpression(expr);
    if (refExpr) {
      // check if it could be a memory reference
      // changed by tps on 16Jan08

      SgAsmExpression* offset = refExpr->get_offset();
      if (isSgAsmMemoryReferenceExpression(offset)) {
	// is a memory reference
	memoryReference = true;
	SgAsmMemoryReferenceExpression* mem = isSgAsmMemoryReferenceExpression(offset);
	SgAsmExpression* memOff = mem->get_address();
	if (isSgAsmx86RegisterReferenceExpression(memOff)) {
	  SgAsmx86RegisterReferenceExpression* memRegRef = isSgAsmx86RegisterReferenceExpression(memOff);
	  code = memRegRef->get_identifier();
	}
      } else {
	// is a register reference
	code = refExpr->get_identifier();
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
				      SgAsmx86Instruction* inst,
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

    refExpr = isSgAsmx86RegisterReferenceExpression(expr);
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
  } else if (isSgAsmx86RegisterReferenceExpression(expr)) {
    SgAsmx86RegisterReferenceExpression* refexp = isSgAsmx86RegisterReferenceExpression(expr);
    std::pair<X86RegisterClass, int>  code;
    if (refexp) {
      code = refexp->get_identifier();
      // we have to add up this value, currently we assign 5000 to the register
      // fixme later
      if (code.first == x86_regclass_gpr && code.second == x86_gpr_bp)
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
  } else if (isSgAsmBinaryMultiply(expr)) {
    // add up the left and right side
    SgAsmBinaryMultiply* binadd = isSgAsmBinaryMultiply(expr);
    SgAsmExpression* left = binadd->get_lhs();
    SgAsmExpression* right = binadd->get_rhs();
    res = getValueInMemoryRefExp(left) * getValueInMemoryRefExp(right);
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
RoseBin_DataFlowAbstract::getValueOfInstr( SgAsmx86Instruction* inst,
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
                                                SgGraphNode* node,
                                                std::pair<X86RegisterClass, int>  codeSearch,
                                                bool& cantTrack,
                                                SgAsmx86RegisterReferenceExpression* refExpr_rightHand) {
  int64_t value = 0xffffffff;
  if (RoseBin_support::DEBUG_MODE())
    cout << "    ........ trying to resolve value for register :: " << codeSearch.first << "." << codeSearch.second << endl;

  SgAsmx86Instruction* inst = isSgAsmx86Instruction(node->get_SgNode());
  ROSE_ASSERT(inst);
  std::pair<X86RegisterClass, int>  code = refExpr_rightHand->get_identifier();
  // iterate up and find an assignment to this register codeSearch i.e. instr codeSearch, esi

  bool condInst = RoseBin_support::isConditionalInstruction(inst);
  bool condInstFlag = RoseBin_support::isConditionalFlagInstruction(inst);
  if (condInstFlag==false) {
    // the instruction is not dependent on a flag
    if (condInst==false) {
      // the instruction is not dependent on a value in one of its operands
      // easiest track
      SgGraphNode* previous = getPredecessor(node);
	/*
      vector <SgGraphNode*> vec;
      vizzGraph->getPredecessors(node, vec);
      if (vec.size()==1) {
	// found one predecessor
	SgGraphNode* previous = vec.back();
	ROSE_ASSERT(previous);
	string name = vizzGraph->getProperty(SgGraph::name, previous);
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
		code.first << "." << code.second << endl;
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
	code.first << "." << code.second << endl;
    }
    cantTrack =true;
  }


  return value;
}

SgGraphNode*
RoseBin_DataFlowAbstract::getPredecessor(SgGraphNode* node){
  vector <SgGraphNode*> vec;
  g_algo->getDirectCFGPredecessors(vizzGraph,node, vec);
  SgGraphNode* previous = NULL;
  if (vec.size()==1) {
    // found one predecessor
    previous = vec.back();
    ROSE_ASSERT(previous);
    string name = vizzGraph->getProperty(SgGraph::name, previous);
    if (RoseBin_support::DEBUG_MODE())
      cout << "    tracking recursive var " << name << endl;

  } else if (vec.size()>1) {
    cerr << " Tracking:: Problem, we have more than one predecessor for a node... cant track this " << endl;
    exit(0);
  }
  return previous;
}

SgGraphNode*
RoseBin_DataFlowAbstract::getSuccessor(SgGraphNode* node){
  vector <SgGraphNode*> vec;
  //cerr << endl << " START ++++++++++++ getSuccessor : call getDirectCFGSucc " << endl;
  g_algo->getDirectCFGSuccessors(vizzGraph,node, vec);
  SgGraphNode* after = NULL;
  //cerr << " END +++++++++++ getSuccessor : >>>>>>>>>>>>>>> Outedges for node " << 
  //    node->get_index() << " " << node->get_name() << " " <<
  //  node->get_SgNode()->class_name() << 
  //   "   size: " <<  vec.size() << endl;
  if (vec.size()==1) {
    // found one predecessor
    after = vec.back();
    ROSE_ASSERT(after);
    string name = vizzGraph->getProperty(SgGraph::name, after);
    if (RoseBin_support::DEBUG_MODE())
      cout << "    tracking recursive var " << name << endl;
    //cerr << " ............ DFA " << node->get_name() << "    succ : " << after->get_name() << endl;
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

  rose_hash::unordered_map <uint64_t, RoseBin_Variable*>::iterator it = variables.find(position);
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

// DQ (4/23/2009): Made a typedef for the type used here since it is sensative to the template default parameters.
// rose_hash::unordered_map <std::string,uint64_t>::iterator it = variablesReverse.find(var);
  variablesReverseType::iterator it = variablesReverse.find(var);

  if (it!=variablesReverse.end()) {
    val = it->second;
  }

  return getVariable(val);
}

RoseBin_Variable*
RoseBin_DataFlowAbstract::getVariable(uint64_t pos) {
  RoseBin_Variable* var=NULL;
  rose_hash::unordered_map <uint64_t, RoseBin_Variable*>::iterator it = variables.find(pos);
  if (it!=variables.end()) {
    var = it->second;
  }
  return var;
}

