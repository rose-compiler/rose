// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "RoseBin_support.h"

using namespace std;
using namespace RoseBin_Def;

bool RoseBin_support::DEBUG_M=false;
bool RoseBin_support::DEBUG_M_MIN=false;
bool RoseBin_support::db=false;
//RoseBin_Def::Language assemblyLanguage = RoseBin_Def::none;
//RoseBin_support::assemblyLanguage=Language.x86;
RoseBin_Def::RoseLanguage RoseBin_Def::RoseAssemblyLanguage;
RoseBin_Arch::Architecture RoseBin_Arch::arch;
RoseBin_OS::OSSYSTEM RoseBin_OS::os_sys;
RoseBin_OS_VER::OS_VERSION RoseBin_OS_VER::os_ver;

bool RoseBin_support::DEBUG_MODE() { return DEBUG_M;};
 void RoseBin_support::setDebugMode(bool mode) {DEBUG_M=mode;}
bool RoseBin_support::DEBUG_MODE_MIN() { return DEBUG_M_MIN;};
 void RoseBin_support::setDebugModeMin(bool mode) {DEBUG_M_MIN=mode;}

 std::string RoseBin_support::getTypeName(RoseBin_DataTypes::DataTypes t) {
   std::string name = "";
   switch (t) {
   case 0: {
     name = "unknown"; break;
   }
   case 1: {
     name = "no"; break;
   }
   case 2: {
     name = "int"; break;
   }
   case 3: {
     name = "unsigned int"; break;
   }
   case 4: {
     name = "struct"; break;
   }
   case 5: {
     name = "char*"; break;
   }
   case 6: {
     name = "size_t"; break;
   }
   case 7: {
     name = "const char*"; break;
   }
   case 8: {
     name = "pid_t"; break;
   }
   case 9: {
     name = "unsigned int*"; break;
   }
   case 10: {
     name = "long"; break;
   }
   default: break;
   }
   return name;
 }


/****************************************************
 * debug information
 ****************************************************/
 void RoseBin_support::printExprNode(exprTreeType expt) {
  std::cout << " node id : " << expt.id << std::endl;
  std::cout << " node type : " << expt.expr_type << std::endl;
  std::cout << " node symbol : " << expt.symbol <<std::endl;
  std::cout << " node immediate: " << expt.immediate <<std::endl;
  std::cout << " node position : " << expt.position <<std::endl;
  std::cout << " node parent: " << expt.parent_id << std::endl;
}

/**********************************************************                          
 *  check if a function needs to be filtered away
 *********************************************************/                   
bool RoseBin_support::filterName(std::string name) {
  if ((name.find("std::") == 0) ||
      (name.find("__") == 0) ||
      (name.find("operator") == 0)
      ) return true;
  return false;
}

/**********************************************************                          
 *  convert a string to uppercase
 *********************************************************/                   
 std::string RoseBin_support::str_to_upper(std::string str)
{
  for (int i=0;i< (int)str.size();i++)
    str[i] = toupper(str[i]);
  return str;
}


/**********************************************************                          
 *  Resolve Boolean Value to String                                                   
 *********************************************************/                   
 std::string RoseBin_support::resBool(bool val) {                                               
  if (val)                                                                      
    return "true";                                                           
  return "false";                                                          
}

/****************************************************
 * resolve value 
 ****************************************************/
std::string RoseBin_support::resolveValue(SgAsmValueExpression* expr, 
                                          bool is_mnemonic_call,
                                          uint8_t &byte_val,
                                          uint16_t &word_val,
                                          uint32_t &double_word_val,
                                          uint64_t &quad_word_val,
                                          bool unparseSignedConstants) {
  string res="...";
  ostringstream os; 
  if (isSgAsmByteValueExpression(expr)) {
    SgAsmByteValueExpression* valExp = isSgAsmByteValueExpression(expr);
    byte_val = valExp->get_value(); 
    //res = "(byte)" + RoseBin_support::ToString(val);
    if (is_mnemonic_call)
      os << hex << (unsigned int)byte_val; 
    else if (unparseSignedConstants)
      os << dec << (int8_t)byte_val;
    else
      os << "0x" << hex << (unsigned int)byte_val; 
    res = os.str();
  } else

    if (isSgAsmDoubleFloatValueExpression(expr)) {
      SgAsmDoubleFloatValueExpression* valExp = isSgAsmDoubleFloatValueExpression(expr);
      double val = valExp->get_value(); 
      os << "0x" << hex << val;
      res = os.str();
      //res = "(dfloat)" + RoseBin_support::ToString(val);
    } else

      if (isSgAsmDoubleWordValueExpression(expr)) {
        SgAsmDoubleWordValueExpression* valExp = isSgAsmDoubleWordValueExpression(expr);
        double_word_val = valExp->get_value(); 
        // int int_val = static_cast<int> (val);
        // res = "(dword)" + RoseBin_support::ToString(int_val);
        if (is_mnemonic_call)
          os << hex << double_word_val; 
        else if (unparseSignedConstants)
          os << dec << (int32_t)double_word_val;
        else
          os << "0x" << hex << double_word_val; 
        res = os.str();
      } else

        if (isSgAsmSingleFloatValueExpression(expr)) {
          SgAsmSingleFloatValueExpression* valExp = isSgAsmSingleFloatValueExpression(expr);
          float val = valExp->get_value(); 
          os << "0x" << hex << val;
          res = os.str();
          //res = "(float)" + RoseBin_support::ToString(val);
        } else

          if (isSgAsmQuadWordValueExpression(expr)) {
            SgAsmQuadWordValueExpression* valExp = isSgAsmQuadWordValueExpression(expr);
            quad_word_val = valExp->get_value(); 
            if (unparseSignedConstants)
              os << dec << (int64_t)quad_word_val;
            else
              os << "0x" << hex << quad_word_val;
            res = os.str();
            //res = "(qword)" + RoseBin_support::ToString(val);
          } else

            if (isSgAsmVectorValueExpression(expr)) {
              //SgAsmVectorValueExpression* valExp = isSgAsmVectorValueExpression(expr);
    
              res = "(vector)";
            } else

              if (isSgAsmWordValueExpression(expr)) {
                SgAsmWordValueExpression* valExp = isSgAsmWordValueExpression(expr);
                word_val = valExp->get_value(); 
                if (unparseSignedConstants)
                  os << dec << (int16_t)word_val;
                else
                  os << "0x" << hex << word_val;
                res = os.str();
                //              res = "(word)" + RoseBin_support::ToString(val);
              } 

  return res;
}

bool isAsmUnconditionalBranch(SgAsmInstruction* insn) {
  switch (insn->variantT()) {
    case V_SgAsmx86Instruction: return x86InstructionIsUnconditionalBranch(isSgAsmx86Instruction(insn));
    // case V_SgAsmArmInstruction: return armInstructionIsUnconditionalBranch(isSgAsmArmInstruction(insn));
        default: { ROSE_ASSERT (!"Bad instruction type"); /* Avoid MSVC warning. */ return false; }
  }
}

bool isAsmBranch(SgAsmInstruction* insn) {
  switch (insn->variantT()) {
    case V_SgAsmx86Instruction: return x86InstructionIsControlTransfer(isSgAsmx86Instruction(insn));
    // case V_SgAsmArmInstruction: return armInstructionIsBranch(isSgAsmArmInstruction(insn));
        default: { ROSE_ASSERT (!"Bad instruction type"); /* Avoid MSVC warning. */ return false; }
  }
}

bool getAsmKnownBranchTarget(SgAsmInstruction* insn, uint64_t& addr) {
  switch (insn->variantT()) {
    case V_SgAsmx86Instruction: return x86GetKnownBranchTarget(isSgAsmx86Instruction(insn), addr);
    // case V_SgAsmArmInstruction: return armGetKnownBranchTarget(isSgAsmArmInstruction(insn), addr);
        default: { ROSE_ASSERT (!"Bad instruction type"); /* Avoid MSVC warning. */ return false; }
  }
}
