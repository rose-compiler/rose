/******************************************                                           
  * Category: DFA                                                                      
  * DefUse Analysis Declaration (per Function)                                         
  * created by tps in Feb 2007                                                         
  *****************************************/   

#include "DefUseAnalysis_perFunction.h"

using namespace std;


/**********************************************************
 *  find element in vector
 *********************************************************/
bool DefUseAnalysisPF::searchVector(vector <filteredCFGNodeType> done, 
				    filteredCFGNodeType cfgNode) {
  bool found = false;
  for (vector<filteredCFGNodeType >::const_iterator i = done.begin(); i != done.end(); ++i) {
    filteredCFGNodeType node = *i;
    if (node==cfgNode)
      found=true;
  }
  return found;
}


/**********************************************************
 *  For a CFG Node, follow the two incoming edges and get 
 *  the other node (than theNodeBefore)
 *********************************************************/
template <typename T>
SgNode* DefUseAnalysisPF::getOtherInNode(T cfgNode, SgNode* oneNode){
  SgNode* otherNode = NULL;  
  vector<filteredCFGEdgeType > in_edges = cfgNode.inEdges();
  for (vector<filteredCFGEdgeType >::const_iterator i = in_edges.begin(); i != in_edges.end(); ++i) {
    filteredCFGEdgeType filterEdge = *i;
    T filterNode = filterEdge.source();
    if (filterNode.getNode()!=oneNode)
      otherNode = filterNode.getNode();
  }  
  if (DEBUG_MODE_EXTRA)
    cout << "getOtherInNode:: other: " << otherNode << "  previous: " << 
      oneNode << "  size of in: " << in_edges.size() << endl;
  return otherNode;
}

/**********************************************************
 *  get the incoming node of a cfg edge
 *********************************************************/
template <typename T>
SgNode* DefUseAnalysisPF::getCFGPredNode(T cfgNode){
  SgNode* node = NULL;
  vector<filteredCFGEdgeType > in_edges = cfgNode.inEdges();
  for (vector<filteredCFGEdgeType >::const_iterator i = in_edges.begin(); i != in_edges.end(); ++i) {
    filteredCFGEdgeType filterEdge = *i;
    node = filterEdge.source().getNode();
  }  
  return node;
}

/**********************************************************
 *  Search for the value and key in the multimap
 *********************************************************/
bool DefUseAnalysisPF::isDoubleExactEntry(const multitype* multi, 
					  SgInitializedName* name, SgNode* sgNode) {
  bool isCurrentValueContained=false;
  multitype::const_iterator i = multi->begin();    
  //SgNode* sgNodeMM = NULL;
  for (; i != multi->end(); ++i) {
    SgInitializedName* initNameMM = (*i).first;
    SgNode* sgnodeMM = (*i).second;
    if (initNameMM==name && sgnodeMM==sgNode)
      isCurrentValueContained=true;
    // cout << " comparing -  map: " << initNameMM << " vs. search: " << initName <<
    //  "   result euqal? " << resBool(isCurrentValueContained) << endl;
  } 
  return isCurrentValueContained;
}

/**********************************************************
 *  Search for the value for a certain key in the multimap
 *********************************************************/
bool DefUseAnalysisPF::searchMulti(const multitype* multi, SgInitializedName* initName) {
  bool isCurrentValueContained=false;
  multitype::const_iterator i = multi->begin();    
  //SgNode* sgNodeMM = NULL;
  for (; i != multi->end(); ++i) {
    SgInitializedName* initNameMM = (*i).first;
    if (initNameMM==initName)
      isCurrentValueContained=true;
    // cout << " comparing -  map: " << initNameMM << " vs. search: " << initName <<
    //  "   result euqal? " << resBool(isCurrentValueContained) << endl;
  } 
  return isCurrentValueContained;
}

/**********************************************************
 *  check if two multimaps are equal
 *********************************************************/
bool DefUseAnalysisPF::checkElementsForEquality(const multitype* t1, const multitype* t2) {
  // if every element of t2 is contained in t1, then no change
  // occured in the map
  
  typedef set<pair<SgInitializedName*, SgNode*> > st;
  st s1(t1->begin(), t1->end());
  st s2(t2->begin(), t2->end());
  assert (s1.size() == t1->size());

  //  if (s2.size() != t2->size())
  //    printMultiMap(t2);
  assert (s2.size() == t2->size());
  return s1 != s2;
  /*


  if (t1->size()!=t2->size())
    return true;
  bool changed=false;
  for (multitype::const_iterator it = t1->begin(); it != t1->end(); ++it) {
    SgInitializedName* init1 = (*it).first;
    SgNode* node1 = (*it).second;
    bool identical=false;
    for (multitype::const_iterator it2 = t2->begin(); it2 != t2->end(); ++it2) {
      SgInitializedName* init2 = (*it2).first;
      SgNode* node2 = (*it2).second;
      if (init1==init2 && node1==node2)
	identical=true;
    }
    if (identical==false)
      changed = true;
  }  
  return changed;
  */
}


/**********************************************************
 *  print the values of a vector
 *********************************************************/
template <typename T >
void DefUseAnalysisPF::printCFGVector(vector< T > worklist) {
  int pos = 0;
  cout << "\n!! Worklist : " ;
  for (typename vector<T >::iterator i = worklist.begin(); i != worklist.end(); ++i, pos++) {  
    T node = *i;
    cout << " - " << node.toStringForDebugging();
  }
  cout << endl;
}

/**********************************************************
 * return the initializedName
 * we need this if we have a VarRefExp on the left hand
 * side of an assignment. We want to know which variable it is.
 * The variable may be hiding down the hirarchy in e.g. an array
 *********************************************************/
SgInitializedName* DefUseAnalysisPF::getInitName(SgNode* l_expr) {
  SgInitializedName* retName = NULL;
  if (isSgPntrArrRefExp(l_expr)) {
    SgPntrArrRefExp* varArrRefExp = isSgPntrArrRefExp(l_expr);
    if (DEBUG_MODE)
      cout << " **********  BINARY OP : " << l_expr->class_name() << endl;
    ROSE_ASSERT(varArrRefExp);
    SgExpression* l_expr = varArrRefExp->get_lhs_operand();
    ROSE_ASSERT(l_expr);
    if (isSgVarRefExp(l_expr)) {
      retName = isSgVarRefExp(l_expr)->get_symbol()->get_declaration();
    } 
  } else if (isSgPointerDerefExp(l_expr)) {
    SgPointerDerefExp* ptr = isSgPointerDerefExp(l_expr);
    if (DEBUG_MODE)
      cout << " **********  BINARY OP : " << l_expr->class_name() << endl;
    ROSE_ASSERT(ptr);
    SgExpression* l_expr = ptr->get_operand();
    ROSE_ASSERT(l_expr);
    if (isSgVarRefExp(l_expr)) {
      retName = isSgVarRefExp(l_expr)->get_symbol()->get_declaration();
    } 
  } else {
    cout << " GETINITNAME:: could not resolve initName " << l_expr->class_name() << endl;
  }
  return retName;
}

/**********************************************************
 * Resolve a CAST
 *********************************************************/
SgExpression* DefUseAnalysisPF::resolveCast(SgExpression* expr) {
  SgCastExp* cast = isSgCastExp(expr);
  SgExpression* retExpr = cast->get_operand();
  ROSE_ASSERT(retExpr);
  return retExpr;
}

/**********************************************************
 * Make sure all nodes (even arguments if &argument) are in table
 * so they can be found for analysis
 *********************************************************/
bool DefUseAnalysisPF::makeSureThatTheDefIsInTable(SgInitializedName* initName) {
  bool addedNode = false;
  multimap <SgInitializedName*, SgNode*> mymap = dfa->getDefMultiMapFor(initName);
  if (mymap.size()==0) {
    dfa->addDefElement(initName, initName, initName);
    addedNode=true;
    if (DEBUG_MODE)
      cout << " FUNCTIONCALL ::: adding parameter to table : " << initName->get_qualified_name().str() << endl;
  }
  if (DEBUG_MODE)
    cout << " nr for that node ( " << initName->get_qualified_name().str() 
	 << ") : " << dfa->getIntForSgNode(initName) << endl;
  return addedNode;
}

/**********************************************************
 * Make sure all nodes (even arguments if &argument) are in table
 * so they can be found for analysis
 *********************************************************/
bool DefUseAnalysisPF::makeSureThatTheUseIsInTable(SgInitializedName* initName) {
  bool addedNode = false;
  multimap <SgInitializedName*, SgNode*> mymap = dfa->getUseMultiMapFor(initName);
  if (mymap.size()==0) {
    dfa->addUseElement(initName, initName, initName);
    addedNode=true;
    if (DEBUG_MODE)
      cout << " FUNCTIONCALL ::: adding parameter to table : " << initName->get_qualified_name().str() << endl;
  }
  if (DEBUG_MODE)
    cout << " nr for that node ( " << initName->get_qualified_name().str() 
	 << ") : " << dfa->getIntForSgNode(initName) << endl;
  return addedNode;
}


/**********************************************************
 *  Build CFG for each function and do DefUse analysis
 *  This is the main analysis function
 *  It determines what should happen at each DFG node
 *********************************************************/
template <typename T>
bool DefUseAnalysisPF::defuse(T cfgNode, bool *unhandled) {
  SgNode* sgNode = cfgNode.getNode();
  SgNode* sgNodeBefore = getCFGPredNode(cfgNode);  
  ROSE_ASSERT(sgNode);

  bool dont_handle=false;
  bool dont_replace=false;
  if (DEBUG_MODE)
    cout << "\n\n------------------------------------------------------------------\ncurrent Node: " << 
      sgNode << "  previous Node : " << sgNodeBefore << endl;

  bool isDefinition=false;
  //  bool isUse=false;
  bool isUsage=false;
  SgInitializedName* initName =NULL;

  if (isSgUnaryOp(sgNode)) {
    breakPoint=0;
    SgUnaryOp* unary = isSgUnaryOp(sgNode);
    SgExpression* l_expr = unary->get_operand();
    SgVarRefExp* varRefExp = NULL;
    if (DEBUG_MODE)
      cout << " **********  UNARY OP. " << unary << endl;
    if (isSgVarRefExp(l_expr)) {
      // if left side is a varrefexp
      varRefExp = isSgVarRefExp(l_expr);
      initName = varRefExp->get_symbol()->get_declaration();  
    
      if (DEBUG_MODE)
	cout << " UNARY OP: " << initName->get_qualified_name().str() 
	     << "  name: " << initName << "  varRefExp: " << varRefExp << endl;
      ROSE_ASSERT(unary);
      switch(unary->variantT()) {
      case V_SgPlusPlusOp:
      case V_SgMinusMinusOp: {
	isDefinition=true;
	//isUse=true;
	break;
      }
      default: {
	isDefinition=false;
	//isUse=true;
	if (DEBUG_MODE)
	  cout << " **** reached default. no action taken. " << endl;
	break;
      }
      } 
    } 
    else {
      if (DEBUG_MODE)
	cout << " **********  UNARY OP : CANT HANDLE YET: " << l_expr->class_name() << endl;
      dont_handle = true;
    }
  }

  else if (isSgBinaryOp(sgNode)) {
    breakPoint=0;
    SgBinaryOp* binary = isSgBinaryOp(sgNode);
    SgExpression* l_expr = binary->get_lhs_operand();
    SgVarRefExp* varRefExp = NULL;
    if (DEBUG_MODE)
      cout << " **********  BINARY OP. " << binary << endl;
    if (isSgVarRefExp(l_expr)) {
      // if left side is a varrefexp
      if (DEBUG_MODE)
	cout << " **********  BINARY OP IS_VAR_REF_EXP. " << endl;

      varRefExp = isSgVarRefExp(l_expr);
      initName = varRefExp->get_symbol()->get_declaration();  
    
      if (DEBUG_MODE)
	cout << " BINARY OP: " << initName->get_qualified_name().str() 
	     << "  name: " << initName << "  varRefExp: " << varRefExp << endl;
      ROSE_ASSERT(binary);
      switch(binary->variantT()) {
      case V_SgAssignOp: {
	isDefinition=true;
	//isUse=false;
	break;
      }
      case V_SgModAssignOp:
      case V_SgDivAssignOp:
      case V_SgMultAssignOp:
      case V_SgLshiftAssignOp:
      case V_SgRshiftAssignOp:
      case V_SgXorAssignOp:
      case V_SgAndAssignOp:
      case V_SgMinusAssignOp:
      case V_SgPlusAssignOp: {
	isDefinition=true;
	//isUse=true;
	break;
      }
      default: {
	if (DEBUG_MODE)
	  cout << " **** reached default. no action taken. " << endl;
	isDefinition=false;
	//isUse=true;
	break;
      }
      } 
    } 
    else {
      if (DEBUG_MODE)
	cout << " **********  BINARY OP : CANT HANDLE YET: " << l_expr->class_name() << endl;
      dont_handle = true;
    }
  }  
  
  else if (isSgAssignInitializer(sgNode)) {
    breakPoint=0;
    if (DEBUG_MODE)
      cout << " **********  ASSIGNINITIALIZER " << endl;
    SgNode* l_expr = isSgAssignInitializer(sgNode)->get_parent();
    ROSE_ASSERT(l_expr);
    if (isSgInitializedName(l_expr)) {
      initName =isSgInitializedName(l_expr);
      if (DEBUG_MODE)
	cout << " **********  ASSIGNINITIALIZER: " << isSgAssignInitializer(sgNode) << endl;
      // if left side is not a varrefexp
      //isUse = false;
      isDefinition=true;
    } else if (isSgExprListExp(l_expr)) { 
      SgExprListExp* listexp = isSgExprListExp(l_expr);
      Rose_STL_Container<SgNode *> exprs =  
	listexp->get_traversalSuccessorContainer();
      cout << " DefuseAnalysis:: unhandled condition : SgExprListExp ...  size: " << exprs.size() << 
        "  file : " << l_expr->get_file_info()->get_filenameString() << 
	"  line : " << l_expr->get_file_info()->get_line() << endl;
      return false;
    }    else {
      cout << " DefuseAnalysis:: unhandled condition - AssignInitializer ... "  
         << "  AssignInitializer - l_expr : " << l_expr->class_name() << "  file : " << l_expr->get_file_info()->get_filenameString() << 
	"  line : " << l_expr->get_file_info()->get_line() << endl;
      cout << " unparse : " << l_expr->unparseToCompleteString() << endl;
      return false;
      //      exit(0);
    }
  }  
  
  else if (isSgInitializedName(sgNode)) {
    breakPoint=0;
    initName =isSgInitializedName(sgNode);
    if (DEBUG_MODE)
      cout << " **********  INITNAME. " << initName << endl;    
    //isUse=false;
    isDefinition=true;
    if (DEBUG_MODE)
      cout << " **********  INITNAME. use " //<< resBool(isUse) 
	   << " def " << resBool(isDefinition) << endl;    
  }  


  else if (isSgVarRefExp(sgNode)) {
    breakPoint=0;
    SgVarRefExp* varRefExp = isSgVarRefExp(sgNode);
    initName = varRefExp->get_symbol()->get_declaration();
    string name = initName->get_qualified_name().str();
    if (DEBUG_MODE)
      cout << " **********  VARREFEXP. " << varRefExp << " .. " << name << endl;     
    //isUse=true;
    isDefinition=false;
    SgNode* parent = varRefExp->get_parent();
    // go up to the parent and check if there is an assignment happening.
    // If yes, check if VarRefExp on right side,
    // if yes, we have a usage -- otherwise its a definition
    if (isSgAssignOp(parent) && isSgAssignOp(parent)->get_lhs_operand()==varRefExp) {
      // definition .. dont handle
    } else {
      // usage
      isUsage =true;
    }
  } // else if 

  else if (isSgFunctionCallExp(sgNode)) {
    breakPoint=0;
    SgFunctionCallExp* fcallExp = isSgFunctionCallExp(sgNode);
    isDefinition=false;
    //isUse=false;
    if (DEBUG_MODE)
      cout << " **********  FUNCTIONCALL. " << fcallExp << endl;     
    SgExprListExp* exprList = fcallExp->get_args();
    SgExpressionPtrList& list = exprList->get_expressions();
    bool isHandled=false;
    if (list.size()==0)
      dont_handle=true;
    else {
      int counter=0;
      SgExpressionPtrList::iterator i;
      SgNode* tmpBefore= sgNodeBefore;
      for (i = list.begin(); i != list.end(); ++i) {
	SgExpression* expr = *i;
	counter++;
	bool hit = false;
	if (isSgCastExp(expr))
	  expr = resolveCast(expr);
	if (isSgVarRefExp(expr)) {
	  SgVarRefExp* varRefExp = isSgVarRefExp(expr);
	  initName = varRefExp->get_symbol()->get_declaration();
	  if (DEBUG_MODE)
	    cout << counter <<" VarRefExp::: expr found in param: << " << expr->class_name() << 
	      "  initName: " << initName->get_qualified_name().str() << endl;
	  //isUse=true;
	} else if (isSgAddressOfOp(expr)) { 
	  if (DEBUG_MODE)
	    cout << counter << " AddressOfOp::: expr found in param: << " << expr->class_name() << endl; 
	  SgNode* node = isSgAddressOfOp(expr)->get_operand();
	  ROSE_ASSERT(node);
	  if (isSgVarRefExp(node)) {
	    SgVarRefExp* varRefExp = isSgVarRefExp(node);
	    initName = varRefExp->get_symbol()->get_declaration();
	    if (DEBUG_MODE)
	      cout << "   --> hit : definition changes conservatively. " <<  
		"  initName: " << initName->get_qualified_name().str() << endl;
	    // we want to add this node to the current set
	    //isUse=true;
	    // but we do not want it to be replaced
	    dont_replace=true;
	    hit = true;
	  }
	} else {
	  if (DEBUG_MODE)
	    cout <<counter << " PARAMETER in FUNC NOT HANDLED YET:: expr found in param: << " << expr->class_name() << endl;
	} // if
	// at any circumstance make sure that all nodes have a unique nr and are in the table
	// that might not happended, if variables are defined global and are not present in the table,
	// e.g. MPI_SEND_WORLD
	if (hit) {
	  isDefinition=true;
	  bool changedTable= performUseAndDefinition(sgNode, initName, false, isDefinition,
						     tmpBefore, dont_replace, cfgNode);
	  if (changedTable) {
	    if (DEBUG_MODE)
	    cout << "TableChanged" << endl;
	  }
	  tmpBefore=sgNode;
	  hit=false;
	  isHandled=true;
	}
	// for each variable, we need to add a definition

      } // for
      // if we get arguments that have no initName at all,
      // then we do not handle this function call
      if (isHandled==false) {
	dont_handle=true;
      } else {
	// in this case we have a special case
	// we assume that the table has changed.
	return true;
      }
      if (DEBUG_MODE)
	cout << " Checking of Paramters done: done_handle: " << resBool(dont_handle) <<endl;
    } // function call exp
  } //else if 

  else if (isSgFunctionDefinition(sgNode)) {
    // make sure global variables are added to the node
    // iterate through all global variables and get initName
    // then call performUseAndDefinition
    vector <SgInitializedName* > globals = dfa->getGlobalVariables();
    bool funcEntry=false;
    if (cfgNode.getIndex()==0)
      funcEntry=true;
    if (funcEntry && DEBUG_MODE)
      cout << "\n\n %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FUNCTION %%%%%%\n" << endl;
    if (DEBUG_MODE)
      cout << " found global vars: " << globals.size() << " funEntry " << 
	resBool(funcEntry) << endl;
    
    if (funcEntry) {
      dfa->addID(sgNode);
      vector <SgInitializedName* >:: iterator it = globals.begin();
      SgInitializedName* otherIn = NULL;
      for (; it != globals.end(); ++it) {
	SgInitializedName* initNameTmp = *it;
	ROSE_ASSERT(initNameTmp);
	// union of global var with current node (function)
	if (otherIn == NULL) {
	  dfa->mapDefUnion(initNameTmp, NULL, sgNode);
	} else {
	  dfa->mapDefUnion(initNameTmp, otherIn, sgNode);
	}
	otherIn = initNameTmp;
	
	if (DEBUG_MODE)
	  cout << "\n >> %%%%% handling globalvar: " << initNameTmp->get_qualified_name().str() << endl;
      }
      return true;
    } else {
      dont_handle=true;
    }    
  } // else if
  
  else {
    // none of the above breakPoints is hit 
    dont_handle=true;
      if (isSgWhileStmt(sgNode) || isSgForStatement(sgNode)) {
	if (breakPointNode==NULL) {
	  breakPointNode=sgNode;
	  breakPoint++;
	  if (DEBUG_MODE)
	    cout << ">>>Setting Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPoint <<endl;
	} else if (sgNode==breakPointNode) {
	  breakPoint++;
	  if (DEBUG_MODE)
	    cout << ">>> Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPoint <<endl;
	} else {
	  if (DEBUG_MODE)
	    cout << ">>> Skipping unhandled node ... " << endl;
	}
      }
  }

  // nodes that are not of interest are handled here
  // i.e they are just copied
  if (dont_handle) {
    if (DEBUG_MODE)
      cout << " ********** UNHANDLED.  This is an unhandled node " << sgNode->class_name() << endl;
    bool success = dfa->addID(sgNode);
    *unhandled = true;
    // take care of the case where we have none of the above within a loop (breakPoint)
    // i.e. no : VarRefExp, InitializedName, FunctionDefinition ...
    // If this unhandled node has been added to the map before (visited)
    // then we do want to mark it as handled.
    if (!success && breakPoint>2) {
      *unhandled = false;
      breakPointNode=NULL;
      breakPoint=0;
      cout << ">>> Resetting Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPoint <<endl;
    }
    handleDefCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);
    handleUseCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);
    return false;
  }

  // If the node is a verRefExp but has not initName, then it means 
  // that it is a new variable that never occured before
  // e.g. it is a function argument f(&var)
  // we need to make sure the variable is inserted to the def table
  if (initName!=NULL && isSgVarRefExp(sgNode)) {
    bool inserted = makeSureThatTheDefIsInTable(initName);
    if (inserted) {
      if (DEBUG_MODE)
	cout << " ... adding to table " << initName->get_qualified_name().str() << endl;
    }
  }

  if (DEBUG_MODE)
    cout << " ... check done. " << endl;

  return performUseAndDefinition(sgNode, initName, isUsage, isDefinition,
				 sgNodeBefore, dont_replace, cfgNode);
}

/**********************************************************
 *  Build CFG for each function and do DefUse analysis
 *********************************************************/
template <typename T>
bool DefUseAnalysisPF::performUseAndDefinition(SgNode* sgNode, 
					       SgInitializedName* initName,
					       bool isUsage,
					       bool isDefinition,
					       SgNode* sgNodeBefore,
					       bool dont_replace,
					       T cfgNode) {
  bool changedTableEntry = false;
  int nrOfInEdges = cfgNode.inEdges().size();

  // how do we handle global variables ? 
  //SgInitializedName* gNode = varRefExp->get_symbol()->get_declaration();
  bool globalVar = dfa->isNodeGlobalVariable(initName);
  if (DEBUG_MODE)
    cout << " isGlobalVar: " << resBool(globalVar) << endl;
  // if it is a global variable, make sure its added to the table

  ROSE_ASSERT(initName);

  
    
  //  if (isUse) {
  if (DEBUG_MODE) 
    cout << "  ----- IS USE. " << sgNode << " : " << sgNode->class_name() 
	 << " : " << initName->get_qualified_name().str()  << endl;
  multitype oldTable = dfa->getDefMultiMapFor(sgNode);
  handleDefCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);
  //    if (nrOfInEdges>1 ) {    
  multitype newTable = dfa->getDefMultiMapFor(sgNode);
  // did the copying change anything ?
  changedTableEntry = checkElementsForEquality(&oldTable, &newTable);
  //} else changedTableEntry=false;
  if (DEBUG_MODE) 
    cout << "  ----- IS USE. CHANGED TABLE ? " << resBool(changedTableEntry) << endl; 
  handleUseCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);
  //  }

  if (isUsage) {
    // tracking the use table
    //handleUseCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);
    multitype mmUse = dfa->getUseMultiMapFor(sgNode);
    if (isDoubleExactEntry(&mmUse, initName, sgNode)==false)
      dfa->addUseElement(sgNode, initName, sgNode);
  }

  if (globalVar && isUsage==false) {
    if (DEBUG_MODE)
      cout << " **********  GLOBALVAR :  " << resBool(globalVar) << 
	"  initName: " << initName->get_qualified_name().str() << endl;     
    // check if global var is contained in this multimap, if not, we nned to add it
    multitype mmap = dfa->getDefMultiMapFor(sgNode);
    bool isGlobalContainedinMM = searchMulti(&mmap, initName);
    bool isGlobalContainedinM = dfa->searchMap(initName);
    if (DEBUG_MODE) {
      cout << " globalVariable is containd in MultiMap ? " << resBool(isGlobalContainedinMM) << endl;
      cout << " globalVariable is containd in Map ? " << resBool(isGlobalContainedinM) << endl;
    }
    if (isGlobalContainedinMM==false) {
      //isDefinition=true;
      // make sure to add the globalvar in the current sgNode as a definition
      if (nrOfInEdges<=1 ) {
	// if we have only one in-edge, then we overwrite the value
	// add this as a new entry to the table
	dfa->mapDefUnion(sgNodeBefore, NULL, sgNode);
	dfa->addDefElement(sgNode, initName, initName);
	if (DEBUG_MODE )      
	  cout << "  ----- globalvar: changed table (one incoming) entry to " << sgNode <<  endl;
      } else  {
	// union of the current new Node with the previous CFG Node
	SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
	ROSE_ASSERT(otherInNode);
	dfa->mapDefUnion(sgNodeBefore, otherInNode, sgNode);
	dfa->addDefElement(sgNode, initName, initName);
	if (DEBUG_MODE )      
	  cout << "  ----- globalvar: changed table (multi incoming) entry to " << sgNode <<  " .. otherNode: " << otherInNode << endl;
      }
      //changedTableEntry = true;
      if (DEBUG_MODE)
	dfa->printDefMap();
    }
    // in addition, if we have a change of a global var
    // get the first definition of the var in map
    // and add conservatively all possible values
    if (isDefinition) {
      // the global variable is being overwritten
      multitype mm = dfa->getDefMultiMapFor(initName);
      if (isDoubleExactEntry(&mm, initName, sgNode)==false)
	dfa->addDefElement(initName, initName, sgNode);
    }
  }

  if (isDefinition ) {
    handleUseCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);
    dfa->clearUseOfElement(sgNode, initName);

    bool isCurrentValueContained = false;
    multitype mul = dfa->getDefMultiMapFor(initName);
    //multitype mul = dfa->getDefUseFor(sgNode);
    if (mul.size()>0) {
      isCurrentValueContained = searchMulti(&mul, initName);
    } 
    /*
    // DEBUG HELP
    if (isSgFunctionCallExp(sgNode)) {
    cerr << " found the function call " << initName->get_qualified_name().str() <<  
    "- contained : " << resBool(isCurrentValueContained) << endl;
    for (multitype::const_iterator j = mul.begin(); j != mul.end(); ++j) {  
    SgInitializedName* sgInitMM = (*j).first;
    SgNode* sgNodeMM = (*j).second;
    ROSE_ASSERT(sgInitMM);
    ROSE_ASSERT(sgNodeMM);
    cerr << "  ..  initName:" << sgInitMM->get_qualified_name().str() << " ( " <<
    ToString(dfa->getIntForSgNode(sgInitMM)) << " ) - SgNode " << 
    ToString(dfa->getIntForSgNode(sgNodeMM)) << endl;
    }      
    }
    */

    if (DEBUG_MODE)
      cout << "  ----- Definition. Is value contained ?  " << resBool(isCurrentValueContained) << endl;
    // read this value from table, if changed return that a change has 
    // occured and update table. Otherwise no change
    // map (cfgNode, multimap (initname, cfgNode))
    if (isCurrentValueContained==false) {
      if (DEBUG_MODE )
	cout << "  ----- NEW Definition. Adding: " << initName->get_qualified_name().str() << " nrInEdges: " << nrOfInEdges << endl;
      if (nrOfInEdges<=1 ) {
	// if we have only one in-edge, then we overwrite the value
	// add this as a new entry to the table
	dfa->mapDefUnion(sgNodeBefore, NULL, sgNode);
	dfa->addDefElement(sgNode, initName, sgNode);
	if (DEBUG_MODE )      
	  cout << "  ----- changed table (one incoming) entry to " << sgNode <<  endl;
      } else  {
	// union of the current new Node with the previous CFG Node
	SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
	ROSE_ASSERT(otherInNode);
	dfa->mapDefUnion(sgNodeBefore, otherInNode, sgNode);
	dfa->addDefElement(sgNode, initName, sgNode);
	if (DEBUG_MODE )      
	  cout << "  ----- changed table (multi incoming) entry to " << sgNode <<  " .. otherNode: " << otherInNode << endl;
      }
      changedTableEntry = true;

    } else  {
      if (DEBUG_MODE )      
	cout << "  ----- Definition EXISTS : " << initName->get_qualified_name().str() << " nrInEdges: " << nrOfInEdges << endl;

      // retrieve the value from the multimap and check
      // if it is the same as current. If yes, done.
      // Otherwise update the multimap with the union.
      if (nrOfInEdges<=1 ) {
	multitype oldTable = dfa->getDefMultiMapFor(sgNode);
	/* / --
	  cout << " !!!!!!!!!!!!!! oldTable " << dfa->getIntForSgNode(sgNode) << endl;
	  dfa->printMultiMap(&oldTable);
	*/ 
	dfa->mapDefUnion(sgNodeBefore, NULL, sgNode);
	/*/ ---
	  cout << " !!!!!!!!!!!!!! unionTable " << dfa->getIntForSgNode(sgNode) << endl;
	  multitype unionTable = dfa->getDefUseFor(sgNode);
	  dfa->printMultiMap(&unionTable);
	  // -- 

	  // --
	  // we do not want to replace a node, if we call e.g a function
	  // we want to conservatively add the current definition
	  
	  multitype mul = dfa->getDefUseFor(sgNode);
	  if (isDoubleExactEntry(&mul, initName, sgNode)==false) {
	  dfa->addElement(sgNode, initName, sgNode);
	*/
	multitype mm = dfa->getDefMultiMapFor(sgNode);
	if (dont_replace==true) {
	  if (isDoubleExactEntry(&mm, initName, sgNode)==false)
	    dfa->addDefElement(sgNode, initName, sgNode);
	}
	else
	  dfa->replaceElement(sgNode, initName);
	mm = dfa->getDefMultiMapFor(sgNode);
	/*/ ---
	  cout << " !!!!!!!!!!!!!! newTable " << dfa->getIntForSgNode(sgNode) << endl;
	  multitype mm2 = dfa->getDefUseFor(sgNode);
	  dfa->printMultiMap(&mm2);
	  // -- */
	changedTableEntry = checkElementsForEquality(&oldTable, &(mm));
	if (DEBUG_MODE )      
	  cout << "  ----- changed table (one incoming)  " << 
	    resBool(changedTableEntry) << "  dont_replace: " << resBool(dont_replace) << endl;
	/*/ ---
	  cout << " !!!!!!!!!!!!!! newTable after check elements " << dfa->getIntForSgNode(sgNode) << endl;
	  multitype mm3 = dfa->getDefUseFor(sgNode);
	  dfa->printMultiMap(&mm3);
	  // -- */

      } else {
	// otherwise, it we have more than one in-edge, we union the maps
	SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
	ROSE_ASSERT(otherInNode);	
	multitype oldTable = dfa->getDefMultiMapFor(sgNode);
	dfa->mapDefUnion(sgNodeBefore, otherInNode, sgNode);
	dfa->replaceElement(sgNode, initName);
	multitype mm = dfa->getDefMultiMapFor(sgNode);
	changedTableEntry = checkElementsForEquality(&oldTable, &(mm));
	if (DEBUG_MODE )      
	  cout << "  ----- changed table (multi incoming)  " << resBool(changedTableEntry) << endl;
      }
    }
    // return that a change has taken place!
    if (DEBUG_MODE)
      dfa->printDefMap();
  } 

 
  return changedTableEntry;
}

/**********************************************************
 * plain copy of the table
 *********************************************************/
void DefUseAnalysisPF::handleDefCopy(SgNode* sgNode, int nrOfInEdges, 
				     SgNode* sgNodeBefore, filteredCFGNodeType cfgNode) {
  if (DEBUG_MODE)
    cout << " ---- JUSTCOPY. " << sgNode << endl;
  if (nrOfInEdges<=1 ) {
    multitype oldTable = dfa->getDefMultiMapFor(sgNode);
    dfa->mapDefUnion(sgNodeBefore, NULL, sgNode);
    //	replaceElement(sgNode, initName);
  } else {
    // otherwise, it we have more than one in-edge, we union the maps
    SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
    ROSE_ASSERT(otherInNode);	
    multitype oldTable = dfa->getDefMultiMapFor(sgNode);
    dfa->mapDefUnion(sgNodeBefore, otherInNode, sgNode);
    //replaceElement(sgNode, initName);
  }
  if (DEBUG_MODE)
    dfa->printDefMap();
}

/**********************************************************
 * plain copy of the table
 *********************************************************/
void DefUseAnalysisPF::handleUseCopy(SgNode* sgNode, int nrOfInEdges, 
				     SgNode* sgNodeBefore, filteredCFGNodeType cfgNode) {
  if (DEBUG_MODE)
    cout << " ---- JUSTCOPY. " << sgNode << endl;
  if (nrOfInEdges<=1 ) {
    multitype oldTable = dfa->getUseMultiMapFor(sgNode);
    dfa->mapUseUnion(sgNodeBefore, NULL, sgNode);
    //	replaceElement(sgNode, initName);
  } else {
    // otherwise, it we have more than one in-edge, we union the maps
    SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
    ROSE_ASSERT(otherInNode);	
    multitype oldTable = dfa->getUseMultiMapFor(sgNode);
    dfa->mapUseUnion(sgNodeBefore, otherInNode, sgNode);
    //replaceElement(sgNode, initName);
  }
  if (DEBUG_MODE)
    dfa->printUseMap();
}

/**********************************************************
 *  How many nodes where visited per function? 
 *********************************************************/
int DefUseAnalysisPF::getNumberOfNodesVisited() {
  return nrOfNodesVisitedPF;
}

/**********************************************************
 *  Build CFG for each function and do DefUse analysis
 *********************************************************/
FilteredCFGNode < IsDFAFilter > DefUseAnalysisPF::run(SgFunctionDefinition* funcDecl) {
  // filter functions -- to only functions in analyzed file  
  nrOfNodesVisitedPF= 0;
  breakPointNode=NULL;
  breakPoint=0;
  string funcName = getFullName(funcDecl);
  //  DEBUG_MODE = false;
  DEBUG_MODE_EXTRA=false;

  if (funcName=="") {
    FilteredCFGNode < IsDFAFilter > empty;
    return empty;
  }
  ROSE_ASSERT(funcDecl);

  if (DEBUG_MODE) 
    cout << " Found function " << funcName << endl;

  // DFA on that function
  vector<FilteredCFGNode<IsDFAFilter> > worklist;
  
  //waitAtMergeNode.clear();

  // add this node to worklist and work through the outgoing edges
  FilteredCFGNode < IsDFAFilter > source =
    FilteredCFGNode < IsDFAFilter > (funcDecl->cfgForBeginning());
  CFGNode cmpSrc = CFGNode(funcDecl->cfgForBeginning());
  FilteredCFGNode < IsDFAFilter > rem_source = source;

  if (DEBUG_MODE) {
    std::ofstream f("cfg.dot");
    cfgToDot(f, string("cfg"), source);
    f.close();
  }

  
  worklist.push_back(source);
  vector<FilteredCFGNode<IsDFAFilter> > debug_path;
  debug_path.push_back(source);


  bool valueHasChanged = false;
  bool unhandledNode=false;
  while (!worklist.empty()) {
    source = worklist.front();
    worklist.erase(worklist.begin());
    // do current node
    unhandledNode = false;
    //    if (isSgVarRefExp(source.getNode()) || isSgInitializedName(source.getNode()))
    valueHasChanged = defuse(source, &unhandledNode);  
    //    valueHasChanged=true; unhandledNode=true;
    // do follow-up nodes
    // get nodes of outgoing edges and pushback (if not already contained)
    if (DEBUG_MODE) {
      cout << " Current Node: " << source.getNode() << " changed: " 
	   << resBool(valueHasChanged) << endl;
      cout << " Current Node: " << source.getNode() << " unhandled: " 
	   << resBool(unhandledNode) << endl;
    }
    if (valueHasChanged || unhandledNode) {
      vector<FilteredCFGEdge < IsDFAFilter > > out_edges = source.outEdges();
      for (vector<FilteredCFGEdge <IsDFAFilter> >::const_iterator i = out_edges.begin(); i != out_edges.end(); ++i) {
	FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
	FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
	if (find(worklist.begin(), worklist.end(), filterNode)==worklist.end()) {
	  worklist.push_back(filterNode);
	  debug_path.push_back(filterNode);
	}
      }
      if (DEBUG_MODE) 
	printCFGVector(worklist);
    }
  }
  if (DEBUG_MODE)
    cout << " Elements in Table: " << dfa->getDefSize() << endl;
  nrOfNodesVisitedPF= debug_path.size();

  if (DEBUG_MODE) {
    cout << " Exiting function " << funcName << endl;
    cout << "\nNr of nodes visited " << debug_path.size() << "  of nodes : " <<
      dfa->getDefSize() << endl;
    printCFGVector(debug_path);
  }
  return rem_source;
}
