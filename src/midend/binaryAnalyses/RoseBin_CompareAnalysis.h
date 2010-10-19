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
  SgProject* project;
  SgAsmNode* globalBin;

  bool main_prolog_end;
  std::string attributeName;

  // stores the last variable that has been modified!
  // i.e. the variable has been on the left side of an 
  // assignment
  //  std::string last_left_source_var;


  typedef std::map<std::string, std::pair<std::string,
    SgAsmValueExpression* > > local_vars_type;
  local_vars_type local_vars;
 
  typedef std::map<std::string, std::pair<SgFunctionDeclaration*,
    SgAsmFunctionDeclaration*> > function_map_type;
  function_map_type function_map;

  std::map<std::string, SgAsmFunctionDeclaration*> bin_funcs;

  std::stack<SgExpression*> srcNodesTodo;
  
  // create an attribute
  MyAstAttribute* createAttribute(int val);

  // compare src statements and  bin instructions
  int match_statements(int array_src_length,
                       int array_bin_length,
		       SgNode* src_statements[],
		       SgAsmNode* bin_statements[],
		       std::string *output);


  bool tookSrcNodeFromStack;

  std::string resolveRegister(const RegisterDescriptor &reg);

  SgAsmValueExpression* valExp;     
  SgAsmx86RegisterReferenceExpression* refExp_Left; 
  SgAsmx86RegisterReferenceExpression* refExp_Right; 

  /**********************************************************
   * check if function calls match ----------------------------------------------------------------------------
   * if we have a FgFunctionCallExp, then the binary instruction must be a binaryfunccall too
   *********************************************************/
  bool isFunctionCall(SgNode* srcNode,
		      SgAsmNode* binNode,
		      std::string *output,
		      int &nodes_matched);

  bool isReturnStmt(SgNode* srcNode,
		    SgAsmNode* binNode,
		    std::string *output,
		    int &nodes_matched);

  bool isVariableDeclaration(SgNode* srcNode,
                             SgAsmNode* binNode,
			     std::string *output,
			     int &nodes_matched,
			     int array_bin_length,
			     int &bin_count,
			     SgNode* src_statements[],
			     SgAsmNode* bin_statements[],
			     bool &increase_source);

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
		  bool &increase_source);		  

  bool isSgPlusPlus(SgNode* srcNode,
		    SgAsmNode* binNode,
		    std::string *output,
		    int &nodes_matched);

  std::string checkVariable(SgAsmValueExpression* rhs);
  void storeVariable(std::string val, std::string name,
                     SgAsmValueExpression* binval);

  std::string getVariableName(std::string val);
  SgAsmValueExpression* getVariableType(std::string val);

  

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
			      bool dont_increase_bool_count);

  SgExpression* isExpression( SgExpression* expr,
			      SgNode* src_statements[],
			      SgAsmNode* bin_statements[],
			      std::string *output,
			      int &bin_count,
			      int src_count,
			      int &nodes_matched,
			      int array_bin_length,
			      int array_src_length,
			      bool& keep_binary_node);

  std::string last_src_variable_left;

  std::string resolve_binaryInstruction(SgAsmInstruction* mov,
					std::string *left,
					std::string *right,
					std::string varName);

  bool existsVariable(std::string value);

  bool isAnExpression(SgExpression* expr);

  void pushOnStack(SgExpression* expr);

  void resolve_bin_vardecl_or_assignment(bool &isVarDecl0, 
                                         bool &isVarDecl1,
					 bool &isAssign0,
					 bool &isAssign1,
					 SgAsmx86Instruction* mov
					 );

 public:

  RoseBin_CompareAnalysis(SgProject *pr, SgAsmNode* global) {
    RoseBin_support::setDebugMode(false);    
    project = pr;
    globalBin = global;
    main_prolog_end=false;
    attributeName = "rosebin_color";
  }

  // visit the binary AST
  void visit(SgNode* node);

  // run this analysis
  void run();


  //traverse Rose src and create map (name, src_func, null)
  void create_map_functions();


  // compare a src function and a binary function
  void checkFunctions(std::string name,
		      SgFunctionDeclaration* funcDecl,
		      SgAsmFunctionDeclaration* binDecl);

  // filter instructions that are known
  bool instruction_filter(SgAsmStatement* stat, std::string name,
			  std::string *output);


};

#endif

