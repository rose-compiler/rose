// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageInterface.h" // for isConstType
#include "CallGraph.h"
#include <vector>
using namespace std;
using namespace VirtualCFG;

unsigned int
SgNode::cfgIndexForEnd() const {
  ROSE_ASSERT (!"CFG functions only work on SgExpression, SgStatement, and SgInitializedName");
  return 0;
}

bool
SgNode::cfgIsIndexInteresting(unsigned int) const {
  ROSE_ASSERT (!"CFG functions only work on SgExpression, SgStatement, and SgInitializedName");
  return false;
}

unsigned int
SgNode::cfgFindChildIndex(SgNode*) {
  ROSE_ASSERT (!"CFG functions only work on SgExpression, SgStatement, and SgInitializedName");
  return 0;
}

unsigned int
SgNode::cfgFindNextChildIndex(SgNode*) {
  ROSE_ASSERT (!"CFG functions only work on SgExpression, SgStatement, and SgInitializedName");
  return 0;
}

std::vector<CFGEdge>
SgNode::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (!"CFG functions only work on SgExpression, SgStatement, and SgInitializedName");
  return std::vector<CFGEdge>();
}

std::vector<CFGEdge>
SgNode::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (!"CFG functions only work on SgExpression, SgStatement, and SgInitializedName");
  return std::vector<CFGEdge>();
}

static void makeEdge(CFGNode from, CFGNode to, vector<CFGEdge>& result) {
  // Makes a CFG edge, adding appropriate labels
  SgNode* fromNode = from.getNode();
  unsigned int fromIndex = from.getIndex();
  SgNode* toNode = to.getNode();
  // unsigned int toIndex = to.getIndex();

  // Exit early if the edge should not exist because of a control flow discontinuity
  if (fromIndex == 1 && (isSgGotoStatement(fromNode) || isSgBreakStmt(fromNode) || isSgContinueStmt(fromNode))) {
    return;
  }
  if (isSgReturnStmt(fromNode) && toNode == fromNode->get_parent()) {
    SgReturnStmt* rs = isSgReturnStmt(fromNode);
    if (fromIndex == 1 || (fromIndex == 0 && !rs->get_expression())) return;
  }
  if (isSgStopOrPauseStatement(fromNode) && toNode == fromNode->get_parent()) {
    SgStopOrPauseStatement* sps = isSgStopOrPauseStatement(fromNode);
    if (fromIndex == 0 && sps->get_stop_or_pause() == SgStopOrPauseStatement::e_stop) return;
  }
  if (fromIndex == 1 && isSgSwitchStatement(fromNode) &&
      isSgSwitchStatement(fromNode)->get_body() == toNode) return;

  // Create the edge
  result.push_back(CFGEdge(from, to));
}

static void addIncomingFortranGotos(SgStatement* stmt, unsigned int index, vector<CFGEdge>& result) {
  bool hasLabel = false;
  if (index == 0 && stmt->get_numeric_label()) hasLabel = true;
  if (index == stmt->cfgIndexForEnd() && stmt->has_end_numeric_label()) hasLabel = true;
  if (index == 0 &&
      isSgIfStmt(stmt->get_parent()) &&
      stmt != NULL &&
      stmt == isSgIfStmt(stmt->get_parent())->get_false_body()) hasLabel = true;
  if (isSgProcedureHeaderStatement(stmt) ||
      isSgProgramHeaderStatement(stmt) ||
      isSgFunctionDefinition(stmt))
    hasLabel = true;
  if (!hasLabel) return;

  CFGNode cfgNode(stmt, index);
  // Find all gotos to this CFG node, functionwide
  SgFunctionDefinition* thisFunction = SageInterface::getEnclosingProcedure(stmt, true);

#if 1 
    // Liao 5/20/2010, NodeQuery::querySubTree() is very expensive
   // using memory pool traversal instead as a workaround
  VariantVector vv(V_SgGotoStatement);
  Rose_STL_Container<SgNode*> allGotos = NodeQuery::queryMemoryPool(vv);
  for (Rose_STL_Container<SgNode*>::const_iterator i = allGotos.begin(); i != allGotos.end(); ++i) 
  {
    if (SageInterface::isAncestor(thisFunction,*i ))
    {
      SgLabelRefExp* lRef = isSgGotoStatement(*i)->get_label_expression();
      if (!lRef) continue;
      SgLabelSymbol* sym = lRef->get_symbol();
      ROSE_ASSERT(sym);
      if (getCFGTargetOfFortranLabelSymbol(sym) == cfgNode) {
        makeEdge(CFGNode(isSgGotoStatement(*i), 0), cfgNode, result);
      }
    }
  }
#else
  Rose_STL_Container<SgNode*> allGotos = NodeQuery::querySubTree(thisFunction, V_SgGotoStatement);
  for (Rose_STL_Container<SgNode*>::const_iterator i = allGotos.begin(); i != allGotos.end(); ++i) 
  {

    SgLabelRefExp* lRef = isSgGotoStatement(*i)->get_label_expression();
    if (!lRef) continue;
    SgLabelSymbol* sym = lRef->get_symbol();
    ROSE_ASSERT(sym);
    if (getCFGTargetOfFortranLabelSymbol(sym) == cfgNode) {
      makeEdge(CFGNode(isSgGotoStatement(*i), 0), cfgNode, result);
    }
  }
#endif  

  // Liao 5/20/2010, NodeQuery::querySubTree() is very expensive when used to generate virtual CFG on the fly
  // I have to skip unnecessary queries here
  if (SageInterface::is_Fortran_language()) 
  {
    Rose_STL_Container<SgNode*> allComputedGotos = NodeQuery::querySubTree(thisFunction, V_SgComputedGotoStatement);
    for (Rose_STL_Container<SgNode*>::const_iterator i = allComputedGotos.begin(); i != allComputedGotos.end(); ++i) {
      const Rose_STL_Container<SgExpression*>& labels = isSgComputedGotoStatement(*i)->get_labelList()->get_expressions();
      for (Rose_STL_Container<SgExpression*>::const_iterator j = labels.begin(); j != labels.end(); ++j) {
        SgLabelRefExp* lRef = isSgLabelRefExp(*j);
        ROSE_ASSERT (lRef);
        SgLabelSymbol* sym = lRef->get_symbol();
        ROSE_ASSERT(sym);
        if (getCFGTargetOfFortranLabelSymbol(sym) == cfgNode) {
          makeEdge(CFGNode(isSgComputedGotoStatement(*i), 1), cfgNode, result);
        }
      }
    }
    Rose_STL_Container<SgNode*> allArithmeticIfs = NodeQuery::querySubTree(thisFunction, V_SgArithmeticIfStatement);
    for (Rose_STL_Container<SgNode*>::const_iterator i = allArithmeticIfs.begin(); i != allArithmeticIfs.end(); ++i) {
      SgArithmeticIfStatement* aif = isSgArithmeticIfStatement(*i);
      if (getCFGTargetOfFortranLabelRef(aif->get_less_label()) == cfgNode ||
          getCFGTargetOfFortranLabelRef(aif->get_equal_label()) == cfgNode ||
          getCFGTargetOfFortranLabelRef(aif->get_greater_label()) == cfgNode) {
        makeEdge(CFGNode(aif, 1), cfgNode, result);
      }
    }
  }
}

static CFGNode getNodeJustAfterInContainer(SgNode* n) {
  // Only handles next-statement control flow
  SgNode* parent = n->get_parent();
  if (isSgFunctionParameterList(n)) {
    SgFunctionDeclaration* decl = isSgFunctionDeclaration(isSgFunctionParameterList(n)->get_parent());
    ROSE_ASSERT (decl);
    return CFGNode(decl->get_definition(), 1);
  }
  return CFGNode(parent, parent->cfgFindNextChildIndex(n));
}

//! Find the CFG node of which n is a child (subtree descended into)
//! This is mostly just doing lookups in the children of n's parent to find
//! out which index n is at
static CFGNode findParentNode(SgNode* n) {
  SgNode* parent = n->get_parent();
  ROSE_ASSERT (parent);
  if (isSgFunctionDefinition(n)) return CFGNode(0, 0); // Should not be used
  if (isSgFunctionParameterList(n)) {
    SgFunctionDeclaration* decl = isSgFunctionDeclaration(isSgFunctionParameterList(n)->get_parent());
    ROSE_ASSERT (decl);
    return CFGNode(decl->get_definition(), 0);
  }
  return CFGNode(parent, parent->cfgFindChildIndex(n));
}

static CFGNode getNodeJustBeforeInContainer(SgNode* n) {
  // Only handles previous-statement control flow
  return findParentNode(n);
}
//---------------------------------------
unsigned int
SgStatement::cfgIndexForEnd() const {
    std::cerr << "Bad statement case " << this->class_name() << " in cfgIndexForEnd()" << std::endl;
    ROSE_ASSERT (false);
    return 0;
  }

bool
SgStatement::cfgIsIndexInteresting(unsigned int idx) const {
    // Default -- overridden in some cases
    return idx == this->cfgIndexForEnd();
  }

unsigned int
SgStatement::cfgFindChildIndex(SgNode* n) {
    // Default -- overridden in some cases
    size_t idx = this->getChildIndex(n);
    ROSE_ASSERT (idx != (size_t)(-1)); // Not found
    return idx;
  }

unsigned int
SgStatement::cfgFindNextChildIndex(SgNode* n) {
    // Default -- overridden in some cases
    return this->cfgFindChildIndex(n) + 1;
  }

std::vector<CFGEdge>
SgStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::cerr << "Bad statement case " << this->class_name() << " in cfgOutEdges()" << std::endl;
    ROSE_ASSERT (false);
    return std::vector<CFGEdge>();
  }

std::vector<CFGEdge>
SgStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::cerr << "Bad statement case " << this->class_name() << " in cfgInEdges()" << std::endl;
    ROSE_ASSERT (false);
    return std::vector<CFGEdge>();
  }

bool SgStatement::isChildUsedAsLValue(const SgExpression* child) const
{
	return false;
}

//---------------------------------------
std::vector<CFGEdge> SgGlobal::cfgOutEdges(unsigned int idx, bool interprocedural) {
  return std::vector<CFGEdge>();
}

std::vector<CFGEdge> SgGlobal::cfgInEdges(unsigned int idx, bool interprocedural) {
  return std::vector<CFGEdge>();
}

unsigned int
SgGlobal::cfgIndexForEnd() const {
  return 0;
}

bool 
SgGlobal::cfgIsIndexInteresting(unsigned int idx) const {
  return false;
}

unsigned int
SgBasicBlock::cfgIndexForEnd() const {
  return this->get_statements().size();
}

bool SgBasicBlock::cfgIsIndexInteresting(unsigned int idx) const {
  return false;
}

std::vector<CFGEdge> SgBasicBlock::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (idx == this->get_statements().size()) {
    makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  } else if (idx < this->get_statements().size()) {
    makeEdge(CFGNode(this, idx), this->get_statements()[idx]->cfgForBeginning(), result);
  } else ROSE_ASSERT (!"Bad index for SgBasicBlock");
  return result;
}

std::vector<CFGEdge> SgBasicBlock::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (idx == 0) {
    makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  } else if (idx <= this->get_statements().size()) {
    makeEdge(this->get_statements()[idx - 1]->cfgForEnd(), CFGNode(this, idx), result);
  } else ROSE_ASSERT (!"Bad index for SgBasicBlock");
  return result;
}

unsigned int
SgIfStmt::cfgIndexForEnd() const
   {
     return 2;
   }

bool
SgIfStmt::cfgIsIndexInteresting(unsigned int idx) const
   {
     return idx == 1;
   }

unsigned int
SgIfStmt::cfgFindChildIndex(SgNode* n)
   {
  // DQ (8/24/2006): Could be rewritten as:
  // Make sure that this is either the conditional, true body, or the false body
  // ROSE_ASSERT (n == this->get_conditional() || n == this->get_true_body() || n == this->get_false_body() );
  // return (n == this->get_conditional()) ? 0 : 1;

     if (n == this->get_conditional())
        {
          return 0;
        }
       else
          if (n != NULL && (n == this->get_true_body() || n == this->get_false_body()))
             {
               return 1;
             } 
            else
               ROSE_ASSERT (!"Bad child in if statement");

  // DQ (8/24/2006): Added return to avoid compiler warning.
     return 0;
   }

std::vector<CFGEdge>
SgIfStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_conditional()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), this->get_true_body()->cfgForBeginning(), result);
	    if (this->get_false_body()) {
	      makeEdge(CFGNode(this, idx), this->get_false_body()->cfgForBeginning(), result);
	    } else {
	      makeEdge(CFGNode(this, idx), CFGNode(this, 2), result);
	    }
	    break;
    case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgIfStmt");
  }
  return result;
}

std::vector<CFGEdge>
SgIfStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_conditional()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 2: makeEdge(this->get_true_body()->cfgForEnd(), CFGNode(this, idx), result);
	    if (this->get_false_body()) {
	      makeEdge(this->get_false_body()->cfgForEnd(), CFGNode(this, idx), result);
	    } else {
	      makeEdge(CFGNode(this, 1), CFGNode(this, idx), result);
	    }
	    break;
    default: ROSE_ASSERT (!"Bad index for SgIfStmt");
  }
  return result;
}

unsigned int
SgForInitStatement::cfgIndexForEnd() const {
  return this->get_init_stmt().size();
}

std::vector<CFGEdge>
SgForInitStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (idx == this->get_init_stmt().size()) {
    makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  } else if (idx < this->get_init_stmt().size()) {
    makeEdge(CFGNode(this, idx), this->get_init_stmt()[idx]->cfgForBeginning(), result);
  } else ROSE_ASSERT (!"Bad index for SgForInitStatement");
  return result;
}

std::vector<CFGEdge>
SgForInitStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (idx == 0) {
    makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  } else if (idx <= this->get_init_stmt().size()) {
    makeEdge(this->get_init_stmt()[idx - 1]->cfgForEnd(), CFGNode(this, idx), result);
  } else ROSE_ASSERT (!"Bad index for SgForInitStatement");
  return result;
}

unsigned int
SgForStatement::cfgIndexForEnd() const {
  return 4;
}

bool SgForStatement::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 2;
}

unsigned int SgForStatement::cfgFindChildIndex(SgNode* n)
   {
  // DQ (8/24/2006): Could be rewritten as:
  // Make sure that this is either the conditional, true body, or the false body
  // ROSE_ASSERT (n == this->get_for_init_stmt() || n == this->get_test() || n == this->get_loop_body() || n == this->get_increment_expr_root() );
  // return (n == this->get_conditional()) ? 0 : ((n == this->get_test()) ? 1 : ((n == this->get_loop_body()) ? 2 : 3) );

     if (n == this->get_for_init_stmt())
        {
          return 0;
        }
       else
        {
          if (n == this->get_test())
             {
               return 1;
             }
            else
             {
               if (n == this->get_loop_body())
                  {
                    return 2;
                  }
                 else
                  {
                 // if (n == this->get_increment_expr_root())
                    if (n == this->get_increment())
                       {
                         return 3;
                       }
                      else
                       {
                         cerr<<"Error: SgForStatement::cfgFindChildIndex(): cannot find a matching child for SgNode n:";
                         cerr<<n->class_name()<<endl;
                         if (isSgLocatedNode(n))
                         {
                           isSgLocatedNode(n)->get_file_info()->display();
                         }
                         ROSE_ASSERT (!"Bad child in for statement");
                       }
                  }
             }
        }

  // DQ (8/24/2006): Added to avoid compiler warning
     return 0;
   }

unsigned int SgForStatement::cfgFindNextChildIndex(SgNode* n)
   {
     unsigned int parentIndex = this->cfgFindChildIndex(n);

  // DQ (8/24/2006): Modified function to avoid compiler warning about no return value
     unsigned int returnValue;
  // if (parentIndex == 3) return 1; else return parentIndex + 1;
     if (parentIndex == 3) 
          returnValue = 1;
       else
          returnValue = parentIndex + 1;

     return returnValue;
   }

std::vector<CFGEdge> SgForStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_for_init_stmt()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), this->get_test()->cfgForBeginning(), result); break;
    case 2: makeEdge(CFGNode(this, idx), this->get_loop_body()->cfgForBeginning(), result);
	    makeEdge(CFGNode(this, idx), CFGNode(this, 4), result); break;
 // case 3: makeEdge(CFGNode(this, idx), this->get_increment_expr_root()->cfgForBeginning(), result); break;
    case 3: makeEdge(CFGNode(this, idx), this->get_increment()->cfgForBeginning(), result); break;
    case 4: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgForStatement");
  }
  return result;
}

std::vector<CFGEdge> SgForStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_for_init_stmt()->cfgForEnd(), CFGNode(this, idx), result);
            makeEdge(this->get_increment()->cfgForEnd(), CFGNode(this, idx), result);
	    break;
    case 2: makeEdge(this->get_test()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 3: {
      makeEdge(this->get_loop_body()->cfgForEnd(), CFGNode(this, idx), result);
      vector<SgContinueStmt*> continueStmts = SageInterface::findContinueStmts(this->get_loop_body(), "");
      for (unsigned int i = 0; i < continueStmts.size(); ++i) {
	makeEdge(CFGNode(continueStmts[i], 0), CFGNode(this, idx), result);
      }
      break;
    }
    case 4: {
      makeEdge(CFGNode(this, 2), CFGNode(this, idx), result);
      vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_loop_body(), "");
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
	makeEdge(CFGNode(breakStmts[i], 0), CFGNode(this, idx), result);
      }
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgForStatement");
  }
  return result;
}

static void addOutEdgeOrBypassForExpressionChild(SgNode* me, unsigned int idx, SgExpression* e, vector<CFGEdge>& result) {
  if (e) {
    makeEdge(CFGNode(me, idx), e->cfgForBeginning(), result);
  } else {
    makeEdge(CFGNode(me, idx), CFGNode(me, idx + 1), result);
  }
}

static void addInEdgeOrBypassForExpressionChild(SgNode* me, unsigned int idx, SgExpression* e, vector<CFGEdge>& result) {
  if (e) {
    makeEdge(e->cfgForEnd(), CFGNode(me, idx), result);
  } else {
    makeEdge(CFGNode(me, idx - 1), CFGNode(me, idx), result);
  }
}

// Forall CFG layout:
// forall:0 -> header -> forall:1 (representing initial assignments)
// forall:1 -> forall:2 (conditioned on loop tests) and forall:7
// forall:2 -> mask (if any) -> forall:3
// forall:3 -> forall:4 (on mask)
// forall:3 -> forall:6 (on !mask)
// forall:4 -> body -> forall:5 -> forall:6 (for increment(s)) -> forall:1
// forall:7 -> successor

unsigned int SgForAllStatement::cfgIndexForEnd() const {
  return 7;
}

bool SgForAllStatement::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 1 || idx == 3 || idx == 6;
}

unsigned int SgForAllStatement::cfgFindChildIndex(SgNode* tgt) {
  if (tgt == this->get_forall_header()) {
    return 0;
  } else if (tgt && tgt == SageInterface::forallMaskExpression(this)) {
    return 2;
  } else if (tgt == this->get_body()) {
    return 4;
  } else { ROSE_ASSERT (!"Bad child in SgForAllStatement::cfgFindChildIndex()"); /* avoid MSVC warning. */ return 0; }
}

unsigned int SgForAllStatement::cfgFindNextChildIndex(SgNode* n) {
  return this->cfgFindChildIndex(n) + 1;
}

std::vector<CFGEdge> SgForAllStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  vector<CFGEdge> result;
  switch (idx) {
    case 0: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_forall_header(), result); break;
    case 1: {
      makeEdge(CFGNode(this, 1), CFGNode(this, 2), result);
      makeEdge(CFGNode(this, 1), CFGNode(this, 7), result);
      break;
    }
    case 2: addOutEdgeOrBypassForExpressionChild(this, idx, SageInterface::forallMaskExpression(this), result); break;
    case 3: {
      makeEdge(CFGNode(this, 3), CFGNode(this, 4), result);
      makeEdge(CFGNode(this, 3), CFGNode(this, 6), result);
      break;
    }
    case 4: makeEdge(CFGNode(this, 4), this->get_body()->cfgForBeginning(), result); break;
    case 5: makeEdge(CFGNode(this, 5), CFGNode(this, 6), result); break;
    case 6: makeEdge(CFGNode(this, 6), CFGNode(this, 1), result); break;
    case 7: makeEdge(CFGNode(this, 7), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index in SgForAllStatement::cfgOutEdges()");
  }
  return result;
}

std::vector<CFGEdge> SgForAllStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: {
      addInEdgeOrBypassForExpressionChild(this, idx, this->get_forall_header(), result);
      makeEdge(CFGNode(this, 6), CFGNode(this, 1), result);
      break;
    }
    case 2: makeEdge(CFGNode(this, 1), CFGNode(this, 2), result); break;
    case 3: addInEdgeOrBypassForExpressionChild(this, idx, SageInterface::forallMaskExpression(this), result); break;
    case 4: makeEdge(CFGNode(this, 3), CFGNode(this, 4), result); break;
    case 5: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, 5), result); break;
    case 6: {
      makeEdge(CFGNode(this, 3), CFGNode(this, 6), result);
      makeEdge(CFGNode(this, 5), CFGNode(this, 6), result);
      break;
    }
    case 7: makeEdge(CFGNode(this, 1), CFGNode(this, 7), result); break;
    default: ROSE_ASSERT (!"Bad index in SgForAllStatement::cfgInEdges()");
  }
  return result;
}

unsigned int SgUpcForAllStatement::cfgIndexForEnd() const
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return 0;
  // return doForallCfgIndexForEnd(this);
   }

bool SgUpcForAllStatement::cfgIsIndexInteresting(unsigned int idx) const 
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return false;
  // return doForallCfgIsIndexInteresting(this, idx);
   }

unsigned int SgUpcForAllStatement::cfgFindChildIndex(SgNode* n)
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return 0;
  // return doForallCfgFindChildIndex(this, n);
   }

unsigned int SgUpcForAllStatement::cfgFindNextChildIndex(SgNode* n)
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return 0;
  // return doForallCfgFindNextChildIndex(this, n);
   }

std::vector<CFGEdge> SgUpcForAllStatement::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return std::vector<CFGEdge>();
  // return doForallCfgOutEdges(this, idx);
   }

std::vector<CFGEdge> SgUpcForAllStatement::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return std::vector<CFGEdge>();
  // return doForallCfgInEdges(this, idx);
   }

unsigned int
SgFunctionDeclaration::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgFunctionDeclaration::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgFunctionDeclaration::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgFunctionParameterList::cfgIndexForEnd() const
   {
     return this->get_args().size();
   }

std::vector<CFGEdge>
SgFunctionParameterList::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     if (idx == this->get_args().size())
        {
          makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
        }
       else
        {
          if (idx < this->get_args().size())
             {
               makeEdge(CFGNode(this, idx), this->get_args()[idx]->cfgForBeginning(), result);
             }
            else
             {
               ROSE_ASSERT (!"Bad index for SgFunctionParameterList");
             }
        }

     return result;
   }

std::vector<CFGEdge>
SgFunctionParameterList::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     addIncomingFortranGotos(this, idx, result);
     if (idx == 0)
        {
          makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
        }
       else
        {
          if (idx <= this->get_args().size())
             {
               makeEdge(this->get_args()[idx - 1]->cfgForEnd(), CFGNode(this, idx), result);
             }
            else
             {
               ROSE_ASSERT (!"Bad index for SgFunctionParameterList");
             }
        }

     return result;
   }

unsigned int
SgFunctionDefinition::cfgIndexForEnd() const {
  return 2;
}

bool
SgFunctionDefinition::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 0 || idx == 2;
}

unsigned int 
SgFunctionDefinition::cfgFindChildIndex(SgNode* n)
   {
  // DQ (8/24/2006): Could be rewritten as:
  // Make sure that this is either the conditional, true body, or the false body
  // ROSE_ASSERT (n == this->get_declaration()->get_parameterList() || n == this->get_body() );
  // return (n == this->get_declaration()->get_parameterList()) ? 0 : 1;

     if (n == this->get_declaration()->get_parameterList())
        {
          return 0;
        }
       else 
          if (n == this->get_body())
             {
               return 1;
             }
            else
               ROSE_ASSERT (!"Bad child in function definition");

  // DQ (8/24/2006): Added return to avoid compiler warning.
     return 0;
   }

std::vector<CFGEdge> 
SgFunctionDefinition::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_declaration()->get_parameterList()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2: { 
     if (! interprocedural) break;
     VariantVector vv(V_SgFunctionCallExp);
     Rose_STL_Container<SgNode*> returnSites = NodeQuery::queryMemoryPool(vv);
     for (unsigned int i = 0; i < returnSites.size(); ++i) { 
         if (isSgFunctionCallExp(returnSites[i])->getAssociatedFunctionDeclaration() ==
             this->get_declaration()) {
         makeEdge(CFGNode(this, idx), returnSites[i]->cfgForEnd(), result);
         }
     }
     break;
    }
    default: ROSE_ASSERT (!"Bad index for SgFunctionDefinition");
  }
  return result;
}

std::vector<CFGEdge> SgFunctionDefinition::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: {
     if (! interprocedural) break;
     VariantVector vv(V_SgFunctionCallExp);
     Rose_STL_Container<SgNode*> callExprs = NodeQuery::queryMemoryPool(vv);
     for (unsigned int i = 0; i < callExprs.size(); ++i) 
         makeEdge(callExprs[i]->cfgForEnd(), CFGNode(this, idx), result);
     break;
    }
    case 1: makeEdge(this->get_declaration()->get_parameterList()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 2: {
      makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result);
      // Liao, 5/21/2010. bad implementation since vectors are created/destroyed  multiple times
      //std::vector<SgReturnStmt*> returnStmts = SageInterface::findReturnStmts(this);
     //      Rose_STL_Container <SgNode*> returnStmts = NodeQuery::querySubTree(this,V_SgReturnStmt);
     VariantVector vv(V_SgReturnStmt);
     Rose_STL_Container<SgNode*> returnStmts = NodeQuery::queryMemoryPool(vv);
     for (unsigned int i = 0; i < returnStmts.size(); ++i) {
       if (SageInterface::isAncestor(this,returnStmts[i] ))
         makeEdge(isSgReturnStmt(returnStmts[i])->cfgForEnd(), CFGNode(this, idx), result);
     }
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgFunctionDefinition");
  }
  return result;
}

unsigned int
SgMemberFunctionDeclaration::cfgIndexForEnd() const {
  return 0;
}

// FIXME: these both assume that a function declaration is outside any
// function, which may not actually be the case.

std::vector<CFGEdge> SgMemberFunctionDeclaration::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  // makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgMemberFunctionDeclaration::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  // makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgVariableDeclaration::cfgIndexForEnd() const {
  return this->get_variables().size();
}

unsigned int
SgVariableDeclaration::cfgFindChildIndex(SgNode* n) {
    size_t idx = this->get_childIndex(n);
    ROSE_ASSERT (idx != (size_t)(-1)); // Not found
    ROSE_ASSERT (idx != 0); // Not found
    return idx - 1;
  }

std::vector<CFGEdge> SgVariableDeclaration::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (idx == this->get_variables().size()) {
    makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  } else if (idx < this->get_variables().size()) {
    makeEdge(CFGNode(this, idx), this->get_variables()[idx]->cfgForBeginning(), result);
  } else ROSE_ASSERT (!"Bad index for SgVariableDeclaration");
  return result;
}

std::vector<CFGEdge> SgVariableDeclaration::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (idx == 0) {
    makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  } else if (idx <= this->get_variables().size()) {
    makeEdge(this->get_variables()[idx - 1]->cfgForEnd(), CFGNode(this, idx), result);
  } else ROSE_ASSERT (!"Bad index for SgVariableDeclaration");
  return result;
}

unsigned int
SgClassDeclaration::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgClassDeclaration::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgClassDeclaration::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgEnumDeclaration::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgEnumDeclaration::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgEnumDeclaration::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgExprStatement::cfgIndexForEnd() const {
  return 1;
}

std::vector<CFGEdge> SgExprStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
 // case 0: makeEdge(CFGNode(this, idx), this->get_expression_root()->cfgForBeginning(), result); break;
    case 0: makeEdge(CFGNode(this, idx), this->get_expression()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgExprStatement");
  }
  return result;
}

std::vector<CFGEdge> SgExprStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
 // case 1: makeEdge(this->get_expression_root()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_expression()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgExprStatement");
  }
  return result;
}

unsigned int
SgLabelStatement::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge> SgLabelStatement::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgLabelStatement");
        }

     return result;
   }

std::vector<CFGEdge> SgLabelStatement::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     addIncomingFortranGotos(this, idx, result);
     switch (idx)
        {
          case 0:
             {
               makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
            // Find all gotos to this label, functionwide
               SgFunctionDefinition* thisFunction = SageInterface::getEnclosingProcedure(this);
               std::vector<SgGotoStatement*> gotos = SageInterface::findGotoStmts(thisFunction, this);
               for (unsigned int i = 0; i < gotos.size(); ++i)
                  {
                    makeEdge(CFGNode(gotos[i], 0), CFGNode(this, idx), result);
                  }
               break;
            }

          default: ROSE_ASSERT (!"Bad index for SgLabelStatement");
        }
     return result;
   }

bool SgWhileStmt::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 1;
}

unsigned int
SgWhileStmt::cfgIndexForEnd() const {
  return 2;
}

unsigned int SgWhileStmt::cfgFindNextChildIndex(SgNode* n)
   {
     unsigned int parentIndex = this->cfgFindChildIndex(n);
  // DQ (8/24/2006): Modified function to avoid compiler warning about no return value
     unsigned int returnValue;
  // if (parentIndex == 1) return 0; else return parentIndex + 1;
     if (parentIndex == 1)
          returnValue = 0;
       else
          returnValue = parentIndex + 1;

     return returnValue;
   }

std::vector<CFGEdge> SgWhileStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result);
	    makeEdge(CFGNode(this, idx), CFGNode(this, 2), result); break;
    case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgWhileStmt");
  }
  return result;
}

std::vector<CFGEdge> SgWhileStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result);
      std::vector<SgContinueStmt*> continueStmts = SageInterface::findContinueStmts(this->get_body(), this->get_string_label());
      for (unsigned int i = 0; i < continueStmts.size(); ++i) {
	makeEdge(CFGNode(continueStmts[i], 0), CFGNode(this, idx), result);
      }
      break;
    }
    case 1: makeEdge(this->get_condition()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 2: {
      makeEdge(CFGNode(this, 1), CFGNode(this, idx), result);
      std::vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_body(), this->get_string_label());
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
	makeEdge(CFGNode(breakStmts[i], 0), CFGNode(this, idx), result);
      }
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgWhileStmt");
  }
  return result;
}

unsigned int
SgDoWhileStmt::cfgIndexForEnd() const {
  return 3;
}

bool SgDoWhileStmt::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 2;
}

std::vector<CFGEdge> SgDoWhileStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    case 2: makeEdge(CFGNode(this, idx), CFGNode(this, 0), result);
	    makeEdge(CFGNode(this, idx), CFGNode(this, 3), result); break;
    case 3: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDoWhileStmt");
  }
  return result;
}

std::vector<CFGEdge> SgDoWhileStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
	    makeEdge(CFGNode(this, 2), CFGNode(this, idx), result); break;
    case 1: {
      makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result);
      vector<SgContinueStmt*> continueStmts = SageInterface::findContinueStmts(this->get_body(), "");
      for (unsigned int i = 0; i < continueStmts.size(); ++i) {
	makeEdge(CFGNode(continueStmts[i], 0), CFGNode(this, idx), result);
      }
      break;
    }
    case 2: {
      makeEdge(this->get_condition()->cfgForEnd(), CFGNode(this, idx), result);
      break;
    }
    case 3: {
      makeEdge(CFGNode(this, 2), CFGNode(this, idx), result);
      vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_body(), "");
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
	makeEdge(CFGNode(breakStmts[i], 0), CFGNode(this, idx), result);
      }
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgDoWhileStmt");
  }
  return result;
}

unsigned int
SgSwitchStatement::cfgIndexForEnd() const {
  return 2;
}

bool SgSwitchStatement::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 1;
}

std::vector<CFGEdge> SgSwitchStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_item_selector()->cfgForBeginning(), result); break;
    case 1: {
      vector<SgStatement*> cases = SageInterface::getSwitchCases(this); // Also includes default statements
      bool hasDefault = false;
      for (unsigned int i = 0; i < cases.size(); ++i) {
	makeEdge(CFGNode(this, idx), cases[i]->cfgForBeginning(), result);
	if (isSgDefaultOptionStmt(cases[i])) {
          hasDefault = true;
        }
      }
      if (!hasDefault) {
        makeEdge(CFGNode(this, idx), CFGNode(this, 2), result);
      }
      break;
    }
    case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgSwitchStatement");
  }
  return result;
}

std::vector<CFGEdge> SgSwitchStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_item_selector()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 2: {
      makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result);
      vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_body(), "");
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
	makeEdge(CFGNode(breakStmts[i], 0), CFGNode(this, idx), result);
      }
      vector<SgStatement*> cases = SageInterface::getSwitchCases(this); // Also includes default statements
      bool hasDefault = false;
      for (unsigned int i = 0; i < cases.size(); ++i) {
	if (isSgDefaultOptionStmt(cases[i])) {
          hasDefault = true;
          break;
        }
      }
      if (!hasDefault) {
        makeEdge(CFGNode(this, 1), CFGNode(this, idx), result);
      }
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgSwitchStatement");
  }
  return result;
}

unsigned int
SgCaseOptionStmt::cfgIndexForEnd() const {
  return 1;
}


bool SgCaseOptionStmt::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 0;
}

unsigned int SgCaseOptionStmt::cfgFindChildIndex(SgNode* n) {
  ROSE_ASSERT (n == this->get_body());
  return 0;
}

std::vector<CFGEdge> SgCaseOptionStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgCaseOptionStmt");
  }
  return result;
}

std::vector<CFGEdge> SgCaseOptionStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      SgSwitchStatement* mySwitch = SageInterface::findEnclosingSwitch(this);
      makeEdge(CFGNode(mySwitch, 1), CFGNode(this, idx), result);
      break;
    }
    case 1: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgCaseOptionStmt");
  }
  return result;
}

unsigned int
SgTryStmt::cfgIndexForEnd() const {
  return 1;
}

bool SgTryStmt::cfgIsIndexInteresting(unsigned int idx) const {
  return true;
}

std::vector<CFGEdge> SgTryStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgTryStmt");
  }
  return result;
}

std::vector<CFGEdge> SgTryStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgTryStmt");
  }
  return result;
}

unsigned int
SgCatchStatementSeq::cfgIndexForEnd() const {
  return this->get_catch_statement_seq().size();
}

bool SgCatchStatementSeq::cfgIsIndexInteresting(unsigned int idx) const {
  return false;
}

std::vector<CFGEdge> SgCatchStatementSeq::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  // FIXME
  if (idx == this->get_catch_statement_seq().size()) {
    makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  } else if (idx < this->get_catch_statement_seq().size()) {
    makeEdge(CFGNode(this, idx), this->get_catch_statement_seq()[idx]->cfgForBeginning(), result);
  }
  return result;
}

std::vector<CFGEdge> SgCatchStatementSeq::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  // FIXME
  if (idx == 0) {
    makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  } else if (idx <= this->get_catch_statement_seq().size()) {
    makeEdge(this->get_catch_statement_seq()[idx - 1]->cfgForEnd(), CFGNode(this, idx), result);
  } else ROSE_ASSERT (!"Bad index for SgCatchStatementSeq");
  return result;
}

unsigned int
SgCatchOptionStmt::cfgIndexForEnd() const {
  return 2;
}

std::vector<CFGEdge> SgCatchOptionStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgCatchOptionStmt");
  }
  return result;
}

std::vector<CFGEdge> SgCatchOptionStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_condition()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 2: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgCatchOptionStmt");
  }
  return result;
}

unsigned int
SgDefaultOptionStmt::cfgIndexForEnd() const {
  return 1;
}

bool SgDefaultOptionStmt::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 0;
}

std::vector<CFGEdge> SgDefaultOptionStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDefaultOptionStmt");
  }
  return result;
}

std::vector<CFGEdge> SgDefaultOptionStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      SgSwitchStatement* mySwitch = SageInterface::findEnclosingSwitch(this);
      makeEdge(CFGNode(mySwitch, 1), CFGNode(this, idx), result);
      break;
    }
    case 1: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDefaultOptionStmt");
  }
  return result;
}

unsigned int
SgBreakStmt::cfgIndexForEnd() const {
  return 1; // So the end of this construct will be an unreachable node
}

bool SgBreakStmt::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 0;
}

std::vector<CFGEdge> SgBreakStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: {
      SgStatement* enc = SageInterface::findEnclosingLoop(this, this->get_do_string_label(), true); // May also find a switch statement in C or C++
      ROSE_ASSERT (enc);
      unsigned int newIndex;
      switch (enc->variantT()) {
	case V_SgDoWhileStmt: newIndex = 3; break;
	case V_SgForStatement: newIndex = 4; break;
	case V_SgWhileStmt: newIndex = 2; break;
	case V_SgSwitchStatement: newIndex = 2; break;
        case V_SgFortranDo: newIndex = 6; break;
	default: ROSE_ASSERT (false);
      }
      makeEdge(CFGNode(this, idx), CFGNode(enc, newIndex), result);
      break;
    }
    case 1: /* No out edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgBreakStmt");
  }
  return result;
}

std::vector<CFGEdge> SgBreakStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: /* No in edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgBreakStmt");
  }
  return result;
}

unsigned int
SgContinueStmt::cfgIndexForEnd() const {
  return 1; // So the end of this construct will be an unreachable node
}

bool SgContinueStmt::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 0;
}

std::vector<CFGEdge> SgContinueStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: {
      SgStatement* loop = SageInterface::findEnclosingLoop(this, this->get_do_string_label(), false);
      ROSE_ASSERT (loop);
      unsigned int newIndex;
      switch (loop->variantT()) {
	case V_SgDoWhileStmt: newIndex = 1; break;
	case V_SgForStatement: newIndex = 3; break;
	case V_SgWhileStmt: newIndex = 0; break;
        case V_SgFortranDo: newIndex = 5; break;
	default: ROSE_ASSERT (false);
      }
      makeEdge(CFGNode(this, idx), CFGNode(loop, newIndex), result);
      break;
    }
    case 1: /* No out edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgContinueStmt");
  }
  return result;
}

std::vector<CFGEdge> SgContinueStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: /* No in edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgContinueStmt");
  }
  return result;
}

unsigned int
SgReturnStmt::cfgIndexForEnd() const
   {
  // return this->get_expression_root() ? 1 : 0;
     return this->get_expression() ? 1 : 0;
   }

std::vector<CFGEdge>
SgReturnStmt::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
  // bool hasReturnValue = this->get_expression_root();
     bool hasReturnValue = this->get_expression();
     bool exitingFunctionNow = idx == 1 || (idx == 0 && !hasReturnValue);
     if (exitingFunctionNow)
        {
          SgFunctionDefinition* enclosingFunc = SageInterface::getEnclosingProcedure(this);
          makeEdge(CFGNode(this, idx), CFGNode(enclosingFunc, 2), result);
        }
       else
        {
          makeEdge(CFGNode(this, idx), this->get_expression()->cfgForBeginning(), result);
        }
     return result;
   }

std::vector<CFGEdge>
SgReturnStmt::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     addIncomingFortranGotos(this, idx, result);
     switch (idx)
        {
          case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
          case 1: ROSE_ASSERT (this->get_expression());
	            makeEdge(this->get_expression()->cfgForEnd(), CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgReturnStmt");
        }
     return result;
   }

unsigned int
SgGotoStatement::cfgIndexForEnd() const {
  return 1; // So the end of this construct will be an unreachable node
}

bool SgGotoStatement::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 0;
}

std::vector<CFGEdge> SgGotoStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: {
      if (this->get_label_expression()) { // A Fortran goto
        makeEdge(CFGNode(this, idx), getCFGTargetOfFortranLabelRef(this->get_label_expression()), result);
      } else { // A C/C++ goto
	makeEdge(CFGNode(this, idx), this->get_label()->cfgForBeginning(), result);
      }
      break;
    }
    case 1: /* No out edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgGotoStatement");
  }
  return result;
}

std::vector<CFGEdge> SgGotoStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: /* No in edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgGotoStatement");
  }
  return result;
}

unsigned int
SgAsmStmt::cfgIndexForEnd() const {
  return 0;
}

// Work around the fact that this node has children that we don't use in the
// CFG, but might still be reached (using outEdges()) from
unsigned int
SgAsmStmt::cfgFindChildIndex(SgNode* n) {
  return 0;
}

std::vector<CFGEdge> SgAsmStmt::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgAsmStmt::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgNullStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgNullStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgNullStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgTypedefDeclaration::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgTypedefDeclaration::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     ROSE_ASSERT (idx == 0);
     std::vector<CFGEdge> result;
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgTypedefDeclaration::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     ROSE_ASSERT (idx == 0);
     std::vector<CFGEdge> result;
     addIncomingFortranGotos(this, idx, result);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgPragmaDeclaration::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgPragmaDeclaration::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgPragmaDeclaration::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgUsingDirectiveStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgUsingDirectiveStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgUsingDirectiveStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgUsingDeclarationStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgUsingDeclarationStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgUsingDeclarationStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgCommonBlock::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge>
SgCommonBlock::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge>
SgCommonBlock::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgModuleStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge>
SgModuleStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge>
SgModuleStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgContainsStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgContainsStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgContainsStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgUseStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgUseStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgUseStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgStopOrPauseStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgStopOrPauseStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  if (this->get_stop_or_pause() == SgStopOrPauseStatement::e_pause) {
    makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  }
  return result;
}

std::vector<CFGEdge> SgStopOrPauseStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

static const unsigned int numberOfFortranIOCommonEdges = 5;

static bool handleFortranIOCommonOutEdges(SgIOStatement* me, unsigned int idx, unsigned int numChildren, vector<CFGEdge>& result) {
  switch (idx - numChildren) {
    case 0: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_io_stmt_list(), result); return true;
    case 1: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_unit(), result); return true;
    case 2: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_iostat(), result); return true;
    case 3: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_err(), result); return true;
    case 4: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_iomsg(), result); return true;
    default: return false;
  }
}

static bool handleFortranIOCommonInEdges(SgIOStatement* me, unsigned int idx, unsigned int numChildren, vector<CFGEdge>& result) {
  switch (idx - numChildren) {
    case 1: addInEdgeOrBypassForExpressionChild(me, idx, me->get_io_stmt_list(), result); return true;
    case 2: addInEdgeOrBypassForExpressionChild(me, idx, me->get_unit(), result); return true;
    case 3: addInEdgeOrBypassForExpressionChild(me, idx, me->get_iostat(), result); return true;
    case 4: addInEdgeOrBypassForExpressionChild(me, idx, me->get_err(), result); return true;
    case 5: addInEdgeOrBypassForExpressionChild(me, idx, me->get_iomsg(), result); return true;
    default: return false;
  }
}

unsigned int
SgPrintStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 1;
}

std::vector<CFGEdge>
SgPrintStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 1, result)) return result;
  switch (idx) {
    case 0: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case numberOfFortranIOCommonEdges + 1: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgPrintStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgPrintStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 1, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    case 1: addInEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgPrintStatement");
  }
  return result;
}

unsigned int
SgReadStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 8;
}

std::vector<CFGEdge>
SgReadStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 8, result)) return result;
  switch (idx) {
    case 0: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case 1: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_rec(), result); break;
    case 2: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_end(), result); break;
    case 3: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_namelist(), result); break;
    case 4: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_advance(), result); break;
    case 5: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_size(), result); break;
    case 6: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_eor(), result); break;
    case 7: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case numberOfFortranIOCommonEdges + 8: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgReadStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgReadStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 8, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    case 1: addInEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case 2: addInEdgeOrBypassForExpressionChild(this, idx, this->get_rec(), result); break;
    case 3: addInEdgeOrBypassForExpressionChild(this, idx, this->get_end(), result); break;
    case 4: addInEdgeOrBypassForExpressionChild(this, idx, this->get_namelist(), result); break;
    case 5: addInEdgeOrBypassForExpressionChild(this, idx, this->get_advance(), result); break;
    case 6: addInEdgeOrBypassForExpressionChild(this, idx, this->get_size(), result); break;
    case 7: addInEdgeOrBypassForExpressionChild(this, idx, this->get_eor(), result); break;
    case 8: addInEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgReadStatement");
  }
  return result;
}

unsigned int
SgWriteStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 5;
}

std::vector<CFGEdge>
SgWriteStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 5, result)) return result;
  switch (idx) {
    case 0: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case 1: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_rec(), result); break;
    case 2: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_namelist(), result); break;
    case 3: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_advance(), result); break;
    case 4: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case numberOfFortranIOCommonEdges + 5: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgWriteStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgWriteStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 5, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    case 1: addInEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case 2: addInEdgeOrBypassForExpressionChild(this, idx, this->get_rec(), result); break;
    case 3: addInEdgeOrBypassForExpressionChild(this, idx, this->get_namelist(), result); break;
    case 4: addInEdgeOrBypassForExpressionChild(this, idx, this->get_advance(), result); break;
    case 5: addInEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgWriteStatement");
  }
  return result;
}

unsigned int
SgOpenStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 13;
}

std::vector<CFGEdge>
SgOpenStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 13, result)) return result;
  switch (idx) {
    case 0: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_file(), result); break;
    case 1: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_status(), result); break;
    case 2: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_access(), result); break;
    case 3: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_form(), result); break;
    case 4: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_recl(), result); break;
    case 5: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_blank(), result); break;
    case 6: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_position(), result); break;
    case 7: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_action(), result); break;
    case 8: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_delim(), result); break;
    case 9: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_pad(), result); break;
    case 10: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_round(), result); break;
    case 11: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_sign(), result); break;
    case 12: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case numberOfFortranIOCommonEdges + 13: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgOpenStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgOpenStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 13, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    case 1: addInEdgeOrBypassForExpressionChild(this, idx, this->get_file(), result); break;
    case 2: addInEdgeOrBypassForExpressionChild(this, idx, this->get_status(), result); break;
    case 3: addInEdgeOrBypassForExpressionChild(this, idx, this->get_access(), result); break;
    case 4: addInEdgeOrBypassForExpressionChild(this, idx, this->get_form(), result); break;
    case 5: addInEdgeOrBypassForExpressionChild(this, idx, this->get_recl(), result); break;
    case 6: addInEdgeOrBypassForExpressionChild(this, idx, this->get_blank(), result); break;
    case 7: addInEdgeOrBypassForExpressionChild(this, idx, this->get_position(), result); break;
    case 8: addInEdgeOrBypassForExpressionChild(this, idx, this->get_action(), result); break;
    case 9: addInEdgeOrBypassForExpressionChild(this, idx, this->get_delim(), result); break;
    case 10: addInEdgeOrBypassForExpressionChild(this, idx, this->get_pad(), result); break;
    case 11: addInEdgeOrBypassForExpressionChild(this, idx, this->get_round(), result); break;
    case 12: addInEdgeOrBypassForExpressionChild(this, idx, this->get_sign(), result); break;
    case 13: addInEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgOpenStatement");
  }
  return result;
}

unsigned int
SgCloseStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 1;
}

std::vector<CFGEdge>
SgCloseStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 1, result)) return result;
  switch (idx) {
    case 0: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_status(), result); break;
    case numberOfFortranIOCommonEdges + 1: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgCloseStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgCloseStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 1, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    case 1: addInEdgeOrBypassForExpressionChild(this, idx, this->get_status(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgCloseStatement");
  }
  return result;
}

unsigned int
SgInquireStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 27;
}

std::vector<CFGEdge>
SgInquireStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 27, result)) return result;
  switch (idx) {
    case  0: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_file(), result); break;
    case  1: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_access(), result); break;
    case  2: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_form(), result); break;
    case  3: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_recl(), result); break;
    case  4: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_blank(), result); break;
    case  5: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_exist(), result); break;
    case  6: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_opened(), result); break;
    case  7: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_number(), result); break;
    case  8: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_named(), result); break;
    case  9: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_name(), result); break;
    case 10: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_sequential(), result); break;
    case 11: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_direct(), result); break;
    case 12: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_formatted(), result); break;
    case 13: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_unformatted(), result); break;
    case 14: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_nextrec(), result); break;
    case 15: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_position(), result); break;
    case 16: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_action(), result); break;
    case 17: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_read(), result); break;
    case 18: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_write(), result); break;
    case 19: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_readwrite(), result); break;
    case 20: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_delim(), result); break;
    case 21: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_pad(), result); break;
    case 22: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case 23: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_decimal(), result); break;
    case 24: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_stream(), result); break;
    case 25: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_size(), result); break;
    case 26: addOutEdgeOrBypassForExpressionChild(this, idx, this->get_pending(), result); break;
    case numberOfFortranIOCommonEdges + 27: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgInquireStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgInquireStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 27, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    case  1: addInEdgeOrBypassForExpressionChild(this, idx, this->get_file(), result); break;
    case  2: addInEdgeOrBypassForExpressionChild(this, idx, this->get_access(), result); break;
    case  3: addInEdgeOrBypassForExpressionChild(this, idx, this->get_form(), result); break;
    case  4: addInEdgeOrBypassForExpressionChild(this, idx, this->get_recl(), result); break;
    case  5: addInEdgeOrBypassForExpressionChild(this, idx, this->get_blank(), result); break;
    case  6: addInEdgeOrBypassForExpressionChild(this, idx, this->get_exist(), result); break;
    case  7: addInEdgeOrBypassForExpressionChild(this, idx, this->get_opened(), result); break;
    case  8: addInEdgeOrBypassForExpressionChild(this, idx, this->get_number(), result); break;
    case  9: addInEdgeOrBypassForExpressionChild(this, idx, this->get_named(), result); break;
    case 10: addInEdgeOrBypassForExpressionChild(this, idx, this->get_name(), result); break;
    case 11: addInEdgeOrBypassForExpressionChild(this, idx, this->get_sequential(), result); break;
    case 12: addInEdgeOrBypassForExpressionChild(this, idx, this->get_direct(), result); break;
    case 13: addInEdgeOrBypassForExpressionChild(this, idx, this->get_formatted(), result); break;
    case 14: addInEdgeOrBypassForExpressionChild(this, idx, this->get_unformatted(), result); break;
    case 15: addInEdgeOrBypassForExpressionChild(this, idx, this->get_nextrec(), result); break;
    case 16: addInEdgeOrBypassForExpressionChild(this, idx, this->get_position(), result); break;
    case 17: addInEdgeOrBypassForExpressionChild(this, idx, this->get_action(), result); break;
    case 18: addInEdgeOrBypassForExpressionChild(this, idx, this->get_read(), result); break;
    case 19: addInEdgeOrBypassForExpressionChild(this, idx, this->get_write(), result); break;
    case 20: addInEdgeOrBypassForExpressionChild(this, idx, this->get_readwrite(), result); break;
    case 21: addInEdgeOrBypassForExpressionChild(this, idx, this->get_delim(), result); break;
    case 22: addInEdgeOrBypassForExpressionChild(this, idx, this->get_pad(), result); break;
    case 23: addInEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case 24: addInEdgeOrBypassForExpressionChild(this, idx, this->get_decimal(), result); break;
    case 25: addInEdgeOrBypassForExpressionChild(this, idx, this->get_stream(), result); break;
    case 26: addInEdgeOrBypassForExpressionChild(this, idx, this->get_size(), result); break;
    case 27: addInEdgeOrBypassForExpressionChild(this, idx, this->get_pending(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgInquireStatement");
  }
  return result;
}

unsigned int
SgFlushStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 0;
}

std::vector<CFGEdge>
SgFlushStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case numberOfFortranIOCommonEdges + 0: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgFlushStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgFlushStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgFlushStatement");
  }
  return result;
}

unsigned int
SgRewindStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 0;
}

std::vector<CFGEdge>
SgRewindStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case numberOfFortranIOCommonEdges + 0: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgRewindStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgRewindStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgRewindStatement");
  }
  return result;
}

unsigned int
SgBackspaceStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 0;
}

std::vector<CFGEdge>
SgBackspaceStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case numberOfFortranIOCommonEdges + 0: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgBackspaceStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgBackspaceStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgBackspaceStatement");
  }
  return result;
}

unsigned int
SgEndfileStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 0;
}

std::vector<CFGEdge>
SgEndfileStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case numberOfFortranIOCommonEdges + 0: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgEndfileStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgEndfileStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgEndfileStatement");
  }
  return result;
}

unsigned int
SgWaitStatement::cfgIndexForEnd() const {
  return numberOfFortranIOCommonEdges + 0;
}

std::vector<CFGEdge>
SgWaitStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  if (handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case numberOfFortranIOCommonEdges + 0: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgWaitStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgWaitStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  if (handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgWaitStatement");
  }
  return result;
}

unsigned int SgFortranDo::cfgIndexForEnd() const {
  return 6;
}

bool SgFortranDo::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 3 || idx == 5;
}

unsigned int SgFortranDo::cfgFindChildIndex(SgNode* n)
   {
  // DQ (8/24/2006): Could be rewritten as:
  // Make sure that this is either the conditional, true body, or the false body
  // ROSE_ASSERT (n == this->get_for_init_stmt() || n == this->get_test() || n == this->get_loop_body() || n == this->get_increment_expr_root() );
  // return (n == this->get_conditional()) ? 0 : ((n == this->get_test()) ? 1 : ((n == this->get_loop_body()) ? 2 : 3) );

     if (n == this->get_initialization()) {
       return 0;
     } else if (n == this->get_increment()) {
       return 1;
     } else if (n == this->get_bound()) {
       return 2;
     } else if (n == this->get_body()) {
       return 4;
     } else {
       ROSE_ASSERT (!"Bad child in for statement");
     }

  // DQ (8/24/2006): Added to avoid compiler warning
     return 0;
   }

unsigned int SgFortranDo::cfgFindNextChildIndex(SgNode* n)
   {
     unsigned int parentIndex = this->cfgFindChildIndex(n);
     return parentIndex + 1;
   }

std::vector<CFGEdge> SgFortranDo::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_initialization()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), this->get_increment()->cfgForBeginning(), result); break;
    case 2: makeEdge(CFGNode(this, idx), this->get_bound()->cfgForBeginning(), result); break;
    case 3: makeEdge(CFGNode(this, idx), CFGNode(this, 4), result);
            makeEdge(CFGNode(this, idx), CFGNode(this, 6), result); break;
    case 4: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 5: makeEdge(CFGNode(this, idx), CFGNode(this, 3), result); break;
    case 6: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgFortranDo");
  }
  return result;
}

std::vector<CFGEdge> SgFortranDo::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_initialization()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 2: makeEdge(this->get_increment()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 3: makeEdge(this->get_bound()->cfgForEnd(), CFGNode(this, idx), result);
            makeEdge(CFGNode(this, 5), CFGNode(this, idx), result); break;
    case 4: makeEdge(CFGNode(this, 3), CFGNode(this, idx), result); break;
    case 5: {
      makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result);
      vector<SgContinueStmt*> continueStmts = SageInterface::findContinueStmts(this->get_body(), this->get_string_label());
      for (unsigned int i = 0; i < continueStmts.size(); ++i) {
        makeEdge(CFGNode(continueStmts[i], 0), CFGNode(this, idx), result);
      }
      break;
    }
    case 6: {
      makeEdge(CFGNode(this, 3), CFGNode(this, idx), result);
      vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_body(), this->get_string_label());
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
        makeEdge(CFGNode(breakStmts[i], 0), CFGNode(this, idx), result);
      }
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgFortranDo");
  }
  return result;
}

unsigned int
SgInterfaceStatement::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge> SgInterfaceStatement::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     ROSE_ASSERT (idx == 0);
     std::vector<CFGEdge> result;
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge> SgInterfaceStatement::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     ROSE_ASSERT (idx == 0);
     std::vector<CFGEdge> result;
     addIncomingFortranGotos(this, idx, result);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgImplicitStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge>
SgImplicitStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge>
SgImplicitStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgWhereStatement::cfgIndexForEnd() const {
  return 3;
}

std::vector<CFGEdge> SgWhereStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0:
    if (this->get_condition()) {
      makeEdge(CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    } else {
      makeEdge(CFGNode(this, idx), CFGNode(this, idx + 1), result); break;
    }
    case 1: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2:
    if (this->get_elsewhere()) {
      makeEdge(CFGNode(this, idx), this->get_elsewhere()->cfgForBeginning(), result); break;
    } else {
      makeEdge(CFGNode(this, idx), CFGNode(this, idx + 1), result); break;
    }
    case 3: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgWhereStatement");
  }
  return result;
}

std::vector<CFGEdge> SgWhereStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1:
    if (this->get_condition()) {
      makeEdge(this->get_condition()->cfgForEnd(), CFGNode(this, idx), result); break;
    } else {
      makeEdge(CFGNode(this, idx - 1), CFGNode(this, idx), result); break;
    }
    case 2: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 3:
    if (this->get_elsewhere()) {
      makeEdge(this->get_elsewhere()->cfgForEnd(), CFGNode(this, idx), result); break;
    } else {
      makeEdge(CFGNode(this, idx - 1), CFGNode(this, idx), result); break;
    }
    default: ROSE_ASSERT (!"Bad index for SgWhereStatement");
  }
  return result;
}

unsigned int
SgElseWhereStatement::cfgIndexForEnd() const {
  return 3;
}

std::vector<CFGEdge> SgElseWhereStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0:
    if (this->get_condition()) {
      makeEdge(CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    } else {
      makeEdge(CFGNode(this, idx), CFGNode(this, idx + 1), result); break;
    }
    case 1: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2:
    if (this->get_elsewhere()) {
      makeEdge(CFGNode(this, idx), this->get_elsewhere()->cfgForBeginning(), result); break;
    } else {
      makeEdge(CFGNode(this, idx), CFGNode(this, idx + 1), result); break;
    }
    case 3: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgElseWhereStatement");
  }
  return result;
}

std::vector<CFGEdge> SgElseWhereStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1:
    if (this->get_condition()) {
      makeEdge(this->get_condition()->cfgForEnd(), CFGNode(this, idx), result); break;
    } else {
      makeEdge(CFGNode(this, idx - 1), CFGNode(this, idx), result); break;
    }
    case 2: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 3:
    if (this->get_elsewhere()) {
      makeEdge(this->get_elsewhere()->cfgForEnd(), CFGNode(this, idx), result); break;
    } else {
      makeEdge(CFGNode(this, idx - 1), CFGNode(this, idx), result); break;
    }
    default: ROSE_ASSERT (!"Bad index for SgElseWhereStatement");
  }
  return result;
}

unsigned int
SgEquivalenceStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge>
SgEquivalenceStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge>
SgEquivalenceStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgAttributeSpecificationStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge>
SgAttributeSpecificationStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge>
SgAttributeSpecificationStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgArithmeticIfStatement::cfgIndexForEnd() const
   {
     return 1;
   }

std::vector<CFGEdge>
SgArithmeticIfStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_conditional()->cfgForBeginning(), result); break;
    case 1: {
      makeEdge(CFGNode(this, idx), getCFGTargetOfFortranLabelRef(this->get_less_label()), result);
      makeEdge(CFGNode(this, idx), getCFGTargetOfFortranLabelRef(this->get_equal_label()), result);
      makeEdge(CFGNode(this, idx), getCFGTargetOfFortranLabelRef(this->get_greater_label()), result);
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgArithmeticIfStatement");
  }
  return result;
}

std::vector<CFGEdge>
SgArithmeticIfStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_conditional()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgArithmeticIfStatement");
  }
  return result;
}

unsigned int
SgComputedGotoStatement::cfgIndexForEnd() const {
  return 2;
}

bool SgComputedGotoStatement::cfgIsIndexInteresting(unsigned int idx) const {
  return idx == 1;
}

// Someone got the order of the fields of SgComputedGotoStatement backwards so
// the switch condition is not the first field (like it is in
// SgSwitchStatement), so we need to change the order for CFG purposes
unsigned int
SgComputedGotoStatement::cfgFindChildIndex(SgNode* n) {
  if (n == this->get_label_index()) {
    return 0;
  } else if (n == this->get_labelList()) {
    ROSE_ASSERT (!"Should not be doing SgComputedGotoStatement::cfgFindChildIndex() on the label list");
    return 1;
  } else {
    ROSE_ASSERT (!"Bad child in SgComputedGotoStatement::cfgFindChildIndex()");
    return 0;
  }
}

std::vector<CFGEdge> SgComputedGotoStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_label_index()->cfgForBeginning(), result); break;
    case 1: {
      SgExprListExp* labels = this->get_labelList();
      const vector<SgExpression*>& cases = labels->get_expressions();
      for (unsigned int i = 0; i < cases.size(); ++i) {
        SgLabelRefExp* lRef = isSgLabelRefExp(cases[i]);
        makeEdge(CFGNode(this, idx), getCFGTargetOfFortranLabelRef(lRef), result);
      }
      makeEdge(CFGNode(this, idx), CFGNode(this, 2), result); // For default
      break;
    }
    case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgComputedGotoStatement");
  }
  return result;
}

std::vector<CFGEdge> SgComputedGotoStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_label_index()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 2: {
      makeEdge(CFGNode(this, 1), CFGNode(this, idx), result); // For default
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgComputedGotoStatement");
  }
  return result;
}

unsigned int
SgNamelistStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge>
SgNamelistStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge>
SgNamelistStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgImportStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgImportStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgImportStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int
SgAssociateStatement::cfgIndexForEnd() const {
  return 2;
}

std::vector<CFGEdge> SgAssociateStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_variable_declaration()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2: {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgAssociateStatement");
  }
  return result;
}

std::vector<CFGEdge> SgAssociateStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_variable_declaration()->cfgForEnd(), CFGNode(this, idx), result); break;
    case 2: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgAssociateStatement");
  }
  return result;
}

unsigned int
SgFormatStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgFormatStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgFormatStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  ROSE_ASSERT (idx == 0);
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int 
SgExpression::cfgIndexForEnd() const 
   {
     std::cerr << "Bad expression case " << this->class_name() << " in cfgIndexForEnd()" << std::endl;
     ROSE_ASSERT (false);

  // DQ (11/28/2009): This function was already commented out, but must return a value for use in MSVC.
     return 0;
   }

bool
SgExpression::cfgIsIndexInteresting(unsigned int idx) const {
    // Default -- overridden in short-circuiting operators
    return idx == this->cfgIndexForEnd();
  }

unsigned int
SgExpression::cfgFindChildIndex(SgNode* n) {
    // Default -- overridden in some cases
    size_t idx = this->get_childIndex(n);
    ROSE_ASSERT (idx != (size_t)(-1)); // Not found
    return idx;
  }

unsigned int
SgExpression::cfgFindNextChildIndex(SgNode* n) {
    return this->cfgFindChildIndex(n) + 1;
  }

  std::vector<CFGEdge> SgExpression::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::cerr << "Bad expression case " << this->class_name() << " in cfgOutEdges()" << std::endl;
    ROSE_ASSERT (false);

  // DQ (11/28/2009): This function was already commented out, but must return a value for use in MSVC.
     return std::vector<CFGEdge>();
  }

  std::vector<CFGEdge> SgExpression::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::cerr << "Bad expression case " << this->class_name() << " in cfgInEdges()" << std::endl;
    ROSE_ASSERT (false);

  // DQ (11/28/2009): This function was already commented out, but must return a value for use in MSVC.
     return std::vector<CFGEdge>();
  }

unsigned int
SgUnaryOp::cfgIndexForEnd() const 
   {
     return 1;
   }

std::vector<CFGEdge>
SgUnaryOp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0:
			if (this->get_operand())
			{
				makeEdge(CFGNode(this, idx), this->get_operand()->cfgForBeginning(), result); break;
				break;
			}
          case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgUnaryOp");
        }

     return result;
   }

std::vector<CFGEdge>
SgUnaryOp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
          case 1:
			if (this->get_operand())
			{
				makeEdge(this->get_operand()->cfgForEnd(), CFGNode(this, idx), result);
				break;
			}
          default: ROSE_ASSERT (!"Bad index for SgUnaryOp");
        }

     return result;
   }

unsigned int SgBinaryOp::cfgIndexForEnd() const
   {
     return 2;
   }

std::vector<CFGEdge>
SgBinaryOp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(CFGNode(this, idx), this->get_lhs_operand()->cfgForBeginning(), result); break;
          case 1: makeEdge(CFGNode(this, idx), this->get_rhs_operand()->cfgForBeginning(), result); break;
          case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgBinaryOp");
        }

     return result;
   }

std::vector<CFGEdge>
SgBinaryOp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_lhs_operand()->cfgForEnd(), CFGNode(this, idx), result); break;
          case 2: makeEdge(this->get_rhs_operand()->cfgForEnd(), CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgBinaryOp");
        }

     return result;
   }

unsigned int
SgExprListExp::cfgIndexForEnd() const
   {
     return this->get_expressions().size();
   }

std::vector<CFGEdge>
SgExprListExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     if (idx == this->get_expressions().size())
        {
          makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
        }
       else
          if (idx < this->get_expressions().size())
             {
               makeEdge(CFGNode(this, idx), this->get_expressions()[idx]->cfgForBeginning(), result);
             }
            else
               ROSE_ASSERT (!"Bad index for SgExprListExp");

     return result;
   }

std::vector<CFGEdge>
SgExprListExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     if (idx == 0)
        {
          makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
        }
       else
          if (idx <= this->get_expressions().size())
             {
               makeEdge(this->get_expressions()[idx - 1]->cfgForEnd(), CFGNode(this, idx), result);
             }
            else
               ROSE_ASSERT (!"Bad index for SgExprListExp");

     return result;
   }

unsigned int
SgVarRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgVarRefExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgVarRefExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgLabelRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgLabelRefExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgLabelRefExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

#if 0
// DQ (12/30/2007): I don't think there is any control flow through this sorts of variables, but this 
// should be discussed (might apply to assigned goto).

unsigned int
SgLabelRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgLabelRefExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgLabelRefExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }
#endif

unsigned int
SgFunctionRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgFunctionRefExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgFunctionRefExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgMemberFunctionRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgMemberFunctionRefExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgMemberFunctionRefExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgPseudoDestructorRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgPseudoDestructorRefExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgPseudoDestructorRefExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

  unsigned int SgValueExp::cfgIndexForEnd() const {
    return 0;
  }

  std::vector<CFGEdge> SgValueExp::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    ROSE_ASSERT (idx == 0);
    makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
    return result;
  }

  std::vector<CFGEdge> SgValueExp::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    ROSE_ASSERT (idx == 0);
    makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
    return result;
  }

  unsigned int SgFunctionCallExp::cfgIndexForEnd() const {
    return 3;
  }

  std::vector<CFGEdge> SgFunctionCallExp::cfgOutEdges(unsigned int idx, bool interprocedural) {
    ROSE_ASSERT(this);
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(CFGNode(this, idx), this->get_function()->cfgForBeginning(), result); break;
      case 1: makeEdge(CFGNode(this, idx), this->get_args()->cfgForBeginning(), result); break;
      case 2: {
                if (interprocedural) {
                  ClassHierarchyWrapper classHierarchy(SageInterface::getProject());
                  Rose_STL_Container<Properties*> functionList;
                  CallTargetSet::retrieveFunctionDeclarations(this, &classHierarchy, functionList);
                  Rose_STL_Container<Properties*>::iterator prop;
                  for (prop = functionList.begin(); prop != functionList.end(); prop++) {
                    SgFunctionDeclaration* funcDecl = (*prop)->functionDeclaration;
                    ROSE_ASSERT(funcDecl);
                    SgFunctionDeclaration* decl = isSgFunctionDeclaration(funcDecl->get_definingDeclaration());
                    ROSE_ASSERT(decl);
                    SgFunctionDefinition* def = decl->get_definition();
                    if (def == NULL) {
                      std::cerr << "no definition for function in SgFunCallExp::cfgOutEdges: " << decl->get_name().str() << std::endl;
                      break;
                    }
                    makeEdge(CFGNode(this, idx), def->cfgForBeginning(),
                        result);
                  }
                }
                else
                  makeEdge(CFGNode(this, idx), CFGNode(this, 3), result);
                break;
      }
      case 3: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgFunctionCallExp");
    }
    return result;
  }

  std::vector<CFGEdge> SgFunctionCallExp::cfgInEdges(unsigned int idx, bool interprocedural) {
    ROSE_ASSERT(this);
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_function()->cfgForEnd(), CFGNode(this, idx), result); break;
      case 2: makeEdge(this->get_args()->cfgForEnd(), CFGNode(this, idx), result); break;
      case 3: {
                if (interprocedural) {
                  ClassHierarchyWrapper classHierarchy(SageInterface::getProject());
                  Rose_STL_Container<Properties*> functionList;
                  CallTargetSet::retrieveFunctionDeclarations(this, &classHierarchy, functionList);
                  Rose_STL_Container<Properties*>::iterator prop;
                  for (prop = functionList.begin(); prop != functionList.end(); prop++) {
                    SgFunctionDeclaration* funcDecl = (*prop)->functionDeclaration;
                    ROSE_ASSERT(funcDecl);
                    SgFunctionDeclaration* decl = isSgFunctionDeclaration(funcDecl->get_definingDeclaration());
                    ROSE_ASSERT(decl);
                    SgFunctionDefinition* def = decl->get_definition();
                    if (def == NULL) {
                      std::cerr << "no definition for function in SgFunCallExp::cfgInEdges: " << decl->get_name().str() << std::endl;
                      break;
                    }
                    makeEdge(decl->get_definition()->cfgForEnd(), CFGNode(this, idx),
                        result);
                  }
                }
                else
                  makeEdge(CFGNode(this, 2), CFGNode(this, idx), result);
                break;
      }
      default: ROSE_ASSERT (!"Bad index for SgFunctionCallExp");
    }
    return result;
  }

bool
SgAndOp::cfgIsIndexInteresting(unsigned int idx) const
   {
     return idx == 1 || idx == 2;
   }

std::vector<CFGEdge>
SgAndOp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(CFGNode(this, idx), this->get_lhs_operand()->cfgForBeginning(), result); break;
       case 1: makeEdge(CFGNode(this, idx), this->get_rhs_operand()->cfgForBeginning(), result);
	       makeEdge(CFGNode(this, idx), CFGNode(this, 2), result); break;
       case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
       default: ROSE_ASSERT (!"Bad index in SgAndOp");
     }
     return result;
   }

std::vector<CFGEdge>
SgAndOp::cfgInEdges(unsigned int idx, bool interprocedural) 
   {
     std::vector<CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
       case 1: makeEdge(this->get_lhs_operand()->cfgForEnd(), CFGNode(this, idx), result); break;
       case 2: makeEdge(this->get_rhs_operand()->cfgForEnd(), CFGNode(this, idx), result);
	       makeEdge(CFGNode(this, 1), CFGNode(this, idx), result); break;
       default: ROSE_ASSERT (!"Bad index in SgAndOp");
     }
     return result;
   }

bool
SgOrOp::cfgIsIndexInteresting(unsigned int idx) const 
   {
     return idx == 1 || idx == 2;
   }

std::vector<CFGEdge>
SgOrOp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(CFGNode(this, idx), this->get_lhs_operand()->cfgForBeginning(), result); break;
       case 1: makeEdge(CFGNode(this, idx), this->get_rhs_operand()->cfgForBeginning(), result);
	       makeEdge(CFGNode(this, idx), CFGNode(this, 2), result); break;
       case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
       default: ROSE_ASSERT (!"Bad index in SgOrOp");
     }
     return result;
   }

   std::vector<CFGEdge> SgOrOp::cfgInEdges(unsigned int idx, bool interprocedural) {
     std::vector<CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
       case 1: makeEdge(this->get_lhs_operand()->cfgForEnd(), CFGNode(this, idx), result); break;
       case 2: makeEdge(this->get_rhs_operand()->cfgForEnd(), CFGNode(this, idx), result);
	       makeEdge(CFGNode(this, 1), CFGNode(this, idx), result); break;
       default: ROSE_ASSERT (!"Bad index in SgOrOp");
     }
     return result;
   }

unsigned int
SgTypeIdOp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgTypeIdOp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgTypeIdOp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

  unsigned int SgVarArgStartOp::cfgIndexForEnd() const {
    return 2;
  }

  std::vector<CFGEdge> SgVarArgStartOp::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(CFGNode(this, idx), this->get_lhs_operand()->cfgForBeginning(), result); break;
      case 1: makeEdge(CFGNode(this, idx), this->get_rhs_operand()->cfgForBeginning(), result); break;
      case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgStartOp");
    }
    return result;
  }

  std::vector<CFGEdge> SgVarArgStartOp::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_lhs_operand()->cfgForEnd(), CFGNode(this, idx), result); break;
      case 2: makeEdge(this->get_rhs_operand()->cfgForEnd(), CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgStartOp");
    }
    return result;
  }

  unsigned int SgVarArgOp::cfgIndexForEnd() const {
    return 1;
  }

  std::vector<CFGEdge> SgVarArgOp::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(CFGNode(this, idx), this->get_operand_expr()->cfgForBeginning(), result); break;
      case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgOp");
    }
    return result;
  }

  std::vector<CFGEdge> SgVarArgOp::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_operand_expr()->cfgForEnd(), CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgOp");
    }
    return result;
  }

  unsigned int SgVarArgEndOp::cfgIndexForEnd() const {
    return 1;
  }

  std::vector<CFGEdge> SgVarArgEndOp::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(CFGNode(this, idx), this->get_operand_expr()->cfgForBeginning(), result); break;
      case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgEndOp");
    }
    return result;
  }

  std::vector<CFGEdge> SgVarArgEndOp::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_operand_expr()->cfgForEnd(), CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgEndOp");
    }
    return result;
  }

unsigned int
SgConditionalExp::cfgIndexForEnd() const 
  {
    return 2;
  }

  bool SgConditionalExp::cfgIsIndexInteresting(unsigned int idx) const {
    return idx == 1 || idx == 2;
  }

unsigned int
SgConditionalExp::cfgFindChildIndex(SgNode* n)
   {
  // DQ (8/24/2006): Could be rewritten as:
  // Make sure that this is either the conditional, true body, or the fals body
  // ROSE_ASSERT (n == this->get_conditional_exp() || n == this->get_true_exp() || n == this->get_false_exp() );
  // return (n == this->get_conditional_exp()) ? 0 : 1;

     if (n == this->get_conditional_exp())
        {
          return 0;
        }
       else
          if (n == this->get_true_exp() || n == this->get_false_exp())
             {
               return 1;
             }
            else
               ROSE_ASSERT (!"Bad child in conditional expression");

  // DQ (8/24/2006): Added return to avoid compiler warning.
     return 0;
   }

  std::vector<CFGEdge> SgConditionalExp::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(CFGNode(this, idx), this->get_conditional_exp()->cfgForBeginning(), result); break;
      case 1: makeEdge(CFGNode(this, idx), this->get_true_exp()->cfgForBeginning(), result);
	      makeEdge(CFGNode(this, idx), this->get_false_exp()->cfgForBeginning(), result); break;
      case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index in SgConditionalExp");
    }
    return result;
  }

  std::vector<CFGEdge> SgConditionalExp::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_conditional_exp()->cfgForEnd(), CFGNode(this, idx), result); break;
      case 2: makeEdge(this->get_true_exp()->cfgForEnd(), CFGNode(this, idx), result);
	      makeEdge(this->get_false_exp()->cfgForEnd(), CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index in SgConditionalExp");
    }
    return result;
  }

unsigned int
SgCastExp::cfgFindChildIndex(SgNode* n)
   {
     if (n == this->get_operand()) return 0;

         ROSE_ASSERT (!"Bad child in cfgFindChildIndex on SgCastExp");
         // DQ (11/29/2009): Avoid MSVC warning about missing return.
         return 0;
   }


unsigned int
SgNewExp::cfgIndexForEnd() const
  {
    return 2; // At most one of constructor args or array size are present
  }

unsigned int
SgNewExp::cfgFindChildIndex(SgNode* n)
   {
  // DQ (8/24/2006): Could be rewritten as:
  // Make sure that this is either the conditional, true body, or the fals body
  // ROSE_ASSERT (n == this->get_placement_args() || n == this->get_constructor_args() || n == this->get_builtin_args() );
  // return (n == this->get_placement_args()) ? 0 : 1;

     if (n == this->get_placement_args())
        {
          return 0;
        }
       else
          if (n == this->get_constructor_args() || n == this->get_builtin_args())
             {
               return 1;
             }
            else
               ROSE_ASSERT (!"Bad child in new expression");

  // DQ (8/24/2006): Added return to avoid compiler warning.
     return 0;
   }

  std::vector<CFGEdge> SgNewExp::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: {
	if (this->get_placement_args()) {
	  makeEdge(CFGNode(this, idx), this->get_placement_args()->cfgForBeginning(), result);
	} else {
	  makeEdge(CFGNode(this, idx), CFGNode(this, 1), result);
	}
	break;
      }
      case 1: {
	if (this->get_constructor_args()) {
	  makeEdge(CFGNode(this, idx), this->get_constructor_args()->cfgForBeginning(), result);
	} else if (this->get_builtin_args()) {
	  makeEdge(CFGNode(this, idx), this->get_builtin_args()->cfgForBeginning(), result);
	} else {
	  makeEdge(CFGNode(this, idx), CFGNode(this, 2), result);
	}
	break;
      }
      case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgNewExp");
    }
    return result;
  }

  std::vector<CFGEdge> SgNewExp::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
      case 1: {
	if (this->get_placement_args()) {
	  makeEdge(this->get_placement_args()->cfgForEnd(), CFGNode(this, idx), result);
	} else {
	  makeEdge(CFGNode(this, 0), CFGNode(this, idx), result);
	}
	break;
      }
      case 2: {
	if (this->get_constructor_args()) {
	  makeEdge(this->get_constructor_args()->cfgForEnd(), CFGNode(this, idx), result);
	} else if (this->get_builtin_args()) {
	  makeEdge(this->get_builtin_args()->cfgForEnd(), CFGNode(this, idx), result);
	} else {
	  makeEdge(CFGNode(this, 1), CFGNode(this, idx), result);
	}
	break;
      }
      default: ROSE_ASSERT (!"Bad index for SgNewExp");
    }
    return result;
  }

unsigned int SgDeleteExp::cfgIndexForEnd() const
   {
     return 1;
   }

std::vector<CFGEdge>
SgDeleteExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(CFGNode(this, idx), this->get_variable()->cfgForBeginning(), result); break;
          case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgDeleteExp");
        }

     return result;
   }

std::vector<CFGEdge>
SgDeleteExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_variable()->cfgForEnd(), CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgDeleteExp");
        }

     return result;
   }

unsigned int
SgThisExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgThisExp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
  }

std::vector<CFGEdge>
SgThisExp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

unsigned int SgInitializer::cfgIndexForEnd() const
   {
     return 1;
   }

std::vector<CFGEdge>
SgAggregateInitializer::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(CFGNode(this, idx), this->get_initializers()->cfgForBeginning(), result); break;
       case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
       default: ROSE_ASSERT (!"Bad index for SgAggregateInitializer");
     }

     return result;
   }

std::vector<CFGEdge>
SgAggregateInitializer::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
       case 1: makeEdge(this->get_initializers()->cfgForEnd(), CFGNode(this, idx), result); break;
       default: ROSE_ASSERT (!"Bad index for SgAggregateInitializer");
     }

     return result;
   }

std::vector<CFGEdge> SgConstructorInitializer::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(CFGNode(this, idx), this->get_args()->cfgForBeginning(), result); break;
      case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgConstructorInitializer");
    }
    return result;
  }

std::vector<CFGEdge> SgConstructorInitializer::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_args()->cfgForEnd(), CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgConstructorInitializer");
    }
    return result;
  }

  std::vector<CFGEdge> SgAssignInitializer::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(CFGNode(this, idx), this->get_operand()->cfgForBeginning(), result); break;
      case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgAssignInitializer");
    }
    return result;
  }

  std::vector<CFGEdge> SgAssignInitializer::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_operand()->cfgForEnd(), CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgAssignInitializer");
    }
    return result;
  }

  unsigned int SgNullExpression::cfgIndexForEnd() const {
    return 0;
  }

  std::vector<CFGEdge> SgNullExpression::cfgOutEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    ROSE_ASSERT (idx == 0);
    makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
    return result;
  }

  std::vector<CFGEdge> SgNullExpression::cfgInEdges(unsigned int idx, bool interprocedural) {
    std::vector<CFGEdge> result;
    ROSE_ASSERT (idx == 0);
    makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
    return result;
  }

unsigned int
SgStatementExpression::cfgIndexForEnd() const 
   {
     return 1;
   }

std::vector<CFGEdge>
SgStatementExpression::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(CFGNode(this, idx), this->get_statement()->cfgForBeginning(), result); break;
          case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgStatementExpression");
        }

     return result;
   }

std::vector<CFGEdge>
SgStatementExpression::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_statement()->cfgForEnd(), CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgStatementExpression");
        }

     return result;
   }

unsigned int
SgAsmOp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<CFGEdge>
SgAsmOp::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<CFGEdge>
SgAsmOp::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
     return result;
   }

unsigned int SgSubscriptExpression::cfgIndexForEnd() const
   {
     return 3;
   }

std::vector<CFGEdge>
SgSubscriptExpression::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(CFGNode(this, idx), this->get_lowerBound()->cfgForBeginning(), result); break;
          case 1: makeEdge(CFGNode(this, idx), this->get_upperBound()->cfgForBeginning(), result); break;
          case 2: makeEdge(CFGNode(this, idx), this->get_stride()->cfgForBeginning(), result); break;
          case 3: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgSubscriptExpression");
        }

     return result;
   }

std::vector<CFGEdge>
SgSubscriptExpression::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_lowerBound()->cfgForEnd(), CFGNode(this, idx), result); break;
          case 2: makeEdge(this->get_upperBound()->cfgForEnd(), CFGNode(this, idx), result); break;
          case 3: makeEdge(this->get_stride()->cfgForEnd(), CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgSubscriptExpression");
        }

     return result;
   }

unsigned int SgAsteriskShapeExp::cfgIndexForEnd() const {
  return 0;
}

std::vector<CFGEdge> SgAsteriskShapeExp::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  ROSE_ASSERT (idx == 0);
  makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<CFGEdge> SgAsteriskShapeExp::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  ROSE_ASSERT (idx == 0);
  makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
  return result;
}

unsigned int SgImpliedDo::cfgIndexForEnd() const
   {
     return 4;
   }

std::vector<CFGEdge>
SgImpliedDo::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(CFGNode(this, idx), this->get_do_var()->cfgForBeginning(), result); break;
          case 1: makeEdge(CFGNode(this, idx), this->get_first_val()->cfgForBeginning(), result); break;
          case 2: makeEdge(CFGNode(this, idx), this->get_last_val()->cfgForBeginning(), result); break;
          case 3: makeEdge(CFGNode(this, idx), this->get_increment()->cfgForBeginning(), result); break;
          case 4: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgImpliedDo");
        }

     return result;
   }

std::vector<CFGEdge>
SgImpliedDo::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_do_var()->cfgForEnd(), CFGNode(this, idx), result); break;
          case 2: makeEdge(this->get_first_val()->cfgForEnd(), CFGNode(this, idx), result); break;
          case 3: makeEdge(this->get_last_val()->cfgForEnd(), CFGNode(this, idx), result); break;
          case 4: makeEdge(this->get_increment()->cfgForEnd(), CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgImpliedDo");
        }

     return result;
   }

unsigned int
SgActualArgumentExpression::cfgIndexForEnd() const 
   {
     return 1;
   }

std::vector<CFGEdge>
SgActualArgumentExpression::cfgOutEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(CFGNode(this, idx), this->get_expression()->cfgForBeginning(), result); break;
          case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgActualArgumentExpression");
        }

     return result;
   }

std::vector<CFGEdge>
SgActualArgumentExpression::cfgInEdges(unsigned int idx, bool interprocedural)
   {
     std::vector<CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_expression()->cfgForEnd(), CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgActualArgumentExpression");
        }

     return result;
   }

std::vector<CFGEdge> SgDesignatedInitializer::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_memberInit()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDesignatedInitializer");
  }
  return result;
}

std::vector<CFGEdge> SgDesignatedInitializer::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_memberInit()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDesignatedInitializer");
  }
  return result;
}

unsigned int
SgDesignatedInitializer::cfgFindChildIndex(SgNode* n)
   {
     if (n == this->get_memberInit()) return 0;
     ROSE_ASSERT (!"Bad child in cfgFindChildIndex on SgDesignatedInitializer");

     // DQ (11/29/2009): Avoid MSVC warning about missing return.
     return 0;
   }

unsigned int
SgInitializedName::cfgIndexForEnd() const {
  return this->get_initializer() ? 1 : 0;
}

bool
SgInitializedName::cfgIsIndexInteresting(unsigned int idx) const {
  return true;
}

unsigned int
SgInitializedName::cfgFindChildIndex(SgNode* n)
   {
// This function could be rewritten to be:
// Check for inappropriate child in initialized name
// ROSE_ASSERT(n == this->get_initializer());
// return 0;

     if (n == this->get_initializer())
        {
          return 0;
        }
       else
          ROSE_ASSERT (!"Bad child in initialized name");

  // DQ (8/24/2006): Added return to avoid compiler warning!
     return 0;
   }

unsigned int
SgInitializedName::cfgFindNextChildIndex(SgNode* n) {
  return this->cfgFindChildIndex(n) + 1;
}

std::vector<CFGEdge>
SgInitializedName::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0:
    if (this->get_initializer()) {
      makeEdge(CFGNode(this, idx), this->get_initializer()->cfgForBeginning(),
               result);
    } else {
      makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
    }
    break;

    case 1:
    ROSE_ASSERT (this->get_initializer());
    makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result);
    break;

    default: ROSE_ASSERT (!"Bad index in SgInitializedName");
  }
  return result;
}

std::vector<CFGEdge>
SgInitializedName::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0:
    makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result);
    break;

    case 1:
    ROSE_ASSERT (this->get_initializer());
    makeEdge(this->get_initializer()->cfgForEnd(),
             CFGNode(this, idx), result);
    break;

    default: ROSE_ASSERT (!"Bad index in SgInitializedName");
  }
  return result;
}
// Liao, 6/11/2009 support for OpenMP nodes
unsigned int
SgOmpBodyStatement::cfgIndexForEnd() const {
  return 1;
}

std::vector<CFGEdge> SgOmpBodyStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgOmpBodyStatement");
  }
  return result;
}

std::vector<CFGEdge> SgOmpBodyStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgOmpBodyStatement");
  }
  return result;
}
//----------------------------------------
 unsigned int
SgOmpClauseBodyStatement::cfgIndexForEnd() const {
  return 2;
}

std::vector<CFGEdge> SgOmpClauseBodyStatement::cfgOutEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: break; // we don't build edges for OpenMP clause list for now //TODO  not sure if the code is correct
    case 2: makeEdge(CFGNode(this, idx), getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgOmpClauseBodyStatement");
  }
  return result;
}

std::vector<CFGEdge> SgOmpClauseBodyStatement::cfgInEdges(unsigned int idx, bool interprocedural) {
  std::vector<CFGEdge> result;
  addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(getNodeJustBeforeInContainer(this), CFGNode(this, idx), result); break;
    case 1: break; // we don't build edges for OpenMP clause list for now //TODO  not sure if the code is correct here
    case 2: makeEdge(this->get_body()->cfgForEnd(), CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgOmpClauseBodyStatement");
  }
  return result;
}

 
	bool SgExpression::isDefinable() const
	{
		return false;
	}

	bool SgExpression::isUsedAsDefinable() const
	{
		if (isDefinable())
		{
//			SgStatement* stmt = isSgStatement(get_parent());
//			if (stmt)
//				return stmt->isChildUsedAsLValue(this);
			if (SgAssignOp* expr = isSgAssignOp(get_parent()))
			{
				if (expr == expr->get_lhs_operand())
					return true;
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
 
	bool SgExpression::isLValue() const
	{
		return false;
	}

	bool SgExpression::isUsedAsLValue() const
	{
		if (isLValue())
		{
			SgStatement* stmt = isSgStatement(get_parent());
			if (stmt)
				return stmt->isChildUsedAsLValue(this);
			SgExpression* expr = isSgExpression(get_parent());
			if (expr)
				return expr->isChildUsedAsLValue(this);
			return false;
		}
		else
			return false;
	}

	bool SgExpression::isChildUsedAsLValue(const SgExpression* child) const
	{
		return false;
	}

/*! std:5.1 par:2*/
bool SgStringVal::isLValue() const
{
	return true;
}

bool SgStringVal::isChildUsedAsLValue(const SgExpression* child) const
{
	ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgStringVal");
	return false;
}

/*! std:5.1 par:7*/
bool SgScopeOp::isLValue() const
{
	return get_rhs_operand()->isLValue();
}

bool SgScopeOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (child == get_lhs_operand())
	{
		ROSE_ASSERT(!"Only the right-hand-side is used as an lvalue for SgScopeOp");
		return false;
	}
	else if (child != get_rhs_operand())
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgScopeOp");
		return false;
	}
	else
	{
		if (isLValue())
		{
			SgStatement* stmt = isSgStatement(get_parent());
			if (stmt)
				return stmt->isChildUsedAsLValue(this);
			SgExpression* expr = isSgExpression(get_parent());
			if (expr)
				return expr->isChildUsedAsLValue(this);
			return false;
		}
		else
			return false;
	}
	return false;
}

/*! std:5.2.1 par:1*/
bool SgPntrArrRefExp::isLValue() const
{
	return true;
}

bool SgPntrArrRefExp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (child == get_lhs_operand())
	{
		if (!child->isLValue())
		{
			ROSE_ASSERT(!"Left-hand-side of a pointer-index expression must be an lvalue in isChildUsedAsLValue on SgPntrArrayRefExp");
			return true;
		}
		return true;
	}
	else if (child == get_rhs_operand())
	{
		return false;
	}
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgPntrArrayRefExp");
		return false;
	}
	return true;
}

/*! std:5.2.1 par:1*/ bool SgPointerDerefExp::isLValue() const
{
	return true;
}

bool SgPointerDerefExp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (child == get_operand())
	{
		return true;
	}
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgPointerDerefExp");
		return true;
	}
	return true;
}

/*! std:5.3.1 par:2 */
bool SgAddressOfOp::isLValue() const
{
	return true;
}

/*! std:5.3.1 par:2 */
bool SgAddressOfOp::isChildUsedAsLValue(const SgExpression* child) const
{
	/*! std:5.3.1 par:2 */
	if (child != this->get_operand())
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgAddressOfOp");
		return false;
	}
	return true;
}

/*! std:5.1 par:7,8 */
bool SgArrowExp::isLValue() const
{
	// TODO: king84
	// if rhs is a non-static member function, the result is not an lvalue (static member functions are lvalues)
	// see std:5.2.5 par:4
	// if rhs is an enum value, then the result is not an lvalue
	// see std:5.2.5 par:4
	return true;
}

bool SgArrowExp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (!isChild(const_cast<SgExpression*>(child)))
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgArrowExp");
		return false;
	}
	return true;
}

/*! std:5.1 par:7,8 */
bool SgDotExp::isLValue() const
{
	// TODO: king84
	// if rhs is a non-static member function, the result is not an lvalue (static member functions are lvalues)
	// see std:5.2.5 par:4
	// if rhs is an enum value, then the result is not an lvalue
	// see std:5.2.5 par:4
	return true;
}

bool SgDotExp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (!isChild(const_cast<SgExpression*>(child)))
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgDotExp");
		return false;
	}
	return true;
}

/*! std:5.4 par:6 */
bool SgDotStarOp::isLValue() const
{
	// TODO: king84
	// rhs must be a data member (and not a member function)
	// this is approximated here with an lvalue check
	// TODO: king84: is this true?
//	if (!get_rhs_operand()->isLValue())
//	{
//		ROSE_ASSERT(!"Right-hand-side must be a pointer to a data member in isLValue for SgDotStarOp");
//		return false;
//	}
	return true;
}

/*! std:5.4 par:6 */
bool SgDotStarOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (child == get_lhs_operand())
		return true;
	else if (child == get_rhs_operand())
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgDotStarExp");
		return false;
	}
	return true;
}

/*! std:5.4 par:6 */
bool SgArrowStarOp::isLValue() const
{
	// TODO: king84
	// rhs must be a data member (and not a member function)
	// this is approximated here with an lvalue check
	// TODO: king84: is this true?
//	if (!get_rhs_operand()->isLValue())
//	{
//		ROSE_ASSERT(!"Right-hand-side must be a pointer to a data member in isLValue for SgArrowStarOp");
//		return false;
//	}
	return true;
}

/*! std:5.4 par:6 */
bool SgArrowStarOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (child == get_lhs_operand())
		return true;
	else if (child == get_rhs_operand())
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgArrowStarExp");
		return false;
	}
	return true;
}


/*! std:5.2.8 par:1 */
bool SgTypeIdOp::isLValue() const
{
	return true;
}

/*! std:5.2.8 par:1 */
bool SgTypeIdOp::isChildUsedAsLValue(const SgExpression* child) const
{
	return false;
}

/*! std:5.2.6 par:2; std:5.3.2 par:2 */
bool SgMinusMinusOp::isLValue() const
{
	/*! std:5.2.6 par:1 */
	if (get_mode() == SgUnaryOp::postfix)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*! std:5.2.6 par:2; std:5.3.2 par:2 */
bool SgMinusMinusOp::isChildUsedAsLValue(const SgExpression* child) const
{
	/*! std:5.2.6 par:1 */
	if (get_mode() == SgUnaryOp::postfix)
	{
		if (child != this->get_operand())
		{
			ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgPlusPlusOp");
			return false;
		}
		else
		{
			return isUsedAsLValue();
		}
	}
	/*! std:5.3.2 par:2 */
	else
	{
		return isUsedAsLValue();
	}
}

/*! std:5.2.6 par:1; std:5.3.2 par:1 */
bool SgPlusPlusOp::isLValue() const
{
	/*! std:5.2.6 par:1 */
	if (get_mode() == SgUnaryOp::postfix)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*! std:5.2.6 par:1; std:5.3.2 par:1 */
bool SgPlusPlusOp::isChildUsedAsLValue(const SgExpression* child) const
{
	/*! std:5.2.6 par:1 */
	if (get_mode() == SgUnaryOp::postfix)
	{
		if (child != this->get_operand())
		{
			ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgPlusPlusOp");
			return false;
		}
		else
		{
			return isUsedAsLValue();
		}
	}
	/*! std:5.3.2 par:1 */
	else
	{
		return isUsedAsLValue();
	}
}

/*! std:5.2.2 par:1,10 */
bool SgFunctionCallExp::isLValue() const
{
	//! Function Pointers don't have a declaration!
//	if (getAssociatedFunctionDeclaration()->get_orig_return_type()->get_ref_to() != NULL)
	SgType* type = get_function()->get_type();
	while (SgTypedefType* type2 = isSgTypedefType(type))
		type = type2->get_base_type();
	SgFunctionType* ftype = isSgFunctionType(type);
	if (!ftype)
	{
		ROSE_ASSERT(!"Error calling a function through a non-function type in isLValue on SgFunctionCallExp");
		return true;
	}
	else
	{
		// depends on the return-type being a reference type
//		if (ftype->get_return_type()->get_ref_to() == NULL)
//		if (isSgReferenceType(ftype->get_return_type()) != NULL)
		if (SageInterface::isReferenceType(ftype->get_return_type()) != NULL)
			return true;
		else
			return false;
	}
}

bool SgFunctionCallExp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_function() == child)
	{
		// King84: I'm unsure if the function itself is an lvalue or an rvalue.  I'm just playing it safe here.
		return false;
	}
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgFunctionCallExp");
		return false;
	}
}

/*! std:5.4 par:1; std:5.2.11 par:1; std:5.2.9 par:1; std:5.2.7 par:2; std:5.2.10 par:1 */
bool SgCastExp::isLValue() const
{
//	printf("%d: %s : %s\n", get_file_info()->get_line(), unparseToString().c_str(), get_type()->unparseToString().c_str());
	switch (cast_type())
	{
		case e_C_style_cast:
//			if (get_type()->get_ref_to() != NULL) /*! std:5.4 par:1 */
//			if (isSgReferenceType(get_type()) != NULL) /*! std:5.4 par:1 */
			if (SageInterface::isReferenceType(get_type()) != NULL) /*! std:5.4 par:1 */
				return true;
			else
				return false;
		case e_const_cast:
			if (SageInterface::isReferenceType(get_type()) != NULL) /*! std:5.2.11 par:1 */
				return true;
			else
				return false;
		case e_static_cast:
			if (SageInterface::isReferenceType(get_type()) != NULL) /*! std:5.2.9 par:1 */
				return true;
			else
				return false;
		case e_dynamic_cast:
			if (SageInterface::isReferenceType(get_type()) != NULL) /*! std:5.2.7 par:2 */
				return true;
			else
				return false;
		case e_reinterpret_cast:
			if (SageInterface::isReferenceType(get_type()) != NULL) /*! std:5.2.10 par:1 */
				return true;
			else
				return false;
		case e_unknown:
		case e_default:
		default:
			return false;
	}
}

bool SgCastExp::isChildUsedAsLValue(const SgExpression* child) const
{
	// if it is an lvalue and we use it as an lvalue, then true
	if (child != this->get_operand())
	{
		ROSE_ASSERT (!"Bad child in isChildUsedAsLValue on SgCastExp");
		return false;
	}
	if (isUsedAsLValue())
	{
		if (child->isLValue())
			return true;
		else
		{
			ROSE_ASSERT (!"Child is not an LValue, so it cannot be used as an LValue in isChildUsedAsLValue on SgCastExp");
			return false;
		}
	}
	else
		return false;
}

//bool SgFunctionRefExp::isLValue() const
//{
//	return true;
//}
//
//bool SgFunctionRefExp::isChildUsedAsLValue(const SgExpression* child) const
//{
//	ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgFunctionRefExp");
//	return false;
//}

/*! std:5.1 par:7,8 */
bool SgMemberFunctionRefExp::isLValue() const
{
	return true;
}

bool SgMemberFunctionRefExp::isChildUsedAsLValue(const SgExpression* child) const
{
	ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgMemberFunctionRefExp");
	return false;
}

/*! std:2.5 par:5 */
bool SgVarRefExp::isDefinable() const
{
	// if not constant
	return !SageInterface::isConstType(get_type());
}

/*! std:5.1 par:7,8 */
bool SgVarRefExp::isLValue() const
{
	return true;
}

bool SgVarRefExp::isChildUsedAsLValue(const SgExpression* child) const
{
	ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgVarRefExp");
	return false;
}

/*! std:5.16 par:4 */
bool SgConditionalExp::isLValue() const
{
	if (get_true_exp()->isLValue() && get_false_exp()->isLValue())
	{
		// TODO: king84
		// need to make sure they are of the same type, possibly without cv-qualifiers
		if (get_true_exp()->get_type() == get_false_exp()->get_type())
			return true;
		else
			return false;
	}
	else
		return false;
}

/*! std:5.16 par:4 */
bool SgConditionalExp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_conditional_exp() == child)
		return false;
	else if (get_true_exp() == child || get_false_exp() == child)
	{
		if (isUsedAsLValue())
			return true;
		else
			return false;
	}
	else// if (!isChild(const_cast<SgExpression*>(child)))
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgConditionalExp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgAssignOp::isLValue() const
{
	return true;
}

bool SgAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgPlusAssignOp::isLValue() const
{
	return true;
}

bool SgPlusAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgPlusAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgMinusAssignOp::isLValue() const
{
	return true;
}

bool SgMinusAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgMinusAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgAndAssignOp::isLValue() const
{
	return true;
}

bool SgAndAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgAndAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgIorAssignOp::isLValue() const
{
	return true;
}

bool SgIorAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgIorAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgMultAssignOp::isLValue() const
{
	return true;
}

bool SgMultAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgMultAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgDivAssignOp::isLValue() const
{
	return true;
}

bool SgDivAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgDivAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgModAssignOp::isLValue() const
{
	return true;
}

bool SgModAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgModAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgXorAssignOp::isLValue() const
{
	return true;
}

bool SgXorAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgXorAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgLshiftAssignOp::isLValue() const
{
	return true;
}

bool SgLshiftAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgLshiftAssignOp");
		return false;
	}
}

/*! std:5.17 par:1 */
bool SgRshiftAssignOp::isLValue() const
{
	return true;
}

bool SgRshiftAssignOp::isChildUsedAsLValue(const SgExpression* child) const
{
	if (get_lhs_operand() == child)
		return true;
	else if (get_rhs_operand() == child)
		return false;
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgRshiftAssignOp");
		return false;
	}
}

bool SgPointerAssignOp::isDefinable() const
{
	return true;
}

/*! std:5.17 par:1 */
bool SgCommaOpExp::isLValue() const
{
	return get_rhs_operand()->isLValue();
}

bool SgCommaOpExp::isChildUsedAsLValue(const SgExpression* child) const
{

	if (get_lhs_operand() == child)
		return false;
	else if (get_rhs_operand() == child)
	{
		if (!isUsedAsLValue())
			return true;
		else
			return false;
	}
	else
	{
		ROSE_ASSERT(!"Bad child in isChildUsedAsLValue on SgCommaOpExp");
		return false;
	}
}

