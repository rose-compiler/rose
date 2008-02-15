/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that actually builds the Rose Tree
 ****************************************************/

#include "rose.h"
#include "RoseBin_buildTree.h"

using namespace std;

/****************************************************
 * return information about the register
 ****************************************************/
void RoseBin_buildTree::resolveRegister(string symbol, 
		     SgAsmRegisterReferenceExpression::arm_register_enum *registerSg,
		     SgAsmRegisterReferenceExpression::arm_position_in_register_enum *regSize) {
  // ARM architecture 
  if (symbol=="R1") {
    *registerSg = SgAsmRegisterReferenceExpression::reg1;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R2") {
    *registerSg = SgAsmRegisterReferenceExpression::reg2;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R3") {
    *registerSg = SgAsmRegisterReferenceExpression::reg3;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R4") {
    *registerSg = SgAsmRegisterReferenceExpression::reg4;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R5") {
    *registerSg = SgAsmRegisterReferenceExpression::reg5;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R6") {
    *registerSg = SgAsmRegisterReferenceExpression::reg6;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R7") {
    *registerSg = SgAsmRegisterReferenceExpression::reg7;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R8") {
    *registerSg = SgAsmRegisterReferenceExpression::reg8;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R9") {
    *registerSg = SgAsmRegisterReferenceExpression::reg9;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R10") {
    *registerSg = SgAsmRegisterReferenceExpression::reg10;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R11") {
    *registerSg = SgAsmRegisterReferenceExpression::reg11;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R12") {
    *registerSg = SgAsmRegisterReferenceExpression::reg12;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R13") {
    *registerSg = SgAsmRegisterReferenceExpression::reg13;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R14") {
    *registerSg = SgAsmRegisterReferenceExpression::reg14;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="R15") {
    *registerSg = SgAsmRegisterReferenceExpression::reg15;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }

  else if (symbol=="SP") {
    *registerSg = SgAsmRegisterReferenceExpression::SP;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="PC") {
    *registerSg = SgAsmRegisterReferenceExpression::PC;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="LR") {
    *registerSg = SgAsmRegisterReferenceExpression::LR;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="SL") {
    *registerSg = SgAsmRegisterReferenceExpression::SL;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="IP") {
    *registerSg = SgAsmRegisterReferenceExpression::IP;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }
  else if (symbol=="FP") {
    *registerSg = SgAsmRegisterReferenceExpression::FP;
    *regSize =    SgAsmRegisterReferenceExpression::arm_dword;
  }


  else {
    cerr << "ERROR !!! ::: arm symbol could not be resolved! : " << symbol << "\n" << endl;
  }
}

/****************************************************
 * return information about the register
 ****************************************************/
void RoseBin_buildTree::resolveRegister(string symbol, 
		     SgAsmRegisterReferenceExpression::x86_register_enum *registerSg,
		     SgAsmRegisterReferenceExpression::x86_position_in_register_enum *regSize) {
  if (symbol=="ST(0)" || symbol=="ST(1)" || symbol=="ST(2)" ||
      symbol=="ST(3)" || symbol=="ST(4)" || symbol=="ST(5)" ||
      symbol=="ST(6)" || symbol=="ST(7)") {
    *registerSg = SgAsmRegisterReferenceExpression::ST;
    *regSize =    SgAsmRegisterReferenceExpression::dword;
  } else

  if (symbol=="RAX" || symbol=="EAX" || symbol=="AX" || symbol=="AL" || symbol=="AH") {
    *registerSg = SgAsmRegisterReferenceExpression::rAX;
    if (symbol=="AL") 
      *regSize =    SgAsmRegisterReferenceExpression::low_byte;
    if (symbol=="AH") 
      *regSize =    SgAsmRegisterReferenceExpression::high_byte;
    if (symbol=="AX") 
      *regSize =    SgAsmRegisterReferenceExpression::word;
    if (symbol=="EAX") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RAX") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="RBX" || symbol=="EBX" || symbol=="BX" || symbol=="BL" || symbol=="BH") {
    *registerSg = SgAsmRegisterReferenceExpression::rBX;
    if (symbol=="BL") 
      *regSize =    SgAsmRegisterReferenceExpression::low_byte;
    if (symbol=="BH") 
      *regSize =    SgAsmRegisterReferenceExpression::high_byte;
    if (symbol=="BX") 
      *regSize =    SgAsmRegisterReferenceExpression::word;
    if (symbol=="EBX") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RBX") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="RCX" || symbol=="ECX" || symbol=="CX" || symbol=="CL" || symbol=="CH") {
    *registerSg = SgAsmRegisterReferenceExpression::rCX;
    if (symbol=="CL") 
      *regSize =    SgAsmRegisterReferenceExpression::low_byte;
    if (symbol=="CH") 
      *regSize =    SgAsmRegisterReferenceExpression::high_byte;
    if (symbol=="CX") 
      *regSize =    SgAsmRegisterReferenceExpression::word;
    if (symbol=="ECX") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RCX") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="RDX" || symbol=="EDX" || symbol=="DX" || symbol=="DL" || symbol=="DH") {
    *registerSg = SgAsmRegisterReferenceExpression::rDX;
    if (symbol=="DL") 
      *regSize =    SgAsmRegisterReferenceExpression::low_byte;
    if (symbol=="DH") 
      *regSize =    SgAsmRegisterReferenceExpression::high_byte;
    if (symbol=="DX") 
      *regSize =    SgAsmRegisterReferenceExpression::word;
    if (symbol=="EDX") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RDX") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="DI" || symbol=="EDI" || symbol=="RDI") {
    *registerSg = SgAsmRegisterReferenceExpression::rDI;
    if (symbol=="DI") 
      *regSize =    SgAsmRegisterReferenceExpression::word;
    if (symbol=="EDI") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RDI") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="SI" || symbol=="ESI" || symbol=="RSI") {
    *registerSg = SgAsmRegisterReferenceExpression::rSI;
    if (symbol=="SI") 
      *regSize =    SgAsmRegisterReferenceExpression::word;
    if (symbol=="ESI") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RSI") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="ESP" || symbol=="RSP") {
    *registerSg = SgAsmRegisterReferenceExpression::rSP;
    if (symbol=="ESP") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RSP") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="EBP" || symbol=="RBP") {
    *registerSg = SgAsmRegisterReferenceExpression::rBP;
    if (symbol=="EBP") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RBP") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="CS") {
    *registerSg = SgAsmRegisterReferenceExpression::CS;
      *regSize =    SgAsmRegisterReferenceExpression::word;
  }

  else if (symbol=="DS") {
    *registerSg = SgAsmRegisterReferenceExpression::DS;
      *regSize =    SgAsmRegisterReferenceExpression::word;
  }

  else if (symbol=="SS") {
    *registerSg = SgAsmRegisterReferenceExpression::SS;
      *regSize =    SgAsmRegisterReferenceExpression::word;
  }

  else if (symbol=="ES") {
    *registerSg = SgAsmRegisterReferenceExpression::ES;
    *regSize =    SgAsmRegisterReferenceExpression::word;
  }

  else if (symbol=="FS" || symbol=="RFS") {
    *registerSg = SgAsmRegisterReferenceExpression::FS;
    if (symbol=="FS") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="RFS") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
  }

  else if (symbol=="GS" || symbol=="RGS") {
    *registerSg = SgAsmRegisterReferenceExpression::GS;
    if (symbol=="GS") 
      *regSize =    SgAsmRegisterReferenceExpression::dword;
    if (symbol=="GS") 
      *regSize =    SgAsmRegisterReferenceExpression::qword;
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
