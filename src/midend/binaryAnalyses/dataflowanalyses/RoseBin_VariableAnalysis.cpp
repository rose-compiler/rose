/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 7Sep07
 * Decription : Dataflow analysis
 ****************************************************/
// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"
#include "RoseBin_VariableAnalysis.h"
#include "AsmUnparser_compat.h"
using namespace std;
using namespace RoseBin_DataTypes;
using namespace RoseBin_OS_VER;
using namespace RoseBin_Arch;


void
RoseBin_VariableAnalysis::getValueForDefinition(std::vector<uint64_t>& vec,
						std::vector<uint64_t>& positions,
						uint64_t& fpos,
						SgGraphNode* node,
						std::pair<X86RegisterClass, int>  reg ) {
  set <SgGraphNode*> defNodeSet = getDefFor(node, reg);
  if (RoseBin_support::DEBUG_MODE())
    cout << "    size of found NodeSet = " << defNodeSet.size() <<endl;
  set <SgGraphNode*>::const_iterator it = defNodeSet.begin();
  for (;it!=defNodeSet.end();++it) {
    SgGraphNode* defNode = *it;
    if (RoseBin_support::DEBUG_MODE() && defNode)
      cout << "    investigating ... " << defNode->get_name() <<endl;
    ROSE_ASSERT(defNode);
    SgAsmx86Instruction* inst = isSgAsmx86Instruction(defNode->get_SgNode());
    ROSE_ASSERT(inst);
    positions.push_back(inst->get_address());
    // the right hand side of the instruction is either a use or a value
    bool memRef = false;
    std::pair<X86RegisterClass, int>  regRight =
      check_isRegister(defNode, inst, true, memRef);

    if (RoseBin_support::DEBUG_MODE()) {
      string regName = unparseX86Register(RegisterDescriptor(reg.first, reg.second, 0, 64));
      string regNameRight = unparseX86Register(RegisterDescriptor(regRight.first, regRight.second, 0, 64));
      cout << " VarAnalysis: getValueForDef . " << regName << "  right hand : " << regNameRight <<endl;
    }
    if (regRight.first == x86_regclass_unknown) {
      // it is either a memref or a value
      if (!memRef) {
	// get value of right hand side instruction
	uint64_t val = getValueOfInstr(inst, true);
	vec.push_back(val);
	fpos = inst->get_address();
	if (RoseBin_support::DEBUG_MODE())
      	  cout << "    found  valueOfInst = " << RoseBin_support::ToString(val) <<endl;
      }
    } else {
      // it is a register reference. I.e we need to follow the usage edge to find the
      // definition of that node
      SgGraphNode* usageNode = g_algo->getDefinitionForUsage(vizzGraph,defNode);
      if (usageNode && usageNode!=node) {
	if (RoseBin_support::DEBUG_MODE() && usageNode)
      	  cout << "    following up usage for " << usageNode->get_name() <<endl;
	getValueForDefinition(vec, positions, fpos, usageNode, regRight);
      } else {
	// we look at the same node.
	cout << " ERROR :: Either following usage to itself or usageNode = NULL. " << usageNode << endl;
      }
    }
  }
}

std::string
RoseBin_VariableAnalysis::getIntCallName(uint64_t rax,
                                         DataTypes& data_ebx,
					 DataTypes& data_ecx,
					 DataTypes& data_edx,
					 vector<uint64_t>& val_rbx,
					 vector<uint64_t>& val_rcx,
					 vector<uint64_t>& val_rdx,
					 std::vector<uint64_t>& pos_rbx,
					 std::vector<uint64_t>& pos_rcx,
					 std::vector<uint64_t>& pos_rdx,
					 uint64_t fpos_rbx, uint64_t fpos_rcx, uint64_t fpos_rdx ) {
  string int_name = "unknown";
  data_ebx = d_none;
  data_ecx = d_none;
  data_edx = d_none;

  if ((os_ver==linux_22 ||
      os_ver==linux_24 ||
      os_ver==linux_26 ||
      os_ver==linux_27) && arch==bit32)
    int_name = getIntCallName_Linux32bit(rax, data_ebx, data_ecx, data_edx,
                                         val_rbx, val_rcx, val_rdx,
					 pos_rbx, pos_rcx, pos_rdx,
					 fpos_rbx, fpos_rcx, fpos_rdx);
  if ((os_ver==linux_22 ||
      os_ver==linux_24 ||
      os_ver==linux_26 ||
      os_ver==linux_27) && arch==bit64)
    int_name = getIntCallName_Linux64bit(rax, data_ebx, data_ecx, data_edx,
                                         val_rbx, val_rcx, val_rdx,
					 pos_rbx, pos_rcx, pos_rdx,
					 fpos_rbx, fpos_rcx, fpos_rdx);

  return int_name;
}

std::string
RoseBin_VariableAnalysis::getIntCallName_Linux32bit(uint64_t rax,
						    DataTypes& data_ebx,
						    DataTypes& data_ecx,
						    DataTypes& data_edx,
						    vector<uint64_t>& val_rbx,
						    vector<uint64_t>& val_rcx,
						    vector<uint64_t>& val_rdx,
						    std::vector<uint64_t>& pos_rbx,
						    std::vector<uint64_t>& pos_rcx,
						    std::vector<uint64_t>& pos_rdx,
						    uint64_t fpos_rbx, uint64_t fpos_rcx, uint64_t fpos_rdx) {
  string int_name = "unknown";
  // linux system calls for kernel 2.2 - 32bit
	switch (rax) {
	case 1: {
	  int_name="sys_exit";
	  data_ebx = d_int;
	  createVariable(fpos_rbx, pos_rbx, "error_code", data_ebx, int_name, 0, val_rbx,false);
	  break;
	}
	case 2: {
	  int_name="sys_fork";
	  data_ebx = d_struct;
	  createVariable(fpos_rbx, pos_rbx, "fork", data_ebx, int_name, 0, val_rbx,false);
	  break;
	}
	case 3: {
	  int_name="sys_read";
	  data_ebx = d_uint;
	  data_ecx = d_char_p;
	  data_edx = d_size_t;
	  createVariable(fpos_rbx, pos_rbx, "fd", data_ebx, int_name, 0, val_rbx,false);
	  createVariable(fpos_rcx, pos_rcx, "buf", data_ecx, int_name, *(val_rdx.begin()), val_rcx,true);
	  createVariable(fpos_rdx, pos_rdx, "count", data_edx, int_name, 0, val_rdx,false);
	  break;
	}
	case 4: {
	  int_name="sys_write";
	  data_ebx = d_uint;
	  data_ecx = d_const_char_p;
	  data_edx = d_size_t;
	  createVariable(fpos_rbx, pos_rbx, "fd", data_ebx, int_name, 0, val_rbx,false);
	  createVariable(fpos_rcx, pos_rcx, "buf", data_ecx, int_name, *(val_rdx.begin()), val_rcx,true);
	  createVariable(fpos_rdx, pos_rdx, "count", data_edx, int_name, 0, val_rdx,false);
	  break;
	}
	case 5: {
	  int_name="sys_open";
	  data_ebx = d_const_char_p;
	  data_ecx = d_int;
	  data_edx = d_int;
	  break;
	}
	case 6: {
	  int_name="sys_close";
	  data_ebx = d_uint;
	  break;
	}
	case 7: {
	  int_name="sys_waitpid";
	  data_ebx = d_pid_t;
	  data_ecx = d_uint_p;
	  data_edx = d_int;
	  break;
	}
	case 8: {
	  int_name="sys_creat";
	  data_ebx = d_const_char_p;
	  data_ecx = d_int;
	  break;
	}
	case 9: {
	  int_name="sys_link";
	  data_ebx = d_const_char_p;
	  data_ecx = d_const_char_p;
	  break;
	}
	case 10: {
	  int_name="sys_unlink";
	  data_ebx = d_const_char_p;
	  break;
	}
	case 11: {
	  int_name="sys_execve";
	  data_ebx = d_struct;
	  break;
	}
	case 12: {
	  int_name="sys_chdir";
	  data_ebx = d_const_char_p;
	  break;
	}
	case 13: {
	  int_name="sys_time";
	  data_ebx = d_int;
	  break;
	}
	case 14: {
	  int_name="sys_mknod"; break;
	}
	case 15: {
	  int_name="sys_chmod"; break;
	}
	case 16: {
	  int_name="sys_lchown"; break;
	}
	case 18: {
	  int_name="sys_stat"; break;
	}
	case 19: {
	  int_name="sys_lseek"; break;
	}
	case 20: {
	  int_name="sys_getpid"; break;
	}
	case 21: {
	  int_name="sys_mount"; break;
	}
	case 22: {
	  int_name="sys_oldumount"; break;
	}
	case 23: {
	  int_name="sys_setuid"; break;
	}
	case 24: {
	  int_name="sys_getuid"; break;
	}
	case 25: {
	  int_name="sys_stime"; break;
	}
	case 26: {
	  int_name="sys_ptrace"; break;
	}
	case 27: {
	  int_name="sys_alarm"; break;
	}
	case 28: {
	  int_name="sys_fstat"; break;
	}
	case 29: {
	  int_name="sys_pause"; break;
	}
	case 30: {
	  int_name="sys_utime"; break;
	}
	case 33: {
	  int_name="sys_access"; break;
	}
	case 34: {
	  int_name="sys_nice"; break;
	}
	case 36: {
	  int_name="sys_sync"; break;
	}
	case 37: {
	  int_name="sys_kill"; break;
	}
	case 38: {
	  int_name="sys_rename"; break;
	}
	case 39: {
	  int_name="sys_mkdir"; break;
	}
	case 40: {
	  int_name="sys_rmdir"; break;
	}
	case 41: {
	  int_name="sys_dup"; break;
	}
	case 42: {
	  int_name="sys_pipe"; break;
	}
	case 43: {
	  int_name="sys_times"; break;
	}
	case 45: {
	  int_name="sys_brk"; break;
	}
	case 46: {
	  int_name="sys_setgid"; break;
	}
	case 47: {
	  int_name="sys_getgid"; break;
	}
	case 48: {
	  int_name="sys_signal"; break;
	}
	case 49: {
	  int_name="sys_geteuid"; break;
	}
	case 50: {
	  int_name="sys_getegid"; break;
	}
	case 51: {
	  int_name="sys_acct"; break;
	}
	case 52: {
	  int_name="sys_umount"; break;
	}
	case 54: {
	  int_name="sys_ioctl"; break;
	}
	case 55: {
	  int_name="sys_fcntl"; break;
	}
	case 57: {
	  int_name="sys_setpgid"; break;
	}
	case 59: {
	  int_name="sys_olduname"; break;
	}
	case 60: {
	  int_name="sys_umask"; break;
	}
	case 61: {
	  int_name="sys_chroot"; break;
	}
	case 62: {
	  int_name="sys_ustat"; break;
	}
	case 63: {
	  int_name="sys_dup2"; break;
	}
	case 64: {
	  int_name="sys_getppid"; break;
	}
	case 65: {
	  int_name="sys_getpgrp"; break;
	}
	case 66: {
	  int_name="sys_setsid"; break;
	}
	case 67: {
	  int_name="sys_sigaction"; break;
	}
	case 68: {
	  int_name="sys_sgetmask"; break;
	}
	case 69: {
	  int_name="sys_ssetmask"; break;
	}
	case 70: {
	  int_name="sys_setreuid"; break;
	}
	case 71: {
	  int_name="sys_setregid"; break;
	}
	case 72: {
	  int_name="sys_sigsuspend"; break;
	}
	default:
	  break;
	}
	return int_name;
}

std::string
RoseBin_VariableAnalysis::getIntCallName_Linux64bit(uint64_t rax,
						    DataTypes& data_ebx,
						    DataTypes& data_ecx,
						    DataTypes& data_edx,
						    vector<uint64_t>& val_rbx,
						    vector<uint64_t>& val_rcx,
						    vector<uint64_t>& val_rdx,
						    std::vector<uint64_t>& pos_rbx,
						    std::vector<uint64_t>& pos_rcx,
						    std::vector<uint64_t>& pos_rdx,
						    uint64_t fpos_rbx, uint64_t fpos_rcx, uint64_t fpos_rdx) {
  string int_name = "unknown";
  // linux system calls for kernel 2.6 - 64bit
  // check in linux src : uinstd.h and syscalls.h
	switch (rax) {
	case 0: {
	  int_name="sys_read";
	  data_ebx = d_uint;
	  data_ecx = d_char_p;
	  data_edx = d_size_t;
	  break;
	}
	case 1: {
	  int_name="sys_write";
	  data_ebx = d_uint;
	  data_ecx = d_const_char_p;
	  data_edx = d_size_t;
	  break;
	}
	case 2: {
	  int_name="sys_open";
	  data_ebx = d_const_char_p;
	  data_ecx = d_int;
	  data_edx = d_int;
	  break;
	}
	case 3: {
	  int_name="sys_close";
	  data_ebx = d_uint;
	  break;
	}
	case 4: {
	  int_name="sys_newstat"; break;
	  data_ebx = d_const_char_p;
	  data_ecx = d_struct;
	}
	case 5: {
	  int_name="sys_newfstat"; break;
	  data_ebx = d_uint;
	  data_ecx = d_struct;
	}
	case 6: {
	  int_name="sys_newlstat"; break;
	  data_ebx = d_const_char_p;
	  data_ecx = d_struct;
	}
	case 7: {
	  int_name="sys_poll"; break;
	  data_ebx = d_struct;
	  data_ecx = d_uint;
	  data_edx = d_long;
	}
	default:
	  break;
	}
	return int_name;
}

bool
RoseBin_VariableAnalysis::run(string& name, SgGraphNode* node,
			      SgGraphNode* previous){

  // check known function calls and resolve variables
  ROSE_ASSERT(node);
  vector<uint64_t> val_rax, val_rbx, val_rcx, val_rdx ;
  std::vector<uint64_t> pos_rax, pos_rbx, pos_rcx, pos_rdx;
  uint64_t fpos_rax, fpos_rbx, fpos_rcx, fpos_rdx=0xffffffff;

  SgAsmx86Instruction* asmNode = isSgAsmx86Instruction(node->get_SgNode());
  if (asmNode) {
    // ANALYSIS 1 : INTERRUPT DETECTION -------------------------------------------

    // verify all interrupts and make sure they do what one expects them to do.
    if (asmNode->get_kind() == x86_int) {
      if (RoseBin_support::DEBUG_MODE())
	cout << "    " << name << " : found int call " << endl;
      // need to resolve rax, rbx, rcx, rdx
      // therefore get the definition for each
      getValueForDefinition(val_rax, pos_rax, fpos_rax, node,std::make_pair(x86_regclass_gpr, x86_gpr_ax));
      getValueForDefinition(val_rbx, pos_rbx, fpos_rbx, node,std::make_pair(x86_regclass_gpr, x86_gpr_bx));
      getValueForDefinition(val_rcx, pos_rcx, fpos_rcx, node,std::make_pair(x86_regclass_gpr, x86_gpr_cx));
      getValueForDefinition(val_rdx, pos_rdx, fpos_rdx, node,std::make_pair(x86_regclass_gpr, x86_gpr_dx));

      string int_name = "unknown ";

      DataTypes data_ebx = unknown;
      DataTypes data_ecx = unknown;
      DataTypes data_edx = unknown;

      bool ambigious_inst=false;
      if (val_rax.size()>1)
	ambigious_inst = true;
      else
      if (val_rax.size()==1) {
	uint64_t rax = *(val_rax.begin());
	int_name = getIntCallName(rax, data_ebx, data_ecx, data_edx,
				  val_rbx, val_rcx, val_rdx,
				  pos_rbx, pos_rcx, pos_rdx,
				  fpos_rbx, fpos_rcx, fpos_rdx);
	ambigious_inst = false;
      }

      if (ambigious_inst) {
	string value = "";
	vector<uint64_t>::iterator it = val_rax.begin();
	for (;it!=val_rax.end();++it) {
	  string i_name = getIntCallName(*it, data_ebx, data_ecx, data_edx,
                                         val_rbx, val_rcx, val_rdx,
					 pos_rbx, pos_rcx, pos_rdx,
					 fpos_rbx, fpos_rcx, fpos_rdx);
          value +="rAX:"+RoseBin_support::HexToString(*it)+" "+i_name+" ";
	  //	  createVariable(fpos_rax, pos_rax, "rax", data_ebx, "rax", 0, val_rax,false);
	}

	cerr << " DataFlow::VariableAnalysis . Ambigious INT call: " <<
	  vizzGraph->getProperty(SgGraph::name, node) << " - " << value << endl;
	value = "PROBLEM: " + value;
	node->append_properties(SgGraph::dfa_unresolved_func,value);

      } else {
	// we know what INT instruction it is
	string t_ebx = RoseBin_support::getTypeName(data_ebx);
	string t_ecx = RoseBin_support::getTypeName(data_ecx);
	string t_edx = RoseBin_support::getTypeName(data_edx);

	int_name += " ("+t_ebx+","+t_ecx+","+t_edx+")";
	//if (RoseBin_support::DEBUG_MODE())
	// cout << " found INT call : " << value << " .. " << int_name << endl;
	node->append_properties(SgGraph::dfa_variable,int_name);
      }
    } else

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
		bool memRef = false;
		std::pair<X86RegisterClass, int>  code;
		code = check_isRegister(pre, asmPre, false, memRef);
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
		bool memRef = false;
		std::pair<X86RegisterClass, int>  code;
		code = check_isRegister(aft, asmAft, true, memRef);
		if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
		  if (RoseBin_support::DEBUG_MODE() && asmAft->get_kind() == x86_mov)
		    cerr << "    found mov of eax of malloc call : " << unparseInstruction(asmAft) <<endl;
		  SgAsmMemoryReferenceExpression* memExpr =
		    isSgAsmMemoryReferenceExpression(getOperand(asmAft,false));
		  if (memExpr) {
		    //SgAsmx86RegisterReferenceExpression* refLeft = getRegister(memref->get_segment(),false);

		    //SgAsmMemoryReferenceExpression* memExpr =
		    //  isSgAsmMemoryReferenceExpression(refLeft->get_offset());
		    //if (memExpr)
		      address_of_var = getValueInMemoryRefExp( memExpr->get_address());
		    if (RoseBin_support::DEBUG_MODE())
		    cerr << " The address of the malloc variable is : " << RoseBin_support::HexToString(address_of_var) << endl;
		    string functionName = "func";
		    //SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(asmAft->get_parent());
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
		    //aft->append_properties(SgGraph::dfa_variable,varStr);
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
    if ( true /* isSgAsmx86DataTransferInstruction(asmNode) */ ) {
      // USAGE OF BUFFER OVERFLOW
	if (asmNode->get_kind() == x86_mov) {
	bool memRef = false;
	uint64_t address_of_var=0;
	std::pair<X86RegisterClass, int>  code;
	// check if eax register on the left hand side
	code = check_isRegister(node, asmNode, false, memRef);
	if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
	  // right hand side is Register Reg / MemoryRef
	  //	  SgAsmx86RegisterReferenceExpression* refRight = getRegister(asmNode,true);
	  //if (refRight) {
	    SgAsmMemoryReferenceExpression* memExpr =
	      isSgAsmMemoryReferenceExpression(getOperand(asmNode,true));
	    if (memExpr) {
	      address_of_var = getValueInMemoryRefExp( memExpr->get_address());
	    if (RoseBin_support::DEBUG_MODE() && asmNode->get_kind() == x86_mov) {
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
		    bool memRef = false;
		    std::pair<X86RegisterClass, int>  code;
		    code = check_isRegister(aft, asmAft, false, memRef);
		    if (code.first == x86_regclass_gpr && code.second == x86_gpr_ax) {
		      uint64_t val = getValueOfInstr(asmAft, true);
		      arrayLength += val;
		    }
		  }
		  if (asmAft->get_kind() == x86_mov) {
		    foundMov = true;
		    bool memRef = false;
		    std::pair<X86RegisterClass, int>  code;
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


