/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 6Jun07
 * Decription : Interface to compare src Tree against
 *              binary Tree
 * todo : color the variables and names in the src/bin tree
 ****************************************************/
// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"
#include "RoseBin_CompareAnalysis.h"

using namespace std;


// DQ (10/20/2010): Moved to source file to support compilation of language only mode which excludes binary analysis support.
RoseBin_CompareAnalysis::RoseBin_CompareAnalysis(SgProject *pr, SgAsmNode* global)
   {
     RoseBin_support::setDebugMode(false);    
     project = pr;
     globalBin = global;
     main_prolog_end=false;
     attributeName = "rosebin_color";
   }

/****************************************************
 * create a attribute that can be passed to the
 * visualization
 ****************************************************/
MyAstAttribute* RoseBin_CompareAnalysis::createAttribute(int val) {
  MyAstAttribute* maas = new MyAstAttribute(val);
  return maas;
}

/****************************************************
 * traverse the binary AST
 ****************************************************/
void RoseBin_CompareAnalysis::visit(SgNode* node) {
  //SgAsmNode* binNode= isSgAsmNode(node);
  
  //  cerr << " traversing node " << binNode << endl;
  if (isSgAsmFunctionDeclaration(node)) {
    SgAsmFunctionDeclaration* binDecl = isSgAsmFunctionDeclaration(node); 
    string name = binDecl->get_name();
    bin_funcs[name] = binDecl;
  }
}

/****************************************************
 * traverse Rose src and create map (name, src_func, bin_func)
 ****************************************************/
void RoseBin_CompareAnalysis::create_map_functions() {
  traverse(globalBin,preorder);

  std::vector<SgNode*> functionCallList = NodeQuery:: querySubTree (project, V_SgFunctionDeclaration);
  std::vector<SgNode*>::iterator it = functionCallList.begin();
  for (;it!=functionCallList.end();++it) {
    SgFunctionDeclaration* funcDef = isSgFunctionDeclaration(*it);
    string name = funcDef->get_name().str();
    if(RoseBin_support::filterName(name)==false) {
      SgAsmFunctionDeclaration* funcD=bin_funcs[name];
      //      cerr << " found function " << name << " " << 
      //	funcDef->get_name().str() << " " << funcD->get_name() << endl;
      function_map[name]=(make_pair(funcDef,funcD));
    }
  }
}

/****************************************************
 * use this filter to remove certain instructions
 * that are known 
2 ****************************************************/
bool RoseBin_CompareAnalysis::instruction_filter(SgAsmStatement* stat, string func_name, string *output) {
  // return true if valid instruction, false if part of filter
  bool isValidInstr=true;
  SgAsmx86Instruction* inst = isSgAsmx86Instruction(stat);
  if (inst==NULL) {
    cerr << " filter function produced a statement that is not an instruction " << endl;
    exit(0);
  }

  if (func_name=="main")
    if (main_prolog_end==false)
      isValidInstr=false;
  
  if (inst->get_kind() == x86_push) {
    //      cerr << " enerting push " << endl;
    // is a push
    SgAsmOperandList* opList = inst->get_operandList();
    SgAsmExpressionPtrList exprList = opList->get_operands();
    SgAsmExpression* binExp = *(exprList.begin());
    SgAsmx86RegisterReferenceExpression* refExp = isSgAsmx86RegisterReferenceExpression(binExp); 
    X86RegisterClass cl = (X86RegisterClass)refExp->get_descriptor().get_major();
    int reg = refExp->get_descriptor().get_minor();
    if (cl == x86_regclass_gpr && reg==x86_gpr_bp) 
      isValidInstr=false;
    if (cl == x86_regclass_gpr && reg==x86_gpr_cx) 
      isValidInstr=false;
  } else
    if (inst->get_kind() == x86_pop) {
      //  cerr << " enerting pop " << endl;
      // its a pop
      SgAsmOperandList* opList = inst->get_operandList();
      SgAsmExpressionPtrList exprList = opList->get_operands();
      SgAsmExpression* binExp = *(exprList.begin());
      SgAsmx86RegisterReferenceExpression* refExp = isSgAsmx86RegisterReferenceExpression(binExp); 
      X86RegisterClass cl = (X86RegisterClass)refExp->get_descriptor().get_major();
      int reg = refExp->get_descriptor().get_minor();
      if (cl == x86_regclass_gpr && reg==x86_gpr_cx) 
	isValidInstr=false;
      if (cl == x86_regclass_gpr && reg==x86_gpr_bp) {
	isValidInstr=false;
	// if this is the main funtion and we find a <pop BP>
	// then we want not to process the rest of the instructions.
	if (func_name=="main")
	  main_prolog_end=false;
      }
    } else
      if (inst->get_kind() == x86_mov) {
	//cerr << " enerting mov " << endl;
	// its a mov
	SgAsmOperandList* opList = inst->get_operandList();
	SgAsmExpressionPtrList exprList = opList->get_operands();
	SgAsmExpressionPtrList::iterator it = exprList.begin();
	int iteration=0;
	bool isMovBp0=false;
	bool isMovSp1=false;
	for (;it!=exprList.end();it++) {
	  SgAsmExpression* binExp = (*it);
	  ROSE_ASSERT(binExp);
	  //cerr << " type " << binExp->class_name() << endl;
	  SgAsmx86RegisterReferenceExpression* refExp = isSgAsmx86RegisterReferenceExpression(binExp);
	  if (refExp) {
	    //ROSE_ASSERT(refExp);
            X86RegisterClass cl = (X86RegisterClass)refExp->get_descriptor().get_major();
            int reg = refExp->get_descriptor().get_minor();
	    if (iteration==0 && cl == x86_regclass_gpr && reg==x86_gpr_bp) 
	      isMovBp0=true;
	    if (iteration==1 && cl == x86_regclass_gpr && reg==x86_gpr_sp) 
	      isMovSp1=true; 
	  }
	  iteration++;
	}
	if (isMovBp0 && isMovSp1) {
	  isValidInstr=false;
	  *output += "skipped Mov Bp,Sp\n";
	  // if this is main, then this is where the function begins
	  if (func_name=="main")
	    main_prolog_end=true;
	}
      } else 
	if (inst->get_kind() == x86_sub) {
	  //cerr << " enerting sub " << endl;
	  // its a sub
	  SgAsmOperandList* opList = inst->get_operandList();
	  SgAsmExpressionPtrList exprList = opList->get_operands();
	  SgAsmExpressionPtrList::iterator it = exprList.begin();
	  int iteration=0;
	  bool isSubSp0=false;
	  bool isSubVal1=false;
	  for (;it!=exprList.end();it++) {
	    SgAsmExpression* binExp = (*it);
	    ROSE_ASSERT(binExp);
	    //cerr << " type " << binExp->class_name() << endl;
	    SgAsmx86RegisterReferenceExpression* refExp = isSgAsmx86RegisterReferenceExpression(binExp);
	    SgAsmValueExpression* valExp = isSgAsmValueExpression(binExp);
	    if (refExp && iteration==0) {
              X86RegisterClass cl = (X86RegisterClass)refExp->get_descriptor().get_major();
              int reg = refExp->get_descriptor().get_minor();
	      if (cl == x86_regclass_gpr && reg==x86_gpr_sp) 
		isSubSp0=true;
	    }
	    if (valExp && iteration==1) {
	      isSubVal1=true;
	    }
	    iteration++;
	  }
	  if (isSubSp0 && isSubVal1) {
	    isValidInstr=false;
	    *output += "skipped Sub Sp,Val\n";
	  }
	} else
	  if (inst->get_kind() == x86_ret) {
	    isValidInstr=false;
	  } else
	    if (inst->get_kind() == x86_leave) {
	      isValidInstr=false;
	    } else{
	      // found generic instruction
	      //    isValidInstr=true;
	    }

  if (isValidInstr==false) {
    stat->setAttribute(attributeName,createAttribute(0));
  }  
  return isValidInstr;
}


/****************************************************
 * determine variable access
 ****************************************************/
string RoseBin_CompareAnalysis::checkVariable(SgAsmValueExpression* rhs) {
  string valStr="";
  if (rhs) {
    if ( isSgAsmByteValueExpression(rhs)) {
      short val = isSgAsmByteValueExpression(rhs)->get_value();
      //cerr << " checking local short variable at bp " << RoseBin_support::ToString(val) << endl;
      valStr = RoseBin_support::ToString(val);
    } else
      if (isSgAsmDoubleFloatValueExpression(rhs)) {
	double val = isSgAsmDoubleFloatValueExpression(rhs)->get_value();
	//cerr << " checking local float variable at bp " << RoseBin_support::ToString(val) << endl;
	valStr = RoseBin_support::ToString(val);
      } else
	if (isSgAsmDoubleWordValueExpression(rhs)) {
	  int val = isSgAsmDoubleWordValueExpression(rhs)->get_value();
	  //cerr << " checking local int variable at bp " << RoseBin_support::ToString(val) << endl;
	  valStr = RoseBin_support::ToString(val);
	} else {
	  cerr << " the type of value is currently not handled - CompareAnalysis " << endl;
	  exit(0);
	}
  } else {
    cerr << " >> cannot check for variable location 3. " << endl;
    exit(0);
  }
  return valStr;  
}

void RoseBin_CompareAnalysis::storeVariable(string val, 
					    string name,
					    SgAsmValueExpression* node
					    ) {
  cerr << " storing variable : " << name << " val: " << val << endl;
  local_vars[val]=(make_pair(name, node));
  node->setAttribute(attributeName,createAttribute(2));
}

string RoseBin_CompareAnalysis::getVariableName(string val) {
  std::pair<string,SgAsmValueExpression*> thepair = local_vars[val];
  return thepair.first;
}

SgAsmValueExpression* RoseBin_CompareAnalysis::getVariableType(string val) {
  std::pair<string,SgAsmValueExpression*> thepair = local_vars[val];
  return thepair.second;
}

/****************************************************
 * check if binary operands are a vardecl or assignment
 ****************************************************/

string RoseBin_CompareAnalysis::resolveRegister(const RegisterDescriptor &reg) {
  string ret="reg";
  
  if (reg.get_major() == x86_regclass_segment && reg.get_minor() == x86_segreg_ss) 
    ret="SS";
  if (reg.get_major() == x86_regclass_gpr && reg.get_minor()==x86_gpr_ax) 
    ret="AX";
  if (reg.get_major() == x86_regclass_gpr && reg.get_minor()==x86_gpr_bx) 
    ret="BX";
  if (reg.get_major() == x86_regclass_gpr && reg.get_minor()==x86_gpr_cx) 
    ret="CX";
  if (reg.get_major() == x86_regclass_gpr && reg.get_minor()==x86_gpr_dx) 
    ret="DX";
  if (reg.get_major() == x86_regclass_gpr && reg.get_minor()==x86_gpr_di) 
    ret="DI";
  if (reg.get_major() == x86_regclass_gpr && reg.get_minor()==x86_gpr_si) 
    ret="SI";
  if (reg.get_major() == x86_regclass_gpr && reg.get_minor()==x86_gpr_sp) 
    ret="SP";
  if (reg.get_major() == x86_regclass_gpr && reg.get_minor()==x86_gpr_bp) 
    ret="BP";
  if (reg.get_major() == x86_regclass_segment && reg.get_minor()==x86_segreg_cs) 
    ret="CS";
  if (reg.get_major() == x86_regclass_segment && reg.get_minor()==x86_segreg_ds) 
    ret="DS";
  if (reg.get_major() == x86_regclass_segment && reg.get_minor()==x86_segreg_es) 
    ret="ES";
  if (reg.get_major() == x86_regclass_segment && reg.get_minor()==x86_segreg_fs) 
    ret="FS";
  if (reg.get_major() == x86_regclass_segment && reg.get_minor()==x86_segreg_gs) 
    ret="GS";

  return ret;
}

bool RoseBin_CompareAnalysis::existsVariable(string value) {
  bool exists =true;
  string name = getVariableName(value);
  if (name=="")
    exists=false;
  return exists;
}

string RoseBin_CompareAnalysis::resolve_binaryInstruction(SgAsmInstruction* mov,
							  string *left,
							  string *right,
							  string name
							  ) {

  SgAsmOperandList* opList = mov->get_operandList();
  SgAsmExpressionPtrList exprList = opList->get_operands();
  SgAsmExpressionPtrList::iterator it = exprList.begin();
  //  SgAsmx86RegisterReferenceExpression* myexp=NULL;
  int iteration=0;
  string value="";
  for (;it!=exprList.end();it++) {
    SgAsmExpression* binExp = (*it);
    ROSE_ASSERT(binExp);
    string str="";
    //myexp = isSgAsmx86RegisterReferenceExpression(binExp);
    //if (myexp) {
    // register
    //SgAsmx86RegisterReferenceExpression::x86_register_enum reg = myexp->get_x86_register_code();
    //string regName = resolveRegister(reg);
    //SgAsmMemoryReferenceExpression* memref = isSgAsmMemoryReferenceExpression(myexp->get_offset());
    SgAsmMemoryReferenceExpression* memref = isSgAsmMemoryReferenceExpression(binExp);
    if (memref) {
      SgAsmx86RegisterReferenceExpression* myexp = isSgAsmx86RegisterReferenceExpression(memref->get_segment());
      if (myexp) {
	// register
	string regName = resolveRegister(myexp->get_descriptor());
	
	SgAsmBinaryAdd* binAdd = isSgAsmBinaryAdd(memref->get_address());
	if (binAdd) {
	  SgAsmValueExpression* rhs = isSgAsmValueExpression(binAdd->get_rhs());
	  value = checkVariable(rhs);
	  if (existsVariable(value)==false) 
	    storeVariable(value, name, rhs);
	  string varname = getVariableName(value);
	  str = regName +": ["+varname +"]";
	  rhs->setAttribute(attributeName,createAttribute(2));	      
	}
      }
      else {
	//str = regName;
	//myexp->setAttribute(attributeName,createAttribute(2));	      
        memref->setAttribute(attributeName,createAttribute(2));	      
      }
    } //left
    SgAsmValueExpression* valExp = isSgAsmValueExpression(binExp);
    if (valExp) {
      string value = checkVariable(valExp);
      str = value ;
      valExp->setAttribute(attributeName,createAttribute(2));	      
    }
    if (iteration==0)
      *left=str;
    else
      *right=str;
  
    iteration++;
  }
  cerr << "         inside resolve_binaryInstruction ... get asm (left, right) .  " << 
    *left << " " << *right << endl;
  // return the nr for a variable , e.g. -8
  return value;
}


bool RoseBin_CompareAnalysis::isReturnStmt(SgNode* srcNode,
                                           SgAsmNode* binNode,
					   string *output,
					   int &nodes_matched) {
  bool isreturn=false;
  // check if return matches
  SgReturnStmt* returnS = isSgReturnStmt(srcNode);
  SgAsmx86Instruction* retBin = isSgAsmx86Instruction(binNode);
  if (returnS && retBin && retBin->get_kind() == x86_ret ) {
    *output+= " !! returnStatement ............... \n";
    nodes_matched++;
    returnS->setAttribute(attributeName,createAttribute(2));
    retBin->setAttribute(attributeName,createAttribute(2));
    retBin->set_comment(returnS->class_name());
    isreturn=true;
  }
  return isreturn;
}

/**********************************************************
 * check if function calls match ----------------------------------------------------------------------------
 * if we have a FgFunctionCallExp, then the binary instruction must be a binaryfunccall too
 *********************************************************/
bool RoseBin_CompareAnalysis::isFunctionCall(SgNode* srcNode,
                                             SgAsmNode* binNode,
					     string *output,
					     int &nodes_matched) {
  bool isfunction=false;
  cerr << "   inside isFunctionCall " << endl;
  SgFunctionCallExp* srcCall = isSgFunctionCallExp(srcNode);
  if (srcCall !=NULL ) {
    SgAsmx86Instruction* binCall = isSgAsmx86Instruction(binNode);
    if (binCall && binCall->get_kind() == x86_call) {
      *output+= " !! functionCall ............... \n";
      nodes_matched++;
      srcCall->setAttribute(attributeName,createAttribute(2));
      binCall->setAttribute(attributeName,createAttribute(2));
      binCall->set_comment(srcCall->class_name());
      isfunction=true;
    }
  }
  return isfunction;
}

bool RoseBin_CompareAnalysis::isSgPlusPlus(SgNode* srcNode,
                                           SgAsmNode* binNode,
					   string *output,
					   int &nodes_matched) {
  bool isplus=false;
  cerr << "   inside isSgPlusPlus " << endl;
  SgPlusPlusOp* srcCall = isSgPlusPlusOp(srcNode);
  if (srcCall !=NULL ) {
    cerr << "   inside isSgPlusPlus found PlusPlus " << endl;
    SgAsmx86Instruction* binCall = isSgAsmx86Instruction(binNode);
    if (binCall && binCall->get_kind() == x86_inc) {
      *output+= " !! SgPlusPlus ............... \n";
      nodes_matched++;
      srcCall->setAttribute(attributeName,createAttribute(2));
      binCall->setAttribute(attributeName,createAttribute(2));
      binCall->set_comment(srcCall->class_name());
      isplus=true;
    }
  }
  return isplus;
}


/**********************************************************
 * check if we have a mov (varDecl) or mov (assign) --------------------------------------------------------
 * SgVariableDeclaration && SgAssignOp (including SgAddOp && SgSubtractOp)
 * if we encounter a varDecl, the binary part must be a mov instruction
 *   isVarDecl0 = lefthandside, isVarDecl1 = righhandside
 * if we encounter a assign, the binary part must be a mov instruction
 *   isAssign0 = lefthandside, isAssign1 = rightHandside
 *
 * if varDecl && mov, then we have a variable declaration
 *   but isVarDecl0 && isVarDecl1 must be true (pattern for variable Declaration) 
 *
 * check if the initialized value exist, and if it is of type integer (doublewordvalue)
 *   if yes, set attributes for proper visualization
 *********************************************************/
bool RoseBin_CompareAnalysis::isVariableDeclaration(SgNode* srcNode,
						    SgAsmNode* binNode,
						    string *output,
						    int &nodes_matched,
						    int array_bin_length,
						    int &bin_count,
						    SgNode* src_statements[],
						    SgAsmNode* bin_statements[],
						    bool &increase_source
						    ) {
  bool isvar=false;
  cerr << "   inside isVariableDeclaration " << endl;
  SgVariableDeclaration* varDecl = isSgVariableDeclaration(srcNode);
  SgAsmx86Instruction* mov = isSgAsmx86Instruction(binNode);
  // check the children of the varDecl , make a disctinction between
  // a) the variable is uninitialized
  // b) the variable is initialized (value on right side) , i.e. there is no corresponding binNode ( hence skip this source node)
  string name="";
  string value="";
  if (varDecl) {
    SgInitializedNamePtrList ptrList = varDecl->get_variables();
    SgInitializedName* nameInit = *(ptrList.begin());
    ROSE_ASSERT(nameInit);
    name =nameInit->get_qualified_name().str();
    SgAssignInitializer* assignInit = isSgAssignInitializer(nameInit->get_initializer());
    if (assignInit) {
      // in this case the variable has an assigned value and we can check the var in the binary
    } else {
      // in this case we cant find this variable in the binary and return
      cerr << " This is a variable declaration .. that has no value . Skipping src node." << endl;
      *output += " This is a variable declaration .. that has no value . Skipping src node.\n" ;
      // increase the source node to the next one
      varDecl->setAttribute(attributeName,createAttribute(2));
      increase_source=true;
      return true;
    }
  }

  if (varDecl && mov && mov->get_kind() == x86_mov) {
    bool isVarDecl0=false;
    bool isVarDecl1=false;
    valExp=NULL;
    refExp_Left=NULL;
    refExp_Right=NULL;


    string left="reg";
    string right="none";
    bool a=false,b=false;
    //SgAsmValueExpression* thenode;
    string roh_val = resolve_binaryInstruction(mov, &left, 
                                               &value, name);
    resolve_bin_vardecl_or_assignment( isVarDecl0,  isVarDecl1, 
                                       a,b, mov);
    *output += " varDecl (left,right) = " + RoseBin_support::resBool(isVarDecl0) + "-" + RoseBin_support::resBool(isVarDecl1) + " , " +
      "\n";
    cerr << " varDecl (left,right) = " << RoseBin_support::resBool(isVarDecl0) << "-" << RoseBin_support::resBool(isVarDecl1) << " , " <<
      "\n";

    string varname = getVariableName(roh_val);
    SgAsmValueExpression* valueExp = getVariableType(roh_val);
    string vartype="not valid";
    if (valueExp)
      vartype = valueExp->class_name();
    cerr << " >>> val: " << roh_val << "  getting var " << varname <<
      "  type: " << vartype << endl; 


    if (isVarDecl0 && isVarDecl1) {
      *output += " !! variableDecl ............... \n";
      // make sure it is a variableDecl
      ROSE_ASSERT(refExp_Left);
      ROSE_ASSERT(valExp);
      SgInitializedNamePtrList& ptrList = varDecl->get_variables();
      SgInitializedName* initName =*(ptrList.begin());
      ROSE_ASSERT(initName);
      SgInitializer* init = initName->get_initptr();
      if (isSgAssignInitializer(init)) {
	SgAssignInitializer* assInit = isSgAssignInitializer(init);
	ROSE_ASSERT(assInit);
	SgExpression* op = assInit->get_operand();
	if (isSgIntVal(op) && isSgAsmDoubleWordValueExpression(valExp)) {
	  // we got a integer && DoubleWord match
	  nodes_matched++;
	  isvar=true;
	  varDecl->setAttribute(attributeName,createAttribute(2));
	  mov->setAttribute(attributeName,createAttribute(2));
	  mov->set_comment("mov SS:[" + name +"] , "+value);
	  //local_last_variable = roh_val;
	  isSgIntVal(op)->setAttribute(attributeName,createAttribute(2));
	  isSgAsmDoubleWordValueExpression(valExp)->setAttribute(attributeName,createAttribute(2));
	  isSgAsmx86RegisterReferenceExpression(refExp_Left)->setAttribute(attributeName,createAttribute(2));
	}
      }
    }
  }
  return isvar;
}

/****************************************************
 * check if binary operands are a vardecl or assignment
 ****************************************************/
void RoseBin_CompareAnalysis::resolve_bin_vardecl_or_assignment(bool &isVarDecl0, 
								bool &isVarDecl1,
								bool &isAssign0,
								bool &isAssign1,
								SgAsmx86Instruction* mov
								) {
  SgAsmOperandList* opList = mov->get_operandList();
  SgAsmExpressionPtrList exprList = opList->get_operands();
  SgAsmExpressionPtrList::iterator it = exprList.begin();
  int iteration=0;
  for (;it!=exprList.end();it++) {
    SgAsmExpression* binExp = (*it);
    ROSE_ASSERT(binExp);
    //cerr << " type " << binExp->class_name() << endl;
    if (iteration==0) {
      refExp_Left = isSgAsmx86RegisterReferenceExpression(binExp);
      if (refExp_Left ) {
	// first operand is refExp (variabledecl || assignOp)
        if (refExp_Left->get_descriptor().get_major() == x86_regclass_segment &&
            refExp_Left->get_descriptor().get_minor() == x86_segreg_ss) { 
	  isVarDecl0=true;
	  isAssign0=true;
	}	else
	  isAssign0=true;
      }
    }
    if (iteration==1) {
      refExp_Right = isSgAsmx86RegisterReferenceExpression(binExp);
      valExp = isSgAsmValueExpression(binExp);
      if (valExp ) {
	// second operand is Value (variabledecl)
	isVarDecl1=true;
      }
      if (valExp || refExp_Right) {
	// second operand is refExp (assignOp) 
	isAssign1=true;
      }
    }
    iteration++;
  }
}


/**********************************************************
 * check if we have a mov (varDecl) or mov (assign) --------------------------------------------------------
 * SgVariableDeclaration && SgAssignOp (including SgAddOp && SgSubtractOp)
 * if we encounter a varDecl, the binary part must be a mov instruction
 *   isVarDecl0 = lefthandside, isVarDecl1 = righhandside
 * if we encounter a assign, the binary part must be a mov instruction
 *   isAssign0 = lefthandside, isAssign1 = rightHandside
 *
 * if assign, then an assignment occured
 * 
 * check the right hand side of the assignment:
 *   we need to know if the right hand side consists of more expressions
 *   if it does, we need to check all possibilities, e.g.
 *   we need to check if the next src statement is an sgAdd or SgSub,
 *   if yes, then we need to use it against the next binary instruction, i.e.
 *   we increase the source counter
 *
 * this needs to be changed to detecting whether we are accessing a variable
 * and whether it is the same variable
 *********************************************************/
bool RoseBin_CompareAnalysis::isAssignOp(SgNode* srcNode,
                                         SgAsmNode* binNode,
					 string *output,
					 int &nodes_matched,
					 int array_bin_length,
					 int array_src_length,
					 int &bin_count,
					 int src_count,
					 SgNode* src_statements[],
					 SgAsmNode* bin_statements[],
					 bool &increase_source
					 ) {
  bool isassign=false;
  cerr << "   inside isAssignOp    " << endl;
  SgAssignOp* assign = isSgAssignOp(srcNode);
  SgAsmx86Instruction* mov = isSgAsmx86Instruction(binNode);
  if ( assign && mov && mov->get_kind() == x86_mov) {
    cerr << "      inside isAssignOp && mov " << endl;
    // in this case it is clear what is happening
    // we have src: assignOp and bin: mov
    // they match, and we can calculate the match
    bool isAssign0=false;
    bool isAssign1=false;

    // found a matching mov for the assignment
    valExp=NULL;
    refExp_Left=NULL;
    refExp_Right=NULL;
    string registerName="reg";
    string variable="none";
    bool a=false,b=false;

    // get the name of the variable on the left hand side
    string name = "none";
    SgVarRefExp* varref = isSgVarRefExp(assign->get_lhs_operand());
    if (varref) {
      name = varref->get_symbol()->get_name().str();
    }
    string roh_val = resolve_binaryInstruction(mov, &registerName, 
                                               &variable, name);
    resolve_bin_vardecl_or_assignment( a,  b, 
                                       isAssign0, isAssign1, mov);

    string varname = getVariableName(roh_val);
    SgAsmValueExpression* valueExp = getVariableType(roh_val);
    string vartype="not valid";
    if (valueExp)
      vartype = valueExp->class_name();
    cerr << "         val: " << roh_val << "  getting var " << varname <<
      "  type: " << vartype << endl; 
    
    *output += "  assign (left, right) = " +
      RoseBin_support::resBool(isAssign0) +"-"+ RoseBin_support::resBool(isAssign1) + "\n";

    cerr << "  assign && mov (left, right) = " <<
      RoseBin_support::resBool(isAssign0) <<"-"<< RoseBin_support::resBool(isAssign1) << "\n";
     	  
    if (isAssign0 && isAssign1) {
      ROSE_ASSERT(mov);
      *output+= " !! assignOp ............... \n" ;
      nodes_matched++;
      mov->setAttribute(attributeName,createAttribute(2));
      ROSE_ASSERT(assign);
      mov->set_comment("mov "+ registerName+", "+variable);
      isassign=true;
      assign->setAttribute(attributeName,createAttribute(2));
      // we need to traverse down the source tree and see if there is more source nodes to process
      // if there are, we need to identify the correct bin nodes
      SgExpression* expr = assign->get_rhs_operand();
      ROSE_ASSERT(expr);
      // if mov shows to the same variable as the last src assignment
      // then we dont want to increase_bool_count because then the assignment
      // can be skipped i.e. SgAssign has no counterpart in bin
      // but if the variables differ, we want to increase the bool_count
      // i.e. dont_increase = false
      bool increase_bool_count=true;
      bool success = handleSourceExpression(expr,
					    &src_statements[0],
					    &bin_statements[0],
					    output,
					    bin_count,
					    src_count,
					    nodes_matched,
					    array_bin_length,
					    array_src_length,
					    assign,
					    increase_bool_count);
      if (!success) {
	cerr << " !! no success .. increasing only source . " << endl;
	increase_source=true;
      }
      //      last_left_source_var=varname;
    }
  } // else if
  
  else  if ( assign && (mov==NULL || mov->get_kind() != x86_mov)) {
    cerr << "      inside isAssignOp && mov==NULL " << endl;
    // in this case the bin is not a mov, this can result from the
    // fact that we use a variable from the last assign, e.g.

    // x = y +3;
    // x = z +5;
    // there might be a couple of instructions following one assignment (to the same register)
    // 8048348:       mov    eax,DWORD PTR [ebp-4]
    // 804834b:       sub    DWORD PTR [ebp-8],eax
    // 804834e:       add    DWORD PTR [ebp-8],0x4
    // where the second add has no assign (but the src does!)

    // we need to determine first, what the bin node is:
    // if it is a variable, determine if it is the last variable used in the registers
    string left="none";
    string right="none";
    SgAsmInstruction* instr = isSgAsmInstruction(binNode);
    ROSE_ASSERT(instr);
    string roh_val = resolve_binaryInstruction(instr, &left, 
                                               &right, "none");
    // we assume we are in an assignment (since SgAssignOp)
    // in this case "left" must be a variable
    *output += " >>>>> checking for variable : " + roh_val +
      "(" + left + ") against last variable \n" ;
    //local_last_variable + "\n";

    assign->setAttribute(attributeName,createAttribute(2));
    SgExpression* expr = assign->get_rhs_operand();
    ROSE_ASSERT(expr);
    
    if (roh_val!="" && expr) {
      // we are assigning a value to the last modified variable
      // this is ok, so we do not need the assignOp (src) and can skip
      // the source to the next src node, e.g. subOp
      cerr << " assign :: trying to handle " << expr->class_name() << " vs. " << binNode->class_name() << endl;
      bool increase_bool_count=false;
      bool success = handleSourceExpression(expr,
					    &src_statements[0],
					    &bin_statements[0],
					    output,
					    bin_count,
					    src_count,
					    nodes_matched,
					    array_bin_length,
					    array_src_length,
					    assign,
					    increase_bool_count);
      if (!success) 
	increase_source=true;
    } else {
      cerr << " !!! ERROR : AssignOp : We cant find the corresponding binary node: >> " << binNode->class_name() << endl;
      //exit(0);
    }
    
  }
  return isassign;  
}

void RoseBin_CompareAnalysis::pushOnStack(SgExpression* expr){
  if (!isSgVarRefExp(expr))
    srcNodesTodo.push(expr);
}


SgExpression* RoseBin_CompareAnalysis::isExpression( SgExpression* expr,
                                                     SgNode* src_statements[],
						     SgAsmNode* bin_statements[],
						     string *output,
						     int &bin_count,
						     int src_count,
						     int &nodes_matched,
						     int array_bin_length,
						     int array_src_length,
						     bool &keep_binary_node) {

  ROSE_ASSERT(expr);
  cerr << "   ---- inside isExpression " << expr->class_name() << endl;

  if (isSgSubtractOp(expr)) {
    SgSubtractOp* subop = isSgSubtractOp(expr);
    SgAsmNode* binNode = NULL;
    // get the possibleAddOp on the binary side
    binNode = bin_statements[(bin_count)];
    ROSE_ASSERT(binNode);
    cerr << "      checking isSgSubstractOp against " << binNode->class_name() << 
      " " << binNode << endl;
    SgAsmx86Instruction* sub = isSgAsmx86Instruction(binNode);
    if (sub && sub->get_kind() == x86_sub) {
      *output += "   !! subOp ............... \n" ;
      cerr << "      found SgSubtract : binCount: " << bin_count << endl;
      (nodes_matched)++;
      string left="";
      string right="";
      string roh_val = resolve_binaryInstruction(sub, &left, 
                                                 &right,"none" );
      sub->set_comment("sub "+left+","+right);
      if (tookSrcNodeFromStack) {
	subop->setAttribute(attributeName,createAttribute(3));
	sub->setAttribute(attributeName,createAttribute(3)); 
      } else {
	subop->setAttribute(attributeName,createAttribute(2));
	sub->setAttribute(attributeName,createAttribute(2));
      }
      //      string isVariable = getVariableName(roh_val);
      //if (isVariable!="")
      //local_last_variable = roh_val;
      if (tookSrcNodeFromStack)
	expr=NULL;
      else {
	expr = subop->get_rhs_operand();
	pushOnStack(subop->get_lhs_operand());
      }
      if (expr!=NULL)
	cerr << "  next expr " << expr->class_name() << endl;
    } else {
      // apperently we have not found this binary node,
      // we want to skip it for now but remember it for the next run
      expr = subop->get_rhs_operand();
      keep_binary_node=true;
      cerr << "        ++++++++++  Not a SgSubtractOp. skipping and remembering this node.  " << endl;
      //switchSrcStatements(&src_statements[0], src_count, src_count+1, array_src_length);
      srcNodesTodo.push(subop);
    }
  } else

    if (isSgAddOp(expr)) {
      SgAddOp* addop = isSgAddOp(expr);
      SgAsmNode* binNode = NULL;
      // get the possibleAddOp on the binary side
      binNode = bin_statements[(bin_count)];
      ROSE_ASSERT(binNode);
      cerr << "      checking isSgAddOp against " << binNode->class_name() << 
	" " << binNode << endl;
      SgAsmx86Instruction* add = isSgAsmx86Instruction(binNode);
      if (add && add->get_kind() == x86_add) {
	cerr << "       found addOp. binCount: " << bin_count << endl;
	*output += "   !! addOp ............... \n" ;
	(nodes_matched)++;
	string left="";
	string right="";
	string roh_val = resolve_binaryInstruction(add, &left, 
                                                   &right,"none" );
        addop->setAttribute(attributeName,createAttribute(2));
	add->setAttribute(attributeName,createAttribute(2));
	add->set_comment("add "+left+","+right);
	//string isVariable = getVariableName(roh_val);
	//if (isVariable!="")
	//local_last_variable = roh_val;
	if (tookSrcNodeFromStack)
	  expr=NULL;
	else {
	  expr = addop->get_rhs_operand();
	  pushOnStack(addop->get_lhs_operand());
	}
	if (expr!=NULL)
	  cerr << "  next expr " << expr->class_name() << endl;
      } else {
	// apperently we have not found this binary node,
	// we want to skip it for now but remember it for the next run
	cerr << "   Not a SgAddOp. skipping and remembering this node.  " << endl;
	expr = addop->get_rhs_operand();
	keep_binary_node=true;
	//	switchSrcStatements(&src_statements[0], src_count, src_count+1,array_src_length);
	srcNodesTodo.push(addop);
      }
    }  else

      if (isSgMultiplyOp(expr)) {
	SgMultiplyOp* addop = isSgMultiplyOp(expr);
	SgAsmNode* binNode = NULL;
	// get the possibleAddOp on the binary side
	binNode = bin_statements[(bin_count)];
	SgAsmNode* binNodeNext = bin_statements[(++bin_count)];
	SgAsmNode* binNodeNextNext = bin_statements[(++bin_count)];
	ROSE_ASSERT(binNode);
	ROSE_ASSERT(binNodeNext);
	ROSE_ASSERT(binNodeNextNext);
	cerr << "      checking isSgMultiplyOp against " << binNode->class_name() <<
	  " " << binNode << endl;
	SgAsmx86Instruction* mov = isSgAsmx86Instruction(binNode);
	SgAsmx86Instruction* shl = isSgAsmx86Instruction(binNodeNext);
	SgAsmx86Instruction* lea = isSgAsmx86Instruction(binNodeNextNext);
	if (mov && mov->get_kind() == x86_mov && shl && shl->get_kind() == x86_shl) {
	  *output += "   !! multiplyOp ............... \n" ;
	  cerr << "       found multiOp. binCount: " << bin_count << endl;
	  (nodes_matched)++;
	  string left="";
	  string right="";
	  string roh_val = resolve_binaryInstruction(mov, &left, 
                                                     &right,"none" );

	  addop->setAttribute(attributeName,createAttribute(2));
	  mov->setAttribute(attributeName,createAttribute(2));
	  mov->set_comment("mul "+left+","+right);
	  shl->setAttribute(attributeName,createAttribute(2));
	  shl->set_comment("mul "+left+","+right);
          ROSE_ASSERT (lea && lea->get_kind() == x86_lea);
	  lea->setAttribute(attributeName,createAttribute(2));
	  lea->set_comment("mul "+left+","+right);
	  
	  if (tookSrcNodeFromStack)
	    expr=NULL;
	  else {
	    expr = addop->get_rhs_operand();
	    pushOnStack(addop->get_lhs_operand());
	  }
	  if (expr!=NULL)
	    cerr << "  next expr " << expr->class_name() << endl;
	} else {
	  // apperently we have not found this binary node,
	  // we want to skip it for now but remember it for the next run
	  expr = addop->get_rhs_operand();
	  keep_binary_node=true;
	  cerr << "   Not a SgMultiplyOp. skipping and remembering this node.  " << endl;
	  //	switchSrcStatements(&src_statements[0], src_count, src_count+1,array_src_length);
	  srcNodesTodo.push(addop);
	}
	//	binNode = binNodeNextNext;
      } else 

	if (isSgDivideOp(expr)) {
	  SgDivideOp* addop = isSgDivideOp(expr);
	  SgAsmNode* binNode = NULL;
	  // get the possibleAddOp on the binary side
	  binNode = bin_statements[(bin_count)];
	  ROSE_ASSERT(binNode);
	  cerr << "      checking isSgDivideOp against " << binNode->class_name() << endl;
	  SgAsmx86Instruction* shr = isSgAsmx86Instruction(binNode);
	  if (shr && shr->get_kind() == x86_shr) {
	    *output += "   !! divideOp ............... \n" ;
	    (nodes_matched)++;
	    string left="";
	    string right="";
	    string roh_val = resolve_binaryInstruction(shr, &left, 
                                                       &right,"none" );

	    addop->setAttribute(attributeName,createAttribute(2));
	    shr->setAttribute(attributeName,createAttribute(2));
	    shr->set_comment("div "+left+","+right);
	    //string isVariable = getVariableName(roh_val);
	    //if (isVariable!="")
	    //local_last_variable = roh_val;
	    if (tookSrcNodeFromStack)
	      expr=NULL;
	    else {
	      expr = addop->get_rhs_operand();
	      pushOnStack(addop->get_lhs_operand());
	    }
	    if (expr!=NULL)
	      cerr << "  next expr " << expr->class_name() << endl;
	  } else {
	    // apperently we have not found this binary node,
	    // we want to skip it for now but remember it for the next run
	    expr = addop->get_rhs_operand();
	    keep_binary_node=true;
	    cerr << "   Not a SgDivideOp. skipping and remembering this node.  " << endl;
	    //	switchSrcStatements(&src_statements[0], src_count, src_count+1,array_src_length);
	    srcNodesTodo.push(addop);
	  }

	} 

	else {
	  cerr << " !!! WARNING : isExpression :: could not resolve the expression : " << expr->class_name() << endl;

	  expr=NULL;
	  //    exit(0);
	}

  if (expr==NULL) {
    if (!srcNodesTodo.empty()) {
      expr = srcNodesTodo.top();
      srcNodesTodo.pop();
      cerr << "   >>>>> checking stack : found : " << expr->class_name() <<endl;
      tookSrcNodeFromStack=true;
      keep_binary_node=true;
      //one_call_only=true;
    } else {
      tookSrcNodeFromStack=false;
    }
  }

  return expr;
}

bool RoseBin_CompareAnalysis::isAnExpression(SgExpression* expr) {
  if ((isSgAddOp(expr) || isSgSubtractOp(expr) || isSgMultiplyOp(expr) ||
       isSgDivideOp(expr))) 
    return true;
  return false;
}

bool RoseBin_CompareAnalysis::handleSourceExpression(
                                                     SgExpression* expr,
						     SgNode* src_statements[],
						     SgAsmNode* bin_statements[],
						     std::string *output,
						     int &bin_count,
						     int src_count,
						     int &nodes_matched,
						     int array_bin_length,
						     int array_src_length,
						     SgAssignOp* assign,
						     bool increase_bin_count) {
  if (!isAnExpression(expr))
    return true;
  bool success =true;
  bool keep_binary_node=false;
  tookSrcNodeFromStack=false;
  while (expr!=NULL && !isSgVarRefExp(expr)) {
    // we have found a new expression within the last expression tree
    // i.e. there is another expression within that statement.
    // we must find the binary instuction for it
    *output += ">>>> found a subNode : " + expr->class_name() + " ... lets check for binNode \n";
    cerr << "    handleSourceExpr :: found subnode " << expr->class_name() << endl;
    // take the next binary node and check it against this source node
    // we need to increase the bin_count, if we look for
    // the next match of a binary node
    if (keep_binary_node) {
      keep_binary_node=false;
    } else {
      // increase this only if the last variable assigned to, is different than current
      if (increase_bin_count) {
	++bin_count;
	cerr << "!!!! increasing bin_count to " << bin_count << endl;
      }
    }
    if (bin_count>=array_bin_length) {
      cerr << " FATAL ERROR :: bin_count too large " << endl;
      exit(0);
    }
    expr = isExpression(expr,	
			&src_statements[0],
			&bin_statements[0],
			output,
			bin_count,
			src_count,
			nodes_matched,
			array_bin_length,
			array_src_length,
			keep_binary_node
			
			);
  } // while loop
  //  if (expr==NULL) 
  //success=false;
  //  if (expr==NULL && one_call_only)
  //bin_count++;
  return success;
}


/****************************************************
 * compare two arrays
 * one filled with src statements and one filled with bin instructions
 ****************************************************/
int RoseBin_CompareAnalysis::match_statements(int array_src_length,
					      int array_bin_length,
					      SgNode* src_statements[],
					      SgAsmNode* bin_statements[],
					      string *output) {

  /**********************************************************
   * Check preconditions
   *********************************************************/

  int nodes_matched=0;
  int old_round_matched=-1;
  int src_count = -1;
  int bin_count = -1;
  bool src_done=false;
  bool bin_done=false;
  if (src_count >= (array_src_length-1))
    src_done=true;
  if (bin_count >= (array_bin_length-1))
    bin_done=true;
  SgAsmNode* binNode=NULL;

  /**********************************************************
   * Iterate through the binary and source array until 
   * BOTH have reached their end
   *********************************************************/
  bool increase_only_src = false;
  while ( !src_done || !bin_done) { 
    // compare source and binary instrutions
    if (nodes_matched==old_round_matched || increase_only_src) {
      // if one round has no matches, then we try to match the same src
      // against the next bin instruction
      if (increase_only_src) {
	if (src_count < (array_src_length-1))
	  src_count++;
	increase_only_src = false;
	cerr << " >> increasing src  " << src_count << endl;
      } else {
	cerr << " >> increasing bin  " << bin_count << endl;
	if (bin_count < (array_bin_length-1))
	  bin_count++;
      }
    } else {
      // else increase both
      cerr << " >> increasing both src and bin " << src_count << " " << bin_count << endl;
      if (src_count < (array_src_length-1))
	src_count++;
      if (bin_count < (array_bin_length-1))
	bin_count++;
      cerr << " >> new round : src and bin " << src_count << " " << bin_count << endl;
    }
    old_round_matched = nodes_matched;

    if (src_count==array_src_length)
      src_count--;
    if (bin_count==array_bin_length)
      bin_count--;

    SgNode* srcNode = src_statements[src_count];
    ROSE_ASSERT(srcNode);
    if (isSgExpression(srcNode))
      srcNode = isSgExpression(srcNode);

    binNode = bin_statements[bin_count];
    ROSE_ASSERT(binNode);

    *output += " >>> checking next nodes: " + srcNode->class_name() +"  vs.  " + binNode->class_name() + "\n";
    cerr << " >>> checking next nodes: " << srcNode->class_name() <<
      " (" << srcNode << ")  vs.  " << binNode->class_name() << "("<<binNode << ") bin_count: "<<bin_count << endl;
    if ((src_count+1) < (array_src_length)) {
      SgNode* srcNext = src_statements[src_count+1];
      cerr << " the next src node is : " << srcNext->class_name() << " " << srcNext <<endl;
    }
    if ((bin_count+1) < (array_bin_length)) {
      SgAsmNode* binNext = bin_statements[bin_count+1];
      cerr << " the next bin node is : " << binNext->class_name() << " " << binNext << endl; 
    }

    /********************************************
     * check for various patterns iteratively
     * compare each source statment with each binary statement
     ********************************************/
    if (isFunctionCall(srcNode, binNode, output, nodes_matched)) {
      cerr << " >>> found FunctionCall . \n";
    } else

      if (isVariableDeclaration(srcNode, binNode, output, nodes_matched,
				array_bin_length, bin_count, 
				&src_statements[0],
				&bin_statements[0],
				increase_only_src)) {
	cerr << " >>> found VariableDeclaration . Increasing source only ? " << RoseBin_support::resBool(increase_only_src) << endl;
      } else

	if (isAssignOp(srcNode, binNode, output, nodes_matched,
                       array_bin_length, array_src_length,bin_count, src_count,
		       &src_statements[0],
		       &bin_statements[0],
		       increase_only_src)) {
          cerr << " >>> found AssignOp . Increasing source only ? " << RoseBin_support::resBool(increase_only_src) << endl;
	} else

	  if (isSgPlusPlus(srcNode, binNode, output, nodes_matched)){
	    cerr << " >>> found SgPlusPlus . \n";
	  } 

	  else {
	    cerr << " !!! ERROR : >>>> > cant resolve " << srcNode->class_name() <<
	      " ( " << binNode->class_name() << ")" << endl;
	    nodes_matched++;
	    binNode->setAttribute(attributeName,createAttribute(1));
	    srcNode->setAttribute(attributeName,createAttribute(1));
	  }

    cerr << " >>> finished checking  nodes: " << srcNode->class_name() << "  vs.  " << binNode->class_name() << "\n" << endl;

    // ********************************************************
  
    if (nodes_matched==old_round_matched && !increase_only_src) {
      ROSE_ASSERT(binNode);
      binNode->setAttribute(attributeName,createAttribute(1));
      srcNode->setAttribute(attributeName,createAttribute(1));
      *output += " +++ unrecognized binNode :: " + binNode->class_name() + "\n";
      cerr << " +++ unrecognized binNode : " << binNode->class_name() << "\n";
    } else {
      cerr << " >>>>>>>> found function match : " << RoseBin_support::ToString(nodes_matched) <<"\n" << endl;
    }
    if (src_count== (array_src_length-1))
      src_done=true;
    if (bin_count== (array_bin_length-1))
      bin_done=true;
    //        cerr << " loop : " << src_count << " - " << bin_count << endl;
  }
  return nodes_matched;
}


/****************************************************
 * compare a src function and a binary function
 * in particular the sgnodes and instructions
 ****************************************************/
void RoseBin_CompareAnalysis::checkFunctions(string name,
                                             SgFunctionDeclaration* funcDecl,
					     SgAsmFunctionDeclaration* binDecl) {

  string output="\n**** *************************\n";
  /*
  //  std::list <SgNode*> src_statements;
  // get statements in src
  local_vars.clear();
  // **********************************************************
  // * Create an array of all the source statements
  // *********************************************************

  SgBasicBlock* bb_src = funcDecl->get_definition()->get_body();
  ROSE_ASSERT(bb_src);
  SgStatementPtrList& stmts = bb_src->get_statements();
  SgStatementPtrList::iterator it = stmts.begin();
  int list_s=0;
  for (;it!=stmts.end();it++) {
  list_s++;
  }
  const int list_size= list_s;
  // failes because : constant value is not known
  SgNode* src_statements[list_size];
  it = stmts.begin();
  int count=0;		       
  for (;it!=stmts.end();it++) {
  SgStatement* state = isSgStatement(*it);
  // if the next node is an SgExprStatement, we want to filter it out
  SgExprStatement* exprstmt = isSgExprStatement(state);
  if (exprstmt!=NULL) {
  SgExpression* expr = exprstmt->get_expression();
  // its an expression
  output += name + ": found expr:" + expr->class_name() +"\n";
  src_statements[count++]=expr;
  } else {
  // its a statement
  string childIndex = RoseBin_support::ToString(bb_src->getChildIndex(state));
  output += name + ": found stmt:" + state->class_name() + "    child nr: " + childIndex + "\n";
  src_statements[count++]=state;
  }
  }

  // **********************************************************
  // * Create an array of all the binary instructions
  // *********************************************************

  //  std::list <SgAsmNode*> bin_statements;

  std::list<SgAsmStatement*> stats;
  set <SgAsmStatement*> filter;
  const int list_size2=0;

  // (10/8/2007) commented out that a declaration may have a block
  // this code wont work anymore

  
  SgAsmBlockPtrList bb_bin_l = binDecl->get_body();
  SgAsmBlockPtrList::iterator itbp = bb_bin_l.begin();
  for (; itbp!=bb_bin_l.end(); ++itbp) {
  SgAsmBlock* bb_bin = *itbp;

  SgAsmStatementPtrList& binstmts = bb_bin->get_statementList();
  SgAsmStatementPtrList::iterator itb = binstmts.begin();
  for (;itb!=binstmts.end();itb++) {
  SgAsmStatement* state = isSgAsmStatement(*itb);
  bool valid = instruction_filter(state, name,&output);
  if (valid) {
  list_size++;
  stats.push_back(state);
  } else
  filter.insert(state);
  }
  }
  

  SgAsmNode* bin_statements[list_size2];
  std::list<SgAsmStatement*>::iterator itb = stats.begin();
  count=0;		       
  for (;itb!=stats.end();itb++) {
  SgAsmStatement* state = isSgAsmStatement(*itb);
  // apply filter for certain known instructions
  set<SgAsmStatement*>::iterator filter_set_it = filter.find(state);
  set<SgAsmStatement*>::iterator filter_set_end = filter.end();
  bool filter=true;
  if (filter_set_it==filter_set_end)
  filter=false;
  if (!filter) {
  //string childIndex = RoseBin_support::ToString(bb_bin->getChildIndex(state));
  output += name + ": found bin stmt:" + state->class_name() + 
  //	"    child nr: " + childIndex + 
  "    valid instr : " + RoseBin_support::resBool(!filter) + 
  "  count " + RoseBin_support::ToString(count) +"\n";
  bin_statements[count++]=state;
  }

  }
  

  int array_src_length = sizeof(src_statements)/sizeof(*src_statements);
  int array_bin_length = sizeof(bin_statements)/sizeof(*bin_statements);
  // compare src elements against binary elements

  // **********************************************************
  // * Check how many src and binary statements match
  // *********************************************************

  int nodes_matched = match_statements(array_src_length,
  array_bin_length,
  &src_statements[0],
  &bin_statements[0],
  &output);

  cerr << "\nfunction: " << name << " nr of matches :  " 
  << nodes_matched << " / " << array_bin_length << endl;

  // **********************************************************
  // * If the matched equals the amount of binary instructions
  // * then we have two identical functions
  // *********************************************************

  if (nodes_matched==array_bin_length) {
  funcDecl->setAttribute(attributeName,createAttribute(2));
  binDecl->setAttribute(attributeName,createAttribute(2));    
  cerr << " ******************* function match ..............." << endl;
  cout << output;
  } else {
  funcDecl->setAttribute(attributeName,createAttribute(1));
  binDecl->setAttribute(attributeName,createAttribute(1));    
  cerr << " function : " << name << "   size of src_statements: " << 
  array_src_length << "   size of bin_statements: " << array_bin_length << endl;
  output+="ERROR*************************\n\n";
  cerr << output;
  }
  */
}

/****************************************************
 * run the compare analysis
 ****************************************************/
void RoseBin_CompareAnalysis::run() {
  // first thing to do is to match up the functions
  // we want to get all src functions and check
  // against the binary functions and create a
  // map of mappings map(src_func,bin_func)

  // traverse Rose src and create map (name, src_func, null)
  // traverse Rose bin and create map (name, src_func, bin_func)
  create_map_functions();

  // traverse src children of each function and compare each statement
  // against each instruction of the bin children
  // ignore unecessary instructions from functions, e.g. pre and post
  // like push and pop 
  function_map_type::iterator it = function_map.begin();
  for (;it!=function_map.end();++it) {
    string name = it->first;
    cerr << " function to check " << name << endl;
    pair <SgFunctionDeclaration*, SgAsmFunctionDeclaration*> my_pair = it->second;
    SgFunctionDeclaration* funcDef = my_pair.first;
    SgAsmFunctionDeclaration* binDef = my_pair.second;
    checkFunctions(name,funcDef, binDef);
  }
  
}

