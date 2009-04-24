/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : supporting functions for RoseBin
 ****************************************************/

#ifndef __RoseBin_support__
#define __RoseBin_support__

#include <stdio.h>
#include <iostream>
#include <sstream>
// #include "rose.h"
#include "stdint.h"

#include "RoseBin_IDAPRO_exprTree.h"

#include <sys/time.h>

#include "x86InstructionProperties.h"

namespace RoseBin_Def {
 enum RoseLanguage {
    none,
    x86,
    arm
 };
 extern RoseLanguage RoseAssemblyLanguage;
}

namespace RoseBin_OS {
 enum OSSYSTEM {
    linux_op,
    windows_op
 };
 extern OSSYSTEM os_sys;
}

namespace RoseBin_Arch {
 enum Architecture {
    bit32,
    bit64
 };
 extern Architecture arch;
}

namespace RoseBin_OS_VER {
 enum OS_VERSION {
    linux_22,
    linux_24,
    linux_26,
    linux_27,
    NT_SP3,
    NT_SP4,
    NT_SP5,
    NT_SP6,
    Win2000_SP0,
    Win2000_SP1,
    Win2000_SP2,
    Win2000_SP3,
    Win2000_SP4,
    WinXP_SP0,
    WinXP_SP1,
    WinXP_SP2,
    WinVista_SP0
 };
 extern OS_VERSION os_ver;
}


namespace RoseBin_DataTypes {

 enum DataTypes {
    unknown,
    d_none,
    d_int,
    d_uint,
    d_struct,
    d_char_p,
    d_size_t,
    d_const_char_p,
    d_pid_t,
    d_uint_p,
    d_long,
    d_array
 };


 extern DataTypes Rose_Data;
}



#include <ext/hash_map>
#include <ext/hash_set>

// DQ (4/23/2009): ROSE_USE_NEW_GRAPH_NODES is not set on the configure commandline.
// #if OLD_GRAPH_NODES
#ifndef ROSE_USE_NEW_GRAPH_NODES
// DQ (8/18/2008): Commented out as part of support for new Graph IR nodes!
namespace __gnu_cxx {
  template <> struct hash<std::string> {
    unsigned long operator()( const std::string &s) const {
      return __gnu_cxx::hash< const char*> () (s.c_str());
    }
  };
}
#endif

class SgDirectedGraphNode;
namespace __gnu_cxx { 
  template <> struct hash <SgDirectedGraphNode*> {
    size_t operator()(SgDirectedGraphNode* const & n) const {
      return (size_t) n;
    }
  };
}

namespace __gnu_cxx { 
  template <> struct hash <X86RegisterClass> {
    size_t operator()(X86RegisterClass const & n) const {
      return (size_t) n;
    }
  };
}

#ifndef __LP64__
namespace __gnu_cxx {
  template <> struct hash <uint64_t> {
    size_t operator()(uint64_t const& n) const {
      return (size_t)((n >> 32) ^ n);
    }
  };
}
#else

#endif




class RoseBin_support {
 private:
   static bool DEBUG_M;
   static bool DEBUG_M_MIN;
   static RoseBin_Def::RoseLanguage assemblyLanguage;
   static bool db;

 public:
   static std::string getTypeName(RoseBin_DataTypes::DataTypes t);


   static bool DEBUG_MODE();
   static void setDebugMode(bool mode);
   static bool DEBUG_MODE_MIN();
   static void setDebugModeMin(bool mode);
  /****************************************************
   * debug information
   ****************************************************/
  static void printExprNode(exprTreeType expt);

  /**********************************************************                          
   *  check if a function needs to be filtered away
   *********************************************************/                   
  static bool filterName(std::string name);

  /**********************************************************                          
   *  convert a string to uppercase
   *********************************************************/                   
  static std::string str_to_upper(std::string str);

  /**********************************************************                          
   *  Resolve Boolean Value to String                                                 
   *********************************************************/                   
  static std::string resBool(bool val);

  /**********************************************************                          
   *  Convert to string
   *********************************************************/                   
  template<typename T>
  static  std::string ToString(T t){
    std::ostringstream myStream; //creates an ostringstream object
    myStream << t << std::flush;
    return myStream.str(); //returns the string form of the stringstream object
  }

  template<typename T>
  static  std::string HexToString(T t){
    std::ostringstream myStream; //creates an ostringstream object
    myStream << std::setw(8) << std::hex <<  t ;
    return myStream.str(); //returns the string form of the stringstream object
  }

  static uint64_t HexToDec(std::string h) {
    return static_cast<uint64_t>(strtol(h.c_str(), NULL, 16));
  }

  template <class T>
    static bool from_string(T& t, 
		     const std::string& s, 
		     std::ios_base& (*f)(std::ios_base&)) {
      std::istringstream iss(s);
      return !(iss >> f >> t).fail();
    }

  static std::string 
    resolveValue(SgAsmValueExpression* expr, 
		 bool is_mnemonic_call,
		 uint8_t &b_val,
		 uint16_t &w_val,
		 uint32_t &dw_val,
		 uint64_t &qw_val,
                 bool unparseSignedConstants = false);


  static bool bigEndian() {
    unsigned char SwapTest[2] = { 1, 0 };
    if( *(short *) SwapTest == 1 ){
	//little endian
	return false;
      } else  {
	//big endian
	return true;
      }
  }

  static bool
    getDataBaseSupport() {
    return db;
  }

  static void
    setDataBaseSupport(bool db_p) {
     db = db_p;
  }

  static bool isConditionalInstruction(SgAsmx86Instruction*inst) {
    if (x86InstructionIsConditionalControlTransfer(inst) ||
	x86InstructionIsConditionalDataTransfer(inst))
      return true;
    return false;
  }

  static bool isConditionalFlagInstruction(SgAsmx86Instruction*inst) {
    if (x86InstructionIsConditionalFlagControlTransfer(inst) ||
	x86InstructionIsConditionalFlagDataTransfer(inst) ||
	x86InstructionIsConditionalFlagBitAndByte(inst))
      return true;
    return false;
  }

#if 0
  static bool 
    isConditionalInstruction(SgAsmInstruction*inst) {
    bool condInst = false;
    if (RoseBin_Def::RoseAssemblyLanguage==RoseBin_Def::x86) {
      if (isSgAsmx86ConditionalDataTransferInstruction(inst) ||
	  isSgAsmx86ConditionalControlTransferInstruction(inst))
	condInst = true;
    }
    return condInst;
  }

  static bool 
    isConditionalFlagInstruction(SgAsmInstruction*inst) {
    bool condInst = false;
    if (RoseBin_Def::RoseAssemblyLanguage==RoseBin_Def::x86) {
      if (isSgAsmx86ConditionalFlagDataTransferInstruction(inst) ||
	  isSgAsmx86ConditionalFlagBitAndByteInstruction(inst) ||
	  isSgAsmx86ConditionalFlagControlTransferInstruction(inst) ||
	  isSgAsmx86ConditionalFlagStringInstruction(inst))
	condInst = true;
    }
  return condInst;
  }
#endif
  

  static RoseBin_Def::RoseLanguage getAssemblyLanguage() {
    return assemblyLanguage;
  }

  static void setAssemblyLanguage(RoseBin_Def::RoseLanguage lang) {
    assemblyLanguage = lang;
  }

  static void checkText (std::string& comment) {
    unsigned int maxsize=30;
    if (comment.size()>maxsize) {
      comment = comment.substr(0,maxsize);
      comment = comment + "... ";
    }

    std::string::size_type endpos = comment.find('\n');
    if (endpos!=std::string::npos)
    comment = comment.substr(0,endpos);

    std::string newComment = "";
    for (int i=0; i< (int)comment.size() ; ++i) {
      if (comment[i]=='&')
	newComment += "&amp;";
      else if (comment[i]=='"')
	newComment += "&quot;";
      else
	newComment += comment[i];
    }
    comment = newComment;
  }

  static inline double getTime() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1.e-6;
  }
  

  
};

// From instructionDispatch.cpp (generated but checked into SVN):
SgAsmArmInstruction* createArmInstruction(uint64_t address,
                                          const std::string& mnemonic);

SgAsmx86Instruction* createx86Instruction(uint64_t address,
                                          const std::string& mnemonic);

void normalizex86Mnemonic(std::string& mnemonic);
void normalizeArmMnemonic(std::string& mnemonic);

// From RoseBin_support.cpp:
bool isAsmUnconditionalBranch(SgAsmInstruction*);
bool isAsmBranch(SgAsmInstruction*);
bool getAsmKnownBranchTarget(SgAsmInstruction*, uint64_t& addr);

#endif


