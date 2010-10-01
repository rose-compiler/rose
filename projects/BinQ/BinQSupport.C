#include "rose.h"
#include <boost/algorithm/string.hpp>
#include "BinQGui.h"
#include <ext/hash_map>

#include <sstream>
#include <QFileDialog>
#include <unistd.h>

#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "icons.h"

//#include "disks.xpm"
#include "folder.xpm"
#include "BinQSupport.h"

#define EMACS

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;




// ----------------------------------------------------------------------------------------------
enum ExpressionCategory {ec_reg = 0, ec_mem = 1, ec_val = 2};
static const size_t numberOfInstructionKinds = x86_last_instruction;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->get_kind();}

inline ExpressionCategory getCategory(SgAsmExpression* e) {
  if (isSgAsmValueExpression(e)) {
    return ec_val;
  } else if (isSgAsmRegisterReferenceExpression(e)) {
    return ec_reg;
  } else if (isSgAsmMemoryReferenceExpression(e)) {
    return ec_mem;
  } else {
    abort();
  }
}

SgAsmExpressionPtrList& getOperands(SgAsmInstruction* insn) {
  SgAsmOperandList* ol = insn->get_operandList();
  SgAsmExpressionPtrList& operands = ol->get_operands();
  return operands;
}
static map<string, void*> internTable;

inline void* intern(const std::string& s) {
  map<string, void*>::const_iterator i = internTable.find(s);
  if (i == internTable.end()) {
    void* sCopy = new string(s);
    internTable.insert(std::make_pair(s, sCopy));
    return sCopy;
  } else {
    return i->second;
  }
}
static map<SgAsmExpression*, void*> unparseAndInternTable;

inline void* unparseAndIntern(SgAsmExpression* e) {
  map<SgAsmExpression*, void*>::const_iterator i = unparseAndInternTable.find(e);
  if (i == unparseAndInternTable.end()) {
    void* sPtr = intern(unparseExpression(e));
    unparseAndInternTable.insert(std::make_pair(e, sPtr));
    return sPtr;
  } else {
    return i->second;
  }
}
 
void numberOperands(std::vector<SgAsmx86Instruction*>::iterator beg,
		    std::vector<SgAsmx86Instruction*>::iterator end, map<SgAsmExpression*, size_t> numbers[3]) {
  map<void*, size_t> stringNumbers[3];
  for (; beg != end; ++beg) {
    SgAsmx86Instruction* insn = *beg;
    const SgAsmExpressionPtrList& operands = getOperands(insn);
    //size_t operandCount = operands.size();
    for (size_t j = 0; j < operands.size(); ++j) {
      SgAsmExpression* e = operands[j];
      ExpressionCategory cat = getCategory(e);
      void* str = unparseAndIntern(e);
      map<void*, size_t>& currentStringNums = stringNumbers[(int)cat];
      map<void*, size_t>::const_iterator stringNumIter = currentStringNums.find(str);
      size_t num = (stringNumIter == currentStringNums.end() ? currentStringNums.size() : stringNumIter->second);
      if (stringNumIter == currentStringNums.end()) currentStringNums.insert(std::make_pair(str, num));
      numbers[(int)cat][e] = num;
    }
  }
}
// ----------------------------------------------------------------------------------------------

std::string BinQSupport::ToUpper(std::string myString)
{
  const int length = myString.length();
  for(int i=0; i!=length ; ++i)
    {
      myString[i] = std::toupper(myString[i]);
    }
  return myString;
}


SgNode* BinQSupport::disassembleFile(std::string tsv_directory, std::string& sourceFile){
  SgNode* globalBlock=NULL;
  size_t found = tsv_directory.rfind(".");
  string ending="";

  if (found!=string::npos) {
    ending =tsv_directory.substr(found+1,tsv_directory.length());
  }
  //std::cout << "\nDisassembling: " << tsv_directory << " Ending : " << ending << std::endl;
    
  if(is_directory( tsv_directory  ) == true ){
    //std::cout << "\nsql: " << tsv_directory << std::endl;
    RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
    RoseBin_Arch::arch=RoseBin_Arch::bit32;
    RoseBin_OS::os_sys=RoseBin_OS::linux_op;
    RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;
    RoseFile* roseBin = new RoseFile( (char*)tsv_directory.c_str() );
    //cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
    // query the DB to retrieve all data
    globalBlock = roseBin->retrieve_DB();
    // traverse the AST and test it
    roseBin->test();
    sourceFile="false";
  } else if (ToUpper(ending)=="C" || ToUpper(ending)=="CPP" || ToUpper(ending)=="CXX") {
    //cerr << "Found C code ... " << endl;
    vector<char*> args;
    args.push_back(strdup(""));
    args.push_back(strdup(tsv_directory.c_str()));
    args.push_back(0);
    globalBlock =  frontend(args.size()-1,&args[0]);
    sourceFile="true";
  }  else{
    vector<char*> args;
    args.push_back(strdup(""));
    args.push_back(strdup(tsv_directory.c_str()));
    args.push_back(0);
    
    ostringstream outStr; 
    for(vector<char*>::iterator iItr = args.begin(); iItr != args.end();
	++iItr )    {
      outStr << *iItr << " ";
    }     
    ;
    //std::cout << "Calling " << outStr.str() << std::endl;
    globalBlock =  frontend(args.size()-1,&args[0]);
    sourceFile="false";
  }
  return globalBlock;
};





std::string
BinQSupport::resolveValue(SgAsmValueExpression* leftVal) {
  string valStr="";
  uint8_t byte_val=0xF;
  uint16_t word_val=0xFF;
  uint32_t double_word_val=0xFFFF;
  uint64_t quad_word_val=0xFFFFFFFFU;
  valStr = 
    RoseBin_support::resolveValue(leftVal, true,
				  byte_val,
				  word_val,
				  double_word_val,
				  quad_word_val);
  return valStr;
}


rose_addr_t 
BinQSupport::evaluateMemoryExpression(SgAsmx86Instruction* destInst,
				      SgAsmMemoryReferenceExpression* mem) {
  rose_addr_t resolveAddr=0;
  SgAsmExpression* exprOffset = mem->get_address();
  SgAsmExpression* left =NULL;
  SgAsmExpression* right =NULL;
  SgAsmBinaryAdd* add = isSgAsmBinaryAdd(exprOffset);
  SgAsmValueExpression* Val = isSgAsmValueExpression(exprOffset);
  if (add) {
    left = add->get_lhs();
    right = add->get_rhs();
  } else if (Val) {
    left=Val;
  }
  if (left || right) {
    SgAsmx86RegisterReferenceExpression* leftReg = isSgAsmx86RegisterReferenceExpression(left);
    SgAsmx86RegisterReferenceExpression* rightReg = isSgAsmx86RegisterReferenceExpression(right);
    SgAsmValueExpression* leftVal = isSgAsmValueExpression(left);
    SgAsmValueExpression* rightVal = isSgAsmValueExpression(right);
    X86RegisterClass regClass;
    if (leftReg) 
      regClass = (X86RegisterClass)leftReg->get_descriptor().get_major();
    if (rightReg) 
      regClass = (X86RegisterClass)rightReg->get_descriptor().get_major();
    //cerr << " print : " << regClass << endl;
    string val = "NULL";
    if (regClass>=0 && regClass <=10)
      val = regclassToString(regClass);
    rose_addr_t next_addr = destInst->get_address() + destInst->get_raw_bytes().size();
    if (val=="ip") {
      resolveAddr+=next_addr;
      //cerr << "Found IP: " << RoseBin_support::HexToString(next_addr) << 
      //	"  resolvAddr: "<<RoseBin_support::HexToString(resolveAddr) << endl;
    }
    if (leftVal) {
      string valStr = resolveValue(leftVal);
      uint64_t val=0;
      if(RoseBin_support::from_string<uint64_t>(val, valStr, std::hex))
	resolveAddr += val;
      //cerr << "Found leftVal: " <<   " ("<<valStr<<")"<<
      //	"  resolvAddr: "<<RoseBin_support::HexToString(resolveAddr) << endl;
    }
    if (rightVal) {
      string valStr = resolveValue(rightVal);
      uint64_t val=0;
      if(RoseBin_support::from_string<uint64_t>(val, valStr, std::hex))
	resolveAddr += val;
      //      cerr << "Found rightVal: " << " ("<<valStr<<")"<<
      //	"  resolvAddr: "<<RoseBin_support::HexToString(resolveAddr) << endl;
    }
	      
  }
  //SgAsmType* type = mem->get_type();
  //adr+="resolved: " +RoseBin_support::HexToString(resolveAddr)+" - orig:";
  //adr += unparseExpression(exprOffset,false);
  return resolveAddr;
}



bool 
BinQSupport::memoryExpressionContainsRegister(X86RegisterClass cl, int registerNumber,
					      SgAsmMemoryReferenceExpression* mem) {
  bool containsRegister=false;
  SgAsmExpression* exprOffset = mem->get_address();
  SgAsmExpression* left =NULL;
  SgAsmExpression* right =NULL;
  SgAsmBinaryAdd* add = isSgAsmBinaryAdd(exprOffset);
  if (add) {
    left = add->get_lhs();
    right = add->get_rhs();
  }
  if (left || right) {
    SgAsmx86RegisterReferenceExpression* leftReg = isSgAsmx86RegisterReferenceExpression(left);
    SgAsmx86RegisterReferenceExpression* rightReg = isSgAsmx86RegisterReferenceExpression(right);
    X86RegisterClass regClass ;
    int regNr =0;
    if (leftReg) {
      regClass = (X86RegisterClass)leftReg->get_descriptor().get_major();
      regNr = leftReg->get_descriptor().get_minor();
    }
    if (rightReg) {
      regClass = (X86RegisterClass)rightReg->get_descriptor().get_major();
      regNr = rightReg->get_descriptor().get_minor();
    }
    if (cl == regClass && regNr==registerNumber) {
      containsRegister=true;
    }
  }
  return containsRegister;
}





SgAsmInstruction*
BinQSupport::getNextStmt(SgAsmInstruction* inst) {
  SgAsmInstruction* nextStmt = NULL;
  SgAsmFunctionDeclaration * parBlock = isSgAsmFunctionDeclaration(inst->get_parent());
  //cerr << " Checking nextStmt  :  parent : " << inst->get_parent()->class_name() << " : " << parBlock << endl;
  if (parBlock) {
    int pos=-1;
    int stop=-2;
    Rose_STL_Container<SgAsmStatement*> stmtList = parBlock->get_statementList();
    Rose_STL_Container<SgAsmStatement*>::const_iterator it  = stmtList.begin();
    for (;it!=stmtList.end();++it) {
      pos++;
      SgAsmStatement* stmt = *it;
      ROSE_ASSERT(stmt);
      SgAsmInstruction* stInst = isSgAsmInstruction(stmt);
      if (stInst) {
	if (stop==pos)
	  nextStmt = stInst;
	if (inst==stInst)
	  stop =(pos+1);
      }
    }
  }
  return nextStmt;
}



rose_addr_t
BinQSupport::checkIfValidAddress(rose_addr_t next_addr, SgAsmInstruction* inst) {
  if (next_addr==inst->get_address()) {
    // cant proceed to next address
    SgAsmInstruction* nextStmt=getNextStmt(inst);
    //cerr << "IDA found next : " << nextStmt << endl;
    if (nextStmt) {
      SgAsmx86Instruction* instN = isSgAsmx86Instruction(nextStmt);
      if (instN)
	next_addr = instN->get_address();
    }
    if (next_addr==inst->get_address())
      return 0;
  }
  return next_addr;
}


SgAsmInstruction*
BinQSupport::getPrevStmt(SgAsmInstruction* inst) {
  SgAsmInstruction* nextStmt = NULL;
  SgAsmFunctionDeclaration * parBlock = isSgAsmFunctionDeclaration(inst->get_parent());
  //cerr << " Checking nextStmt  :  parent : " << inst->get_parent()->class_name() << " : " << parBlock << endl;
  if (parBlock) {
    Rose_STL_Container<SgAsmStatement*> stmtList = parBlock->get_statementList();
    Rose_STL_Container<SgAsmStatement*>::const_iterator it  = stmtList.begin();
    SgAsmInstruction* before=NULL;
    for (;it!=stmtList.end();++it) {
      SgAsmStatement* stmt = *it;
      ROSE_ASSERT(stmt);
      SgAsmInstruction* stInst = isSgAsmInstruction(stmt);
      if (stInst) {
	if (inst==stInst)
	  nextStmt=before;
	before=stInst;
      }
    }
  }
  return nextStmt;
}

SgAsmx86Instruction*
BinQSupport::checkIfValidPredecessor(rose_addr_t next_addr, SgAsmInstruction* inst) {
  SgAsmx86Instruction* ninst = isSgAsmx86Instruction(inst);
  if (next_addr==inst->get_address()) {
    // cant proceed to next address
    SgAsmInstruction* nextStmt=getPrevStmt(inst);
    //cerr << "IDA found next : " << nextStmt << endl;
    if (nextStmt) {
      SgAsmx86Instruction* instN = isSgAsmx86Instruction(nextStmt);
      if (instN) {
	next_addr = instN->get_address();
	ninst=instN;
      }
    }
    if (next_addr==inst->get_address())
      return ninst;
  }
  return ninst;
}
