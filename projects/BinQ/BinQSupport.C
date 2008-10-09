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
    void* sPtr = intern(unparseX86Expression(e));
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




SgNode* BinQSupport::disassembleFile(std::string tsv_directory){
  SgNode* globalBlock;
  std::cout << "\nDisassembling: " << tsv_directory << std::endl;
  if(exists(tsv_directory) == false)  {
    char buf[4096] = "\0";
    int i = 0; 
    while( exists ( relativePathPrefix +"/"+ tsv_directory) == false )     {
      if(i>10){
        std::string error_message = "user error: Relative Path to  ";
        error_message+=tsv_directory;
        error_message+=" not selected in " ;
        error_message+= boost::lexical_cast<std::string>(i);
        error_message+=" attempts.";
        eAssert(0,  (error_message.c_str()) );
      }

      QFileDialog dialog;
      dialog.setFileMode(QFileDialog::DirectoryOnly);
      //relativePathPrefix = dialog.getOpenFileName( 0, "Relative Path To Binaries", getcwd(buf, 4096), "ASCII (*)").toStdString();
      relativePathPrefix = dialog.getExistingDirectory(0,  "get existing directory", getcwd(buf, 4096)).toStdString();
      i++;
    }
    tsv_directory = relativePathPrefix +"/"+tsv_directory;
  };
  
  if(is_directory( tsv_directory  ) == true )
    {
      RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
      RoseBin_Arch::arch=RoseBin_Arch::bit32;
      RoseBin_OS::os_sys=RoseBin_OS::linux_op;
      RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;
      RoseFile* roseBin = new RoseFile( (char*)tsv_directory.c_str() );
      cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
      // query the DB to retrieve all data
      globalBlock = roseBin->retrieve_DB();
      // traverse the AST and test it
      roseBin->test();
    }else{
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
    std::cout << "Calling " << outStr.str() << std::endl;
    globalBlock =  frontend(args.size()-1,&args[0]);
  }
  return globalBlock;
};




std::string 
BinQSupport::normalizeInstructionsToHTML(std::vector<SgAsmx86Instruction*>::iterator beg, 
					    std::vector<SgAsmx86Instruction*>::iterator end) {
  string normalizedUnparsedInstructions;
  map<SgAsmExpression*, size_t> valueNumbers[3];
  numberOperands( beg,end, valueNumbers);

  // Unparse the normalized forms of the instructions
  for (; beg != end; ++beg ) {
    SgAsmx86Instruction* insn = *beg;
    string mne = insn->get_mnemonic();
    boost::to_lower(mne);
    mne = "<font color=\"red\">" + StringUtility::htmlEscape(mne)+"</font>";

    normalizedUnparsedInstructions += mne;
    const SgAsmExpressionPtrList& operands = getOperands(insn);
    // Add to total for this variant
    // Add to total for each kind of operand
    size_t operandCount = operands.size();

    normalizedUnparsedInstructions += "<font color=\"blue\">";
    for (size_t i = 0; i < operandCount; ++i) {
      SgAsmExpression* operand = operands[i];
      ExpressionCategory cat = getCategory(operand);
      map<SgAsmExpression*, size_t>::const_iterator numIter = valueNumbers[(int)cat].find(operand);
      assert (numIter != valueNumbers[(int)cat].end());
      size_t num = numIter->second;

      normalizedUnparsedInstructions += (cat == ec_reg ? " R" : cat == ec_mem ? " M" : " V") + boost::lexical_cast<string>(num);
    }
    normalizedUnparsedInstructions += "; </font> <br> ";
  }
  return normalizedUnparsedInstructions;
};


std::string BinQSupport::unparseX86InstructionToHTMLWithAddress(SgAsmx86Instruction* insn) {
  if (insn == NULL) return "BOGUS:NULL";
  string result = "<font color=\"green\">" + StringUtility::htmlEscape(StringUtility::intToHex(insn->get_address())) + "</font>:";
  result += "<font color=\"red\">" + StringUtility::htmlEscape(insn->get_mnemonic());
  switch (insn->get_branchPrediction()) {
  case x86_branch_prediction_none: break;
  case x86_branch_prediction_taken: result += ",pt"; break;
  case x86_branch_prediction_not_taken: result += ",pn"; break;
  default: ROSE_ASSERT (!"Bad branch prediction");
  }
  result += "</font>";
  result += std::string((result.size() >= 7 ? 1 : 7 - result.size()), ' ');
  SgAsmOperandList* opList = insn->get_operandList();
  const SgAsmExpressionPtrList& operands = opList->get_operands();
  for (size_t i = 0; i < operands.size(); ++i) {
    if (i != 0) result += ", ";
    result += "<font color=\"blue\">" + StringUtility::htmlEscape(unparseX86Expression(operands[i], (insn->get_kind() == x86_lea))) + "</font>";
  }
  return result;
}






