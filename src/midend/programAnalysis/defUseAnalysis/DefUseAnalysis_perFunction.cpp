/******************************************
 * Category: DFA
 * DefUse Analysis Declaration (per Function)
 * created by tps in Feb 2007
 *****************************************/
// tps : Switching from rose.h to sage3 changed size from 18,6 MB to 8,3MB
#include "sage3basic.h"
#include "DefUseAnalysis_perFunction.h"

using namespace std;

/**********************************************************
 * Make sure all nodes (even arguments if &argument) are in table
 * so they can be found for analysis
 *********************************************************/
bool DefUseAnalysisPF::makeSureThatTheDefIsInTable(SgInitializedName* initName) {
  bool addedNode = false;
  vector<pair<SgInitializedName*, SgNode*> > mymap = dfa->getDefMultiMapFor(
									    initName);
  if (mymap.size() == 0) {
    dfa->addDefElement(initName, initName, initName);
    addedNode = true;
    if (DEBUG_MODE)
      cout << " FUNCTIONCALL ::: adding parameter to table : "
	   << initName->get_qualified_name().str() << endl;
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
  vector<pair<SgInitializedName*, SgNode*> > mymap = dfa->getUseMultiMapFor(
									    initName);
  if (mymap.size() == 0) {
    dfa->addUseElement(initName, initName, initName);
    addedNode = true;
    if (DEBUG_MODE)
      cout << " FUNCTIONCALL ::: adding parameter to table : "
	   << initName->get_qualified_name().str() << endl;
  }
  if (DEBUG_MODE)
    cout << " nr for that node ( " << initName->get_qualified_name().str()
	 << ") : " << dfa->getIntForSgNode(initName) << endl;
  return addedNode;
}

/**********************************************************
 * Traverse the CFG backwards from a given node to
 * determine whether a change has occured at that path
 * until the first split (2 inedges) or root.
 *********************************************************/
template<typename T>
bool DefUseAnalysisPF::hasANodeAboveCurrentChanged(T source) {
  bool changed = false;
  // go cfg back until split or root
  // check nodeChangeMap if a all nodes have changed.
  // if any has changed return true.
  vector<FilteredCFGEdge<IsDFAFilter> > in_edges = source.inEdges();
  if (in_edges.size() == 1) {
    FilteredCFGEdge<IsDFAFilter> filterEdge = in_edges[0];
    FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.source();
    SgNode* sgNode = filterNode.getNode();
    ROSE_ASSERT(sgNode);
    bool changedInTable = nodeChangedMap[sgNode];
    if (DEBUG_MODE)
      cout << " >>> backward CFG : visiting node : " << sgNode << " "
	   << sgNode->class_name() << "  changed : " << changedInTable
	   << endl;
    if (changedInTable) {
      return true;
    } else {
      changed = hasANodeAboveCurrentChanged(filterNode);
    }
  }
  return changed;

}

/**********************************************************
 *  Build CFG for each function and do DefUse analysis
 *  This is the main analysis function
 *  It determines what should happen at each DFG node
 *********************************************************/
template<typename T>
bool DefUseAnalysisPF::defuse(T cfgNode, bool *unhandled) {
  SgNode* sgNode = cfgNode.getNode();
  SgNode* sgNodeBefore = getCFGPredNode(cfgNode);
  ROSE_ASSERT(sgNode);

  bool dont_handle = false;
  bool dont_replace = false;
  if (DEBUG_MODE)
    cout
      << "\n\n------------------------------------------------------------------\ncurrent Node: "
      << sgNode << "  previous Node : " << sgNodeBefore << endl;

  bool isDefinition = false;
  //  bool isUse=false;
  bool isUsage = false;
  SgInitializedName* initName = NULL;

  if (isSgUnaryOp(sgNode)) {
    SgUnaryOp* unary = isSgUnaryOp(sgNode);
    SgExpression* l_expr = unary->get_operand();
    SgVarRefExp* varRefExp = NULL;
    if (DEBUG_MODE)
      cout << " **********  UNARY OP. " << unary << "  " << varRefExp << endl;
    if (isSgAssignOp(l_expr)) {
      // maybe the subtree on the left hand side has a varRefExp.
      // find first leftmost varRefExp -- this would currently work for (t=i)++ but not for (t.x=i)++ 
      Rose_STL_Container<SgNode*> vars  = NodeQuery::querySubTree(l_expr,V_SgVarRefExp);
      if (vars.size()>0) 
	l_expr=isSgVarRefExp(*vars.begin());
    }
    if (isSgVarRefExp(l_expr)) {
      // if left side is a varrefexp
      varRefExp = isSgVarRefExp(l_expr);
      initName = varRefExp->get_symbol()->get_declaration();
      ROSE_ASSERT(initName);
      if (DEBUG_MODE)
	cout << " UNARY OP: " << initName->get_qualified_name().str()
	     << "  name: " << initName << "  varRefExp: "
	     << varRefExp << endl;
      ROSE_ASSERT(unary);
      switch (unary->variantT()) {
      case V_SgPlusPlusOp:
      case V_SgMinusMinusOp: {
	isDefinition = true;
	//isUse=true;
	break;
      }
      default: {
	isDefinition = false;
	//isUse=true;
	if (DEBUG_MODE)
	  cout << " **** reached default. no action taken. " << endl;
	break;
      }
      }
    } else {
       if (DEBUG_MODE)
	cerr << " **********  UNARY OP : CANT HANDLE YET: "
	     << l_expr->class_name() << endl;
      dont_handle = true;
    }
  }

  else if (isSgBinaryOp(sgNode)) {
    SgBinaryOp* binary = isSgBinaryOp(sgNode);
    SgExpression* l_expr = binary->get_lhs_operand();
    SgVarRefExp* varRefExp = isSgVarRefExp(l_expr);
#if 1
    SgPntrArrRefExp* varPntrRefExp = isSgPntrArrRefExp(l_expr);
#endif
    if (DEBUG_MODE)
      cout << " **********  BINARY OP. " << binary << endl;
    if (varRefExp
#if 1
	|| varPntrRefExp
#endif
	) {
      // if left side is a varrefexp
      if (DEBUG_MODE)
	cout << " **********  BINARY OP IS_VAR_REF_EXP. " << endl;
      if (varRefExp) {
	ROSE_ASSERT(varRefExp->get_symbol());
	initName = varRefExp->get_symbol()->get_declaration();
	if (DEBUG_MODE)
	  cout << " BINARY OP: "
	       << initName->get_qualified_name().str()
	       << "  name: " << initName << "  varRefExp: "
	       << varRefExp << endl;
      }
#if 1
      else if (varPntrRefExp) {
	while (isSgPntrArrRefExp(varPntrRefExp->get_lhs_operand())) {
	  varPntrRefExp = isSgPntrArrRefExp(
					    varPntrRefExp->get_lhs_operand());
	}
	SgVarRefExp* varRefExpL = isSgVarRefExp(
						varPntrRefExp->get_lhs_operand());
	if (varRefExpL == NULL) {
	  Rose_STL_Container<SgNode*> vars = NodeQuery::querySubTree(varPntrRefExp, V_SgVarRefExp);
	  if (vars.size()>1) {
	    //cerr << " There is more than one VarRefExp in this PntrArrRefExp. " << endl;
	    //	    ROSE_ASSERT(varRefExpL);
	    varRefExpL = isSgVarRefExp(*vars.begin());
	  } else if (vars.size()==1)
	    varRefExpL = isSgVarRefExp(*vars.begin());
	  if (varRefExpL==NULL) {
	    //cerr << " TYPE of LHS : " << varPntrRefExp->get_lhs_operand()->class_name() << endl;
	    ROSE_ASSERT(varRefExpL);
	  }
	}
#if 0
	SgArrowExp* arrow = isSgArrowExp(varPntrRefExp->get_lhs_operand());
	if (arrow) {
	  if (DEBUG_MODE) {
	    cerr << " TYPE of LHS OF ARROW: left: " << arrow->get_lhs_operand()->class_name() << endl;
	    cerr << " TYPE of LHS OF ARROW: right: " << arrow->get_rhs_operand()->class_name() << endl;
	  }
	  varRefExpL = isSgVarRefExp( arrow->get_rhs_operand());
	  if (varRefExpL==NULL)
	    if (DEBUG_MODE)
	      cerr << " TYPE of LHS IN ARROW: " << arrow->get_rhs_operand()->class_name() << endl;
	  ROSE_ASSERT(varRefExpL);
	}
	if (varRefExpL) {
	  ROSE_ASSERT(varRefExpL);
	  ROSE_ASSERT(varRefExpL->get_symbol());
	} else if (arrow) {
	} else {
	  if (DEBUG_MODE)
	    cerr << " TYPE of LHS : " << varPntrRefExp->get_lhs_operand()->class_name() << endl;
	  ROSE_ASSERT(varRefExpL);
	}
#endif

	initName = varRefExpL->get_symbol()->get_declaration();
	if (DEBUG_MODE)
	  cout << " BINARY OP: " << initName->get_qualified_name().str()
	       << "  name: " << initName << "  varRefExp: " << varRefExp << endl;
      }
#endif
      ROSE_ASSERT(initName);
      ROSE_ASSERT(binary);
      switch(binary->variantT()) {
      case V_SgAssignOp:
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
      if (DEBUG_MODE)
	cout << " DefuseAnalysis:: unhandled condition : SgExprListExp ...  size: " << exprs.size() <<
	  "  file : " << l_expr->get_file_info()->get_filenameString() <<
	  "  line : " << l_expr->get_file_info()->get_line() << endl;
      return false;
    } else {
      if (DEBUG_MODE)
	cout << " DefuseAnalysis:: unhandled condition - AssignInitializer ... "
	     << "  AssignInitializer - l_expr : " << l_expr->class_name() << "  file : " << l_expr->get_file_info()->get_filenameString() <<
	  "  line : " << l_expr->get_file_info()->get_line() << endl;
      if (DEBUG_MODE)
	cout << " unparse : " << l_expr->unparseToCompleteString() << endl;
      return false;
      //      exit(0);
    }
  }

  else if (isSgInitializedName(sgNode)) {
    initName =isSgInitializedName(sgNode);
    if (DEBUG_MODE)
      cout << " **********  INITNAME. " << initName << endl;
    //isUse=false;
    isDefinition=true;
    if (DEBUG_MODE)
      cout << " **********  INITNAME. "
	   << " def " << resBool(isDefinition) << endl;
  }

  else if (isSgVarRefExp(sgNode)) {
    SgVarRefExp* varRefExp = isSgVarRefExp(sgNode);
    initName = varRefExp->get_symbol()->get_declaration();
    string name = initName->get_qualified_name().str();
    if (DEBUG_MODE)
      cout << " **********  VARREFEXP. " << varRefExp << " .. " << name << endl;
    //isUse=true;
    isDefinition=false;
    SgNode* parent = varRefExp->get_parent();
    ROSE_ASSERT(parent);
    // go up to the parent and check if there is an assignment happening.
    // If yes, check if VarRefExp on right side,
    // if yes, we have a usage -- otherwise its a definition
    SgPntrArrRefExp* array = isSgPntrArrRefExp(parent);
    if (array)
      ROSE_ASSERT(array->get_lhs_operand());
    SgNode* parentsparent = parent->get_parent();
    ROSE_ASSERT(parentsparent);
#if 0
    if ((isSgAssignOp(parentsparent) && isSgAssignOp(parentsparent)->get_lhs_operand()==array)) {
      //isDefinition=true;
    } else {
      // array on right hand side ...
      isUsage=true;
    }
#else
    if ((isSgAssignOp(parent) && isSgAssignOp(parent)->get_lhs_operand()==varRefExp) ||
	(isSgPntrArrRefExp(parent) && isSgPntrArrRefExp(parent)->get_lhs_operand()==varRefExp) &&
	(isSgAssignOp(parentsparent) && isSgAssignOp(parentsparent)->get_lhs_operand()==array)
	) {
      // definition .. dont handle
    } else {
      // usage
      isUsage =true;
    }
#endif
  } // else if


  else if (isSgFunctionCallExp(sgNode)) {
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
    // none of the above breakPointForWhiles is hit
    dont_handle=true;
    // take care of the case where we have none of the above within a loop (breakPointForWhile)
    // i.e. no : VarRefExp, InitializedName, FunctionDefinition ...
    // If this unhandled node has been added to the map before (visited)
    // then we do want to mark it as handled.
    if (isSgWhileStmt(sgNode) || isSgForStatement(sgNode)
	|| isSgDoWhileStmt(sgNode)) {
      if (breakPointForWhileNode==NULL) {
	breakPointForWhileNode=sgNode;
	breakPointForWhile++;
	if (DEBUG_MODE)
	  cout << ">>> Setting Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
      } else if (sgNode==breakPointForWhileNode) {
	// reaching the breakPoint for a second time
	// check if any node above this node up to the branch or root has changed
	breakPointForWhile++;
	bool hasAnyNodeAboveChanged = hasANodeAboveCurrentChanged(cfgNode);
	if (hasAnyNodeAboveChanged==false) {
	  // need to break this loop
	  // add current node to doNotVisitMap
	  doNotVisitMap.insert(sgNode);
	}

	if (DEBUG_MODE)
	  cout << ">>> Inc Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
	*unhandled = false;
	breakPointForWhileNode=NULL;
	breakPointForWhile=0;
	if (DEBUG_MODE)
	  cout << ">>> Resetting Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
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
    dfa->addID(sgNode);
    *unhandled = true;
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
template<typename T>
bool DefUseAnalysisPF::performUseAndDefinition(SgNode* sgNode,
					       SgInitializedName* initName, bool isUsage, bool isDefinition,
					       SgNode* sgNodeBefore, bool dont_replace, T cfgNode) {
  bool changedTableEntry = false;
  int nrOfInEdges = cfgNode.inEdges().size();

  // how do we handle global variables ?
  //SgInitializedName* gNode = varRefExp->get_symbol()->get_declaration();
  bool globalVar = dfa->isNodeGlobalVariable(initName);
  if (DEBUG_MODE)
    cout << " isGlobalVar: " << resBool(globalVar) << endl;
  // if it is a global variable, make sure its added to the table

  ROSE_ASSERT(initName);

  if (DEBUG_MODE)
    cout << "  ----- IS USE. " << sgNode << " : " << sgNode->class_name()
	 << " : " << initName->get_qualified_name().str() << endl;
  multitype oldTable = dfa->getDefMultiMapFor(sgNode);
  handleDefCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);
  multitype newTable = dfa->getDefMultiMapFor(sgNode);
  // did the copying change anything ?
  changedTableEntry = checkElementsForEquality(&oldTable, &newTable);

  if (DEBUG_MODE)
    cout << "  ----- IS USE. CHANGED TABLE ? "
	 << resBool(changedTableEntry) << endl;
  handleUseCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);

  if (isUsage) {
    // tracking the use table
    multitype mmUse = dfa->getUseMultiMapFor(sgNode);
    if (isDoubleExactEntry(&mmUse, initName, sgNode) == false)
      dfa->addUseElement(sgNode, initName, sgNode);
  }

  if (globalVar && isUsage == false) {
    if (DEBUG_MODE)
      cout << " **********  GLOBALVAR :  " << resBool(globalVar)
	   << "  initName: " << initName->get_qualified_name().str()
	   << endl;
    // check if global var is contained in this multimap, if not, we nned to add it
    multitype mmap = dfa->getDefMultiMapFor(sgNode);
    bool isGlobalContainedinMM = searchMulti(&mmap, initName);
    bool isGlobalContainedinM = dfa->searchMap(initName);
    if (DEBUG_MODE) {
      cout << " globalVariable is containd in MultiMap ? " << resBool(
								      isGlobalContainedinMM) << endl;
      cout << " globalVariable is containd in Map ? " << resBool(
								 isGlobalContainedinM) << endl;
    }
    if (isGlobalContainedinMM == false) {
      //isDefinition=true;
      // make sure to add the globalvar in the current sgNode as a definition
      if (nrOfInEdges <= 1) {
	// if we have only one in-edge, then we overwrite the value
	// add this as a new entry to the table
	dfa->mapDefUnion(sgNodeBefore, NULL, sgNode);
	dfa->addDefElement(sgNode, initName, initName);
	if (DEBUG_MODE)
	  cout
	    << "  ----- globalvar: changed table (one incoming) entry to "
	    << sgNode << endl;
      } else {
	// union of the current new Node with the previous CFG Node
	SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
	ROSE_ASSERT(otherInNode);
	dfa->mapDefUnion(sgNodeBefore, otherInNode, sgNode);
	dfa->addDefElement(sgNode, initName, initName);
	if (DEBUG_MODE)
	  cout
	    << "  ----- globalvar: changed table (multi incoming) entry to "
	    << sgNode << " .. otherNode: " << otherInNode
	    << endl;
      }
      //changedTableEntry = true;
      if (DEBUG_MODE) {
	cout << " GLOBALCINTAINED : printDEFMAP : " << endl;
	dfa->printDefMap();
      }
    }
    // in addition, if we have a change of a global var
    // get the first definition of the var in map
    // and add conservatively all possible values
    if (isDefinition) {
      // the global variable is being overwritten
      multitype mm = dfa->getDefMultiMapFor(initName);
      if (isDoubleExactEntry(&mm, initName, sgNode) == false)
	dfa->addDefElement(initName, initName, sgNode);
    }
  }

  if (isDefinition) {
    handleUseCopy(sgNode, cfgNode.inEdges().size(), sgNodeBefore, cfgNode);
    dfa->clearUseOfElement(sgNode, initName);

    bool isCurrentValueContained = false;
    multitype mul = dfa->getDefMultiMapFor(initName);
    //multitype mul = dfa->getDefUseFor(sgNode);
    if (mul.size() > 0) {
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
      cout << "  ----- Definition. Is value contained ?  " << resBool(
								      isCurrentValueContained) << endl;
    // read this value from table, if changed return that a change has
    // occured and update table. Otherwise no change
    // map (cfgNode, multimap (initname, cfgNode))
    if (isCurrentValueContained == false) {
      if (DEBUG_MODE)
	cout << "  ----- NEW Definition. Adding: "
	     << initName->get_qualified_name().str()
	     << " nrInEdges: " << nrOfInEdges << endl;
      if (nrOfInEdges <= 1) {
	// if we have only one in-edge, then we overwrite the value
	// add this as a new entry to the table
	dfa->mapDefUnion(sgNodeBefore, NULL, sgNode);
	dfa->addDefElement(sgNode, initName, sgNode);
	if (DEBUG_MODE)
	  cout << "  ----- changed table (one incoming) entry to "
	       << sgNode << endl;
      } else {
	// union of the current new Node with the previous CFG Node
	SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
	ROSE_ASSERT(otherInNode);
	dfa->mapDefUnion(sgNodeBefore, otherInNode, sgNode);
	dfa->addDefElement(sgNode, initName, sgNode);
	if (DEBUG_MODE)
	  cout << "  ----- changed table (multi incoming) entry to "
	       << sgNode << " .. otherNode: " << otherInNode
	       << endl;
      }
      changedTableEntry = true;

    } else {
      if (DEBUG_MODE)
	cout << "  ----- Definition EXISTS : "
	     << initName->get_qualified_name().str()
	     << " nrInEdges: " << nrOfInEdges << endl;

      // retrieve the value from the multimap and check
      // if it is the same as current. If yes, done.
      // Otherwise update the multimap with the union.
      if (nrOfInEdges <= 1) {
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
	if (dont_replace == true) {
	  if (isDoubleExactEntry(&mm, initName, sgNode) == false)
	    dfa->addDefElement(sgNode, initName, sgNode);
	} else
	  dfa->replaceElement(sgNode, initName);
	mm = dfa->getDefMultiMapFor(sgNode);
	/*/ ---
	  cout << " !!!!!!!!!!!!!! newTable " << dfa->getIntForSgNode(sgNode) << endl;
	  multitype mm2 = dfa->getDefUseFor(sgNode);
	  dfa->printMultiMap(&mm2);
	  // -- */
	changedTableEntry = checkElementsForEquality(&oldTable, &(mm));
	if (DEBUG_MODE)
	  cout << "  ----- changed table (one incoming)  "
	       << resBool(changedTableEntry) << "  dont_replace: "
	       << resBool(dont_replace) << endl;
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
	// if the value contained is the same, replace it
	// otherwise add it
	if (isDoubleExactEntry(&oldTable, initName, sgNode) == false) {
	  // important case: if the decision node (2 inedges) is
	  // changing the value as well, handle special
	  dfa->replaceElement(sgNode, initName);
	  //cout << ">>> addElement" << endl;
	  changedTableEntry = false;
	  breakPointForWhileNode = NULL;
	  breakPointForWhile = 0;
	} else {
	  dfa->replaceElement(sgNode, initName);
	  //cout << ">>> replaceElement" << endl;
	  multitype mm = dfa->getDefMultiMapFor(sgNode);
	  changedTableEntry = checkElementsForEquality(&oldTable,
						       &(mm));
	}

	if (DEBUG_MODE)
	  cout << "  ----- changed table (multi incoming)  "
	       << resBool(changedTableEntry) << endl;
      }
    }
    // return that a change has taken place!
    if (DEBUG_MODE) {
      cout << " FINAL DEFMAP : " << endl;
      dfa->printDefMap();
    }
  }

  return changedTableEntry;
}

/**********************************************************
 * plain copy of the table
 *********************************************************/
void DefUseAnalysisPF::handleDefCopy(SgNode* sgNode, int nrOfInEdges,
				     SgNode* sgNodeBefore, filteredCFGNodeType cfgNode) {
  if (DEBUG_MODE) {
    cout << " DEFMAP BEFORE UNION: " << endl;
    dfa->printDefMap();
  }
  if (nrOfInEdges <= 1) {
    if (DEBUG_MODE)
      cout << " ---- DEFCOPY: 1 EDGE " << sgNode << endl;
    multitype oldTable = dfa->getDefMultiMapFor(sgNode);
    dfa->mapDefUnion(sgNodeBefore, NULL, sgNode);
    //	replaceElement(sgNode, initName);
  } else {
    if (DEBUG_MODE)
      cout << " ---- DEFCOPY: many EDGEs " << sgNode << endl;
    // otherwise, it we have more than one in-edge, we union the maps
    SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
    ROSE_ASSERT(otherInNode);
    multitype oldTable = dfa->getDefMultiMapFor(sgNode);
    dfa->mapDefUnion(sgNodeBefore, otherInNode, sgNode);
    //replaceElement(sgNode, initName);
  }
  if (DEBUG_MODE) {
    cout << " DEFMAP AFTER UNION: " << endl;
    dfa->printDefMap();
  }

}

/**********************************************************
 * plain copy of the table
 *********************************************************/
void DefUseAnalysisPF::handleUseCopy(SgNode* sgNode, int nrOfInEdges,
				     SgNode* sgNodeBefore, filteredCFGNodeType cfgNode) {
  if (DEBUG_MODE) {
    cout << " USEMAP BEFORE UNION: " << endl;
    dfa->printUseMap();
  }

  if (nrOfInEdges <= 1) {
    if (DEBUG_MODE)
      cout << " ---- USECOPY: 1 EDGE " << sgNode << endl;
    multitype oldTable = dfa->getUseMultiMapFor(sgNode);
    dfa->mapUseUnion(sgNodeBefore, NULL, sgNode);
    //	replaceElement(sgNode, initName);
  } else {
    if (DEBUG_MODE)
      cout << " ---- USECOPY: many EDGEs " << sgNode << endl;
    // otherwise, it we have more than one in-edge, we union the maps
    SgNode* otherInNode = getOtherInNode(cfgNode, sgNodeBefore);
    ROSE_ASSERT(otherInNode);
    multitype oldTable = dfa->getUseMultiMapFor(sgNode);
    dfa->mapUseUnion(sgNodeBefore, otherInNode, sgNode);
    //replaceElement(sgNode, initName);
  }
  if (DEBUG_MODE) {
    cout << " USEMAP AFTER UNION: " << endl;
    dfa->printUseMap();
  }
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
FilteredCFGNode<IsDFAFilter> DefUseAnalysisPF::run(
						   SgFunctionDefinition* funcDecl, bool& abortme) {
  // filter functions -- to only functions in analyzed file
  nrOfNodesVisitedPF = 0;
  breakPointForWhileNode = NULL;
  breakPointForWhile = 0;
  // clear those maps for each function run
  doNotVisitMap.clear();
  nodeChangedMap.clear();

  string funcName = getFullName(funcDecl);
  //  DEBUG_MODE = false;
  DEBUG_MODE_EXTRA = false;

  if (funcName == "") {
    FilteredCFGNode<IsDFAFilter> empty(CFGNode(NULL, 0));
    return empty;
  }
  ROSE_ASSERT(funcDecl);

  if (DEBUG_MODE)
    cout << " Found function " << funcName << endl;

  // DFA on that function
  vector<FilteredCFGNode<IsDFAFilter> > worklist;

  //waitAtMergeNode.clear();

  // add this node to worklist and work through the outgoing edges
  FilteredCFGNode<IsDFAFilter> source = FilteredCFGNode<IsDFAFilter> (
								      funcDecl->cfgForBeginning());
  CFGNode cmpSrc = CFGNode(funcDecl->cfgForBeginning());
  FilteredCFGNode<IsDFAFilter> rem_source = source;

  if (DEBUG_MODE) {
    std::ofstream f("cfg.dot");
    cfgToDot(f, string("cfg"), source);
    f.close();
  }

  worklist.push_back(source);
  vector<FilteredCFGNode<IsDFAFilter> > debug_path;
  debug_path.push_back(source);

  bool valueHasChanged = false;
  bool unhandledNode = false;
  while (!worklist.empty()) {
    source = worklist.front();
    worklist.erase(worklist.begin());
    // do current node
    unhandledNode = false;
    SgNode* next = source.getNode();
    if (doNotVisitMap.find(next) != doNotVisitMap.end())
      continue;
    valueHasChanged = defuse(source, &unhandledNode);
    nodeChangedMap[source.getNode()] = valueHasChanged;
    // do follow-up nodes
    // get nodes of outgoing edges and pushback (if not already contained)
    if (DEBUG_MODE) {
      cout << " Current Node: " << source.getNode() << " changed: "
	   << resBool(valueHasChanged) << endl;
      cout << " Current Node: " << source.getNode() << " unhandled: "
	   << resBool(unhandledNode) << endl;
    }
    if (valueHasChanged || unhandledNode) {
      vector<FilteredCFGEdge<IsDFAFilter> > out_edges = source.outEdges();
      for (vector<FilteredCFGEdge<IsDFAFilter> >::const_iterator i =
	     out_edges.begin(); i != out_edges.end(); ++i) {
	FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
	FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
	if (find(worklist.begin(), worklist.end(), filterNode)
	    == worklist.end()) {
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
  nrOfNodesVisitedPF = debug_path.size();

  if (DEBUG_MODE) {
    cout << " Exiting function " << funcName << endl;
    cout << "\nNr of nodes visited " << debug_path.size()
	 << "  of nodes : " << dfa->getDefSize() << endl;
    printCFGVector(debug_path);
  }
  return rem_source;
}
