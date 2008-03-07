/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 7Sep07
 * Decription : Dataflow analysis
 ****************************************************/
#include "rose.h"
#include "RoseBin_Emulate.h"

using namespace std;





void 
RoseBin_Emulate::getRegister_val(SgAsmRegisterReferenceExpression::x86_register_enum code,
					 SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos,
					 uint64_t &val) {
  uint8_t b_val=0xF;
  uint16_t w_val=0xFF;
  uint32_t dw_val=0xFFFF;
  uint64_t qw_val=0xFFFFFFFF;

  getRegister_val(code, pos, b_val, w_val, dw_val, qw_val);
  
  val =0xFFFFFFFF;
  if (b_val!=0xF)
    val = uint64_t(b_val);
  else if (w_val!=0xFF)
    val = uint64_t(w_val);
  else if (dw_val!=0xFFFF)
    val = uint64_t(dw_val);
  else if (qw_val!=0xFFFFFFFF)
    val = uint64_t(qw_val);

}

void 
RoseBin_Emulate::getRegister_val(SgAsmRegisterReferenceExpression::x86_register_enum code,
					 SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos,
					 uint8_t &b_val,
					 uint16_t &w_val,
					 uint32_t &dw_val,
					 uint64_t &qw_val) {


  uint8_t b8,b7=0xF;
  uint16_t b78=0xFF;
  uint32_t b5678=0xFFFF;
  
  string res="";
  switch (code) {
  case SgAsmRegisterReferenceExpression::rAX: {
    // assuming little endian
    if (!RoseBin_support::bigEndian()) {
      b7 = ( rax>>8 ) & 255;
      b8 = ( rax ) & 255;
      b78 = ( rax ) & 65535;
      //      b5678 = ( rax ) & 4294967295;
      b5678 = ( rax ) & 0xffffffff;
    } else {
      cerr << " We dont handle big endian's " << endl;
      exit(0);
    }
    res="rAX:undefined";
    if (pos==SgAsmRegisterReferenceExpression::low_byte)
      b_val = b8;
    else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte)
	b_val = b7;
      else 
	if (pos==SgAsmRegisterReferenceExpression::word)
	  w_val = b78;
	else 
	  if (pos==SgAsmRegisterReferenceExpression::dword)
	    dw_val = b5678;
	  else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      qw_val = rax;
    break; 
  }
  case SgAsmRegisterReferenceExpression::rBX: {
    // assuming little endian
    if (!RoseBin_support::bigEndian()) {
      b7 = ( rbx>>8 ) & 255;
      b8 = ( rbx ) & 255;
      b78 = ( rbx ) & 65535;
      b5678 = ( rbx ) & 0xffffffff;
    } else {
      cerr << " We dont handle big endian's " << endl;
      exit(0);
    }
    res="rBX:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::low_byte)
      b_val = b8;
    else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte)
	b_val = b7;
      else 
	if (pos==SgAsmRegisterReferenceExpression::word)
	  w_val = b78;
	else 
	  if (pos==SgAsmRegisterReferenceExpression::dword)
	    dw_val = b5678;
	  else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      qw_val = rbx;
    break;
  }
  case SgAsmRegisterReferenceExpression::rCX: {
    // assuming little endian
    if (!RoseBin_support::bigEndian()) {
      b7 = ( rcx>>8 ) & 255;
      b8 = ( rcx ) & 255;
      b78 = ( rcx ) & 65535;
      b5678 = ( rcx ) & 0xffffffff;
    } else {
      cerr << " We dont handle big endian's " << endl;
      exit(0);
    }
    res="rCX:undefined";
    if (pos==SgAsmRegisterReferenceExpression::low_byte)
      b_val =b8;
    else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte)
	 b_val =b7;
      else 
	if (pos==SgAsmRegisterReferenceExpression::word)
	   w_val =b78;
	else 
	  if (pos==SgAsmRegisterReferenceExpression::dword)
	     dw_val= b5678;
	  else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      qw_val =rcx;
    break;
  } 
  case SgAsmRegisterReferenceExpression::rDX: {
    // assuming little endian
    if (!RoseBin_support::bigEndian()) {
      b7 = ( rdx>>8 ) & 255;
      b8 = ( rdx ) & 255;
      b78 = ( rdx ) & 65535;
      b5678 = ( rdx ) & 0xffffffff;
    } else {
      cerr << " We dont handle big endian's " << endl;
      exit(0);
    }
    res="rDX:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::low_byte)
       b_val =b8;
    else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte)
	b_val=b7;
      else 
	if (pos==SgAsmRegisterReferenceExpression::word)
	   w_val =b78;
	else 
	  if (pos==SgAsmRegisterReferenceExpression::dword)
	    dw_val =b5678;
	  else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      qw_val=rdx;
    break;
  }
  case SgAsmRegisterReferenceExpression::rDI: {
    // assuming little endian
    if (!RoseBin_support::bigEndian()) {
      b78 = ( rdi ) & 65535;
      b5678 = ( rdi ) & 0xffffffff;
    } else {
      cerr << " We dont handle big endian's " << endl;
      exit(0);
    }
    res="rDI:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::word)
       w_val=b78;
    else 
      if (pos==SgAsmRegisterReferenceExpression::dword)
	 dw_val=b5678;
      else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  qw_val=rdi;
    break;
  }
  case SgAsmRegisterReferenceExpression::rSI: {
    // assuming little endian
    if (!RoseBin_support::bigEndian()) {
      b78 = ( rsi ) & 65535;
      b5678 = ( rsi ) & 0xffffffff;
    } else {
      cerr << " We dont handle big endian's " << endl;
      exit(0);
    }
    res="rSI:undefined";
    if (pos==SgAsmRegisterReferenceExpression::word)
       w_val=b78;
    else 
      if (pos==SgAsmRegisterReferenceExpression::dword)
	dw_val=b5678;
      else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  qw_val=rsi;
    break;
  } 
  case SgAsmRegisterReferenceExpression::rSP: {
    // assuming little endian
    if (!RoseBin_support::bigEndian()) {
      b78 = ( rsp ) & 65535;
      b5678 = ( rsp ) & 0xffffffff;
    } else {
      cerr << " We dont handle big endian's " << endl;
      exit(0);
    }
    res="rSP:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::word)
       w_val=b78;
    else 
      if (pos==SgAsmRegisterReferenceExpression::dword)
	dw_val=b5678;
      else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  qw_val=rsp;
    break;
  }
  case SgAsmRegisterReferenceExpression::rBP: {
    // assuming little endian
    if (!RoseBin_support::bigEndian()) {
      b78 = ( rbp ) & 65535;
      b5678 = ( rbp ) & 0xffffffff;
    } else {
      cerr << " We dont handle big endian's " << endl;
      exit(0);
    }
    res="rBP:undefined";
    if (pos==SgAsmRegisterReferenceExpression::word)
      w_val=b78;
    else 
      if (pos==SgAsmRegisterReferenceExpression::dword)
	dw_val=b5678;
      else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  qw_val=rbp;
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
    break;
  }
  //return res;
}


uint64_t RoseBin_Emulate::getRandomValue(int val) {
  int random_integer = rand()%val;
  return random_integer;//0x10ULL;
}




bool
RoseBin_Emulate::evaluateInstruction( SgAsmInstruction* binInst, string& operands) {
  SgAsmOperandList* opList = binInst->get_operandList();
  ROSE_ASSERT(opList);
  //string operands = "";
  bool success = true;

  // ***************************************************************
  // handle special cases
  // handle function calls to cpuid and int
  if (isSgAsmx86Cpuid(binInst)) {
    operands += " :: specialOp cpuid";
    //clearRegisters();
    rbx = getRandomValue(100); // example return value
    rcx = getRandomValue(100); // example return value
    rdx = getRandomValue(100); // example return value
    return success;
  }
  else if (isSgAsmx86Int(binInst)) {
    operands += " :: specialOp Int";
    //clearRegisters();
    getRegister_val(SgAsmRegisterReferenceExpression::rAX, SgAsmRegisterReferenceExpression::qword, rax);
    // should get the values from memory!
    string values = "";
    rose_hash::hash_map <uint64_t, uint64_t>::iterator it = memory.begin();                                      
    for (;it!=memory.end();++it) {
      uint64_t loc = it->first;
      uint64_t val = it->second;
      string loc_s = RoseBin_support::HexToString(loc);
      string val_s = RoseBin_support::HexToString(val);
      values+= "("+loc_s+":"+val_s+")-";
    }

    //uint64_t rbx_v = getMemory(0x1c);
    //uint64_t rcx_v = getMemory(0x20);
    //uint64_t rdx_v = getMemory(0x24);
    if (rax==4) {
      // sys_write
      //string values = "" + RoseBin_support::HexToString(rbx_v);
      //values += "-" + RoseBin_support::HexToString(rcx_v);
      //values += "-" + RoseBin_support::HexToString(rdx_v);
      operands += " :: syswrite : "+values;
    } else {
      success = false;
    }
    return success;
  }
  // handle call to rdtsc  
  else if (binInst->get_mnemonic()=="rdtsc") {
    // simulate timer function
    operands += " :: specialOp rdtsc";
    rdx = 0x0ULL;
    rax = getRandomValue(10);
    return success;
  }
  // **************************************************************
  

  int counter=0;
  SgAsmRegisterReferenceExpression* refExpr =NULL;
  SgAsmRegisterReferenceExpression::x86_register_enum code ;
  SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos ;

  // iterate through the operands (for x86 = 2 operands)
  SgAsmExpressionPtrList ptrList = opList->get_operands();
  SgAsmMemoryReferenceExpression* memRef = NULL;
  for (SgAsmExpressionPtrList::iterator it=ptrList.begin(); it!=ptrList.end(); ++it) {
    SgAsmExpression* expr = *it;
    ROSE_ASSERT(expr);
    //string type = "DWORD";
    //string result = unparser->resolveOperand(expr,&type);
    if (counter==0) {
      // left hand side *************************************************************
      refExpr =	isSgAsmRegisterReferenceExpression(expr);
      
      // check what it could be
      // ******** 1. its a RegisterReferenceExpression on the left side
      if (refExpr) {
	code = refExpr->get_x86_register_code();
	pos = refExpr->get_x86_position_in_register_code();
	operands = "\\nleft :: refExpr ";
	//SgAsmExpression* expression = refExpr->get_offset();
	//memRef = isSgAsmMemoryReferenceExpression(expression);
	//if (memRef)
	//  operands += " :: memoryRefExp ";
	operands += "\\n";
      } 

      // ******** 2. Its a BitAndByteInstruction
      else if (isSgAsmx86BitAndByteInstruction(binInst)) {
	if (isSgAsmx86Sete(binInst)) {
	  // if the instruction is a sete, then we want to check ZF and set the 
	  // proper register to 1
	  if (ZF) {
	    uint64_t qw_val=1;
	    assignRegister(code, qw_val);
	    ZF=false;
	  }
	}
      } // bit and byte instruction

      // ******** 3. Its an ArithmeticInstruction
      else if (isSgAsmx86ArithmeticInstruction(binInst)) {
	if (isSgAsmx86Dec(binInst)) {
	  uint64_t left = getRegister(code);	    
	  assignRegister(code, --left);	    
	}
      } // binaryArithmetic
      
      else {
	success = false;
      }


    } else {
      // right hand side ************************************************************
      SgAsmValueExpression* valExp = isSgAsmValueExpression(expr);
      SgAsmRegisterReferenceExpression* refExprR = isSgAsmRegisterReferenceExpression(expr);
      uint8_t b_val=0xF;
      uint16_t w_val=0xFF;
      uint32_t dw_val=0xFFFF;
      uint64_t qw_val=0xFFFFFFFF;

      // check what it could be
      // ****** 1. valueExpression
      if (valExp) {
	// value expr
	operands += "right :: valExp ";
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

	if (refExpr) {
	  // 1.1 ValueExp:: data transfer instruction
	  if (isSgAsmx86DataTransferInstruction(binInst)) {
	    operands += " :: DataTrans";
	    // mov instruction
	    if (isSgAsmx86Mov(binInst)) {
	      operands += " :: Mov ";
	      assignRegister(code, pos, b_val,
			     w_val, dw_val,
			     qw_val);
	      uint64_t addr_value=0;
	      getRegister_val(code, SgAsmRegisterReferenceExpression::qword, addr_value);
	      //string str="";
	      //string var = createVariable(addr_value, str, type, description, length);
	      //string varHex = RoseBin_support::HexToString(addr_value) ;
	      //operands += "\\nVariable : " + var + "("+varHex+")";
	    }
	  }

	  // 1.2 ValueExp:: logical instruction -- and, or , xor
	  else if (isSgAsmx86LogicalInstruction(binInst)) {
	    operands += " :: Logical";
	    if (isSgAsmx86And(binInst)) {
	      operands += " :: And";
	      uint64_t reg = getRegister(code);
	      operands+=printRegister("reg",reg);
	      operands+=printRegister("val",val);
	      reg &= val;
	      assignRegister(code, reg);
	      if (reg==0)
		ZF = true;
	      else ZF=false;
	    }
	  } // logical

	  // ValueExp:: 1.3 arithmetic instruction
	  else if (isSgAsmx86ArithmeticInstruction(binInst)) {
	    operands += " :: Arithm";
	    if (isSgAsmx86Add(binInst)) {
	      operands += " :: Add";
	      uint64_t left = getRegister(code);	    
	      uint64_t sum = left+val;
	      assignRegister(code, sum);	    
	    }
	  } // binaryArithmetic

	  else {
	    success = false;
	  }

	} // refExp
	else { success = false;}
      } // valExp
      else if (refExprR) {
        // ****** 2. referenceExpression
	// the right hand side is also a register or memory location
	SgAsmRegisterReferenceExpression::x86_register_enum codeR ;
	SgAsmRegisterReferenceExpression::x86_position_in_register_enum posR ;
	codeR = refExprR->get_x86_register_code();
	posR = refExprR->get_x86_position_in_register_code();
	
	operands += "right ::  refExpr ";
	
	// 2.1 RefExp:: Mov Instruction
	if (isSgAsmx86DataTransferInstruction(binInst)) {
	  operands += " :: DataTrans";
	  if (isSgAsmx86Mov(binInst)) {
	    operands += " :: Mov";
	    if (memRef) {
	      operands += " :: MemRef ";
	      SgAsmExpression* mem_loc = memRef->get_address();
	      string res = unparser->resolveOperand(mem_loc, this);
	      uint64_t pos = 0;
	      RoseBin_support::from_string<uint64_t>(pos, res, std::hex);
	      uint64_t value = getRegister(codeR);
	      assignMemory(pos, value);
	      operands += "["+res+"] "+" ("+RoseBin_support::HexToString(pos)+":"+RoseBin_support::HexToString(value)+")";
	    } else {
	      operands += " :: RegRef ";
	      // copy value in right register to left register
	      getRegister_val(codeR, posR, b_val,
			      w_val, dw_val,
			      qw_val);
	      assignRegister(code, pos, b_val,
			     w_val, dw_val,
			     qw_val);
	    }
	  } //mov instruction
	}
	
	// 2.2 RefExp:: Arithmetic Instruction
	else if (isSgAsmx86ArithmeticInstruction(binInst)) {
	  operands += " :: Arith";
	  if (isSgAsmx86Cmp(binInst)) {
	    operands += " :: Cmp";
	    uint64_t left = getRegister(code);	    
	    uint64_t right = getRegister(codeR);
	    if (left==right)
	      ZF=true;
	  }
	} // binaryArithmetic

	else {
	  success =false;
	}

      } //refExprR
      else { success =false;}
    }
    counter++;
  }
  operands += "\\n";
  return success;
}


std::string 
RoseBin_Emulate::printRegister(std::string text, uint64_t reg) {
  string ret = "\\n"+text + " : " +   RoseBin_support::HexToString(reg) ;
  return ret;
}


uint64_t 
RoseBin_Emulate::getRegister(SgAsmRegisterReferenceExpression::x86_register_enum code) {
  uint64_t reg=0;
 switch (code) {
  case SgAsmRegisterReferenceExpression::rAX: {
    reg = rax;
    break; 
  }
  case SgAsmRegisterReferenceExpression::rBX: {
    reg = rbx;
    break;
  }
  case SgAsmRegisterReferenceExpression::rCX: {
    reg = rcx;
    break;
  } 
  case SgAsmRegisterReferenceExpression::rDX: {
    reg = rdx;
    break;
  }
  case SgAsmRegisterReferenceExpression::rDI: {
    reg = rdi;
    break;
  }
  case SgAsmRegisterReferenceExpression::rSI: {
    reg = rsi;
    break;
  } 
  case SgAsmRegisterReferenceExpression::rSP: {
    reg = rsp;
    break;
  }
  case SgAsmRegisterReferenceExpression::rBP: {
    reg = rbp;
    break;
  } 
  default:
    break;
  }
 return reg;
}

void 
RoseBin_Emulate::assignMemory(uint64_t position, uint64_t value) {
  memory[position] = value;
}

uint64_t 
RoseBin_Emulate::getMemory(uint64_t position) {
  rose_hash::hash_map <uint64_t, uint64_t>::iterator it = memory.find(position);
  uint64_t value = 0xFFFFFFFF;
  if (it!=memory.end())
    value = it->second;
  return value;
}



/****************************************************
 * resolve expression
 ****************************************************/
void 
RoseBin_Emulate::assignRegister(SgAsmRegisterReferenceExpression::x86_register_enum code,
					 SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos,
					 uint8_t &b_val,
					 uint16_t &w_val,
					 uint32_t &dw_val,
					 uint64_t &qw_val) {
  string res="";
  switch (code) {
  case SgAsmRegisterReferenceExpression::rAX: {
    res="rAX:undefined";
    if (pos==SgAsmRegisterReferenceExpression::low_byte) {
      rax &=~0xFFULL;
      rax |= uint64_t(b_val);
    } else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte) {
	rax &=~0xFF00ULL;
	rax |= uint64_t(b_val)<<8;
      }else 
	if (pos==SgAsmRegisterReferenceExpression::word) {
	  rax &=~0xFFFFULL;
	  rax |= uint64_t(w_val);
	}else 
	  if (pos==SgAsmRegisterReferenceExpression::dword) {
	    rax &=~0xFFFFFFFFULL;
	    rax |= uint64_t(dw_val);
	  } else 
	    if (pos==SgAsmRegisterReferenceExpression::qword) {
	      rax = qw_val;
	    }
    break; 
  }
  case SgAsmRegisterReferenceExpression::rBX: {
    res="rBX:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::low_byte) {
      rbx &=~0xFFULL;
      rbx |= uint64_t(b_val);
    }else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte) {
	rbx &=~0xFF00ULL;
	rbx |= uint64_t(b_val)<<8;
      }else 
	if (pos==SgAsmRegisterReferenceExpression::word) {
	  rbx &=~0xFFFFULL;
	  rbx |= uint64_t(w_val);
	}else 
	  if (pos==SgAsmRegisterReferenceExpression::dword) {
	    rbx &=~0xFFFFFFFFULL;
	    rbx |= uint64_t(dw_val);
	  }else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      rbx = qw_val;
    break;
  }
  case SgAsmRegisterReferenceExpression::rCX: {
    res="rCX:undefined";
    if (pos==SgAsmRegisterReferenceExpression::low_byte) {
      rcx &=~0xFFULL;
      rcx |= uint64_t(b_val);
    }else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte) {
	rcx &=~0xFF00ULL;
	rcx |= uint64_t(b_val)<<8;
      }else 
	if (pos==SgAsmRegisterReferenceExpression::word) {
	  rcx &=~0xFFFFULL;
	  rcx |= uint64_t(w_val);
	}else 
	  if (pos==SgAsmRegisterReferenceExpression::dword) {
	    rcx &=~0xFFFFFFFFULL;
	    rcx |= uint64_t(dw_val);
	  }else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      rcx = qw_val;
    break;
  } 
  case SgAsmRegisterReferenceExpression::rDX: {
    res="rDX:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::low_byte) {
      rdx &=~0xFFULL;
      rdx |= uint64_t(b_val);
    }else 
      if (pos==SgAsmRegisterReferenceExpression::high_byte) {
	rdx &=~0xFF00ULL;
	rdx |= uint64_t(b_val)<<8;
      }else 
	if (pos==SgAsmRegisterReferenceExpression::word) {
	  rdx &=~0xFFFFULL;
	  rdx |= uint64_t(w_val);
	}else 
	  if (pos==SgAsmRegisterReferenceExpression::dword) {
	    rdx &=~0xFFFFFFFFULL;
	    rdx |= uint64_t(dw_val);
	  }else 
	    if (pos==SgAsmRegisterReferenceExpression::qword)
	      rdx = qw_val;
    break;
  }
  case SgAsmRegisterReferenceExpression::rDI: {
    res="rDI:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::word) {
      rdi &=~0xFFFFULL;
      rdi |= uint64_t(w_val);
    } else 
      if (pos==SgAsmRegisterReferenceExpression::dword) {
	rdi &=~0xFFFFFFFFULL;
	rdi |= uint64_t(dw_val);
      } else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  rdi = qw_val;
    break;
  }
  case SgAsmRegisterReferenceExpression::rSI: {
    res="rSI:undefined";
    if (pos==SgAsmRegisterReferenceExpression::word) {
      rsi &=~0xFFFFULL;
      rsi |= uint64_t(w_val);
    } else 
      if (pos==SgAsmRegisterReferenceExpression::dword) {
	rsi &=~0xFFFFFFFFULL;
	rsi |= uint64_t(dw_val);
      } else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  rsi = qw_val;
    break;
  } 
  case SgAsmRegisterReferenceExpression::rSP: {
    res="rSP:undefined"; 
    if (pos==SgAsmRegisterReferenceExpression::word) {
      rsp &=~0xFFFFULL;
      rsp |= uint64_t(w_val);
    } else 
      if (pos==SgAsmRegisterReferenceExpression::dword) {
	rsp &=~0xFFFFFFFFULL;
	rsp |= uint64_t(dw_val);
      } else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  rsp = qw_val;
    break;
  }
  case SgAsmRegisterReferenceExpression::rBP: {
    res="rBP:undefined";
    if (pos==SgAsmRegisterReferenceExpression::word) {
      rbp &=~0xFFFFULL;
      rbp |= uint64_t(w_val);
    } else 
      if (pos==SgAsmRegisterReferenceExpression::dword) {
	rbp &=~0xFFFFFFFFULL;
	rbp |= uint64_t(dw_val);
      } else 
	if (pos==SgAsmRegisterReferenceExpression::qword)
	  rbp = qw_val;
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
    break;
  }
  //return res;
}


void 
RoseBin_Emulate::assignRegister(SgAsmRegisterReferenceExpression::x86_register_enum code,
					 uint64_t &qw_val) {
  string res="";
  switch (code) {
  case SgAsmRegisterReferenceExpression::rAX: {
    res="rAX:undefined";
    rax = qw_val;
    break; 
  }
  case SgAsmRegisterReferenceExpression::rBX: {
    res="rBX:undefined"; 
    rbx = qw_val;
    break;
  }
  case SgAsmRegisterReferenceExpression::rCX: {
    res="rCX:undefined";
    rcx = qw_val;
    break;
  } 
  case SgAsmRegisterReferenceExpression::rDX: {
    res="rDX:undefined"; 
    rdx = qw_val;
    break;
  }
  case SgAsmRegisterReferenceExpression::rDI: {
    res="rDI:undefined"; 
    rdi = qw_val;
    break;
  }
  case SgAsmRegisterReferenceExpression::rSI: {
    res="rSI:undefined";
    rsi = qw_val;
    break;
  } 
  case SgAsmRegisterReferenceExpression::rSP: {
    res="rSP:undefined"; 
    rsp = qw_val;
    break;
  }
  case SgAsmRegisterReferenceExpression::rBP: {
    res="rBP:undefined";
    rbp = qw_val;
    break;
  } 
  default:
    break;
  }
  //return res;
}

void RoseBin_Emulate::clearRegisters() {
  uint8_t cv1 = 0xF;
  uint16_t cv2 = 0xFF;
  uint32_t cv3 = 0xFFFF;
  uint64_t cv4 = 0xFFFFFFFF;
  assignRegister(SgAsmRegisterReferenceExpression::rAX ,
		 SgAsmRegisterReferenceExpression::qword, cv1, cv2, cv3, cv4);
  assignRegister(SgAsmRegisterReferenceExpression::rBX ,
		 SgAsmRegisterReferenceExpression::qword, cv1, cv2, cv3, cv4);
  assignRegister(SgAsmRegisterReferenceExpression::rCX ,
		 SgAsmRegisterReferenceExpression::qword, cv1, cv2, cv3, cv4);
  assignRegister(SgAsmRegisterReferenceExpression::rDX ,
		 SgAsmRegisterReferenceExpression::qword, cv1, cv2, cv3, cv4);
}


std::string RoseBin_Emulate::evaluateRegisters() {
    string regs = "";
    string space = "      ";

      if (!isCode64bit) {
	regs += "eax: " + RoseBin_support::HexToString(uint32_t(rax)) + space;
	regs += "edi: " + RoseBin_support::HexToString(uint32_t(rdi)) + "\\n";

	regs += "ebx: " + RoseBin_support::HexToString(uint32_t(rbx)) + space;
	regs += "esi: " + RoseBin_support::HexToString(uint32_t(rsi)) + "\\n";

	regs += "ecx: " + RoseBin_support::HexToString(uint32_t(rcx)) + space;
	regs += "esp: " + RoseBin_support::HexToString(uint32_t(rsp)) + "\\n";

	regs += "edx: " + RoseBin_support::HexToString(uint32_t(rdx)) + space;
	regs += "ebp: " + RoseBin_support::HexToString(uint32_t(rbp)) + "\\n";

	regs += "ZF: " + RoseBin_support::resBool(ZF) + "\\n";
      } else {
	regs += "rax: " + RoseBin_support::HexToString(rax) + space;
	regs += "rdi: " + RoseBin_support::HexToString(rdi) + "\\n";

	regs += "rbx: " + RoseBin_support::HexToString(rbx) + space;
	regs += "rsi: " + RoseBin_support::HexToString(rsi) + "\\n";

	regs += "rcx: " + RoseBin_support::HexToString(rcx) + space;
	regs += "rsp: " + RoseBin_support::HexToString(rsp) + "\\n";

	regs += "rdx: " + RoseBin_support::HexToString(rdx) + space;
	regs += "rbp: " + RoseBin_support::HexToString(rbp) + "\\n";

	regs += "ZF: " + RoseBin_support::resBool(ZF) + "\\n";
      }
      //      regs += eval;
      return regs;
}

bool 
RoseBin_Emulate::run(string& name, SgDirectedGraphNode* node,
		     SgDirectedGraphNode* pervious) {

  // check known function calls and resolve variables
  ROSE_ASSERT(unparser);
  ROSE_ASSERT(node);
  SgAsmInstruction* inst = isSgAsmInstruction(node->get_SgNode());
  if (inst) {
    // make sure the dataflow info gets passed to the graph nodes ------------------
    //string unp_name = unparser->unparseInstruction(inst);
    if (RoseBin_support::DEBUG_MODE()) {
      string regs = evaluateRegisters();
      string unp_name = vizzGraph->getProperty(RoseBin_Def::name, node);
      cout << "EMULATE BEFORE::  name: " << unp_name << " \n regs: " << regs << endl;
    }
    string eval = "";
    bool success=evaluateInstruction(inst, eval);
    //node->append_properties(RoseBin_Def::name,unp_name);
    node->append_properties(RoseBin_Def::eval,eval);
    string regs = evaluateRegisters();
    if (RoseBin_support::DEBUG_MODE()) {
      string unp_name = vizzGraph->getProperty(RoseBin_Def::name, node);
      cout << "EMULATE AFTER ::  name: " << unp_name << " \n regs: " << regs << endl;
    }
    node->append_properties(RoseBin_Def::regs,regs);
    if (success)
      node->append_properties(RoseBin_Def::done,RoseBin_support::ToString("done"));
  // ----------------------------------------------------------------------------
  }
  return false;
}




