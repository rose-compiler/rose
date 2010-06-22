/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 7Sep07
 * Decription : Dataflow analysis
 ****************************************************/
// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"
#include "RoseBin_Emulate.h"
#include "AsmUnparser_compat.h"
using namespace std;





void
RoseBin_Emulate::getRegister_val(std::pair<X86RegisterClass, int>  code,
					 X86PositionInRegister pos,
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
RoseBin_Emulate::getRegister_val(std::pair<X86RegisterClass, int>  code,
					 X86PositionInRegister pos,
					 uint8_t &b_val,
					 uint16_t &w_val,
					 uint32_t &dw_val,
					 uint64_t &qw_val) {


  uint8_t b8,b7=0xF;
  uint16_t b78=0xFF;
  uint32_t b5678=0xFFFF;

  string res="";
  ROSE_ASSERT (!RoseBin_support::bigEndian());
  switch (code.first) {
    case x86_regclass_gpr: {
      switch (code.second) {
#define DO_ONE_GPR(reg) \
        case x86_gpr_##reg: { \
          /* assuming little endian */ \
          if (!RoseBin_support::bigEndian()) { \
            b7 = ( r##reg>>8 ) & 255; \
            b8 = ( r##reg ) & 255; \
            b78 = ( r##reg ) & 65535; \
            /* b5678 = ( rax ) & 4294967295; */ \
            b5678 = ( r##reg ) & 0xffffffff; \
          } else { \
            cerr << " We dont handle big endian's " << endl; \
            exit(0); \
          } \
          res="r" #reg ":undefined"; \
          if (pos==x86_regpos_low_byte) \
          b_val = b8; \
          else  \
          if (pos==x86_regpos_high_byte) \
          b_val = b7; \
          else  \
          if (pos==x86_regpos_word) \
          w_val = b78; \
          else  \
          if (pos==x86_regpos_dword) \
          dw_val = b5678; \
          else  \
          if (pos==x86_regpos_qword) \
          qw_val = r##reg; \
          break;  \
        }

        DO_ONE_GPR (ax);
        DO_ONE_GPR (bx);
        DO_ONE_GPR (cx);
        DO_ONE_GPR (dx);
        DO_ONE_GPR (si);
        DO_ONE_GPR (di);
        DO_ONE_GPR (bp);
        DO_ONE_GPR (sp);
#undef DO_ONE_GPR

        case 8: {
          res="r8";
          break;
        }
        case 9: {
          res="r9";
          break;
        }
        case 10: {
          res="r10";
          break;
        }
        case 11: {
          res="r11";
          break;
        }
        case 12: {
          res="r12";
          break;
        }
        case 13: {
          res="r13";
          break;
        }
        case 14: {
          res="r14";
          break;
        }
        case 15: {
          res="r15";
          break;
        }
        default: ROSE_ASSERT (false);
      }
      break;
    }

    case x86_regclass_segment: {
      switch (code.second) {
        case x86_segreg_cs: {
          res="cs";
          break;
        }
        case x86_segreg_ds: {
          res="ds";
          break;
        }
        case x86_segreg_ss: {
          res="ss";
          break;
        }
        case x86_segreg_es: {
          res="es";
          break;
        }
        case x86_segreg_fs: {
          res="fs";
          break;
        }
        case x86_segreg_gs: {
          res="gs";
          break;
        }
        default:
          ROSE_ASSERT (false);
      }
      break;
    }
    default: break;
  }
  //return res;
}


uint64_t RoseBin_Emulate::getRandomValue(int val) {
  int random_integer = rand()%val;
  return random_integer;//0x10ULL;
}




bool
RoseBin_Emulate::evaluateInstruction( SgAsmx86Instruction* binInst, string& operands) {
  SgAsmOperandList* opList = binInst->get_operandList();
  ROSE_ASSERT(opList);
  //string operands = "";
  bool success = true;

  // ***************************************************************
  // handle special cases
  // handle function calls to cpuid and int
  if (binInst->get_kind() == x86_cpuid) {
    operands += " :: specialOp cpuid";
    //clearRegisters();
    rbx = getRandomValue(100); // example return value
    rcx = getRandomValue(100); // example return value
    rdx = getRandomValue(100); // example return value
    return success;
  }
  else if (binInst->get_kind() == x86_int) {
    operands += " :: specialOp Int";
    //clearRegisters();
    getRegister_val(std::make_pair(x86_regclass_gpr, x86_gpr_ax), x86_regpos_qword, rax);
    // should get the values from memory!
    string values = "";
    rose_hash::unordered_map <uint64_t, uint64_t>::iterator it = memory.begin();
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
  else if (binInst->get_kind() == x86_rdtsc) {
    // simulate timer function
    operands += " :: specialOp rdtsc";
    rdx = 0x0ULL;
    rax = getRandomValue(10);
    return success;
  }
  // **************************************************************


  int counter=0;
  SgAsmx86RegisterReferenceExpression* refExpr =NULL;
  std::pair<X86RegisterClass, int>  code ;
  X86PositionInRegister pos ;

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
      refExpr =	isSgAsmx86RegisterReferenceExpression(expr);

      // check what it could be
      // ******** 1. its a RegisterReferenceExpression on the left side
      if (refExpr) {
	code = refExpr->get_identifier();
	pos = refExpr->get_position_in_register();
	operands = "\\nleft :: refExpr ";
	//SgAsmExpression* expression = refExpr->get_offset();
	//memRef = isSgAsmMemoryReferenceExpression(expression);
	//if (memRef)
	//  operands += " :: memoryRefExp ";
	operands += "\\n";
      }

      // ******** 2. Its a BitAndByteInstruction
      else  {
        switch (binInst->get_kind()) {
          case x86_sete: {
            // if the instruction is a sete, then we want to check ZF and set the
            // proper register to 1
            if (ZF) {
              uint64_t qw_val=1;
              assignRegister(code, qw_val);
              ZF=false;
            }
            break;
          }
          case x86_dec: {
            uint64_t left = getRegister(code);
            assignRegister(code, --left);
            break;
          }
          default: {
            success = false;
          }
        }
      }

    } else {
      // right hand side ************************************************************
      SgAsmValueExpression* valExp = isSgAsmValueExpression(expr);
      SgAsmx86RegisterReferenceExpression* refExprR = isSgAsmx86RegisterReferenceExpression(expr);
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
          switch (binInst->get_kind()) {
	    // mov instruction
            case x86_mov: {
	      operands += " :: DataTrans :: Mov ";
	      assignRegister(code, pos, b_val,
			     w_val, dw_val,
			     qw_val);
	      uint64_t addr_value=0;
	      getRegister_val(code, x86_regpos_qword, addr_value);
	      //string str="";
	      //string var = createVariable(addr_value, str, type, description, length);
	      //string varHex = RoseBin_support::HexToString(addr_value) ;
	      //operands += "\\nVariable : " + var + "("+varHex+")";
              break;
	    }

            case x86_and: {
	      operands += " :: Logical :: And";
	      uint64_t reg = getRegister(code);
	      operands+=printRegister("reg",reg);
	      operands+=printRegister("val",val);
	      reg &= val;
	      assignRegister(code, reg);
	      if (reg==0)
		ZF = true;
	      else ZF=false;
              break;
	    }

            case x86_add: {
	      operands += " :: Arithm :: Add";
	      uint64_t left = getRegister(code);
	      uint64_t sum = left+val;
	      assignRegister(code, sum);
              break;
	    }

            default:
              success = false;
	  }

	} // refExp
	else { success = false;}
      } // valExp
      else if (refExprR) {
        // ****** 2. referenceExpression
	// the right hand side is also a register or memory location
	std::pair<X86RegisterClass, int>  codeR ;
	X86PositionInRegister posR ;
	codeR = refExprR->get_identifier();
	posR = refExprR->get_position_in_register();

	operands += "right ::  refExpr ";

        switch (binInst->get_kind()) {
          case x86_mov: {
	    operands += " :: DataTrans :: Mov";
	    if (memRef) {
	      operands += " :: MemRef ";
	      SgAsmExpression* mem_loc = memRef->get_address();
	      string res = unparseExpression(mem_loc);
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
            break;
	  } //mov instruction

          case x86_cmp: {
	    operands += " :: Arith :: Cmp";
	    uint64_t left = getRegister(code);
	    uint64_t right = getRegister(codeR);
	    if (left==right)
	      ZF=true;
            break;
	  }

          default: {
            success =false;
          }
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
RoseBin_Emulate::getRegister(std::pair<X86RegisterClass, int>  code) {
  uint64_t reg=0;
  ROSE_ASSERT (code.first == x86_regclass_gpr);
 switch (code.second) {
  case x86_gpr_ax: {
    reg = rax;
    break;
  }
  case x86_gpr_bx: {
    reg = rbx;
    break;
  }
  case x86_gpr_cx: {
    reg = rcx;
    break;
  }
  case x86_gpr_dx: {
    reg = rdx;
    break;
  }
  case x86_gpr_di: {
    reg = rdi;
    break;
  }
  case x86_gpr_si: {
    reg = rsi;
    break;
  }
  case x86_gpr_sp: {
    reg = rsp;
    break;
  }
  case x86_gpr_bp: {
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
  rose_hash::unordered_map <uint64_t, uint64_t>::iterator it = memory.find(position);
  uint64_t value = 0xFFFFFFFF;
  if (it!=memory.end())
    value = it->second;
  return value;
}



/****************************************************
 * resolve expression
 ****************************************************/
void
RoseBin_Emulate::assignRegister(std::pair<X86RegisterClass, int>  code,
					 X86PositionInRegister pos,
					 uint8_t &b_val,
					 uint16_t &w_val,
					 uint32_t &dw_val,
					 uint64_t &qw_val) {
  switch (code.first) {
    case x86_regclass_gpr: {
      switch (code.second) {
        case x86_gpr_ax: {
          if (pos==x86_regpos_low_byte) {
            rax &=~0xFFULL;
            rax |= uint64_t(b_val);
          } else
            if (pos==x86_regpos_high_byte) {
              rax &=~0xFF00ULL;
              rax |= uint64_t(b_val)<<8;
            }else
              if (pos==x86_regpos_word) {
                rax &=~0xFFFFULL;
                rax |= uint64_t(w_val);
              }else
                if (pos==x86_regpos_dword) {
                  rax &=~0xFFFFFFFFULL;
                  rax |= uint64_t(dw_val);
                } else
                  if (pos==x86_regpos_qword) {
                    rax = qw_val;
                  }
                break;
        }
        case x86_gpr_bx: {
          if (pos==x86_regpos_low_byte) {
            rbx &=~0xFFULL;
            rbx |= uint64_t(b_val);
          }else
            if (pos==x86_regpos_high_byte) {
              rbx &=~0xFF00ULL;
              rbx |= uint64_t(b_val)<<8;
            }else
              if (pos==x86_regpos_word) {
                rbx &=~0xFFFFULL;
                rbx |= uint64_t(w_val);
              }else
                if (pos==x86_regpos_dword) {
                  rbx &=~0xFFFFFFFFULL;
                  rbx |= uint64_t(dw_val);
                }else
                  if (pos==x86_regpos_qword)
                    rbx = qw_val;
                break;
        }
        case x86_gpr_cx: {
          if (pos==x86_regpos_low_byte) {
            rcx &=~0xFFULL;
            rcx |= uint64_t(b_val);
          }else
            if (pos==x86_regpos_high_byte) {
              rcx &=~0xFF00ULL;
              rcx |= uint64_t(b_val)<<8;
            }else
              if (pos==x86_regpos_word) {
                rcx &=~0xFFFFULL;
                rcx |= uint64_t(w_val);
              }else
                if (pos==x86_regpos_dword) {
                  rcx &=~0xFFFFFFFFULL;
                  rcx |= uint64_t(dw_val);
                }else
                  if (pos==x86_regpos_qword)
                    rcx = qw_val;
                break;
        }
        case x86_gpr_dx: {
          if (pos==x86_regpos_low_byte) {
            rdx &=~0xFFULL;
            rdx |= uint64_t(b_val);
          }else
            if (pos==x86_regpos_high_byte) {
              rdx &=~0xFF00ULL;
              rdx |= uint64_t(b_val)<<8;
            }else
              if (pos==x86_regpos_word) {
                rdx &=~0xFFFFULL;
                rdx |= uint64_t(w_val);
              }else
                if (pos==x86_regpos_dword) {
                  rdx &=~0xFFFFFFFFULL;
                  rdx |= uint64_t(dw_val);
                }else
                  if (pos==x86_regpos_qword)
                    rdx = qw_val;
                break;
        }
        case x86_gpr_di: {
          if (pos==x86_regpos_word) {
            rdi &=~0xFFFFULL;
            rdi |= uint64_t(w_val);
          } else
            if (pos==x86_regpos_dword) {
              rdi &=~0xFFFFFFFFULL;
              rdi |= uint64_t(dw_val);
            } else
              if (pos==x86_regpos_qword)
                rdi = qw_val;
            break;
        }
        case x86_gpr_si: {
          if (pos==x86_regpos_word) {
            rsi &=~0xFFFFULL;
            rsi |= uint64_t(w_val);
          } else
            if (pos==x86_regpos_dword) {
              rsi &=~0xFFFFFFFFULL;
              rsi |= uint64_t(dw_val);
            } else
              if (pos==x86_regpos_qword)
                rsi = qw_val;
            break;
        }
        case x86_gpr_sp: {
          if (pos==x86_regpos_word) {
            rsp &=~0xFFFFULL;
            rsp |= uint64_t(w_val);
          } else
            if (pos==x86_regpos_dword) {
              rsp &=~0xFFFFFFFFULL;
              rsp |= uint64_t(dw_val);
            } else
              if (pos==x86_regpos_qword)
                rsp = qw_val;
            break;
        }
        case x86_gpr_bp: {
          if (pos==x86_regpos_word) {
            rbp &=~0xFFFFULL;
            rbp |= uint64_t(w_val);
          } else
            if (pos==x86_regpos_dword) {
              rbp &=~0xFFFFFFFFULL;
              rbp |= uint64_t(dw_val);
            } else
              if (pos==x86_regpos_qword)
                rbp = qw_val;
            break;
        }
        case 8: {
          break;
        }
        case 9: {
          break;
        }
        case 10: {
          break;
        }
        case 11: {
          break;
        }
        case 12: {
          break;
        }
        case 13: {
          break;
        }
        case 14: {
          break;
        }
        case 15: {
          break;
        }
        default: ROSE_ASSERT (false);
      }
      break;
    }
    case x86_regclass_segment: break;
    default: break;
  }
}


void
RoseBin_Emulate::assignRegister(std::pair<X86RegisterClass, int>  code,
					 uint64_t &qw_val) {
  if (code.first != x86_regclass_gpr) return;
  switch (code.second) {
    case x86_gpr_ax: {
      rax = qw_val;
      break;
    }
    case x86_gpr_bx: {
      rbx = qw_val;
      break;
    }
    case x86_gpr_cx: {
      rcx = qw_val;
      break;
    }
    case x86_gpr_dx: {
      rdx = qw_val;
      break;
    }
    case x86_gpr_di: {
      rdi = qw_val;
      break;
    }
    case x86_gpr_si: {
      rsi = qw_val;
      break;
    }
    case x86_gpr_sp: {
      rsp = qw_val;
      break;
    }
    case x86_gpr_bp: {
      rbp = qw_val;
      break;
    }
    default:
      break;
  }
}

void RoseBin_Emulate::clearRegisters() {
  uint8_t cv1 = 0xF;
  uint16_t cv2 = 0xFF;
  uint32_t cv3 = 0xFFFF;
  uint64_t cv4 = 0xFFFFFFFF;
  assignRegister(std::make_pair(x86_regclass_gpr, x86_gpr_ax) ,
		 x86_regpos_qword, cv1, cv2, cv3, cv4);
  assignRegister(std::make_pair(x86_regclass_gpr, x86_gpr_bx) ,
		 x86_regpos_qword, cv1, cv2, cv3, cv4);
  assignRegister(std::make_pair(x86_regclass_gpr, x86_gpr_cx) ,
		 x86_regpos_qword, cv1, cv2, cv3, cv4);
  assignRegister(std::make_pair(x86_regclass_gpr, x86_gpr_dx) ,
		 x86_regpos_qword, cv1, cv2, cv3, cv4);
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
RoseBin_Emulate::run(string& name, SgGraphNode* node,
		     SgGraphNode* pervious) {

  // check known function calls and resolve variables
  ROSE_ASSERT(node);
  SgAsmInstruction* inst = isSgAsmInstruction(node->get_SgNode());
  if (inst) {
    // make sure the dataflow info gets passed to the graph nodes ------------------
    //string unp_name = unparser->unparseInstruction(inst);
    if (RoseBin_support::DEBUG_MODE()) {
      string regs = evaluateRegisters();
      string unp_name = vizzGraph->getProperty(SgGraph::name, node);
      cout << "EMULATE BEFORE::  name: " << unp_name << " \n regs: " << regs << endl;
    }
    string eval = "";
    bool success=evaluateInstruction(isSgAsmx86Instruction(inst), eval);
    //node->append_properties(SgGraph::name,unp_name);
    node->append_properties(SgGraph::eval,eval);
    string regs = evaluateRegisters();
    if (RoseBin_support::DEBUG_MODE()) {
      string unp_name = vizzGraph->getProperty(SgGraph::name, node);
      cout << "EMULATE AFTER ::  name: " << unp_name << " \n regs: " << regs << endl;
    }
    node->append_properties(SgGraph::regs,regs);
    if (success)
      node->append_properties(SgGraph::done,RoseBin_support::ToString("done"));
  // ----------------------------------------------------------------------------
  }
  return false;
}




