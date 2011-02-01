/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Interface to user
 ****************************************************/
#include "rose.h"
#include <sys/time.h>

using namespace std;
/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Interface to user
 ****************************************************/

//#ifndef __RoseBin__
//#define __RoseBin__
#include <mysql.h>
//#include <mysql_include/mysql.h>

#include <stdio.h>
#include <iostream>

#include "RoseBin.h"
#include "RoseBin_DB_IDAPRO.h"
#include "RoseBin_support.h"

/*
  class RoseBin : public AstSimpleProcessing {
  private:
  MYSQL_RES *res_set;
  // the DB connection
  RoseBin_DB_IDAPRO* idaDB; 
  MYSQL* conn;
  RoseBin_unparse* unparser;
  SgAsmNode* globalNode;
  std::list<std::string> functionNames;



  int trav_inst;
  int trav_blocks;
  int trav_funcs;
  int nodes;

  int num_func;
  int num_inst;


  void loadAST(std::string filename);
  void saveAST(std::string filename);


  public:

  RoseBin(char* host, 
  char* user, 
  char* passw, 
  char* dbase) {
  RoseBin_support::setDebugMode(true);    
  //RoseBin_support::setDebugMode(false);    
  idaDB = new RoseBin_DB_IDAPRO(host, user, passw, dbase);
  unparser = new RoseBin_unparse();
  RoseBin_support::setUnparseVisitor(unparser->getVisitor());
  res_set = 0;
  conn = 0;
  globalNode = 0;
  functionNames.clear();
  num_inst=0;
  num_func=0;
  RoseBin_support::setAssemblyLanguage(RoseBin_Def::none);
  }
  
  ~RoseBin() {
  delete idaDB;
  delete unparser;

  idaDB = NULL;
  unparser = NULL;

  if (globalNode)
  delete globalNode;
  }


  // allow filtering of functions
  void setFunctionFilter(std::list<std::string> functionName);

  void visit(SgNode* node);

  // connect to the DB
  void connect_DB(const char* socket);

  // query the DB to retrieve all data
  SgAsmNode* retrieve_DB_IDAPRO();

  // close the DB
  void close_DB();

  // unparse the AST to assembly
  void unparse(char* fileName);

  void test();
  };
*/
//#endif



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
void RoseBin::loadAST(std::string filename){
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
void RoseBin::saveAST(std::string filename) {
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
 * connect to the DB
 ****************************************************/
void RoseBin::connect_DB(const char* socket) {
  conn = idaDB->connect_DB(socket);
}

/****************************************************
 * allow functions from DB to be filtered to a certain set
 * e.g. to the set of source AST functions
 ****************************************************/
void RoseBin::setFunctionFilter(list<string> functionName) {
  functionNames = functionName;
}

inline double getTime() {
  timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1.e-6;
}


/****************************************************
 * retrieve and store all data in local data structures
 * and build the binary AST
 ****************************************************/
SgAsmNode* RoseBin::retrieve_DB_IDAPRO() {
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
  bool debug=false;
  memusage =  ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug)
    cerr << ">> processing comments.   " ;
  idaDB->process_comments_query(conn,res_set);
  ends = getTime();
  if (debug)  cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug)  cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  //    cerr << ">> processing callgraph." << endl;
  //idaDB->process_callgraph_query(conn,res_set);
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug)    cerr << ">> processing functions.  " ;
  idaDB->process_functions_query(conn,res_set,globalBlock, functionNames);
  ends = getTime();
  if (debug) cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  
  num_func = idaDB->get_numberOfFunctions();
  if (debug) cerr << " Nr of functions: " << ( num_func) << endl;

  // the order is important. First build blocks
  // then the branchgraph, since the branchgraph needs info
  // from blocks
  //  idaDB->process_basicblock_query(conn,res_set, globalBlock);
  //idaDB->process_branchgraph_query(conn,res_set);
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  cerr << ">> processing instructions.  " ;
  idaDB->process_instruction_query(conn,res_set);
  ends = getTime();
  if (debug)  cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  
  num_inst = idaDB->get_numberOfInstructions();
  if (debug) cerr << " Nr of instructions: " << ( num_inst) << endl;

  // preparation for expression resolution
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << ">> processing op_strings.  " ;
  idaDB->process_operand_strings_query(conn,res_set);
  ends = getTime();
  if (debug) cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug)  cerr << ">> processing expr_tree.  ";
  idaDB->process_expression_tree_query(conn,res_set);
  ends = getTime();
  if (debug) cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << ">> processing op_expr.  " ;
  idaDB->process_operand_expressions_query(conn,res_set);
  ends = getTime();
  if (debug) cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << ">> processing substitution.  ";
  idaDB->process_substitutions_query(conn,res_set);
  ends = getTime();
  if (debug) cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  // get the map (op_id, root)
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << ">> processing op_root.  ";
  idaDB->process_operand_root_query(conn,res_set);
  // resolve the expressions
  ends = getTime();
  if (debug) cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  


  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << ">> processing op_tuples.  ";
  idaDB->process_operand_tuples_query(conn,res_set);
  ends = getTime();
  if (debug) cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  // this is important to resolve the jump to address of instructions
  start = getTime();
  memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << ">> processing jumps.  " ;
  //  idaDB->process_jumps();
  ends = getTime();
  if (debug) cerr << " " << (double) (ends - start)   << " sec";
  memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  if (debug) cerr << "    Memory usage: " << (memusageend-memusage) << endl;  

  idaDB->cleanUpDBMemory();

  globalNode=globalBlock;
  // return the ROOT node, for visualization
  return globalBlock;
}

void RoseBin::test() {
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
void RoseBin::visit(SgNode* node) {
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
      cerr << " func with no global :: " << addrhex.str() << " " << address << endl; 
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
 * close the DB
 ****************************************************/
void RoseBin::close_DB() {
  //mysql_free_result(res_set);
  ROSE_ASSERT(conn);
  mysql_close(conn);
}


/****************************************************
 * unparse AST to assembly
 ****************************************************/
void RoseBin::unparse(char* fileName) { 

  //Language language = RoseBin_support::getAssemblyLanguage();
  if (RoseBin_Def::RoseAssemblyLanguage==RoseBin_Def::none) {
    std::cerr << " no assembly language specified. " << endl;
    std::cerr << " use RoseBin_support::setAssemblyLanguage(string) " << endl;
    std::cerr << " options : arm, x86 " << endl;
    abort();
  }

  ROSE_ASSERT(globalNode);
  unparseAsmStatementToFile(fileName, globalNode);
}
