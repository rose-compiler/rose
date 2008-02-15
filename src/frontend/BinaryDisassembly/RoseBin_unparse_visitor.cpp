/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/

#include "rose.h"
#include "RoseBin_unparse_visitor.h"
#include "RoseBin_DataFlowAnalysis.h"
#include <iomanip>

using namespace std;

/****************************************************
 * resolve expression
 ****************************************************/
string RoseBin_unparse_visitor::resolveRegister(SgAsmRegisterReferenceExpression::x86_register_enum code,
						SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos) {
  string res="";
  switch (code) {
  case SgAsmRegisterReferenceExpression::rAX: {
    res="rAX:undefined";
    if (pos==SgAsmRegisterReferenceExpression::low_byte)
      res="al";
    else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte)
	res="ah";
      else 
	if (pos==SgAsmRegisterReferenceExpression::word)
	  res="ax";
	else 
	  if (pos==SgAsmRegisterReferenceExpression::dword)
	    res="eax";
	  else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      res="rax";
    break; 
  }
  case SgAsmRegisterReferenceExpression::rBX: {
    res="rBX:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::low_byte)
      res="bl";
    else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte)
	res="bh";
      else 
	if (pos==SgAsmRegisterReferenceExpression::word)
	  res="bx";
	else 
	  if (pos==SgAsmRegisterReferenceExpression::dword)
	    res="ebx";
	  else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      res="rbx";
    break;
  }
  case SgAsmRegisterReferenceExpression::rCX: {
    res="rCX:undefined";
    if (pos==SgAsmRegisterReferenceExpression::low_byte)
      res="cl";
    else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte)
	res="ch";
      else 
	if (pos==SgAsmRegisterReferenceExpression::word)
	  res="cx";
	else 
	  if (pos==SgAsmRegisterReferenceExpression::dword)
	    res="ecx";
	  else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      res="rcx";
    break;
  } 
  case SgAsmRegisterReferenceExpression::rDX: {
    res="rDX:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::low_byte)
      res="dl";
    else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte)
	res="dh";
      else 
	if (pos==SgAsmRegisterReferenceExpression::word)
	  res="dx";
	else 
	  if (pos==SgAsmRegisterReferenceExpression::dword)
	    res="edx";
	  else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      res="rdx";
    break;
  }
  case SgAsmRegisterReferenceExpression::rDI: {
    res="rDI:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::word)
      res="di";
    else 
      if (pos==SgAsmRegisterReferenceExpression::dword)
	res="edi";
      else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  res="rdi";
    break;
  }
  case SgAsmRegisterReferenceExpression::rSI: {
    res="rSI:undefined";
    if (pos==SgAsmRegisterReferenceExpression::word)
      res="si";
    else 
      if (pos==SgAsmRegisterReferenceExpression::dword)
	res="esi";
      else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  res="rsi";
    break;
  } 
  case SgAsmRegisterReferenceExpression::rSP: {
    res="rSP:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::word)
      res="sp";
    else 
      if (pos==SgAsmRegisterReferenceExpression::dword)
	res="esp";
      else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  res="rsp";
    break;
  }
  case SgAsmRegisterReferenceExpression::rBP: {
    res="rBP:undefined";
    if (pos==SgAsmRegisterReferenceExpression::word)
      res="bp";
    else 
      if (pos==SgAsmRegisterReferenceExpression::dword)
	res="ebp";
      else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  res="rbp";
    break;
  } 
  case SgAsmRegisterReferenceExpression::r8: {
    res="r8";
    break;
  } 
  case SgAsmRegisterReferenceExpression::r9: {
    res="r9";
    break;
  } 
  case SgAsmRegisterReferenceExpression::r10: {
    res="r10";
    break;
  } 
  case SgAsmRegisterReferenceExpression::r11: {
    res="r11";
    break;
  } 
  case SgAsmRegisterReferenceExpression::r12: {
    res="r12";
    break;
  } 
  case SgAsmRegisterReferenceExpression::r13: {
    res="r13";
    break;
  } 
  case SgAsmRegisterReferenceExpression::r14: {
    res="r14";
    break;
  } 
  case SgAsmRegisterReferenceExpression::r15: {
    res="r15";
    break;
  } 
  case SgAsmRegisterReferenceExpression::CS: {
    res="cs";
    break;
  } 
  case SgAsmRegisterReferenceExpression::DS: {
    res="ds";
    break;
  } 
  case SgAsmRegisterReferenceExpression::SS: {
    res="ss";
    break;
  } 
  case SgAsmRegisterReferenceExpression::ES: {
    res="es";
    break;
  } 
  case SgAsmRegisterReferenceExpression::FS: {
    res="fs:undefined";
    if (pos==SgAsmRegisterReferenceExpression::dword)
      res="fs";
    else 
      if (pos==SgAsmRegisterReferenceExpression::qword)
	res="rfs";
    break;
  } 
  case SgAsmRegisterReferenceExpression::GS: {
    res="gs:undefined";
    if (pos==SgAsmRegisterReferenceExpression::dword)
      res="gs";
    else 
      if (pos==SgAsmRegisterReferenceExpression::qword)
	res="rgs";
    break;
  } 
  default:
    res = "undefined";
    break;
  }
  return res;
}

/****************************************************
 * resolve expression
 ****************************************************/
string RoseBin_unparse_visitor::resolveRegister(SgAsmRegisterReferenceExpression::arm_register_enum code,
						SgAsmRegisterReferenceExpression::arm_position_in_register_enum pos) {
  string res="";
  switch (code) {
  case SgAsmRegisterReferenceExpression::reg1: {
    res="r1";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg2: {
    res="r2";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg3: {
    res="r3";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg4: {
    res="r4";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg5: {
    res="r5";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg6: {
    res="r6";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg7: {
    res="r7";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg8: {
    res="r8";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg9: {
    res="r9";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg10: {
    res="r10";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg11: {
    res="r11";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg12: {
    res="r12";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg13: {
    res="r13";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg14: {
    res="r14";
    break;
  } 
  case SgAsmRegisterReferenceExpression::reg15: {
    res="r15";
    break;
  } 
  case SgAsmRegisterReferenceExpression::SP: {
    res="sp";
    break;
  } 
  case SgAsmRegisterReferenceExpression::PC: {
    res="pc";
    break;
  } 
  case SgAsmRegisterReferenceExpression::LR: {
    res="lr";
    break;
  } 
  case SgAsmRegisterReferenceExpression::SL: {
    res="sl";
    break;
  } 
  case SgAsmRegisterReferenceExpression::IP: {
    res="ip";
    break;
  } 
  case SgAsmRegisterReferenceExpression::FP: {
    res="fp";
    break;
  } 
  default:
    break;
  }
  return res;
}

/****************************************************
 * resolve binary expression, plus, minus, etc 
 ****************************************************/
string RoseBin_unparse_visitor::resolveBinaryBinaryExpression(SgAsmBinaryExpression* expr) {
  string res="...";
  if (isSgAsmBinaryAdd(expr)) {
    res = "+";
  } else
    if (isSgAsmBinarySubtract(expr)) {
      res = "-";
    } else
      if (isSgAsmBinaryMod(expr)) {
	res = "%";
      } else
	if (isSgAsmBinaryDivide(expr)) {
	  res = "/";
	} else
	  if (isSgAsmBinaryMultiply(expr)) {
	    res = "*";
	  }

  return res;
}


/****************************************************
 * resolve mnemonic
 ****************************************************/
string RoseBin_unparse_visitor::get_mnemonic_from_instruction(SgAsmExpression* expr) {
  string mnemonic="";
	// attention: slow operation. Need to go up the tree to find the Instruction
	SgAsmInstruction* binInst = NULL;
	SgAsmNode* node = expr;

	while (binInst==NULL) {
	  //cerr << " node : " << node->class_name() << endl;
	  SgNode* node2 = node->get_parent();
	  if (node2==NULL) {cerr << " no_parent " << endl; exit(0);}

	  node = isSgAsmNode(node2);
	  ROSE_ASSERT(node);
	  binInst = isSgAsmInstruction(node);
	  if (binInst!=NULL) {
	    mnemonic = binInst->get_mnemonic();
	  }
	}
	return mnemonic;
}

string RoseBin_unparse_visitor::resolveOperand(SgAsmExpression* expr, string *type,
					       RoseBin_DataFlowAbstract* dfa) {
  encode=true;
  analysis = dfa;
  ROSE_ASSERT(analysis);
  string replace = "";
  string val = resolveOperand(expr, type, &replace);
  //cerr << " found val and type " << val << " " << *type << endl;
  analysis=NULL;
  encode=false;
  return val;
}

/****************************************************
 * resolve expression
 ****************************************************/
string RoseBin_unparse_visitor::resolveOperand(SgAsmExpression* expr, string *type,
					       string *replace) {
  string res="...";
  ROSE_ASSERT(expr);
  ROSE_ASSERT(replace);
  ROSE_ASSERT(type);
  if (isSgAsmRegisterReferenceExpression(expr)) {
    // 1. its a RegisterReferenceExpression -------------------------
    //SgAsmExpression* offset = NULL;
    SgAsmRegisterReferenceExpression* refExpr = isSgAsmRegisterReferenceExpression(expr);
    if (RoseAssemblyLanguage==x86) {
      SgAsmRegisterReferenceExpression::x86_register_enum code = refExpr->get_x86_register_code();
      SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos = refExpr->get_x86_position_in_register_code();
      //offset = refExpr->get_offset();
    // if offset==null, then we have no children, must be a plain register
      res = resolveRegister(code, pos);

    } else if (RoseAssemblyLanguage==arm) {
      SgAsmRegisterReferenceExpression::arm_register_enum code = refExpr->get_arm_register_code();
      SgAsmRegisterReferenceExpression::arm_position_in_register_enum pos = refExpr->get_arm_position_in_register_code();
      //offset = refExpr->get_offset();
    // if offset==null, then we have no children, must be a plain register
      res = resolveRegister(code, pos);
    } 
    //if (offset!=NULL) {
      // Segment register -- remove for now FIXME
      //res = "";
      // res=res+":";
    //}

    *replace = expr->get_replacement();
    if (*replace!="") 
      res = res + " <" + *replace + ">";
    // recursive call to check children
    //if (offset!=NULL)
    // res = res + resolveOperand(offset, type, replace);

  } else
    // 2. its a MemoryReferenceExpression -------------------------
    if (isSgAsmMemoryReferenceExpression(expr)) {
      SgAsmMemoryReferenceExpression* memRef = isSgAsmMemoryReferenceExpression(expr);
      SgAsmExpression* address = memRef->get_address();
      ROSE_ASSERT(address);

      string operand = resolveOperand(address, type, replace);

      string mnemonic = "";
      mnemonic = get_mnemonic_from_instruction(expr);
      bool exception = false;
      if (mnemonic=="lea")
	exception=true;
      res = "[";
      if (exception==false) {
	string sizeString = *type; // FIXME
	res = sizeString + " PTR [";
      }
      *replace = expr->get_replacement();
      if (*replace!="") 
	res = res + " <" + *replace + ">";

      res = res + operand;
      res = res+"]";

    } else
    // 3. its a ValueExpression -------------------------
      if (isSgAsmValueExpression(expr)) {
	SgAsmValueExpression* valExp = isSgAsmValueExpression(expr);

	string mnemonic = "";
	mnemonic = get_mnemonic_from_instruction(expr);
	bool is_mnemonic_call = false;
	if (mnemonic=="call")
	  is_mnemonic_call=true;
	//unsigned int dw_v=0;
	uint8_t byte_val=0xF;
	uint16_t word_val=0xFF;
	uint32_t double_word_val=0xFFFF;
	uint64_t quad_word_val=0xFFFFFFFF;
	res = RoseBin_support::resolveValue(valExp, is_mnemonic_call,
				      byte_val,
				      word_val,
				      double_word_val,
				      quad_word_val);

	if (isSgAsmByteValueExpression(valExp)) {
	  *type ="BYTE";
	}
	if (isSgAsmWordValueExpression(valExp)) {
	  *type ="WORD";
	}
	if (isSgAsmDoubleWordValueExpression(valExp)) {
	  *type ="DWORD";
	}
	if (isSgAsmQuadWordValueExpression(valExp)) {
	  *type="QWORD";
	}
	//*replace="<" + RoseBin_support::ToString(valExp) + ">"+" rep= " + valExp->get_replacement();
	*replace = valExp->get_replacement();
	if (*replace!="") 
	  res = res + " <" + *replace + ">";
      } 
    // 4. its a BinaryExpression -------------------------	
	else if (isSgAsmBinaryExpression(expr)) {
	  SgAsmBinaryExpression* binExp = isSgAsmBinaryExpression(expr);
	  res = resolveBinaryBinaryExpression(binExp);

	  SgAsmExpression* left = binExp->get_lhs();
	  SgAsmExpression* right = binExp->get_rhs();

	  if (left && right) {
	    string type1="";
	    string type2="";
	    string resLeft = resolveOperand(left,&type1,replace);
	    string resRight = resolveOperand(right,&type2,replace);
	    //cerr << " type left : " << type1 << "   type right : " << type2 << endl;
	    if (type1==type2) *type=type1;
	    else if (type1=="DWORD" && type2=="QWORD") *type="QWORD";
	    else if (type1=="QWORD" && type2=="DWORD") *type="QWORD";
	    uint64_t l =0;
	    uint64_t r =0;
	    if (encode) {
	      SgAsmRegisterReferenceExpression* refExpr=NULL;
	      if (isSgAsmRegisterReferenceExpression(left)) {
		refExpr =  isSgAsmRegisterReferenceExpression(left);
		SgAsmRegisterReferenceExpression::x86_register_enum code ;
		SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos ;
		code = refExpr->get_x86_register_code();
		pos = refExpr->get_x86_position_in_register_code();
		ROSE_ASSERT(analysis);
		//RoseBin_DataFlowAbstract* dfa = dynamic_cast<RoseBin_DataFlowAbstract*>(analysis);
		//ROSE_ASSERT(dfa);
		analysis->getRegister_val(code, pos, l);
	      } else
		RoseBin_support::from_string<uint64_t>(l, resLeft, std::hex);

	      if (isSgAsmRegisterReferenceExpression(right)) {
		refExpr =  isSgAsmRegisterReferenceExpression(right);
		SgAsmRegisterReferenceExpression::x86_register_enum code ;
		SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos ;
		code = refExpr->get_x86_register_code();
		pos = refExpr->get_x86_position_in_register_code();
		ROSE_ASSERT(analysis);
		//RoseBin_DataFlowAbstract* dfa = dynamic_cast<RoseBin_DataFlowAbstract*>(analysis);
		//ROSE_ASSERT(dfa);
		analysis->getRegister_val(code, pos, r);
	      } else
		RoseBin_support::from_string<uint64_t>(r, resRight, std::hex);
	      int64_t res_h=0;
	      if (res=="+") res_h=l+r;
	      else if (res=="-") res_h=l-r;
	      else if (res=="*") res_h=l*r;
	      else if (res=="/") res_h=l/r;
	      else if (res=="%") res_h=l%r;

	      res = RoseBin_support::HexToString(res_h);

	    } else
	      res = resLeft + res + resRight;
	  }


	  //	  res = resolveOperand(left,type) + res + resolveOperand(right,type);
	}

	else {
	  cerr << " unknown expression " << expr->class_name() << endl;
	  exit(0);
	}

  if (!expr) res = "0";

  if (res == "...") {
    ROSE_ASSERT (expr);
    cout << expr->class_name() << endl;
    ROSE_ASSERT (!"Unhandled expression kind");
  }

  return res;
}



string 
RoseBin_unparse_visitor::unparseInstruction(SgAsmInstruction* binInst) {
  SgAsmOperandList* opList = binInst->get_operandList();
  ROSE_ASSERT(opList);
  string mnemonic = "none";

  mnemonic = binInst->get_mnemonic();
  unsigned int address = binInst->get_address();
  ostringstream addrhex;
  addrhex << hex << setw(8) << address ;
  
  //cerr << " unparseing " << addrhex.str() << "  " << mnemonic << endl;
  //string address_str = RoseBin_support::ToString(address);
  string address_str = addrhex.str();
  SgAsmNode* parent_block = dynamic_cast<SgAsmNode*>( binInst->get_parent());
  //ROSE_ASSERT(parent_block);
  if (parent_block==NULL) {
    cerr << " ERROR : Unparser - Node has no parent : " << address_str << " - " << 
      binInst->class_name() << endl;
  }
  /*
  SgAsmBlock* block = isSgAsmBlock(parent_block);
  ROSE_ASSERT(block);
  unsigned int block_address = block->get_address();
  unsigned int nextB_tid = block->get_next_block_true_address();
  unsigned int nextB_fid = block->get_next_block_false_address();
  */

  //if (address==134513359)
  // cerr << " !!!!!!!!! unparser contains 82cf ... " << endl;

  /*
    if (block_address!=previous_block) {
    // a new block begins
    if (nextB_fid==0 and nextB_tid==0) {
    // if both true and false branch are 0, its strange
    cerr << " >>> new Block: " << block_address<< " -------------------- next Block (switch?) "  << endl;
    } else
    if (nextB_fid==0)
    cerr << " >>> new Block: " << block_address<< " -------------------- next Block (unconditional): " << nextB_tid << endl;
    else
    cerr << " >>> new Block: " << block_address<< " -------------------- next Block (true): " << nextB_tid << " (false): " << nextB_fid << endl;
    }
  */

  string operands = "";
  int counter=0;
  SgAsmExpressionPtrList ptrList = opList->get_operands();
  string lastType = "DWORD";
  for (SgAsmExpressionPtrList::reverse_iterator it=ptrList.rbegin(); it!=ptrList.rend(); ++it) { // Do backwards to propagate types from constants to mem refs
    SgAsmExpression* expr = *it;
    ROSE_ASSERT(expr);
    // resolve each operand
    string type = lastType;
    string replace="";
    string result = resolveOperand(expr,&type,&replace);
    lastType = type;
    operands = result + operands;
    if (counter < (int) (ptrList.size()-1))
      operands = "," + operands;
    counter++;
  }

  // print the instruction
  string instruction = address_str + ":\t" + mnemonic + string(mnemonic.size() > 6 ? 1 : 7 - mnemonic.size(), ' ') + operands;

  return instruction;
}

/****************************************************
 * unparse binary instruction
 ****************************************************/
void RoseBin_unparse_visitor::visit(SgNode* n) {
  SgAsmInstruction* binInst = isSgAsmInstruction(n);
  if (isSgAsmBlock(n)) {
 // myfile << "; Block 0x" << std::hex << isSgAsmBlock(n)->get_id() << "\n";
     myfile << "/* Block 0x" << std::hex << isSgAsmBlock(n)->get_id() << " */ \n";
  } else if (isSgAsmFunctionDeclaration(n)) {
     myfile << "/* Function 0x" << std::hex << isSgAsmFunctionDeclaration(n)->get_address() << ": " << isSgAsmFunctionDeclaration(n)->get_name() << " */ \n";
  }
  if (binInst==NULL) return;

  string instruction = unparseInstruction(binInst);
  //cerr <<  instruction << endl;

  myfile << instruction << "\n";

  //previous_block = block_address;
}


/****************************************************
 * init the filename
 ****************************************************/
void RoseBin_unparse_visitor::init(char* filename) {
  myfile.open(filename);
}

/****************************************************
 * close the file
 ****************************************************/
void RoseBin_unparse_visitor::close() {
  ROSE_ASSERT(myfile);
  myfile.close();  
}

