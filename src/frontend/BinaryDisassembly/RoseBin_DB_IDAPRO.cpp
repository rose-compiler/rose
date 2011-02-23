/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that handles the IDAPRO DB
 ****************************************************/
#include "rose.h"
#include "RoseBin_DB_IDAPRO.h"
#include <errno.h>
#include <mysql.h>
//#include "mysql_include/mysql.h"

using namespace std;

/****************************************************
 * resolve the control flow of the blocks
 * for each block, check what the next block is
 ***************************************************
void RoseBin_DB_IDAPRO::getControlFlow_Of_BasicBlocks(int block_id, 
                                                      int *nextBlockTrue_address, 
                                                      int *nextBlockFalse_address) {
  multimap<int, BranchGraphType>::iterator it = rememberBranchGraph.find(block_id);
  for (it; it!=rememberBranchGraph.end(); ++it) {
    int src_block_id = it->first;
    if (block_id==src_block_id) {
      BranchGraphType branchGraph = it->second;
      int dst_id = branchGraph.dst;
      SgAsmBlock* destBlock = rememberBlocks[dst_id];
      int dst_address = destBlock->get_address();
      //dst_address=dst_id;
      int kind = branchGraph.kind;
      //cerr << " src_id " << src_block_id <<
      //         " kind : " << kind << " dst_id: " << dst_id <<
      //         " dst_addr: " << dst_address << endl;

      if (kind==0) {
      // todo: I assume that 0 means true
      *nextBlockTrue_address=dst_address;
      } else if (kind==1) {
      // todo: I assume that 1 means false
      *nextBlockFalse_address=dst_address;
      } else if (kind==2) {
      // todo: I assume that 2 means unconditional
      // true means also unconditional, since false will be 0
      *nextBlockTrue_address=dst_address;
      }
    }
  }

}
*/

/****************************************************
 * process branchgraph
 ****************************************************
void RoseBin_DB_IDAPRO::process_branchgraph_query(MYSQL* conn, MYSQL_RES* res_set) {
  rememberBranchGraph.clear();
  // get the functions
  char* q = (char*)"SELECT * FROM control_flow_graph_1";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    int operand_id=-1;
    int expr_id=-1;
    
    BranchGraphType branchGraph;
    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) { 
          ret = (char*)"<NULL>";
          if (i==0) branchGraph.id = -1;
          if (i==1) branchGraph.parent_function = -1;
          if (i==2) branchGraph.src = -1;
          if (i==3) branchGraph.dst = -1;
          if (i==4) branchGraph.kind = -1;
        } else {
          ret= row[i];
          if (i==0) branchGraph.id = atoi(ret);
          if (i==1) branchGraph.parent_function = atoi(ret);
          if (i==2) branchGraph.src = atoi(ret);
          if (i==3) branchGraph.dst = atoi(ret);
          if (i==4) branchGraph.kind = atoi(ret);
        }
      } // for
      if (RoseBin_support::DEBUG_MODE())
        cout << ">> creating branchgraph : src: " << branchGraph.src << " - " << branchGraph.dst << endl;
        // remembering for each basic block, the block that is being called from here
        // must be a multimap
      rememberBranchGraph.insert(make_pair(branchGraph.src, branchGraph));
    } // while

    // need some additional task:
    // since we have all basic blocks now, we would like to add information
    // of their control flow saved in BranchGraphType
    map<int, SgAsmBlock*>::iterator it= rememberBlocks.begin();
    for (it; it!=rememberBlocks.end(); ++it) {
      int block_id = it->first;
      SgAsmBlock* block = it->second;
      int nextBlockTrue_address = 0;
      int nextBlockFalse_address = 0;
      getControlFlow_Of_BasicBlocks(block_id, &nextBlockTrue_address, &nextBlockFalse_address);
      block->set_next_block_true_address(nextBlockTrue_address);
      block->set_next_block_false_address(nextBlockFalse_address);
    }


  } // if (res_set==NULL)
  checkError(conn,res_set);
}
*/

/****************************************************
 * process callgraph
 ****************************************************/
void RoseBin_DB_IDAPRO::process_callgraph_query(MYSQL* conn, MYSQL_RES* res_set) {
  rememberCallGraph.clear();
  // get the functions
  char* q = (char*)"SELECT * FROM callgraph_1";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    //int operand_id=-1;
    //int expr_id=-1;
    
    CallGraphType callGraph;

    //vector<struct>* vector = andreas->getVector("Callgraph"); 
    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) { 
          ret = (char*)"<NULL>";
          if (i==0) callGraph.id = -1;
          if (i==1) callGraph.src = -1;
          if (i==2) callGraph.src_basic_block_id = -1;
          if (i==3) callGraph.src_address = -1;
          if (i==4) callGraph.dst = -1;
        } else {
          ret= row[i];
          if (i==0) callGraph.id = atoi(ret);
          if (i==1) callGraph.src = atoi(ret);
          if (i==2) callGraph.src_basic_block_id = atoi(ret);
          if (i==3) callGraph.src_address = atoi(ret);
          if (i==4) callGraph.dst = atoi(ret);
        }
      } // for
      if (RoseBin_support::DEBUG_MODE())
        cout << ">> creating callgraph : " << callGraph.id << " - " << callGraph.src_address << endl;

      rememberCallGraph[callGraph.src_address] = callGraph;
      
    } // while
  } // if (res_set==NULL)
  checkError(conn,res_set);
}


std::string 
RoseBin_DB_IDAPRO::getName(string name) {
  int i=-1;
  while( name.find(".") != string::npos) {
    i =  name.find(".");
    name.replace(i,i+1,"_");
  }
  return name;
}



/****************************************************
 * process all functions in the DB
 ****************************************************/
void RoseBin_DB_IDAPRO::process_functions_query(MYSQL* conn, MYSQL_RES* res_set,
                                                SgAsmBlock* globalBlock,
                                                list<string> functionFilter) {
  // get the functions
  rememberFunctions.clear();
  //int counter=0;

  char* q = (char*)"SELECT * FROM functions_1 order by address";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    
    MYSQL_ROW row;
    long address=0;
    string name = (char*)"";
    int type=-1;
    string name_md5 =(char*)"";

    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) {
          ret = (char*)"<NULL>";
          if (i==0) address = -1;
          if (i==1) name = ret;
          if (i==2) type= -1;
          if (i==3) name_md5= ret;
        } else {
          ret= row[i];
          if (i==0) address = atoi(ret);
          if (i==1) name = ret;
          if (i==2) type= atoi(ret);
          if (i==3) name_md5= ret;
        }
      }

      // demangling *****************************************************
      // we need to demangle this function name, so
      // we can correspond it with the source name
      char *newName;// = new char[size*2];
      int status;
      size_t length;
      newName = abi::__cxa_demangle(name.c_str(), 0, 
                                    &length,&status);

      //      cerr << "converting name : " << name <<  "   status: " << status <<endl;

      if (status==0) {
        name =(char*)"";
        bool found_bracket=false;
        for (unsigned int i=0; i<length;i++) {
          if (newName[i]=='(')
            found_bracket=true;
          if (found_bracket==false)
            name += newName[i];
        }
        if (RoseBin_support::DEBUG_MODE())
          cout << " demangling name to " << name << endl;
      }
      name = getName(name);
      RoseBin_support::checkText(name);
      //cerr << " checking for function : " << name << endl;

      // demangling *****************************************************

      // filter : we filter away all kind of functions that are not
      // relevant == slicing of binaries, relevant = source AST functions
      // if (functionFilter.size()>0) {
      // list<string>::iterator filterMe = functionFilter.begin();
      // bool skip=true;
      if (!functionFilter.empty() && std::find(functionFilter.begin(), functionFilter.end(), name) == functionFilter.end()) {
        continue;
      }
#if 0
      for (filterMe;filterMe!=functionFilter.end();++filterMe) {
        string name_list = *filterMe;
        //          cerr << " comparing strings : " << name_list << " and " << name << endl;
        if (name_list==name) {
          
          //  if (name.find(name_list)!=string::npos)
          skip=false;
        }
      }
      if (skip)
        continue;
#endif
      // }
      //make sure we dont get too many nodes, so the visualization works for testing
      // find me : counter
      //counter++;
      //if (counter>50)
      //        break;
      // --------------------------------------------------------------------------
      if (RoseBin_support::DEBUG_MODE()) {
        ostringstream addrhex;
        addrhex << hex << setw(8) << address ;
        cout << ">> creating function : " << address << " " << addrhex.str() << " - " << name << " - " << type << endl;
      }
      SgAsmFunctionDeclaration* fd = new SgAsmFunctionDeclaration();
      fd->set_address(address);
      fd->set_name(name);
      globalBlock->append_statement(fd);
      fd->set_parent(globalBlock);

      // added a block between function and instruction to conform to jeremiahs code
      //SgAsmBlock* block = new SgAsmBlock();
      //fd->append_statement(block);
      //block->set_parent(fd);

      rememberFunctions[address]= fd ;
    } // while
  } // if (res_set==NULL)
  checkError(conn,res_set);
}

/****************************************************
 * process all comments in the DB
 ****************************************************/
void RoseBin_DB_IDAPRO::process_comments_query(MYSQL* conn, MYSQL_RES* res_set) {
  // get the functions
  rememberComments.clear();

  char* q = (char*)"SELECT * FROM address_comments_1";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    long address=0;
    string comment=(char*)"";

    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) {
          ret = (char*)"<NULL>";
          if (i==0) address = -1;
          if (i==1) comment = (char*)"";
        } else { 
          ret= row[i];
          if (i==0) address = atoi(ret);
          if (i==1) comment = ret;
        }
      }
      RoseBin_support::checkText(comment);
      if (RoseBin_support::DEBUG_MODE())
        cout << ">> creating comments : " << address << " - " << comment << endl;
      rememberComments[address]= comment ;

    } // while
  } // if (res_set==NULL)

  checkError(conn,res_set);
}


/****************************************************
 * process all basic blocks in the DB
 * add the blocks to the functions
 ****************************************************
void RoseBin_DB_IDAPRO::process_basicblock_query(MYSQL* conn, MYSQL_RES* res_set, SgAsmBlock* globalBlock) {
// get the functions
  rememberBlocks.clear();

  char* q = (char*)"SELECT * FROM basic_blocks_1";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    long address=0;
    int id=-1;
    int parent_function =0;

    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) {
          ret = (char*)"<NULL>";
          if (i==0) id = -1;
          if (i==2) address = -1;
          if (i==1) parent_function = -1;
        } else { 
          ret= row[i];
          if (i==0) id = atoi(ret);
          if (i==2) address = atoi(ret);
          if (i==1) parent_function = atoi(ret);
        }
      }

      ostringstream addrhex;
      addrhex << hex << setw(8) << address ;
      ostringstream addrhex_parent;
      addrhex_parent << hex << setw(8) << parent_function ;

      if (RoseBin_support::DEBUG_MODE()) {
        cerr << ">> creating basic_block : " << id << " - " << address << " " << addrhex.str() << " - " << parent_function << endl;
      }


      SgAsmBlock* bb = new SgAsmBlock(address);
      bb->set_id(id);
      rememberBlocks[id]= bb ;

      bool foundFunction = false;
      // get function and append this instruction
      map <int, SgAsmFunctionDeclaration* >::iterator funcIt;      
      for (funcIt=rememberFunctions.begin();funcIt!=rememberFunctions.end();++funcIt) {
        int addressFunc = funcIt->first;
        ostringstream addrhex_func;
        addrhex_func << hex << setw(8) << addressFunc ;
        //cerr << " checking for parent ... " << addressFunc << " " << parent_function << endl;
        if (addressFunc == parent_function) {
        // found the right basic_block
          foundFunction=true;
          SgAsmFunctionDeclaration* remFunc = funcIt->second;
          remFunc->append_block(bb);
          bb->set_parent(remFunc);
          //if (RoseBin_support::DEBUG_MODE()) 
          //cerr << ">>>>>>> appended function declaration " << remFunc->get_name() << "to  binary block: " << id << endl;
          break;
        }
      }
      if (!foundFunction)   {
      // in this case there is no matching function in the DB that this block points to via parent_function
      // what we do is to connect the block to the global instead
        cerr << " !! cant append block to function!! " << id << " " << address << " " << addrhex.str() << endl;
        //cerr << ">> creating basic_block : " << id << " - " << address << " " << addrhex.str() << 
        //  " " << addrhex_p.str() << " - " << parent_function << endl;
        
        //bb->set_parent(globalBlock);
        //globalBlock->append_statement(bb);

        // problem:
        // if you comment in the above 2 statements, the missing nodes will appear, but not in the correct place
        // hence the python scripts must be fixed before all instructions are visible (at the right function)
      }

    } // while
  } // if (res_set==NULL)

  

  checkError(conn,res_set);
}
*/

/****************************************************
 * resolve for each instruction which type it has
 ****************************************************/
SgAsmInstruction* RoseBin_DB_IDAPRO::createInstruction(int address, SgAsmFunctionDeclaration* bb, 
                                                          string mnemonic) {
  SgAsmInstruction* instruction=NULL;

  // *************************************************************
  // DO NOT CHANGE ... THIS IS GENERATED CODE - BELOW
  // *************************************************************
  
  if (RoseBin_Def::RoseAssemblyLanguage==RoseBin_Def::x86) {
    return createx86Instruction(address, mnemonic);
  } else  if (RoseBin_Def::RoseAssemblyLanguage==RoseBin_Def::arm) {
    return createArmInstruction(address, mnemonic);
  } else {
    cerr << " no assembly language selected! " << endl;
    abort();
  }

  // *************************************************************
  // DO NOT CHANGE ... THIS IS GENERATED CODE - ABOVE
  // *************************************************************

  if (instruction)
    instruction->set_parent(bb);
  ROSE_ASSERT(instruction);
  return instruction; 
}

/****************************************************
 * process all instructions in the DB
 * add the instructions to the blocks
 ****************************************************/
void RoseBin_DB_IDAPRO::process_instruction_query(MYSQL* conn, MYSQL_RES* res_set) {
  rememberInstructions.clear();
  // get the functions
  //  char* q = (char*)"SELECT * FROM instructions_1";
  char *q = (char*)"select *,     (select parent_function from basic_blocks_1 where id = i.basic_block_id      and (i.address - parent_function) >= 0     and (i.address - parent_function) =     (select min(i.address - parent_function) from basic_blocks_1 where id = i.basic_block_id       and (i.address - parent_function) >= 0)     ) as i_f from instructions_1 i order by i.address"; 

  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    
    MYSQL_ROW row;
    string mnemonic=(char*)"";
    uint64_t address=0;
    int basic_block=-1;
    int sequence =-1;
    string data=(char*)"";
    int i_func;

    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) { 
          ret = (char*)"<NULL>";
          if (i==0) address = -1;
          if (i==1) basic_block = -1;
          if (i==2) mnemonic = ret;
          if (i==3) sequence = -1;
          if (i==4) data=ret;
          if (i==5) i_func= -1;
        } else {
          ret= row[i];
          if (i==0) address = atoi(ret);
          if (i==1) basic_block = atoi(ret);
          if (i==2) mnemonic = ret;
          if (i==3) sequence = atoi(ret);
          if (i==4) data=ret;
          if (i==5) i_func = atoi(ret);
        }
      }
      // patched to adjust to objdump , Apr 26 2007
      if (mnemonic ==(char*)"retn")
        mnemonic = (char*)"ret";
      
      if (RoseBin_support::DEBUG_MODE()) {
        ostringstream addrhex;
        addrhex << hex << setw(8) << address ;
        cout << ">> creating instruction : " << addrhex.str() << " " << address << 
          " - " << basic_block << " - " << mnemonic << " - " << sequence << endl;
      }
      // check if it is an instruction or if it appears in the callgraph,
      // if it is in the callgraph, one wants to create a BinaryCall instead

      // append the instruction to its function
      rose_hash::unordered_map <int, SgAsmFunctionDeclaration* >::iterator func_it = rememberFunctions.find(i_func);
      SgAsmFunctionDeclaration* func = NULL;
      // for (func_it; func_it!=rememberFunctions.end(); ++func_it) {
      if (func_it != rememberFunctions.end()) {
        func = func_it->second;
      } else {
        if (i_func!=-1)
        cerr << " ERROR : cant find the function i_func : " << i_func << " in rememberFunctions for instruction : " << mnemonic << endl;
      }

      
      SgAsmInstruction* instruction = NULL;
      instruction = createInstruction(address, func, mnemonic);
      //        instruction = new SgAsmInstruction(address,bb,mnemonic,"");
      // Sep 29, tps : commented the following line out, since the function was removed.
      //instruction->set_raw_bytes(data);

      ROSE_ASSERT(instruction);

      SgAsmOperandList* operandList = new SgAsmOperandList();
      instruction->set_operandList(operandList);
      operandList->set_parent(instruction);

      ostringstream hexaddr;
      hexaddr << hex << setw(8) << address ;
      if (RoseBin_support::DEBUG_MODE())
        cout << " .rememberInstruction " << instruction->class_name() 
             << "  at : " << address << " hex: " << hexaddr.str() << endl;
      rememberInstructions[address]= instruction ;


      if (func) {
        // get the block in the func and append to it to conform to jeremiah
        func->append_statement(instruction);
        instruction->set_parent(func);
        //vector <SgNode*> blockVec =func->get_traversalSuccessorContainer();
        //SgAsmBlock* block = isSgAsmBlock(blockVec[0]);
        //ROSE_ASSERT(block);
        //block->append_statement(instruction);
        //instruction->set_parent(block);

        ROSE_ASSERT(instruction->get_parent());

        //SgAsmNode* nInst = (SgAsmNode*) instruction;
        //nInst->set_parent(func);

        ostringstream addrhex;
        addrhex << hex << setw(8) << i_func ;
        if (RoseBin_support::DEBUG_MODE())
          cout << ">> appended instruction to function: " << func->get_name() << " addr " << addrhex.str() << " " << address << endl;
      } else {
        if (i_func!=-1) {
          cerr << " ERROR :: could not append instruction to function : " << endl;
          //exit(0);
        }
      }
      
    } // while

  } // if (res_set==NULL)
  checkError(conn,res_set);
}


/****************************************************
 * process operand strings. used in process_operand_tuples
 ****************************************************/
void RoseBin_DB_IDAPRO::process_operand_strings_query(MYSQL* conn, MYSQL_RES* res_set) {
  rememberOperandStrings.clear();
  // get the functions
  char* q = (char*)"SELECT * FROM operand_strings_1";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    int id=-1;
    string str=(char*)"";

    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) { 
          ret = (char*)"<NULL>";
          if (i==0) id = -1;
          if (i==1) str = ret;
        } else {
          ret= row[i];
          if (i==0) id = atoi(ret);
          if (i==1) str = ret;
        }
      }
      RoseBin_support::checkText(str);
      if (RoseBin_support::DEBUG_MODE())
        cout << ">> creating operand_strings : " << id << " - " << str << endl;
      if (id >= (int)rememberOperandStrings.size()) rememberOperandStrings.resize(id + 1);
      rememberOperandStrings[id] = str;      
    } // while
  } // if (res_set==NULL)
  checkError(conn,res_set);
}


/****************************************************
 * process operand expressions. used in process_operand_tuples
 ****************************************************/
void RoseBin_DB_IDAPRO::process_operand_root_query(MYSQL* conn, MYSQL_RES* res_set) {
  rememberExpressionTree_ROOT.clear();
  // get the functions
  // char* q = (char*)"select * from operand_expressions_1 where expr_id in (select id from expression_tree_1 where parent_id is NULL)";
  char* q = (char*)"select operand_id, expr_id from operand_expressions_1 as oe, expression_tree_1 as t where oe.expr_id = t.id and t.parent_id is NULL";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    int operand_id=-1;
    int expr_id=-1;

    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) { 
          ret = (char*)"<NULL>";
          if (i==0) operand_id = -1;
          if (i==1) expr_id = -1;
        } else {
          ret= row[i];
          if (i==0) operand_id = atoi(ret);
          if (i==1) expr_id = atoi(ret);
        }
      }
      if (RoseBin_support::DEBUG_MODE())
        cout << ">> creating operand_roots : " << operand_id << " - " << expr_id << endl;
      if (operand_id >= (int) rememberExpressionTree_ROOT.size())
        rememberExpressionTree_ROOT.resize(operand_id + 1);
      rememberExpressionTree_ROOT[operand_id] = expr_id;      
    } // while
  } // if (res_set==NULL)
  checkError(conn,res_set);
}


/****************************************************
 * process expression tree. used in process_operand_tuples
 ****************************************************/
void RoseBin_DB_IDAPRO::process_expression_tree_query(MYSQL* conn, MYSQL_RES* res_set) {
  rememberExpressionTree.clear();
  // get the functions
  char* q = (char*)"SELECT * FROM expression_tree_1";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    exprTreeType exprTree;
    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) {
          ret = (char*)"<NULL>";
          if (i==0) exprTree.id = -1;
          if (i==1) exprTree.expr_type = -1;
          if (i==2) exprTree.symbol = ret;
          if (i==3) exprTree.immediate = -1;
          if (i==4) exprTree.position = -1;
          if (i==5) exprTree.parent_id = -1;  
        } else {
          ret= row[i];
          if (i==0) exprTree.id = atoi(ret);
          if (i==1) exprTree.expr_type = atoi(ret);
          if (i==2) exprTree.symbol = ret;
          if (i==3) {
            errno = 0;
            exprTree.immediate = strtol(ret, NULL, 10);
            if (errno != 0) {
              errno = 0;
              exprTree.immediate = strtoul(ret, NULL, 10);
              ROSE_ASSERT (errno == 0);
            }
            if (RoseBin_support::DEBUG_MODE())
              cout << "immediate = (char*)" << exprTree.immediate << endl;
          }
          if (i==4) exprTree.position = atoi(ret);
          if (i==5) exprTree.parent_id = atoi(ret);
        }
      } // for
      if (RoseBin_support::DEBUG_MODE())
        cout << ">> creating exprTree : " << exprTree.id << " - " << exprTree.expr_type << " - " 
             << exprTree.symbol << " - " << exprTree.immediate << " - " << exprTree.position 
             << " - " << exprTree.parent_id << endl;

      if (exprTree.id >= (int)rememberExpressionTree.size())
        rememberExpressionTree.resize(exprTree.id + 1);
      rememberExpressionTree[exprTree.id] = exprTree;


    } // while
  } // if (res_set==NULL)
  checkError(conn,res_set);
}

/****************************************************
 * process operand expressions. used in process_operand_tuples
 ****************************************************/
void RoseBin_DB_IDAPRO::process_operand_expressions_query(MYSQL* conn, MYSQL_RES* res_set) {
  // rememberOperandExpressions.clear();
  rememberExpressionTree_ParentChild.clear();
  // get the functions
  char* q = (char*)"SELECT * FROM operand_expressions_1 order by operand_id desc";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    int operand_id=-1;
    int expr_id=-1;

    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) { 
          ret = (char*)"<NULL>";
          if (i==0) operand_id = -1;
          if (i==1) expr_id = -1;
        } else {
          ret= row[i];
          if (i==0) operand_id = atoi(ret);
          if (i==1) expr_id = atoi(ret);
        }
      }
      if (RoseBin_support::DEBUG_MODE())
        cout << ">> creating operand_expressions : " << operand_id << " - " << expr_id << endl;
#if 0
      if (operand_id >= rememberOperandExpressions.size())
        rememberOperandExpressions.resize(operand_id + 1);
      rememberOperandExpressions[operand_id].push_back(expr_id);
      // rememberOperandExpressions.insert(make_pair(operand_id, expr_id));      
#endif
      
      ROSE_ASSERT (expr_id < (int)rememberExpressionTree.size());
      exprTreeType exprTree = rememberExpressionTree[expr_id];
      if (operand_id >= (int)rememberExpressionTree_ParentChild.size())
        rememberExpressionTree_ParentChild.resize(operand_id + 1);
      rememberExpressionTree_ParentChild[operand_id][exprTree.parent_id].push_back(exprTree.id);
      if (RoseBin_support::DEBUG_MODE())
        cout << " building operand expression_tree -- (operand_id, (parent_id, id))  :  (" << operand_id << ",(" << exprTree.parent_id << "," << exprTree.id << "))" << endl; 
      
    } // while
  } // if (res_set==NULL)
  checkError(conn,res_set);
}


/****************************************************
 * process substitutions. used for Navi
 ****************************************************/
void RoseBin_DB_IDAPRO::process_substitutions_query(MYSQL* conn, MYSQL_RES* res_set) {
  rememberSubstitution.clear();
  // get the functions
  char* q = (char*)"SELECT * FROM expression_substitutions_1";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    MYSQL_ROW row;
    exprSubstitutionType subst;
    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) {
          ret = (char*)"<NULL>";
          if (i==0) subst.id = -1;
          if (i==1) subst.address = -1;
          if (i==2) subst.operand_id = -1;
          if (i==3) subst.expr_id = -1;
          if (i==4) subst.replacement = ret;
        } else { 
          ret= row[i];
          if (i==0) subst.id = atoi(ret);
          if (i==1) subst.address = atoi(ret);
          if (i==2) subst.operand_id = atoi(ret);
          if (i==3) subst.expr_id = atoi(ret);
          if (i==4) subst.replacement = ret;
        }
      }
      RoseBin_support::checkText(subst.replacement);
      if (RoseBin_support::DEBUG_MODE())
        cout << ">> creating substitutions : " << subst.address << " - " << subst.operand_id << " - " 
             << subst.expr_id << " - " << subst.replacement << endl;
      rememberSubstitution[subst.address] = subst;
    } // while
  } // if (res_set==NULL)
  checkError(conn,res_set);
}

/****************************************************
 * check the type of each operand
 ****************************************************/
string RoseBin_DB_IDAPRO::resolveType(exprTreeType* expt) {
  string type=(char*)"";
  if (expt->symbol=="b8") {
    return "QWORD";
  } else  
    if (expt->symbol=="b6") {
      // FIXME: dont know what this is, but lets return dword for now
      return "DWORD";
    } else  
      if (expt->symbol=="b4") {
      return "DWORD";
      } else  
        if (expt->symbol=="b2") {
          return "WORD";
        } else
          if (expt->symbol=="b1") {
          return "BYTE";
          } else {
            //    exprTreeType parentExp = rememberExpressionTree_Root[expt.parent_id];
            //type = resolveType(parentExp);
          cerr << " resolveType: unknown type:: " << expt->symbol << endl;
          }
  return type;
}


/****************************************************
 * process operand tuples. 
 * Handles all expressions to be added to the instructions.
 ****************************************************/
void RoseBin_DB_IDAPRO::process_operand_tuples_query(MYSQL* conn, MYSQL_RES* res_set) {
  // get the functions
  rememberOperandStrings.clear(); // Not needed right now

  char* q = (char*)"SELECT * FROM operand_tuples_1 order by operand_id desc";
  if (RoseBin_support::DEBUG_MODE())
    cout << "\n>> QUERY:: " << q << "\n" << endl;
  res_set = process_query(conn,q);
  if (res_set == NULL) {
    print_problemWithResults(conn);
  } else {
    map < int, vector < SgAsmExpression* > > tmp_instruction_map;
    // tmp_instruction_map.clear();
    
    MYSQL_ROW row;
    long address=0;
    int operand_id=-1;
    int position =-1;

    while ((row = mysql_fetch_row(res_set))!=NULL) {
      for (unsigned int i=0; i<mysql_num_fields(res_set);i++) {
        char* ret=(char*)"";
        if (row[i] ==NULL) {
          ret = (char*)"<NULL>";
          if (i==0) address = -1;
          if (i==1) operand_id = -1;
          if (i==2) position = -1;
        } else { 
          ret= row[i];
          if (i==0) address = atoi(ret);
          if (i==1) operand_id = atoi(ret);
          if (i==2) position = atoi(ret);
        }
      }
      //cerr << " >>>>>>>>>> position : " << position;
      // if this would have to be saved in a map, it would need to be a multimap!!
      //if (RoseBin_support::DEBUG_MODE()) {
      ostringstream addrhex;
      addrhex << hex << setw(8) << address ;

      //cout<< "\n\n----------------------------------------------------------------" << endl;
      if (operand_id % 5000 == 0) {
        cout << ">> creating operand_tuple : address: " << addrhex.str() << " " << address << 
          " -  operand_id: " << operand_id << " -  position:" << position << endl;
      }
      //}

      // get the expr_id for the operand_id
      SgAsmExpression* binExp=NULL;
      //string operand_str=(char*)"";
      if (operand_id>=0) {
        // operand_str = rememberOperandStrings.find(operand_id) != rememberOperandStrings.end() ? rememberOperandStrings[operand_id] : "";
        //if (RoseBin_support::DEBUG_MODE())
        //cout << ">>>> operand_str: " << operand_str <<  endl;
        ROSE_ASSERT (operand_id < (int)rememberExpressionTree_ROOT.size());
        int expr_id_root = rememberExpressionTree_ROOT[operand_id];
        ROSE_ASSERT (operand_id < (int)rememberExpressionTree_ParentChild.size());
        map <int, vector<int> > subTree = rememberExpressionTree_ParentChild[operand_id];

        rememberExpressionTree_ROOT.resize(operand_id + 1);
        rememberExpressionTree_ParentChild.resize(operand_id + 1);

        ROSE_ASSERT (expr_id_root < (int)rememberExpressionTree.size());
        exprTreeType exprTree = rememberExpressionTree[expr_id_root];
        string typeOfOperand = resolveType(&exprTree);

#if 0
        // print multimapsolveRe
        if (RoseBin_support::DEBUG_MODE()) {
          multimap<int,int>::iterator it = subTree.begin();
          for (; it!=subTree.end();++it) {
            int f=it->first;
            int s=it->second;
            cout << " mm : " << f << "," << s << endl;
          }
        }
#endif
        binExp = buildROSE->resolveRecursivelyExpression(address,expr_id_root, 
                                                         subTree, 
                                                         typeOfOperand,
                                                         &rememberExpressionTree,
                                                         operand_id,
                                                         &rememberSubstitution,
                                                         &rememberComments);

      } // if operand

      
      // should save for each instruction, a list of operands and their position
      // and add the operand later on to the instruction
      // map < address, map < pos, binExp > >
      vector<SgAsmExpression*>& currentOperands = tmp_instruction_map[address];
      if (position >= (int)currentOperands.size()) currentOperands.resize(position + 1);
      currentOperands[position] = binExp;
#if 0
      map <int, map <int, SgAsmExpression*> >::iterator tmpIt = tmp_instruction_map.find(address);
      bool found=false;
      if (tmpIt!=tmp_instruction_map.end())
        found=true;
      if (found) {
        //cerr << " found   position: " << position << endl;
        // I do not want to add an address into tmp if not in (side effect!)
        map <int, SgAsmExpression*> tmp_map = tmp_instruction_map[address];
        tmp_map[position] = binExp;
        tmp_instruction_map[address] = tmp_map;
      } else {
        // first time visit
        //cerr << " notfound   position: " << position << endl;
        map <int, SgAsmExpression*> tmp_map;
        tmp_map[position] = binExp;
        tmp_instruction_map[address] = tmp_map;
      }
#endif

    } // while


    // get basic_block and append this instruction
    if (RoseBin_support::DEBUG_MODE())
      cout << "\n\n> appending operandList to instruction.  " << endl;
    rose_hash::unordered_map <uint64_t, SgAsmInstruction* >::iterator blockIt;      
    int count = 0;
    cerr << "Instruction count: " << rememberInstructions.size() << endl;
    for (blockIt=rememberInstructions.begin();blockIt!=rememberInstructions.end();++blockIt) {
      ++count;
      if (count % 1000 == 0) cout << "Adding operands to instruction " << count << endl;
      int inst_address = blockIt->first;
      SgAsmInstruction* remInstr = blockIt->second;
      map<int, vector< SgAsmExpression*> >::iterator it = tmp_instruction_map.find(inst_address);
      if (it != tmp_instruction_map.end()) { // Skip for instructions without operands
        vector < SgAsmExpression*>& exprList_forInst = it->second;
        int sizeList = exprList_forInst.size();
        // find each element separately
        for (int i=0; i<sizeList; i++) {
          SgAsmExpression* binExp = exprList_forInst[i];
#if 0
          map <int, SgAsmExpression*>::iterator it = exprList_forInst.find(i);
          // get the elements in order!! this is important.
          SgAsmExpression* binExp = it->second;
#endif
          
          remInstr->get_operandList()->append_operand(binExp);
          binExp->set_parent(remInstr->get_operandList());
          
          exprTreeType exprTree = buildROSE->getDebugHelp(binExp);
          if (RoseBin_support::DEBUG_MODE())
            cout << ">> append operand (to instruction): binExp: " <<binExp  
                 << " - sym: " <<exprTree.symbol << " - immedi: " << exprTree.immediate << endl;
        }
        tmp_instruction_map.erase(it);
      }
    }

  } // if (res_set==NULL)
  checkError(conn,res_set);
}

