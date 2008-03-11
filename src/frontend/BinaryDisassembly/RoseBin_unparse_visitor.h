/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/

#ifndef __RoseBin_unparse_visitor_
#define __RoseBin_unparse_visitor_

#include <stdio.h>
#include <iostream>
// #include "rose.h"
// #include "RoseBin_support.h"

#include <sstream>
#include <iomanip>
#include <fstream>

class RoseBin_DataFlowAbstract;

class RoseBin_unparse_visitor : public AstSimpleProcessing { //ROSE_VisitorPattern {
 private:
  int previous_block;
  std::ofstream myfile;
  bool encode;
  RoseBin_DataFlowAbstract* analysis;

 public:

  static std::string unparseInstruction(SgAsmInstruction* binInst);

  RoseBin_unparse_visitor(){
    previous_block=-1; 
    encode=false;
    analysis=NULL;
  };

  /****************************************************
   * init the file
   ****************************************************/
  void init(char* filename);

  /****************************************************
   * close the file
   ****************************************************/
  void close();

  /****************************************************
   * unparse binary instruction
   ****************************************************/
  //void visit(SgAsmInstruction* binInstruction);
  virtual void visit(SgNode* n);


  /****************************************************
   * resolve binary expression, plus, minus, etc 
   ****************************************************/
  static std::string resolveBinaryBinaryExpression(SgAsmBinaryExpression* expr);

  /****************************************************
   * resolve Operand
   ****************************************************/
  static std::string resolveOperand(SgAsmExpression* expr, std::string *replace, bool unparseSignedConstants = false);
  std::string resolveOperand(SgAsmExpression* expr, RoseBin_DataFlowAbstract* dfa, bool unparseSignedConstants = false);

  /****************************************************
   * resolve expression
   ****************************************************/
  static std::string resolveRegister(SgAsmRegisterReferenceExpression::x86_register_enum code,
						  SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos);
  /****************************************************
   * resolve expression
   ****************************************************/
  static std::string resolveRegister(SgAsmRegisterReferenceExpression::arm_register_enum code,
						  SgAsmRegisterReferenceExpression::arm_position_in_register_enum pos);

  /****************************************************
   * resolve mnemonic
   ****************************************************/
  static std::string get_mnemonic_from_instruction(SgAsmExpression* expr);

};

#endif


