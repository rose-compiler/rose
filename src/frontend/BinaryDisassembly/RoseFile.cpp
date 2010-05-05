/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Interface to user
 ****************************************************/
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "AsmUnparser_compat.h"

#ifdef _MSC_VER
#include <time.h>
#include <windows.h>	// GetSystemTimeAsFileTime()
#else
// This header file is not available in MSVC.
#include <sys/time.h>
#endif

using namespace std;
/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Interface to user
 ****************************************************/

#include <stdio.h>
#include <iostream>

#include "RoseFile.h"
#include "RoseBin_file.h"
#include "RoseBin_support.h"




class UserDefinedAttribute : public AstAttribute
{
public:
  // Function interface for supporting File IO.
  // Required if user-defined attributes are to be reconstructed in File input.
  // But not defined as pure virtual functions in the AstAttribute base class.
  virtual string attribute_class_name() { return "UserDefinedAttribute"; }
  virtual UserDefinedAttribute* constructor() { return new UserDefinedAttribute(); }
  virtual char* packed() { return (char*) this; }
  virtual int packed_size() { return 0; }
  virtual void unpacked( char* ptr ) { }
};


/* ******************************************************
 * load the AST from file
 * ******************************************************/
void RoseFile::loadAST(std::string filename){
  /*
  std::cout << "ROSE: AST LOAD." << std::endl;
  double memusage = ROSE_MemoryUsage::getMemoryUsageMegabytes();

  REGISTER_ATTRIBUTE_FOR_FILE_IO(UserDefinedAttribute);
  AST_FILE_IO::clearAllMemoryPools();
  globalNode = (SgProject*)
    (AST_FILE_IO::readASTFromFile ( filename + ".binaryAST" ) );

  double memusageend = ROSE_MemoryUsage::getMemoryUsageMegabytes();
  cout << "Memory usage for ROSE PARSING: " << (memusageend-memusage) << " MB. "  << std::endl;

  std::cout << "ROSE: AST LOAD COMPLETE -------------------- " << endl;
  */
}


/* ******************************************************
 * save AST to file
 * ******************************************************/
void RoseFile::saveAST(std::string filename) {
  /*
  std::cout << "ROSE: AST SAVE .. project file: " << filename << std::endl;
  double memusage = ROSE_MemoryUsage::getMemoryUsageMegabytes();

  REGISTER_ATTRIBUTE_FOR_FILE_IO(UserDefinedAttribute);
  AST_FILE_IO::startUp( (SgProject*) globalNode ) ;
  std::cout << "ROSE: AST SAVE ... startup done." << std::endl;
  AST_FILE_IO::writeASTToFile ( filename + ".binaryAST" );
  std::cout << "ROSE: AST SAVE ... writeToFile done." << std::endl;
  double memusageend = ROSE_MemoryUsage::getMemoryUsageMegabytes();
  cout << "Memory usage for ROSE PARSING: " << (memusageend-memusage) << " MB. "  << std::endl;

  std::cout << "ROSE: AST SAVE COMPLETE -------------------- " << endl;
  */
}



/****************************************************
 * allow functions from DB to be filtered to a certain set
 * e.g. to the set of source AST functions
 ****************************************************/
void RoseFile::setFunctionFilter(list<string> functionName) {
  functionNames = functionName;
}

inline double getTime() {
  timeval tv;

#ifdef _MSC_VER
    // CH (4/16/2010): Use WinAPI to get the value wanted
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	__int64 tmpres = 0;
	tmpres |= ft.dwHighDateTime;
	tmpres <<= 32;
	tmpres |= ft.dwLowDateTime;
	tmpres -= 11644473600000000Ui64;
	tmpres /= 10;
	return double(tmpres) / 1000000;
//#pragma message ("WARNING: Linux gettimeofday() not available in MSVC.")
//  printf ("WARNING: Linux gettimeofday() not available in MSVC. \n");
//  tv.tv_sec  = 0;
//  tv.tv_usec = 0;
#else
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1.e-6;
#endif
}


/****************************************************
 * retrieve and store all data in local data structures
 * and build the binary AST
 ****************************************************/
SgAsmNode* RoseFile::retrieve_DB() {
  // -----------------------------------------------------------------------
  double start = getTime();
  double ends = getTime();
  double memusage, memusageend =0;

  //RoseBin_DB_IDAPRO* idaDB = new RoseBin_DB_IDAPRO();
 
  // get all the necessary information from the DB 
  // and create DB
  SgAsmBlock* globalBlock = new SgAsmBlock();
  ROSE_ASSERT(globalBlock);
  start = getTime();
  memusage =  ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing comments.   " ;
  idaDB->process_comments_query( );
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  //    cerr << ">> processing callgraph." << endl;
  //idaDB->process_callgraph_query( );
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing functions.  " ;
  idaDB->process_functions_query( globalBlock, functionNames);
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  
  num_func = idaDB->get_numberOfFunctions();
  //cerr << " Nr of functions: " << ( num_func) << endl;

  // the order is important. First build blocks
  // then the branchgraph, since the branchgraph needs info
  // from blocks
  //  idaDB->process_basicblock_query( globalBlock);
  //idaDB->process_branchgraph_query( );
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing instructions.  " ;
  idaDB->process_instruction_query( );
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  
  num_inst = idaDB->get_numberOfInstructions();
  //cerr << " Nr of instructions: " << ( num_inst) << endl;

  // preparation for expression resolution
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing op_strings.  " ;
  idaDB->process_operand_strings_query( );
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing expr_tree.  ";
  idaDB->process_expression_tree_query( );
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing op_expr.  " ;
  idaDB->process_operand_expressions_query( );
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing substitution.  ";
  idaDB->process_substitutions_query( );
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  // get the map (op_id, root)
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing op_root.  ";
  idaDB->process_operand_root_query( );
  // resolve the expressions
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  


  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing op_tuples.  ";
  idaDB->process_operand_tuples_query( );
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  // this is important to resolve the jump to address of instructions
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> processing jumps.  " ;
  //  idaDB->process_jumps();
  ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  idaDB->cleanUpDBMemory();

  globalNode=globalBlock;
  // return the ROOT node, for visualization
  return globalBlock;
}

void RoseFile::test() {
 // run the consistency test -----------------------------------------------------
  trav_funcs=0;
  trav_inst=0;
  trav_blocks=0;
  nodes=0;
  double start = getTime();
  double memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << ">> traversing AST...  " ;
  traverse(globalNode,preorder);
  double ends = getTime();
  //cerr << " " << (double) (ends - start)   << " sec";
  double memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  //cerr << "    Memory usage: " << (memusageend-memusage) << endl;  
  
  //cerr << " traversed nodes : " << nodes << endl;
  //cerr << " num_func / trav_funcs  : " << num_func << "/ " << trav_funcs << endl;
  //cerr << " num_inst  / trav_inst   : " << num_inst << "/ " << trav_inst << endl;

  //ROSE_ASSERT(trav_funcs==num_func);
  //ROSE_ASSERT(trav_blocks==num_blocks);
  //ROSE_ASSERT(trav_inst==num_inst);
  // -------------------------------------------------------------------------------
 
}

/****************************************************
 * traverse the binary AST to test if all nodes are there
 ****************************************************/
void RoseFile::visit(SgNode* node) {
  SgAsmFunctionDeclaration* funcDecl= isSgAsmFunctionDeclaration(node);
  SgAsmInstruction* instr= isSgAsmInstruction(node);
  //SgAsmBlock* block= isSgAsmBlock(node);
  nodes++;
  //cerr << " traversing node " << binNode << endl;
  if (funcDecl) { 
    trav_funcs++;
    // should have a parent
    SgAsmBlock* glob = isSgAsmBlock(funcDecl->get_parent());
    if (glob==NULL) {
      int address = funcDecl->get_address();
      ostringstream addrhex;
      addrhex << hex << setw(8) << address ;
      //cerr << " func with no global :: " << addrhex.str() << " " << address << endl; 
    }
    //    ROSE_ASSERT(glob);
  }
  /*
  if (block) {
    trav_blocks++;
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(block->get_parent());
    int address = block->get_address();
    ostringstream addrhex;
    addrhex << hex << setw(8) << address ;
    //if (func==NULL) {
    //cerr << trav_blocks << " block with no function :: " << addrhex.str() << " " << address << endl; 
    //} else 
    //cerr << trav_blocks << " block with no function :: " << addrhex.str() << " " << address << endl; 
    //ROSE_ASSERT(func);
  }
  */
  if (isSgAsmMemoryReferenceExpression(node)) {
    SgAsmMemoryReferenceExpression* n = isSgAsmMemoryReferenceExpression(node);
    // cerr << "Found a SgAsmMemoryReferenceExpression" << endl;
    ROSE_ASSERT(n->get_type());
  }
  if (instr) {
    trav_inst++;
    SgAsmFunctionDeclaration* ins = isSgAsmFunctionDeclaration(instr->get_parent());
    if (ins==NULL) {
      int address = ins->get_address();
      ostringstream addrhex;
      addrhex << hex << setw(8) << address ;
      cerr << " ERROR :: instr with no parent function :: " << addrhex.str() << " " << address << endl; 
    }
    //ROSE_ASSERT(ins);
  }
  SgAsmNode* asmNode = isSgAsmNode(node);
  if (asmNode)
    if (asmNode->get_parent()==NULL) {
      if (!isSgAsmBlock(asmNode)) {
	cerr << " PARENT == NULL :: " << asmNode->class_name() << endl;
	ROSE_ASSERT(asmNode->get_parent());
      }
    }
}




/****************************************************
 * unparse AST to assembly
 ****************************************************/
void RoseFile::unparse(char* fileName) { 
  ROSE_ASSERT(globalNode != NULL);

// DQ (8/23/2008): Modified to use more precise interface taking "SgAsmStatement*" instead of "SgAsmNode*"
  SgAsmStatement* asmStatement = isSgAsmStatement(globalNode);
  ROSE_ASSERT(asmStatement);

  unparseAsmStatementToFile(fileName, asmStatement);
}
