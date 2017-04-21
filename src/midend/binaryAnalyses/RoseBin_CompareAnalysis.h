/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 6Jun07
 * Decription : Interface for analysis to compare
 *              Source Tree with Binary Tree
 ****************************************************/

#ifndef __RoseBin_CompareAnalysis__
#define __RoseBin_CompareAnalysis__

//#include <mysql/mysql.h>
#include <stdio.h>
#include <iostream>

#include "RoseBin_support.h"
#include "MyAstAttribute.h"

class RoseBin_CompareAnalysis : public AstSimpleProcessing{
 private:
#if 0 // [Robb P Matzke 2017-03-27]
  SgProject* project;
  SgAsmNode* globalBin;

  bool main_prolog_end;
#endif

  std::string attributeName;

  // stores the last variable that has been modified!
  // i.e. the variable has been on the left side of an 
  // assignment
  //  std::string last_left_source_var;


  typedef std::map<std::string, std::pair<std::string,
    SgAsmValueExpression* > > local_vars_type;
  local_vars_type local_vars;
 
  typedef std::map<std::string, std::pair<SgFunctionDeclaration*,
    SgAsmFunction*> > function_map_type;
  function_map_type function_map;

  std::map<std::string, SgAsmFunction*> bin_funcs;

  std::stack<SgExpression*> srcNodesTodo;
  
  // create an attribute
  MyAstAttribute* createAttribute(int val);

  // compare src statements and  bin instructions
  int match_statements(int array_src_length,
                       int array_bin_length,
                       SgNode* src_statements[],
                       SgAsmNode* bin_statements[],
                       std::string *output) ROSE_DEPRECATED("no longer supported");


#if 0 // [Robb P Matzke 2017-03-27]
  bool tookSrcNodeFromStack;
#endif

  std::string resolveRegister(const RegisterDescriptor &reg) ROSE_DEPRECATED("no longer supported");

#if 0 // [Robb P Matzke 2017-03-27]
  SgAsmValueExpression* valExp;     
  SgAsmRegisterReferenceExpression* refExp_Left; 
  SgAsmRegisterReferenceExpression* refExp_Right;
#endif

  /**********************************************************
   * check if function calls match ----------------------------------------------------------------------------
   * if we have a FgFunctionCallExp, then the binary instruction must be a binaryfunccall too
   *********************************************************/
  bool isFunctionCall(SgNode* srcNode,
                      SgAsmNode* binNode,
                      std::string *output,
                      int &nodes_matched) ROSE_DEPRECATED("no longer supported");

  bool isReturnStmt(SgNode* srcNode,
                    SgAsmNode* binNode,
                    std::string *output,
                    int &nodes_matched) ROSE_DEPRECATED("no longer supported");

  bool isVariableDeclaration(SgNode* srcNode,
                             SgAsmNode* binNode,
                             std::string *output,
                             int &nodes_matched,
                             int array_bin_length,
                             int &bin_count,
                             SgNode* src_statements[],
                             SgAsmNode* bin_statements[],
                             bool &increase_source) ROSE_DEPRECATED("no longer supported");

  bool isAssignOp(SgNode* srcNode,
                  SgAsmNode* binNode,
                  std::string *output,
                  int &nodes_matched,
                  int array_bin_length,
                  int array_src_length,
                  int &bin_count,
                  int src_count,
                  SgNode* src_statements[],
                  SgAsmNode* bin_statements[],
                  bool &increase_source) ROSE_DEPRECATED("no longer supported");

  bool isSgPlusPlus(SgNode* srcNode,
                    SgAsmNode* binNode,
                    std::string *output,
                    int &nodes_matched) ROSE_DEPRECATED("no longer supported");

  std::string checkVariable(SgAsmValueExpression* rhs) ROSE_DEPRECATED("no longer supported");
  void storeVariable(std::string val, std::string name,
                     SgAsmValueExpression* binval) ROSE_DEPRECATED("no longer supported");

  std::string getVariableName(std::string val) ROSE_DEPRECATED("no longer supported");
  SgAsmValueExpression* getVariableType(std::string val) ROSE_DEPRECATED("no longer supported");

  

  bool handleSourceExpression(SgExpression* expr,
                              SgNode* src_statements[],
                              SgAsmNode* bin_statements[],
                              std::string *output,
                              int &bin_count,
                              int src_count,
                              int &nodes_matched,
                              int array_bin_length,
                              int array_src_length,
                              SgAssignOp* assign,
                              bool dont_increase_bool_count) ROSE_DEPRECATED("no longer supported");

  SgExpression* isExpression( SgExpression* expr,
                              SgNode* src_statements[],
                              SgAsmNode* bin_statements[],
                              std::string *output,
                              int &bin_count,
                              int src_count,
                              int &nodes_matched,
                              int array_bin_length,
                              int array_src_length,
                              bool& keep_binary_node) ROSE_DEPRECATED("no longer supported");

  std::string last_src_variable_left;

  std::string resolve_binaryInstruction(SgAsmInstruction* mov,
                                        std::string *left,
                                        std::string *right,
                                        std::string varName) ROSE_DEPRECATED("no longer supported");

  bool existsVariable(std::string value) ROSE_DEPRECATED("no longer supported");

  bool isAnExpression(SgExpression* expr) ROSE_DEPRECATED("no longer supported");

  void pushOnStack(SgExpression* expr) ROSE_DEPRECATED("no longer supported");

  void resolve_bin_vardecl_or_assignment(bool &isVarDecl0, 
                                         bool &isVarDecl1,
                                         bool &isAssign0,
                                         bool &isAssign1,
                                         SgAsmX86Instruction* mov
                                         ) ROSE_DEPRECATED("no longer supported");

 public:

// DQ (10/20/2010): Moved to source file to support compilation of language only mode which excludes binary analysis support.
  RoseBin_CompareAnalysis(SgProject *pr, SgAsmNode* global) ROSE_DEPRECATED("no longer supported");

  // visit the binary AST
  void visit(SgNode* node) ROSE_DEPRECATED("no longer supported");

  // run this analysis
  void run() ROSE_DEPRECATED("no longer supported");


  //traverse Rose src and create map (name, src_func, null)
  void create_map_functions() ROSE_DEPRECATED("no longer supported");


  // compare a src function and a binary function
  void checkFunctions(std::string name,
                      SgFunctionDeclaration* funcDecl,
                      SgAsmFunction* binDecl) ROSE_DEPRECATED("no longer supported");

  // filter instructions that are known
  bool instruction_filter(SgAsmStatement* stat, std::string name,
                          std::string *output) ROSE_DEPRECATED("no longer supported");


};

#endif

