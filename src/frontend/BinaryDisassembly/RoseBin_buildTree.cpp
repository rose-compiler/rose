/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that actually builds the Rose Tree
 ****************************************************/

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "RoseBin_buildTree.h"

using namespace std;

/****************************************************
 * return information about the register
 ****************************************************/
void RoseBin_buildTree::resolveRegister(string symbol,
                                        RegisterDescriptor *registerSg) {

  // ARM architecture
  const RegisterDictionary *rdict = RegisterDictionary::arm7();

  /* Symbol is upper case. Dictionary stores register names in lower case. */
  for (string::size_type i=0; i<symbol.size(); i++)
    symbol[i] = tolower(symbol[i]);

  // Some of the names pased in here are not present in the dictionary. The dictionary doesn't have these names because these
  // registers are not always used for this purpose.
  if (symbol=="sp") {
    symbol = "r13";
  } else if (symbol=="pc") {
    symbol = "r15";
  } else if (symbol=="lr") {
    symbol = "r14";
  } else if (symbol=="sl") {
    symbol = "r10";
  } else if (symbol=="ip") {
    symbol = "r12";
  } else if (symbol=="fp") {
    symbol = "r11";
  }

  const RegisterDescriptor *rdesc = rdict->lookup(symbol);
  if (rdesc) {
    *registerSg = *rdesc;
  } else {
    cerr << "ERROR !!! ::: arm symbol could not be resolved! : " << symbol << "\n" << endl;
  }
}

/****************************************************
 * return information about the register
 ****************************************************/
void RoseBin_buildTree::resolveRegister(string symbol, 
                                        X86RegisterClass *regClassSg,
                                        int *registerSg,
                                        X86PositionInRegister *regSize) {
  if (symbol=="ST(0)" || symbol=="ST(1)" || symbol=="ST(2)" ||
      symbol=="ST(3)" || symbol=="ST(4)" || symbol=="ST(5)" ||
      symbol=="ST(6)" || symbol=="ST(7)") {
    *regClassSg = x86_regclass_st;
    *registerSg = symbol[4] - '0';
    *regSize =    x86_regpos_dword;
  } else

  if (symbol=="RAX" || symbol=="EAX" || symbol=="AX" || symbol=="AL" || symbol=="AH") {
    *regClassSg = x86_regclass_gpr;
    *registerSg = x86_gpr_ax;
    if (symbol=="AL") 
      *regSize =    x86_regpos_low_byte;
    if (symbol=="AH") 
      *regSize =    x86_regpos_high_byte;
    if (symbol=="AX") 
      *regSize =    x86_regpos_word;
    if (symbol=="EAX") 
      *regSize =    x86_regpos_dword;
    if (symbol=="RAX") 
      *regSize =    x86_regpos_qword;
  }

  else if (symbol=="RBX" || symbol=="EBX" || symbol=="BX" || symbol=="BL" || symbol=="BH") {
    *regClassSg = x86_regclass_gpr;
    *registerSg = x86_gpr_bx;
    if (symbol=="BL") 
      *regSize =    x86_regpos_low_byte;
    if (symbol=="BH") 
      *regSize =    x86_regpos_high_byte;
    if (symbol=="BX") 
      *regSize =    x86_regpos_word;
    if (symbol=="EBX") 
      *regSize =    x86_regpos_dword;
    if (symbol=="RBX") 
      *regSize =    x86_regpos_qword;
  }

  else if (symbol=="RCX" || symbol=="ECX" || symbol=="CX" || symbol=="CL" || symbol=="CH") {
    *regClassSg = x86_regclass_gpr;
    *registerSg = x86_gpr_cx;
    if (symbol=="CL") 
      *regSize =    x86_regpos_low_byte;
    if (symbol=="CH") 
      *regSize =    x86_regpos_high_byte;
    if (symbol=="CX") 
      *regSize =    x86_regpos_word;
    if (symbol=="ECX") 
      *regSize =    x86_regpos_dword;
    if (symbol=="RCX") 
      *regSize =    x86_regpos_qword;
  }

  else if (symbol=="RDX" || symbol=="EDX" || symbol=="DX" || symbol=="DL" || symbol=="DH") {
    *regClassSg = x86_regclass_gpr;
    *registerSg = x86_gpr_dx;
    if (symbol=="DL") 
      *regSize =    x86_regpos_low_byte;
    if (symbol=="DH") 
      *regSize =    x86_regpos_high_byte;
    if (symbol=="DX") 
      *regSize =    x86_regpos_word;
    if (symbol=="EDX") 
      *regSize =    x86_regpos_dword;
    if (symbol=="RDX") 
      *regSize =    x86_regpos_qword;
  }

  else if (symbol=="DI" || symbol=="EDI" || symbol=="RDI") {
    *regClassSg = x86_regclass_gpr;
    *registerSg = x86_gpr_di;
    if (symbol=="DI") 
      *regSize =    x86_regpos_word;
    if (symbol=="EDI") 
      *regSize =    x86_regpos_dword;
    if (symbol=="RDI") 
      *regSize =    x86_regpos_qword;
  }

  else if (symbol=="SI" || symbol=="ESI" || symbol=="RSI") {
    *regClassSg = x86_regclass_gpr;
    *registerSg = x86_gpr_si;
    if (symbol=="SI") 
      *regSize =    x86_regpos_word;
    if (symbol=="ESI") 
      *regSize =    x86_regpos_dword;
    if (symbol=="RSI") 
      *regSize =    x86_regpos_qword;
  }

  else if (symbol=="SP" || symbol=="ESP" || symbol=="RSP") {
    *regClassSg = x86_regclass_gpr;
    *registerSg = x86_gpr_sp;
    if (symbol=="ESP") 
      *regSize =    x86_regpos_dword;
    if (symbol=="RSP") 
      *regSize =    x86_regpos_qword;
  }

  else if (symbol=="BP" || symbol=="EBP" || symbol=="RBP") {
    *regClassSg = x86_regclass_gpr;
    *registerSg = x86_gpr_bp;
    if (symbol=="EBP") 
      *regSize =    x86_regpos_dword;
    if (symbol=="RBP") 
      *regSize =    x86_regpos_qword;
  }

  else if (symbol=="CS") {
    *regClassSg = x86_regclass_segment;
    *registerSg = x86_segreg_cs;
      *regSize =    x86_regpos_word;
  }

  else if (symbol=="DS") {
    *regClassSg = x86_regclass_segment;
    *registerSg = x86_segreg_ds;
      *regSize =    x86_regpos_word;
  }

  else if (symbol=="SS") {
    *regClassSg = x86_regclass_segment;
    *registerSg = x86_segreg_ss;
      *regSize =    x86_regpos_word;
  }

  else if (symbol=="ES") {
    *regClassSg = x86_regclass_segment;
    *registerSg = x86_segreg_es;
    *regSize =    x86_regpos_word;
  }

  else if (symbol=="FS") {
    *regClassSg = x86_regclass_segment;
    *registerSg = x86_segreg_fs;
    *regSize =    x86_regpos_word;
  }

  else if (symbol=="GS") {
    *regClassSg = x86_regclass_segment;
    *registerSg = x86_segreg_gs;
    *regSize =    x86_regpos_word;
  } 



  else {
    cerr << "ERROR !!! ::: symbol could not be resolved! : " << symbol <<  endl;
  }
}


  /****************************************************
   * return debug information from the helpMap
   ****************************************************/
exprTreeType RoseBin_buildTree::getDebugHelp (SgAsmNode* sgBinNode) {
  return debugHelpMap[sgBinNode];
}
