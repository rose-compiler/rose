/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that handles the IDAPRO DB
 ****************************************************/
#include "rose.h"
#include "RoseBin_file.h"
#include <errno.h>
#include "readTicl.h"

using namespace std;
using namespace ticl;

/****************************************************
 * process callgraph
 ****************************************************/
void RoseBin_FILE::process_callgraph_query() {
  rememberCallGraph.clear();
  // get the functions
  vector<callgraph_1> vec_callgraph_1;
  get_callgraph_1(filename, vec_callgraph_1);
  //  char* q = (char*)"SELECT * FROM callgraph_1";
  CallGraphType callGraph;

  for (unsigned int i=0; i<vec_callgraph_1.size(); i++) {
    callGraph.id = atoi(vec_callgraph_1[i].id.c_str());
    callGraph.src = atoi(vec_callgraph_1[i].src.c_str());
    callGraph.src_basic_block_id = atoi(vec_callgraph_1[i].src_basic_block_id.c_str());
    callGraph.src_address = atoi(vec_callgraph_1[i].src_address.c_str());
    callGraph.dst = atoi(vec_callgraph_1[i].dst.c_str());

    if (RoseBin_support::DEBUG_MODE())
      cout << ">> creating callgraph : " << callGraph.id << " - " << callGraph.src_address << endl;

    rememberCallGraph[callGraph.src_address] = callGraph;
      
  } // while
}

std::string 
RoseBin_FILE::getName(string name) {
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
void RoseBin_FILE::process_functions_query(SgAsmBlock* globalBlock,
					   list<string> functionFilter) {
  // get the functions
  rememberFunctions.clear();
  //int counter=0;

  vector<functions_1> vec_functions_1;
  get_functions_1(filename, vec_functions_1);
  //  char* q = (char*)"SELECT * FROM functions_1";

  for (unsigned int i=0; i<vec_functions_1.size(); i++) {
    long address=0;
    string name = (char*)"";
    int type=-1;
    string name_md5 =(char*)"";
    
    address = atoi(vec_functions_1[i].address.c_str());
    name = vec_functions_1[i].name;
    type= atoi(vec_functions_1[i].function_type.c_str());
    name_md5= vec_functions_1[i].name_md5;

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

      rememberFunctions[address]= fd ;
  } // while
}

/****************************************************
 * process all comments in the DB
 ****************************************************/
void RoseBin_FILE::process_comments_query( ) {
  // get the functions
  rememberComments.clear();

  vector<address_comments_1> vec_address_comments_1;
  get_address_comments_1(filename, vec_address_comments_1);
  //  char* q = (char*)"SELECT * FROM address_address_comments_1";
    long address=0;
    string comment=(char*)"";

  for (unsigned int i=0; i<vec_address_comments_1.size(); i++) {
    address = atoi(vec_address_comments_1[i].address.c_str());
     comment = vec_address_comments_1[i].comment;

      RoseBin_support::checkText(comment);
      if (RoseBin_support::DEBUG_MODE())
	cout << ">> creating comments : " << address << " - " << comment << endl;
      rememberComments[address]= comment ;

    } // while
}



/****************************************************
 * resolve for each instruction which type it has
 ****************************************************/
SgAsmInstruction* RoseBin_FILE::createInstruction(int address, SgAsmFunctionDeclaration* bb, 
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
    exit(0); 
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
void RoseBin_FILE::process_instruction_query( ) {
  rememberInstructions.clear();
  // get the functions
  //  char* q = (char*)"SELECT * FROM instructions_1";


  vector<basic_blocks_1> vec_basic_blocks_1;
  get_basic_blocks_1(filename, vec_basic_blocks_1);

  vector<instructions_1> vec_instructions_1;
  get_instructions_1(filename, vec_instructions_1);

  // Get first address in each basic block
  /*
  declare first_insn_address as map from id to address
    for each (id, parent_function, address) in basic_blocks_1 {
	if (address >= parent_function &&
	    (first_insn_address[id] does not exist ||
	     address < first_insn_address[id])) {
	  first_insn_address[id] = address;
	}
      }
  */
  map<int , uint64_t> first_insn_address;
  for (int i=0; i<(int)vec_basic_blocks_1.size(); i++) {
    int id  = atoi(vec_basic_blocks_1[i].id.c_str());
    uint64_t address  = atoi(vec_basic_blocks_1[i].address.c_str());
    int parent_function = atoi(vec_basic_blocks_1[i].parent_function.c_str());
    if (address >= parent_function && first_insn_address.find(id) == first_insn_address.end()
	|| address < first_insn_address[id]) {
      first_insn_address[id] = address;
    } 
  }

  
  // Get the parent function for the first instruction in each basic block
  /*
  declare get_parent_function as map from address to parent_function
    for each (address, basic_block_id, parent_function) in basic_blocks_1 {
	if (first_insn_address[basic_block_id] exists &&
	    address == first_insn_address[basic_block_id]) {
	  get_parent_function[address] = parent_function;
	}
      }
  */
  map<uint64_t, int> get_parent_function;
  for (int i=0; i<(int)vec_basic_blocks_1.size(); i++) {
    int id  = atoi(vec_basic_blocks_1[i].id.c_str());
    uint64_t address  = atoi(vec_basic_blocks_1[i].address.c_str());
    int parent_function = atoi(vec_basic_blocks_1[i].parent_function.c_str());
    if (first_insn_address.find(id) != first_insn_address.end() && address==first_insn_address[id]) {
      get_parent_function[address] = parent_function;
    } 
  }


  //  char *q = (char*)"select *,     
  //(select parent_function from basic_blocks_1 where id = i.basic_block_id      
  // and (i.address - parent_function) >= 0     
  // and (i.address - parent_function) =     
  // (select min(i.address - parent_function) from basic_blocks_1 where id = i.basic_block_id       
  // and (i.address - parent_function) >= 0)     ) as i_f from instructions_1 i"; 

    string mnemonic=(char*)"";
    uint64_t address=0;
    int basic_block=-1;
    int sequence =-1;
    string data=(char*)"";
    int i_func;

    for (unsigned int i=0; i<vec_instructions_1.size(); i++) {
       address = atoi(vec_instructions_1[i].address.c_str());
       basic_block = atoi(vec_instructions_1[i].basic_block_id.c_str());
       mnemonic = vec_instructions_1[i].mnemonic;
       sequence = atoi(vec_instructions_1[i].sequence.c_str());
       data = vec_instructions_1[i].data;
       i_func = get_parent_function[address];

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
      rose_hash::hash_map <int, SgAsmFunctionDeclaration* >::iterator func_it = rememberFunctions.find(i_func);
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
      //	instruction = new SgAsmInstruction(address,bb,mnemonic,"");
      instruction->set_raw_bytes(data);
      // set file pointer for each instruction
      //instruction->set_file_info(this_file);

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

}


/****************************************************
 * process operand strings. used in process_operand_tuples
 ****************************************************/
void RoseBin_FILE::process_operand_strings_query( ) {
  rememberOperandStrings.clear();
  // get the functions
  vector<operand_strings_1> vec_operand_strings_1;
  get_operand_strings_1(filename, vec_operand_strings_1);

  //  char* q = (char*)"SELECT * FROM operand_strings_1";
    int id=-1;
    string str=(char*)"";

  for (unsigned int i=0; i<vec_operand_strings_1.size(); i++) {
    id = atoi(vec_operand_strings_1[i].id.c_str());
    str = vec_operand_strings_1[i].str;

      RoseBin_support::checkText(str);
      if (RoseBin_support::DEBUG_MODE())
	cout << ">> creating operand_strings : " << id << " - " << str << endl;
      if (id >= (int)rememberOperandStrings.size()) rememberOperandStrings.resize(id + 1);
      rememberOperandStrings[id] = str;      
    } // while
}


/****************************************************
 * process operand expressions. used in process_operand_tuples
 ****************************************************/
void RoseBin_FILE::process_operand_root_query( ) {
  rememberExpressionTree_ROOT.clear();
  // get the functions

  vector<operand_expressions_1> vec_operand_expressions_1;
  get_operand_expressions_1(filename, vec_operand_expressions_1);

  //char* q = (char*)"select operand_id, expr_id from operand_expressions_1 as oe, expression_tree_1 as t where oe.expr_id = t.id and t.parent_id is NULL";
    int operand_id=-1;
    int expr_id=-1;

    for (unsigned int i=0; i<vec_operand_expressions_1.size(); i++) {
      operand_id = atoi(vec_operand_expressions_1[i].operand_id.c_str());
      expr_id = atoi(vec_operand_expressions_1[i].expr_id.c_str());

      if (RoseBin_support::DEBUG_MODE())
	cout << ">> creating operand_roots : " << operand_id << " - " << expr_id << endl;
      if (operand_id >= (int) rememberExpressionTree_ROOT.size())
	rememberExpressionTree_ROOT.resize(operand_id + 1);
      rememberExpressionTree_ROOT[operand_id] = expr_id;      
    } // while
}


/****************************************************
 * process expression tree. used in process_operand_tuples
 ****************************************************/
void RoseBin_FILE::process_expression_tree_query( ) {
  rememberExpressionTree.clear();
  // get the functions
  vector<expression_tree_1> vec_expression_tree_1;
  get_expression_tree_1(filename, vec_expression_tree_1);

  //  char* q = (char*)"SELECT * FROM expression_tree_1";
    exprTreeType exprTree;
  for (unsigned int i=0; i<vec_expression_tree_1.size(); i++) {
    exprTree.id = atoi(vec_expression_tree_1[i].id.c_str());
    exprTree.expr_type = atoi(vec_expression_tree_1[i].expr_type.c_str());
    exprTree.symbol = vec_expression_tree_1[i].symbol;
    
      errno = 0;
      exprTree.immediate = strtol(vec_expression_tree_1[i].immediate.c_str(), NULL, 10);
      if (errno != 0) {
	errno = 0;
	exprTree.immediate = strtoul(vec_expression_tree_1[i].immediate.c_str(), NULL, 10);
	ROSE_ASSERT (errno == 0);
      }
      if (RoseBin_support::DEBUG_MODE())
	cout << "immediate = (char*)" << exprTree.immediate << endl;

    exprTree.position = atoi(vec_expression_tree_1[i].position.c_str());
    exprTree.parent_id = atoi(vec_expression_tree_1[i].parent_id.c_str());
    
      if (RoseBin_support::DEBUG_MODE())
	cout << ">> creating exprTree : " << exprTree.id << " - " << exprTree.expr_type << " - " 
	     << exprTree.symbol << " - " << exprTree.immediate << " - " << exprTree.position 
	     << " - " << exprTree.parent_id << endl;

      if (exprTree.id >= (int)rememberExpressionTree.size())
	rememberExpressionTree.resize(exprTree.id + 1);
      rememberExpressionTree[exprTree.id] = exprTree;


    } // while
}

//compare function for std sort
struct SortDescending_operand_expression_1 : public std::binary_function<operand_expressions_1, operand_expressions_1,bool>
{
  bool operator()(const operand_expressions_1& s1, const operand_expressions_1& s2) const
  {
    return (s1.operand_id > s2.operand_id);
  }
};


/****************************************************
 * process operand expressions. used in process_operand_tuples
 ****************************************************/
void RoseBin_FILE::process_operand_expressions_query( ) {
  // rememberOperandExpressions.clear();
  rememberExpressionTree_ParentChild.clear();
  // get the functions

  vector<operand_expressions_1> vec_operand_expressions_1;
  get_operand_expressions_1(filename, vec_operand_expressions_1);

  std::sort(vec_operand_expressions_1.begin(), vec_operand_expressions_1.end(), SortDescending_operand_expression_1());

  //  char* q = (char*)"SELECT * FROM operand_expressions_1 order by operand_id desc";
  int operand_id=-1;
  int expr_id=-1;
  
  for (unsigned int i=0; i<vec_operand_expressions_1.size(); i++) {
    operand_id = atoi(vec_operand_expressions_1[i].operand_id.c_str());
    expr_id = atoi(vec_operand_expressions_1[i].expr_id.c_str());

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
      rememberExpressionTree_ParentChild[operand_id].insert(make_pair(exprTree.parent_id, exprTree.id));
      if (RoseBin_support::DEBUG_MODE())
	cout << " building operand expression_tree -- (operand_id, (parent_id, id))  :  (" << operand_id << ",(" << exprTree.parent_id << "," << exprTree.id << "))" << endl; 
      
    } // while
}


/****************************************************
 * process substitutions. used for Navi
 ****************************************************/
void RoseBin_FILE::process_substitutions_query( ) {
  rememberSubstitution.clear();
  // get the functions
  vector<expression_substitutions_1> vec_expression_substitutions_1;
  get_expression_substitutions_1(filename, vec_expression_substitutions_1);

  //  char* q = (char*)"SELECT * FROM expression_substitutions_1";
    exprSubstitutionType subst;
  for (unsigned int i=0; i<vec_expression_substitutions_1.size(); i++) {
    subst.id = atoi(vec_expression_substitutions_1[i].id.c_str());
    subst.address = atoi(vec_expression_substitutions_1[i].address.c_str());
    subst.operand_id = atoi(vec_expression_substitutions_1[i].operand_id.c_str());
    subst.expr_id = atoi(vec_expression_substitutions_1[i].expr_id.c_str());
    subst.replacement = vec_expression_substitutions_1[i].replacement;

      RoseBin_support::checkText(subst.replacement);
      if (RoseBin_support::DEBUG_MODE())
	cout << ">> creating substitutions : " << subst.address << " - " << subst.operand_id << " - " 
	     << subst.expr_id << " - " << subst.replacement << endl;
      rememberSubstitution[subst.address] = subst;
    } // while
}

/****************************************************
 * check the type of each operand
 ****************************************************/
string RoseBin_FILE::resolveType(exprTreeType* expt) {
  string type=(char*)"";
  if (expt->symbol==(char*)"b8") {
    return "QWORD";
  } else  
    if (expt->symbol==(char*)"b6") {
      // FIXME: dont know what this is, but lets return dword for now
      return "DWORD";
    } else  
      if (expt->symbol==(char*)"b4") {
	return "DWORD";
      } else  
	if (expt->symbol==(char*)"b2") {
	  return "WORD";
	} else
	  if (expt->symbol==(char*)"b1") {
	    return "BYTE";
	  } else {
	    //    exprTreeType parentExp = rememberExpressionTree_Root[expt.parent_id];
	    //type = resolveType(parentExp);
	    cerr << " resolveType: unknown type:: " << expt->symbol << endl;
	  }
  return type;
}


//compare function for std sort
struct SortDescending_operand_tuples_1 : public std::binary_function<operand_tuples_1, operand_tuples_1,bool>
{
  bool operator()(const operand_tuples_1& s1, const operand_tuples_1& s2) const
  {
    return (s1.operand_id > s2.operand_id);
  }
};

/****************************************************
 * process operand tuples. 
 * Handles all expressions to be added to the instructions.
 ****************************************************/
void RoseBin_FILE::process_operand_tuples_query( ) {
  // get the functions
  rememberOperandStrings.clear(); // Not needed right now

  vector<operand_tuples_1> vec_operand_tuples_1;
  get_operand_tuples_1(filename, vec_operand_tuples_1);

  std::sort(vec_operand_tuples_1.begin(), vec_operand_tuples_1.end(), SortDescending_operand_tuples_1());

  //char* q = (char*)"SELECT * FROM operand_tuples_1 order by operand_id desc";
    map < int, vector < SgAsmExpression* > > tmp_instruction_map;
    // tmp_instruction_map.clear();
    
    long address=0;
    int operand_id=-1;
    int position =-1;

    for (unsigned int i=0; i<vec_operand_tuples_1.size(); i++) {
      address = atoi(vec_operand_tuples_1[i].address.c_str());
      operand_id = atoi(vec_operand_tuples_1[i].operand_id.c_str());
      position = atoi(vec_operand_tuples_1[i].position.c_str());

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
	multimap <int,int>  subTree = rememberExpressionTree_ParentChild[operand_id];

	rememberExpressionTree_ROOT.resize(operand_id + 1);
	rememberExpressionTree_ParentChild.resize(operand_id + 1);

	ROSE_ASSERT (expr_id_root < (int)rememberExpressionTree.size());
	exprTreeType exprTree = rememberExpressionTree[expr_id_root];
	string typeOfOperand = resolveType(&exprTree);

	// print multimapsolveRe
	if (RoseBin_support::DEBUG_MODE()) {
	  multimap<int,int>::iterator it = subTree.begin();
	  for (; it!=subTree.end();++it) {
	    int f=it->first;
	    int s=it->second;
	    cout << " mm : " << f << "," << s << endl;
	  }
	}
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
    rose_hash::hash_map <uint64_t, SgAsmInstruction* >::iterator blockIt;      
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

}

