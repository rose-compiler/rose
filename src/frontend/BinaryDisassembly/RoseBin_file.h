/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : DB specific information to handle IDA
 ****************************************************/

#ifndef __RoseBin_file__
#define __RoseBin_file__

#include <stdio.h>
#include <iostream>
// #include "rose.h"
// #include "RoseBin_support.h"
#include "RoseBin_IDAPRO_substTree.h"
#include "RoseBin_IDAPRO_exprTree.h"
//#include "RoseBin_DB.h"
#include "RoseBin_IDAPRO_buildTree.h"
#include "RoseBin_IDAPRO_callGraph.h"
#include "RoseBin_IDAPRO_branchGraph.h"

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

class RoseBin_FILE  {
 private:
  RoseBin_IDAPRO_buildTree* buildROSE;
  //Sg_File_Info* this_file;
  std::string filename;

  // remember a map of comments
  rose_hash::unordered_map <int, std::string > rememberComments; // Instruction addr -> comment
  // remember the map of functions
  rose_hash::unordered_map <int, SgAsmFunctionDeclaration* > rememberFunctions; // Start of function -> decl
  // remember a map of blocks
  //std::map <int, SgAsmBlock* > rememberBlocks;
  // remember a map of instructions
  rose_hash::unordered_map <uint64_t, SgAsmInstruction* > rememberInstructions; // Insn address -> ROSE insn
  //typedef __gnu_debug::hash_map <uint64_t, SgAsmInstruction* > remInstType;
  //remInstType rememberInstructions; // Insn address -> ROSE insn
  // remember a map of all operand strings
  // std::map <int, std::string > rememberOperandStrings; // Operand ID -> string
  std::vector < std::string > rememberOperandStrings; // Operand ID -> string
  // remember a map of operand expressions
  // std::map <int, int > rememberExpressionTree_ROOT; // Operand ID -> expr tree ID
  std::vector < int > rememberExpressionTree_ROOT; // Operand ID -> expr tree ID
  // remember a map of expression trees
  // std::map <int, exprTreeType> rememberExpressionTree;
  std::vector <exprTreeType> rememberExpressionTree;
  // remember a map of operand expressions
  // std::multimap <int, int > rememberOperandExpressions; // Operand ID -> expr ID
  // std::vector < std::vector < int > > rememberOperandExpressions; // Operand ID -> set of expr ID
  // std::map <int, std::multimap<int,int> > rememberExpressionTree_ParentChild;
  std::vector < std::map<int, std::vector<int> > > rememberExpressionTree_ParentChild;
  // map of substitutions
  rose_hash::unordered_map <int, exprSubstitutionType > rememberSubstitution;
  // map of callgraph info , <address, callgraphType>
  rose_hash::unordered_map <int, CallGraphType > rememberCallGraph;
  // map of branchgraph info , <address of src_blockid, callgraphType>
  //  std::multimap <int, BranchGraphType > rememberBranchGraph;


  std::string getName(std::string name);

  // converts string to hex
  template <class T>
    bool from_string(T& t, 
                     const std::string& s, 
		     std::ios_base& (*f)(std::ios_base&))
    {
      std::istringstream iss(s);
      return !(iss >> f >> t).fail();
    }

  /****************************************************
   * resolve the control flow of the blocks
   * for each block, check what the next block is
   ****************************************************/
  //  void getControlFlow_Of_BasicBlocks(int block_id, 
  //				     int *nextBlockTrue_address, 
  //				     int *nextBlockFalse_address);





 public:
  RoseBin_FILE(std::string& f) {
  filename = f;
    //this_file = Sg_File_Info::generateDefaultFileInfo();
    buildROSE = new RoseBin_IDAPRO_buildTree();
 };

  ~RoseBin_FILE() {
    delete buildROSE;    
    buildROSE = NULL;

    //delete this_file;
    //this_file = NULL;
    /*
    std::map <int, SgAsmFunctionDeclaration* >::iterator it;
    for (it = rememberFunctions.begin(); 
         it!= rememberFunctions.end(); it++) {
      delete it->second;
    }
    std::map <int, SgAsmInstruction* >::iterator it3;
    for (it3 = rememberInstructions.begin(); 
         it3!= rememberInstructions.end(); it3++) {
      delete it3->second;
    }
    */

  }

  void cleanUpDBMemory() {
    rememberFunctions.clear();
    rememberInstructions.clear();
    rememberComments.clear();
    rememberOperandStrings.clear();
    rememberExpressionTree_ROOT.clear(); 
    rememberExpressionTree.clear(); 
    rememberExpressionTree_ParentChild.clear();
    rememberSubstitution.clear();
  }

  /****************************************************
   * resolve for each instruction which type it has
   ****************************************************/
  SgAsmInstruction* createInstruction(int address, 
                                      SgAsmFunctionDeclaration* bb, 
                                      std::string mnemonic);



  /****************************************************
   * for testing, how many instructions where created
   ****************************************************/
  int get_numberOfInstructions() {
    return rememberInstructions.size();
  }

  /****************************************************
   * for testing, how many blocks where created
   ****************************************************
  int get_numberOfBlocks() {
    return rememberBlocks.size();
  }
  */

  /****************************************************
   * for testing, how many functions where created
   ****************************************************/
  int get_numberOfFunctions() {
    return rememberFunctions.size();
  }


  /****************************************************
   * process all comments in the DB
   ****************************************************/
  void process_comments_query( );

  /****************************************************
   * process all functions in the DB
   ****************************************************/
  void process_functions_query(
                               SgAsmBlock* globalBlock,
			       std::list<std::string> functionName);

  /****************************************************
   * process all basic blocks in the DB
   * add the blocks to the functions
   ****************************************************
  void process_basicblock_query( ,
				SgAsmBlock* globalBlock);
  */

  /****************************************************
   * process all instructions in the DB
   * add the instructions to the blocks
   ****************************************************/
  void process_instruction_query( );

  /****************************************************
   * process operand strings. used in process_operand_tuples
   ****************************************************/
  void process_operand_strings_query( );

  /****************************************************
   * process operand expressions. used in process_operand_tuples
   ****************************************************/
  void process_operand_root_query( );

  /****************************************************
   * process expression tree. used in process_operand_tuples
   ****************************************************/
  void process_expression_tree_query( );

  /****************************************************
   * process operand expressions. used in process_operand_tuples
   ****************************************************/
  void process_operand_expressions_query( );

  /****************************************************
   * process substitutions. used for Navi
   ****************************************************/
  void process_substitutions_query( );

  /****************************************************
   * process operand tuples. 
   * Handles all expressions to be added to the instructions.
   ****************************************************/
  void process_operand_tuples_query( );

  /****************************************************
   * check the type of each operand
   ****************************************************/
  std::string resolveType(exprTreeType* expt);

  /****************************************************
   * process callgraph
   ****************************************************/
  void process_callgraph_query( );

  /****************************************************
   * process branchgraph
   ****************************************************/
  //  void process_branchgraph_query( );

};

#endif


