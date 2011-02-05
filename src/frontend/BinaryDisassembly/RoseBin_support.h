/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : supporting functions for RoseBin
 ****************************************************/


#ifndef __RoseBin_support__
#define __RoseBin_support__
#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <sstream>
// #include "rose.h"
#include "stdint.h"


#include "RoseBin_IDAPRO_exprTree.h"

#ifdef _MSC_VER
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "x86InstructionProperties.h"

namespace RoseBin_Def {
 enum RoseLanguage 
 {
    none,
    x86,
    arm
 } ;
 extern RoseLanguage RoseAssemblyLanguage;
}

namespace RoseBin_OS {
 enum OSSYSTEM 
 {
    linux_op,
    windows_op
 } ;
 extern OSSYSTEM os_sys;
}

namespace RoseBin_Arch {
 enum Architecture 
 {
    bit32,
    bit64
 };
 extern Architecture arch;
}

namespace RoseBin_OS_VER {
 enum OS_VERSION 
 {
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
 } ;
 extern OS_VERSION os_ver;
}


namespace RoseBin_DataTypes {

 enum DataTypes 
 {
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
 } ;


 extern DataTypes Rose_Data;
}

// CH (4/9/2010): Use boost::unordered instead
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#if 0
#ifdef _MSC_VER
#include <hash_map>
#include <hash_set>
#else
#include <ext/hash_map>
#include <ext/hash_set>
#endif
#endif

class SgDirectedGraphNode;
#if 0
//#ifndef _MSC_VER
#if 1
namespace rose_hash {
  template <> struct hash <SgDirectedGraphNode*> {
    size_t operator()(SgDirectedGraphNode* const & n) const {
      return (size_t) n;
    }
  };

  template <> struct hash <X86RegisterClass> {
    size_t operator()(X86RegisterClass const & n) const {
      return (size_t) n;
    }
  };

#ifndef __LP64__
  template <> struct hash <uint64_t> {
    size_t operator()(uint64_t const& n) const {
      return (size_t)((n >> 32) ^ n);
    }
  };
}
#endif
#else
// DQ (11/27/2009): No message is required, since this is the correct fix for MSVC.
// #pragma message ("WARNING: Commented out hash_map operators in RoseBin_support.h for Windows. Might need to fix this.")
#endif

#endif





class RoseBin_support {
 private:
   static bool DEBUG_M;
   static bool DEBUG_M_MIN;
   static RoseBin_Def::RoseLanguage assemblyLanguage;
   static bool db;

 public:
   /* This enum used to be defined as part of x86InstructionEnum.h, but we don't represent registers this way anymore. The
    * new representation uses the RegisterDescriptor type, which includes separate offset and size values rather than using a
    * single enum to represent both.  I've moved the definition here because it seems to be used quite extensively by the
    * various RoseBin_* classes. This also changes the name from X86PositionInRegister so it's not accidently used elsewhere.
    *
    * Also, RoseBin seems to have a large body of code that deals with converting between various enumeration constants (like
    * X86RegisterClass and X86PositionInRegister) and integers. This is probably no longer necessary. An alternative approach is
    * to use the RegisterDictionary class lookup methods.  This would also make RoseBin less dependent on actual architectures
    * since RegisterDictionary generalizes most aspects of register descriptions.
    *
    * [RPM 2010-10-12] */
   enum X86PositionInRegister 
   {
     x86_regpos_unknown, /*!< unknown (error or unitialized value) */
     x86_regpos_low_byte, /*!< 1st byte of register (bits 0-7), only for GPRs */
     x86_regpos_high_byte, /*!< 2nd byte of register (bits 8-15), only for ax,bx,cx,dx */
     x86_regpos_word, /*!< 16 bit part of register, only for GPRs, CR0, CR8? */
     x86_regpos_dword, /*!< lower 32 bit part of register, only for GPRs */
     x86_regpos_qword, /*!< lower 64 bit part of register, only for GPRs */
     x86_regpos_all /*!< the full register is used (default value), only value allowed for segregs and st */
   };


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

#ifndef _MSC_VER
  static inline double getTime() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1.e-6;
  }
#else
    static inline double getTime() {
        // tps (12/08/2009): GetTime unimplemented but will pass like this.
                return 0;
        }
#endif


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


