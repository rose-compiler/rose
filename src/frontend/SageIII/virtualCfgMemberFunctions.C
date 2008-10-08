#include "rose.h"
#include <vector>
using namespace std;

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

std::vector<VirtualCFG::CFGEdge>
SgNode::cfgOutEdges(unsigned int) {
  ROSE_ASSERT (!"CFG functions only work on SgExpression, SgStatement, and SgInitializedName");
  return std::vector<VirtualCFG::CFGEdge>();
}

std::vector<VirtualCFG::CFGEdge>
SgNode::cfgInEdges(unsigned int) {
  ROSE_ASSERT (!"CFG functions only work on SgExpression, SgStatement, and SgInitializedName");
  return std::vector<VirtualCFG::CFGEdge>();
}

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

std::vector<VirtualCFG::CFGEdge>
SgStatement::cfgOutEdges(unsigned int) {
    std::cerr << "Bad statement case " << this->class_name() << " in cfgOutEdges()" << std::endl;
    ROSE_ASSERT (false);
    return std::vector<VirtualCFG::CFGEdge>();
  }

std::vector<VirtualCFG::CFGEdge>
SgStatement::cfgInEdges(unsigned int) {
    std::cerr << "Bad statement case " << this->class_name() << " in cfgInEdges()" << std::endl;
    ROSE_ASSERT (false);
    return std::vector<VirtualCFG::CFGEdge>();
  }

std::vector<VirtualCFG::CFGEdge> SgGlobal::cfgOutEdges(unsigned int idx) {
  return std::vector<VirtualCFG::CFGEdge>();
}

std::vector<VirtualCFG::CFGEdge> SgGlobal::cfgInEdges(unsigned int idx) {
  return std::vector<VirtualCFG::CFGEdge>();
}

unsigned int
SgBasicBlock::cfgIndexForEnd() const {
  return this->get_statements().size();
}

bool SgBasicBlock::cfgIsIndexInteresting(unsigned int idx) const {
  return false;
}

std::vector<VirtualCFG::CFGEdge> SgBasicBlock::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (idx == this->get_statements().size()) {
    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  } else if (idx < this->get_statements().size()) {
    makeEdge(VirtualCFG::CFGNode(this, idx), this->get_statements()[idx]->cfgForBeginning(), result);
  } else ROSE_ASSERT (!"Bad index for SgBasicBlock");
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgBasicBlock::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (idx == 0) {
    makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  } else if (idx <= this->get_statements().size()) {
    makeEdge(this->get_statements()[idx - 1]->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge>
SgIfStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_conditional()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_true_body()->cfgForBeginning(), result);
	    if (this->get_false_body()) {
	      makeEdge(VirtualCFG::CFGNode(this, idx), this->get_false_body()->cfgForBeginning(), result);
	    } else {
	      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 2), result);
	    }
	    break;
    case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgIfStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgIfStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_conditional()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 2: makeEdge(this->get_true_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	    if (this->get_false_body()) {
	      makeEdge(this->get_false_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	    } else {
	      makeEdge(VirtualCFG::CFGNode(this, 1), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge>
SgForInitStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (idx == this->get_init_stmt().size()) {
    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  } else if (idx < this->get_init_stmt().size()) {
    makeEdge(VirtualCFG::CFGNode(this, idx), this->get_init_stmt()[idx]->cfgForBeginning(), result);
  } else ROSE_ASSERT (!"Bad index for SgForInitStatement");
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgForInitStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (idx == 0) {
    makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  } else if (idx <= this->get_init_stmt().size()) {
    makeEdge(this->get_init_stmt()[idx - 1]->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgForStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_for_init_stmt()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_test()->cfgForBeginning(), result); break;
    case 2: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_loop_body()->cfgForBeginning(), result);
	    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 4), result); break;
 // case 3: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_increment_expr_root()->cfgForBeginning(), result); break;
    case 3: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_increment()->cfgForBeginning(), result); break;
    case 4: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgForStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgForStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_for_init_stmt()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
    // makeEdge(this->get_increment_expr_root()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
       makeEdge(this->get_increment()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	    break;
    case 2: makeEdge(this->get_test()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 3: {
      makeEdge(this->get_loop_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
      vector<SgContinueStmt*> continueStmts = SageInterface::findContinueStmts(this->get_loop_body(), "");
      for (unsigned int i = 0; i < continueStmts.size(); ++i) {
	makeEdge(VirtualCFG::CFGNode(continueStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
      }
      break;
    }
    case 4: {
      makeEdge(VirtualCFG::CFGNode(this, 2), VirtualCFG::CFGNode(this, idx), result);
      vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_loop_body(), "");
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
	makeEdge(VirtualCFG::CFGNode(breakStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
      }
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgForStatement");
  }
  return result;
}

unsigned int SgForAllStatement::cfgIndexForEnd() const
   {
     return VirtualCFG::doForallCfgIndexForEnd(this);
   }

bool SgForAllStatement::cfgIsIndexInteresting(unsigned int idx) const 
   {
     return VirtualCFG::doForallCfgIsIndexInteresting(this, idx);
   }

unsigned int SgForAllStatement::cfgFindChildIndex(SgNode* n)
   {
     return VirtualCFG::doForallCfgFindChildIndex(this, n);
   }

unsigned int SgForAllStatement::cfgFindNextChildIndex(SgNode* n)
   {
     return VirtualCFG::doForallCfgFindNextChildIndex(this, n);
   }

std::vector<VirtualCFG::CFGEdge> SgForAllStatement::cfgOutEdges(unsigned int idx)
   {
     return VirtualCFG::doForallCfgOutEdges(this, idx);
   }

std::vector<VirtualCFG::CFGEdge> SgForAllStatement::cfgInEdges(unsigned int idx)
   {
     return VirtualCFG::doForallCfgInEdges(this, idx);
   }

unsigned int SgUpcForAllStatement::cfgIndexForEnd() const
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return 0;
  // return VirtualCFG::doForallCfgIndexForEnd(this);
   }

bool SgUpcForAllStatement::cfgIsIndexInteresting(unsigned int idx) const 
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return false;
  // return VirtualCFG::doForallCfgIsIndexInteresting(this, idx);
   }

unsigned int SgUpcForAllStatement::cfgFindChildIndex(SgNode* n)
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return 0;
  // return VirtualCFG::doForallCfgFindChildIndex(this, n);
   }

unsigned int SgUpcForAllStatement::cfgFindNextChildIndex(SgNode* n)
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return 0;
  // return VirtualCFG::doForallCfgFindNextChildIndex(this, n);
   }

std::vector<VirtualCFG::CFGEdge> SgUpcForAllStatement::cfgOutEdges(unsigned int idx)
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return std::vector<VirtualCFG::CFGEdge>();
  // return VirtualCFG::doForallCfgOutEdges(this, idx);
   }

std::vector<VirtualCFG::CFGEdge> SgUpcForAllStatement::cfgInEdges(unsigned int idx)
   {
     printf ("Sorry not implemented, need CFG support for UPC forall IR node \n");
     ROSE_ASSERT(false);

     return std::vector<VirtualCFG::CFGEdge>();
  // return VirtualCFG::doForallCfgInEdges(this, idx);
   }

unsigned int
SgFunctionDeclaration::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgFunctionDeclaration::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgFunctionDeclaration::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgFunctionParameterList::cfgIndexForEnd() const
   {
     return this->get_args().size();
   }

std::vector<VirtualCFG::CFGEdge>
SgFunctionParameterList::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     if (idx == this->get_args().size())
        {
          makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
        }
       else
        {
          if (idx < this->get_args().size())
             {
               makeEdge(VirtualCFG::CFGNode(this, idx), this->get_args()[idx]->cfgForBeginning(), result);
             }
            else
             {
               ROSE_ASSERT (!"Bad index for SgFunctionParameterList");
             }
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgFunctionParameterList::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     VirtualCFG::addIncomingFortranGotos(this, idx, result);
     if (idx == 0)
        {
          makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
        }
       else
        {
          if (idx <= this->get_args().size())
             {
               makeEdge(this->get_args()[idx - 1]->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> 
SgFunctionDefinition::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_declaration()->get_parameterList()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2: /* No out edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgFunctionDefinition");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgFunctionDefinition::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: /* No in edges */ break;
    case 1: makeEdge(this->get_declaration()->get_parameterList()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 2: {
      makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
      std::vector<SgReturnStmt*> returnStmts = SageInterface::findReturnStmts(this);
      for (unsigned int i = 0; i < returnStmts.size(); ++i) {
	makeEdge(returnStmts[i]->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgMemberFunctionDeclaration::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  // makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgMemberFunctionDeclaration::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  // makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgVariableDeclaration::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (idx == this->get_variables().size()) {
    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  } else if (idx < this->get_variables().size()) {
    makeEdge(VirtualCFG::CFGNode(this, idx), this->get_variables()[idx]->cfgForBeginning(), result);
  } else ROSE_ASSERT (!"Bad index for SgVariableDeclaration");
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgVariableDeclaration::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (idx == 0) {
    makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  } else if (idx <= this->get_variables().size()) {
    makeEdge(this->get_variables()[idx - 1]->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
  } else ROSE_ASSERT (!"Bad index for SgVariableDeclaration");
  return result;
}

unsigned int
SgClassDeclaration::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgClassDeclaration::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgClassDeclaration::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgEnumDeclaration::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgEnumDeclaration::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgEnumDeclaration::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgExprStatement::cfgIndexForEnd() const {
  return 1;
}

std::vector<VirtualCFG::CFGEdge> SgExprStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
 // case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_expression_root()->cfgForBeginning(), result); break;
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_expression()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgExprStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgExprStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
 // case 1: makeEdge(this->get_expression_root()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_expression()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgExprStatement");
  }
  return result;
}

unsigned int
SgLabelStatement::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<VirtualCFG::CFGEdge> SgLabelStatement::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgLabelStatement");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge> SgLabelStatement::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     VirtualCFG::addIncomingFortranGotos(this, idx, result);
     switch (idx)
        {
          case 0:
             {
               makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
            // Find all gotos to this label, functionwide
               SgFunctionDefinition* thisFunction = SageInterface::getEnclosingProcedure(this);
               std::vector<SgGotoStatement*> gotos = SageInterface::findGotoStmts(thisFunction, this);
               for (unsigned int i = 0; i < gotos.size(); ++i)
                  {
                    makeEdge(VirtualCFG::CFGNode(gotos[i], 0), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgWhileStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result);
	    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 2), result); break;
    case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgWhileStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgWhileStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
      std::vector<SgContinueStmt*> continueStmts = SageInterface::findContinueStmts(this->get_body(), this->get_string_label());
      for (unsigned int i = 0; i < continueStmts.size(); ++i) {
	makeEdge(VirtualCFG::CFGNode(continueStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
      }
      break;
    }
    case 1: makeEdge(this->get_condition()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 2: {
      makeEdge(VirtualCFG::CFGNode(this, 1), VirtualCFG::CFGNode(this, idx), result);
      std::vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_body(), this->get_string_label());
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
	makeEdge(VirtualCFG::CFGNode(breakStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgDoWhileStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 0), result);
	    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 3), result); break;
    case 3: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDoWhileStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgDoWhileStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
	    makeEdge(VirtualCFG::CFGNode(this, 2), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: {
      makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
      vector<SgContinueStmt*> continueStmts = SageInterface::findContinueStmts(this->get_body(), "");
      for (unsigned int i = 0; i < continueStmts.size(); ++i) {
	makeEdge(VirtualCFG::CFGNode(continueStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
      }
      break;
    }
    case 2: {
      makeEdge(this->get_condition()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case 3: {
      makeEdge(VirtualCFG::CFGNode(this, 2), VirtualCFG::CFGNode(this, idx), result);
      vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_body(), "");
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
	makeEdge(VirtualCFG::CFGNode(breakStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgSwitchStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_item_selector()->cfgForBeginning(), result); break;
    case 1: {
      vector<SgStatement*> cases = SageInterface::getSwitchCases(this); // Also includes default statements
      bool hasDefault = false;
      for (unsigned int i = 0; i < cases.size(); ++i) {
	makeEdge(VirtualCFG::CFGNode(this, idx), cases[i]->cfgForBeginning(), result);
	if (isSgDefaultOptionStmt(cases[i])) {
          hasDefault = true;
        }
      }
      if (!hasDefault) {
        makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 2), result);
      }
      break;
    }
    case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgSwitchStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgSwitchStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_item_selector()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 2: {
      makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
      vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_body(), "");
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
	makeEdge(VirtualCFG::CFGNode(breakStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
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
        makeEdge(VirtualCFG::CFGNode(this, 1), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgCaseOptionStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgCaseOptionStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgCaseOptionStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      SgSwitchStatement* mySwitch = SageInterface::findEnclosingSwitch(this);
      makeEdge(VirtualCFG::CFGNode(mySwitch, 1), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case 1: makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge> SgTryStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgTryStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgTryStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge> SgCatchStatementSeq::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  // FIXME
  if (idx == this->get_catch_statement_seq().size()) {
    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  } else if (idx < this->get_catch_statement_seq().size()) {
    makeEdge(VirtualCFG::CFGNode(this, idx), this->get_catch_statement_seq()[idx]->cfgForBeginning(), result);
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgCatchStatementSeq::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  // FIXME
  if (idx == 0) {
    makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  } else if (idx <= this->get_catch_statement_seq().size()) {
    makeEdge(this->get_catch_statement_seq()[idx - 1]->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
  } else ROSE_ASSERT (!"Bad index for SgCatchStatementSeq");
  return result;
}

unsigned int
SgCatchOptionStmt::cfgIndexForEnd() const {
  return 2;
}

std::vector<VirtualCFG::CFGEdge> SgCatchOptionStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgCatchOptionStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgCatchOptionStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_condition()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 2: makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge> SgDefaultOptionStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDefaultOptionStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgDefaultOptionStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      SgSwitchStatement* mySwitch = SageInterface::findEnclosingSwitch(this);
      makeEdge(VirtualCFG::CFGNode(mySwitch, 1), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case 1: makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge> SgBreakStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
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
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(enc, newIndex), result);
      break;
    }
    case 1: /* No out edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgBreakStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgBreakStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge> SgContinueStmt::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
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
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(loop, newIndex), result);
      break;
    }
    case 1: /* No out edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgContinueStmt");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgContinueStmt::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge>
SgReturnStmt::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
  // bool hasReturnValue = this->get_expression_root();
     bool hasReturnValue = this->get_expression();
     bool exitingFunctionNow = idx == 1 || (idx == 0 && !hasReturnValue);
     if (exitingFunctionNow)
        {
          SgFunctionDefinition* enclosingFunc = SageInterface::getEnclosingProcedure(this);
          makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(enclosingFunc, 2), result);
        }
       else
        {
          makeEdge(VirtualCFG::CFGNode(this, idx), this->get_expression()->cfgForBeginning(), result);
        }
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgReturnStmt::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     VirtualCFG::addIncomingFortranGotos(this, idx, result);
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
          case 1: ROSE_ASSERT (this->get_expression());
	            makeEdge(this->get_expression()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge> SgGotoStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: {
      if (this->get_label_expression()) { // A Fortran goto
        makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getCFGTargetOfFortranLabelRef(this->get_label_expression()), result);
      } else { // A C/C++ goto
	makeEdge(VirtualCFG::CFGNode(this, idx), this->get_label()->cfgForBeginning(), result);
      }
      break;
    }
    case 1: /* No out edges */ break;
    default: ROSE_ASSERT (!"Bad index for SgGotoStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgGotoStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge> SgAsmStmt::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgAsmStmt::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgNullStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgNullStatement::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgNullStatement::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgTypedefDeclaration::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<VirtualCFG::CFGEdge>
SgTypedefDeclaration::cfgOutEdges(unsigned int idx)
   {
     ROSE_ASSERT (idx == 0);
     std::vector<VirtualCFG::CFGEdge> result;
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgTypedefDeclaration::cfgInEdges(unsigned int idx)
   {
     ROSE_ASSERT (idx == 0);
     std::vector<VirtualCFG::CFGEdge> result;
     VirtualCFG::addIncomingFortranGotos(this, idx, result);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgPragmaDeclaration::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgPragmaDeclaration::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgPragmaDeclaration::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgUsingDirectiveStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgUsingDirectiveStatement::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgUsingDirectiveStatement::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgUsingDeclarationStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgUsingDeclarationStatement::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgUsingDeclarationStatement::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgCommonBlock::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge>
SgCommonBlock::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgCommonBlock::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgModuleStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge>
SgModuleStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgModuleStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgContainsStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgContainsStatement::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgContainsStatement::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgUseStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgUseStatement::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgUseStatement::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgStopOrPauseStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgStopOrPauseStatement::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  if (this->get_stop_or_pause() == SgStopOrPauseStatement::e_pause) {
    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgStopOrPauseStatement::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgPrintStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 1;
}

std::vector<VirtualCFG::CFGEdge>
SgPrintStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 1, result)) return result;
  switch (idx) {
    case 0: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case VirtualCFG::numberOfFortranIOCommonEdges + 1: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgPrintStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgPrintStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 1, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case 1: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgPrintStatement");
  }
  return result;
}

unsigned int
SgReadStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 8;
}

std::vector<VirtualCFG::CFGEdge>
SgReadStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 8, result)) return result;
  switch (idx) {
    case 0: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case 1: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_rec(), result); break;
    case 2: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_end(), result); break;
    case 3: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_namelist(), result); break;
    case 4: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_advance(), result); break;
    case 5: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_size(), result); break;
    case 6: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_eor(), result); break;
    case 7: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case VirtualCFG::numberOfFortranIOCommonEdges + 8: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgReadStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgReadStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 8, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case 1: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case 2: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_rec(), result); break;
    case 3: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_end(), result); break;
    case 4: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_namelist(), result); break;
    case 5: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_advance(), result); break;
    case 6: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_size(), result); break;
    case 7: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_eor(), result); break;
    case 8: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgReadStatement");
  }
  return result;
}

unsigned int
SgWriteStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 5;
}

std::vector<VirtualCFG::CFGEdge>
SgWriteStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 5, result)) return result;
  switch (idx) {
    case 0: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case 1: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_rec(), result); break;
    case 2: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_namelist(), result); break;
    case 3: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_advance(), result); break;
    case 4: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case VirtualCFG::numberOfFortranIOCommonEdges + 5: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgWriteStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgWriteStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 5, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case 1: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_format(), result); break;
    case 2: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_rec(), result); break;
    case 3: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_namelist(), result); break;
    case 4: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_advance(), result); break;
    case 5: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgWriteStatement");
  }
  return result;
}

unsigned int
SgOpenStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 13;
}

std::vector<VirtualCFG::CFGEdge>
SgOpenStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 13, result)) return result;
  switch (idx) {
    case 0: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_file(), result); break;
    case 1: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_status(), result); break;
    case 2: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_access(), result); break;
    case 3: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_form(), result); break;
    case 4: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_recl(), result); break;
    case 5: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_blank(), result); break;
    case 6: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_position(), result); break;
    case 7: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_action(), result); break;
    case 8: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_delim(), result); break;
    case 9: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_pad(), result); break;
    case 10: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_round(), result); break;
    case 11: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_sign(), result); break;
    case 12: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case VirtualCFG::numberOfFortranIOCommonEdges + 13: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgOpenStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgOpenStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 13, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case 1: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_file(), result); break;
    case 2: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_status(), result); break;
    case 3: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_access(), result); break;
    case 4: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_form(), result); break;
    case 5: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_recl(), result); break;
    case 6: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_blank(), result); break;
    case 7: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_position(), result); break;
    case 8: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_action(), result); break;
    case 9: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_delim(), result); break;
    case 10: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_pad(), result); break;
    case 11: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_round(), result); break;
    case 12: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_sign(), result); break;
    case 13: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgOpenStatement");
  }
  return result;
}

unsigned int
SgCloseStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 1;
}

std::vector<VirtualCFG::CFGEdge>
SgCloseStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 1, result)) return result;
  switch (idx) {
    case 0: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_status(), result); break;
    case VirtualCFG::numberOfFortranIOCommonEdges + 1: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgCloseStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgCloseStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 1, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case 1: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_status(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgCloseStatement");
  }
  return result;
}

unsigned int
SgInquireStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 27;
}

std::vector<VirtualCFG::CFGEdge>
SgInquireStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 27, result)) return result;
  switch (idx) {
    case  0: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_file(), result); break;
    case  1: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_access(), result); break;
    case  2: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_form(), result); break;
    case  3: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_recl(), result); break;
    case  4: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_blank(), result); break;
    case  5: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_exist(), result); break;
    case  6: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_opened(), result); break;
    case  7: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_number(), result); break;
    case  8: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_named(), result); break;
    case  9: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_name(), result); break;
    case 10: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_sequential(), result); break;
    case 11: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_direct(), result); break;
    case 12: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_formatted(), result); break;
    case 13: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_unformatted(), result); break;
    case 14: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_nextrec(), result); break;
    case 15: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_position(), result); break;
    case 16: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_action(), result); break;
    case 17: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_read(), result); break;
    case 18: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_write(), result); break;
    case 19: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_readwrite(), result); break;
    case 20: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_delim(), result); break;
    case 21: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_pad(), result); break;
    case 22: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case 23: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_decimal(), result); break;
    case 24: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_stream(), result); break;
    case 25: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_size(), result); break;
    case 26: VirtualCFG::addOutEdgeOrBypassForExpressionChild(this, idx, this->get_pending(), result); break;
    case VirtualCFG::numberOfFortranIOCommonEdges + 27: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgInquireStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgInquireStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 27, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    case  1: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_file(), result); break;
    case  2: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_access(), result); break;
    case  3: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_form(), result); break;
    case  4: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_recl(), result); break;
    case  5: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_blank(), result); break;
    case  6: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_exist(), result); break;
    case  7: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_opened(), result); break;
    case  8: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_number(), result); break;
    case  9: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_named(), result); break;
    case 10: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_name(), result); break;
    case 11: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_sequential(), result); break;
    case 12: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_direct(), result); break;
    case 13: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_formatted(), result); break;
    case 14: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_unformatted(), result); break;
    case 15: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_nextrec(), result); break;
    case 16: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_position(), result); break;
    case 17: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_action(), result); break;
    case 18: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_read(), result); break;
    case 19: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_write(), result); break;
    case 20: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_readwrite(), result); break;
    case 21: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_delim(), result); break;
    case 22: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_pad(), result); break;
    case 23: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_asynchronous(), result); break;
    case 24: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_decimal(), result); break;
    case 25: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_stream(), result); break;
    case 26: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_size(), result); break;
    case 27: VirtualCFG::addInEdgeOrBypassForExpressionChild(this, idx, this->get_pending(), result); break;
    default: ROSE_ASSERT (!"Invalid index for SgInquireStatement");
  }
  return result;
}

unsigned int
SgFlushStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 0;
}

std::vector<VirtualCFG::CFGEdge>
SgFlushStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case VirtualCFG::numberOfFortranIOCommonEdges + 0: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgFlushStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgFlushStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgFlushStatement");
  }
  return result;
}

unsigned int
SgRewindStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 0;
}

std::vector<VirtualCFG::CFGEdge>
SgRewindStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case VirtualCFG::numberOfFortranIOCommonEdges + 0: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgRewindStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgRewindStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgRewindStatement");
  }
  return result;
}

unsigned int
SgBackspaceStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 0;
}

std::vector<VirtualCFG::CFGEdge>
SgBackspaceStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case VirtualCFG::numberOfFortranIOCommonEdges + 0: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgBackspaceStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgBackspaceStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgBackspaceStatement");
  }
  return result;
}

unsigned int
SgEndfileStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 0;
}

std::vector<VirtualCFG::CFGEdge>
SgEndfileStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case VirtualCFG::numberOfFortranIOCommonEdges + 0: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgEndfileStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgEndfileStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgEndfileStatement");
  }
  return result;
}

unsigned int
SgWaitStatement::cfgIndexForEnd() const {
  return VirtualCFG::numberOfFortranIOCommonEdges + 0;
}

std::vector<VirtualCFG::CFGEdge>
SgWaitStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  if (VirtualCFG::handleFortranIOCommonOutEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case VirtualCFG::numberOfFortranIOCommonEdges + 0: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Invalid index for SgWaitStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgWaitStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  if (VirtualCFG::handleFortranIOCommonInEdges(this, idx, 0, result)) return result;
  switch (idx) {
    case 0: {
      makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgFortranDo::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_initialization()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_increment()->cfgForBeginning(), result); break;
    case 2: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_bound()->cfgForBeginning(), result); break;
    case 3: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 4), result);
            makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 6), result); break;
    case 4: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 5: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 3), result); break;
    case 6: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgFortranDo");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgFortranDo::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_initialization()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 2: makeEdge(this->get_increment()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 3: makeEdge(this->get_bound()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
            makeEdge(VirtualCFG::CFGNode(this, 5), VirtualCFG::CFGNode(this, idx), result); break;
    case 4: makeEdge(VirtualCFG::CFGNode(this, 3), VirtualCFG::CFGNode(this, idx), result); break;
    case 5: {
      makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
      vector<SgContinueStmt*> continueStmts = SageInterface::findContinueStmts(this->get_body(), this->get_string_label());
      for (unsigned int i = 0; i < continueStmts.size(); ++i) {
        makeEdge(VirtualCFG::CFGNode(continueStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
      }
      break;
    }
    case 6: {
      makeEdge(VirtualCFG::CFGNode(this, 3), VirtualCFG::CFGNode(this, idx), result);
      vector<SgBreakStmt*> breakStmts = SageInterface::findBreakStmts(this->get_body(), this->get_string_label());
      for (unsigned int i = 0; i < breakStmts.size(); ++i) {
        makeEdge(VirtualCFG::CFGNode(breakStmts[i], 0), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge> SgInterfaceStatement::cfgOutEdges(unsigned int idx)
   {
     ROSE_ASSERT (idx == 0);
     std::vector<VirtualCFG::CFGEdge> result;
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge> SgInterfaceStatement::cfgInEdges(unsigned int idx)
   {
     ROSE_ASSERT (idx == 0);
     std::vector<VirtualCFG::CFGEdge> result;
     VirtualCFG::addIncomingFortranGotos(this, idx, result);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgImplicitStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge>
SgImplicitStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgImplicitStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgWhereStatement::cfgIndexForEnd() const {
  return 3;
}

std::vector<VirtualCFG::CFGEdge> SgWhereStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0:
    if (this->get_condition()) {
      makeEdge(VirtualCFG::CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, idx + 1), result); break;
    }
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2:
    if (this->get_elsewhere()) {
      makeEdge(VirtualCFG::CFGNode(this, idx), this->get_elsewhere()->cfgForBeginning(), result); break;
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, idx + 1), result); break;
    }
    case 3: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgWhereStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgWhereStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1:
    if (this->get_condition()) {
      makeEdge(this->get_condition()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx - 1), VirtualCFG::CFGNode(this, idx), result); break;
    }
    case 2: makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 3:
    if (this->get_elsewhere()) {
      makeEdge(this->get_elsewhere()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx - 1), VirtualCFG::CFGNode(this, idx), result); break;
    }
    default: ROSE_ASSERT (!"Bad index for SgWhereStatement");
  }
  return result;
}

unsigned int
SgElseWhereStatement::cfgIndexForEnd() const {
  return 3;
}

std::vector<VirtualCFG::CFGEdge> SgElseWhereStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0:
    if (this->get_condition()) {
      makeEdge(VirtualCFG::CFGNode(this, idx), this->get_condition()->cfgForBeginning(), result); break;
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, idx + 1), result); break;
    }
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2:
    if (this->get_elsewhere()) {
      makeEdge(VirtualCFG::CFGNode(this, idx), this->get_elsewhere()->cfgForBeginning(), result); break;
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, idx + 1), result); break;
    }
    case 3: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgElseWhereStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgElseWhereStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1:
    if (this->get_condition()) {
      makeEdge(this->get_condition()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx - 1), VirtualCFG::CFGNode(this, idx), result); break;
    }
    case 2: makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 3:
    if (this->get_elsewhere()) {
      makeEdge(this->get_elsewhere()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx - 1), VirtualCFG::CFGNode(this, idx), result); break;
    }
    default: ROSE_ASSERT (!"Bad index for SgElseWhereStatement");
  }
  return result;
}

unsigned int
SgEquivalenceStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge>
SgEquivalenceStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgEquivalenceStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgAttributeSpecificationStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge>
SgAttributeSpecificationStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgAttributeSpecificationStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgArithmeticIfStatement::cfgIndexForEnd() const
   {
     return 1;
   }

std::vector<VirtualCFG::CFGEdge>
SgArithmeticIfStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_conditional()->cfgForBeginning(), result); break;
    case 1: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getCFGTargetOfFortranLabelRef(this->get_less_label()), result);
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getCFGTargetOfFortranLabelRef(this->get_equal_label()), result);
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getCFGTargetOfFortranLabelRef(this->get_greater_label()), result);
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgArithmeticIfStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgArithmeticIfStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_conditional()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
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

std::vector<VirtualCFG::CFGEdge> SgComputedGotoStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_label_index()->cfgForBeginning(), result); break;
    case 1: {
      SgExprListExp* labels = this->get_labelList();
      const vector<SgExpression*>& cases = labels->get_expressions();
      for (unsigned int i = 0; i < cases.size(); ++i) {
        SgLabelRefExp* lRef = isSgLabelRefExp(cases[i]);
        makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getCFGTargetOfFortranLabelRef(lRef), result);
      }
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 2), result); // For default
      break;
    }
    case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgComputedGotoStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgComputedGotoStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_label_index()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 2: {
      makeEdge(VirtualCFG::CFGNode(this, 1), VirtualCFG::CFGNode(this, idx), result); // For default
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

std::vector<VirtualCFG::CFGEdge>
SgNamelistStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgNamelistStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgImportStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgImportStatement::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgImportStatement::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int
SgAssociateStatement::cfgIndexForEnd() const {
  return 2;
}

std::vector<VirtualCFG::CFGEdge> SgAssociateStatement::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_variable_declaration()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_body()->cfgForBeginning(), result); break;
    case 2: {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
      break;
    }
    default: ROSE_ASSERT (!"Bad index for SgAssociateStatement");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgAssociateStatement::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_variable_declaration()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    case 2: makeEdge(this->get_body()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgAssociateStatement");
  }
  return result;
}

unsigned int
SgFormatStatement::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgFormatStatement::cfgOutEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgFormatStatement::cfgInEdges(unsigned int idx) {
  ROSE_ASSERT (idx == 0);
  std::vector<VirtualCFG::CFGEdge> result;
  VirtualCFG::addIncomingFortranGotos(this, idx, result);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int 
SgExpression::cfgIndexForEnd() const 
   {
     std::cerr << "Bad expression case " << this->class_name() << " in cfgIndexForEnd()" << std::endl;
     ROSE_ASSERT (false);
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

  std::vector<VirtualCFG::CFGEdge> SgExpression::cfgOutEdges(unsigned int) {
    std::cerr << "Bad expression case " << this->class_name() << " in cfgOutEdges()" << std::endl;
    ROSE_ASSERT (false);
  }

  std::vector<VirtualCFG::CFGEdge> SgExpression::cfgInEdges(unsigned int) {
    std::cerr << "Bad expression case " << this->class_name() << " in cfgInEdges()" << std::endl;
    ROSE_ASSERT (false);
  }

unsigned int
SgUnaryOp::cfgIndexForEnd() const 
   {
     return 1;
   }

std::vector<VirtualCFG::CFGEdge>
SgUnaryOp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_operand()->cfgForBeginning(), result); break;
          case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgUnaryOp");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgUnaryOp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgUnaryOp");
        }

     return result;
   }

unsigned int SgBinaryOp::cfgIndexForEnd() const
   {
     return 2;
   }

std::vector<VirtualCFG::CFGEdge>
SgBinaryOp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_lhs_operand()->cfgForBeginning(), result); break;
          case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_rhs_operand()->cfgForBeginning(), result); break;
          case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgBinaryOp");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgBinaryOp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_lhs_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          case 2: makeEdge(this->get_rhs_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgBinaryOp");
        }

     return result;
   }

unsigned int
SgExprListExp::cfgIndexForEnd() const
   {
     return this->get_expressions().size();
   }

std::vector<VirtualCFG::CFGEdge>
SgExprListExp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     if (idx == this->get_expressions().size())
        {
          makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
        }
       else
          if (idx < this->get_expressions().size())
             {
               makeEdge(VirtualCFG::CFGNode(this, idx), this->get_expressions()[idx]->cfgForBeginning(), result);
             }
            else
               ROSE_ASSERT (!"Bad index for SgExprListExp");

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgExprListExp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     if (idx == 0)
        {
          makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
        }
       else
          if (idx <= this->get_expressions().size())
             {
               makeEdge(this->get_expressions()[idx - 1]->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge>
SgVarRefExp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgVarRefExp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgLabelRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<VirtualCFG::CFGEdge>
SgLabelRefExp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgLabelRefExp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge>
SgLabelRefExp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgLabelRefExp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }
#endif

unsigned int
SgFunctionRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<VirtualCFG::CFGEdge>
SgFunctionRefExp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgFunctionRefExp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }

unsigned int
SgMemberFunctionRefExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<VirtualCFG::CFGEdge>
SgMemberFunctionRefExp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgMemberFunctionRefExp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }

  unsigned int SgValueExp::cfgIndexForEnd() const {
    return 0;
  }

  std::vector<VirtualCFG::CFGEdge> SgValueExp::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    ROSE_ASSERT (idx == 0);
    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgValueExp::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    ROSE_ASSERT (idx == 0);
    makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
    return result;
  }

  unsigned int SgFunctionCallExp::cfgIndexForEnd() const {
    return 3;
  }

  std::vector<VirtualCFG::CFGEdge> SgFunctionCallExp::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_function()->cfgForBeginning(), result); break;
      case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_args()->cfgForBeginning(), result); break;
      case 2: {
	SgFunctionDeclaration* decl =
	  interproceduralControlFlowGraph ?
	  VirtualCFG::getDeclaration(this->get_function()) : NULL;
	if (decl)
	  makeEdge(VirtualCFG::CFGNode(this, idx), decl->cfgForBeginning(),
		   result);
	else
	  makeEdge(VirtualCFG::CFGNode(this, idx),
		   VirtualCFG::CFGNode(this, 3), result);
	break;
      }
      case 3: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgFunctionCallExp");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgFunctionCallExp::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_function()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      case 2: makeEdge(this->get_args()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      case 3: {
	SgFunctionDeclaration* decl =
	  interproceduralControlFlowGraph ?
	  VirtualCFG::getDeclaration(this->get_function()) : NULL;
	if (decl)
	  makeEdge(decl->cfgForEnd(), VirtualCFG::CFGNode(this, idx),
		   result);
	else
	  makeEdge(VirtualCFG::CFGNode(this, 2),
		   VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge>
SgAndOp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_lhs_operand()->cfgForBeginning(), result); break;
       case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_rhs_operand()->cfgForBeginning(), result);
	       makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 2), result); break;
       case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
       default: ROSE_ASSERT (!"Bad index in SgAndOp");
     }
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgAndOp::cfgInEdges(unsigned int idx) 
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
       case 1: makeEdge(this->get_lhs_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
       case 2: makeEdge(this->get_rhs_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	       makeEdge(VirtualCFG::CFGNode(this, 1), VirtualCFG::CFGNode(this, idx), result); break;
       default: ROSE_ASSERT (!"Bad index in SgAndOp");
     }
     return result;
   }

bool
SgOrOp::cfgIsIndexInteresting(unsigned int idx) const 
   {
     return idx == 1 || idx == 2;
   }

std::vector<VirtualCFG::CFGEdge>
SgOrOp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_lhs_operand()->cfgForBeginning(), result); break;
       case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_rhs_operand()->cfgForBeginning(), result);
	       makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 2), result); break;
       case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
       default: ROSE_ASSERT (!"Bad index in SgOrOp");
     }
     return result;
   }

   std::vector<VirtualCFG::CFGEdge> SgOrOp::cfgInEdges(unsigned int idx) {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
       case 1: makeEdge(this->get_lhs_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
       case 2: makeEdge(this->get_rhs_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	       makeEdge(VirtualCFG::CFGNode(this, 1), VirtualCFG::CFGNode(this, idx), result); break;
       default: ROSE_ASSERT (!"Bad index in SgOrOp");
     }
     return result;
   }

unsigned int
SgTypeIdOp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<VirtualCFG::CFGEdge>
SgTypeIdOp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgTypeIdOp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }

  unsigned int SgVarArgStartOp::cfgIndexForEnd() const {
    return 2;
  }

  std::vector<VirtualCFG::CFGEdge> SgVarArgStartOp::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_lhs_operand()->cfgForBeginning(), result); break;
      case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_rhs_operand()->cfgForBeginning(), result); break;
      case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgStartOp");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgVarArgStartOp::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_lhs_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      case 2: makeEdge(this->get_rhs_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgStartOp");
    }
    return result;
  }

  unsigned int SgVarArgOp::cfgIndexForEnd() const {
    return 1;
  }

  std::vector<VirtualCFG::CFGEdge> SgVarArgOp::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_operand_expr()->cfgForBeginning(), result); break;
      case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgOp");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgVarArgOp::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_operand_expr()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgOp");
    }
    return result;
  }

  unsigned int SgVarArgEndOp::cfgIndexForEnd() const {
    return 1;
  }

  std::vector<VirtualCFG::CFGEdge> SgVarArgEndOp::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_operand_expr()->cfgForBeginning(), result); break;
      case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgVarArgEndOp");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgVarArgEndOp::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_operand_expr()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
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

  std::vector<VirtualCFG::CFGEdge> SgConditionalExp::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_conditional_exp()->cfgForBeginning(), result); break;
      case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_true_exp()->cfgForBeginning(), result);
	      makeEdge(VirtualCFG::CFGNode(this, idx), this->get_false_exp()->cfgForBeginning(), result); break;
      case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index in SgConditionalExp");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgConditionalExp::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_conditional_exp()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      case 2: makeEdge(this->get_true_exp()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	      makeEdge(this->get_false_exp()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index in SgConditionalExp");
    }
    return result;
  }

unsigned int
SgCastExp::cfgFindChildIndex(SgNode* n)
   {
     if (n == this->get_operand()) return 0;
     ROSE_ASSERT (!"Bad child in cfgFindChildIndex on SgCastExp");
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

  std::vector<VirtualCFG::CFGEdge> SgNewExp::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: {
	if (this->get_placement_args()) {
	  makeEdge(VirtualCFG::CFGNode(this, idx), this->get_placement_args()->cfgForBeginning(), result);
	} else {
	  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 1), result);
	}
	break;
      }
      case 1: {
	if (this->get_constructor_args()) {
	  makeEdge(VirtualCFG::CFGNode(this, idx), this->get_constructor_args()->cfgForBeginning(), result);
	} else if (this->get_builtin_args()) {
	  makeEdge(VirtualCFG::CFGNode(this, idx), this->get_builtin_args()->cfgForBeginning(), result);
	} else {
	  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::CFGNode(this, 2), result);
	}
	break;
      }
      case 2: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgNewExp");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgNewExp::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
      case 1: {
	if (this->get_placement_args()) {
	  makeEdge(this->get_placement_args()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	} else {
	  makeEdge(VirtualCFG::CFGNode(this, 0), VirtualCFG::CFGNode(this, idx), result);
	}
	break;
      }
      case 2: {
	if (this->get_constructor_args()) {
	  makeEdge(this->get_constructor_args()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	} else if (this->get_builtin_args()) {
	  makeEdge(this->get_builtin_args()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result);
	} else {
	  makeEdge(VirtualCFG::CFGNode(this, 1), VirtualCFG::CFGNode(this, idx), result);
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

std::vector<VirtualCFG::CFGEdge>
SgDeleteExp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_variable()->cfgForBeginning(), result); break;
          case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgDeleteExp");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgDeleteExp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_variable()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgDeleteExp");
        }

     return result;
   }

unsigned int
SgThisExp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<VirtualCFG::CFGEdge>
SgThisExp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
  }

std::vector<VirtualCFG::CFGEdge>
SgThisExp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }

unsigned int SgInitializer::cfgIndexForEnd() const
   {
     return 1;
   }

std::vector<VirtualCFG::CFGEdge>
SgAggregateInitializer::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_initializers()->cfgForBeginning(), result); break;
       case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
       default: ROSE_ASSERT (!"Bad index for SgAggregateInitializer");
     }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgAggregateInitializer::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx) {
       case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
       case 1: makeEdge(this->get_initializers()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
       default: ROSE_ASSERT (!"Bad index for SgAggregateInitializer");
     }

     return result;
   }

std::vector<VirtualCFG::CFGEdge> SgConstructorInitializer::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_args()->cfgForBeginning(), result); break;
      case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgConstructorInitializer");
    }
    return result;
  }

std::vector<VirtualCFG::CFGEdge> SgConstructorInitializer::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_args()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgConstructorInitializer");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgAssignInitializer::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_operand()->cfgForBeginning(), result); break;
      case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
      default: ROSE_ASSERT (!"Bad index for SgAssignInitializer");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgAssignInitializer::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
      case 1: makeEdge(this->get_operand()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
      default: ROSE_ASSERT (!"Bad index for SgAssignInitializer");
    }
    return result;
  }

  unsigned int SgNullExpression::cfgIndexForEnd() const {
    return 0;
  }

  std::vector<VirtualCFG::CFGEdge> SgNullExpression::cfgOutEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    ROSE_ASSERT (idx == 0);
    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> SgNullExpression::cfgInEdges(unsigned int idx) {
    std::vector<VirtualCFG::CFGEdge> result;
    ROSE_ASSERT (idx == 0);
    makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
    return result;
  }

unsigned int
SgStatementExpression::cfgIndexForEnd() const 
   {
     return 1;
   }

std::vector<VirtualCFG::CFGEdge>
SgStatementExpression::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_statement()->cfgForBeginning(), result); break;
          case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgStatementExpression");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgStatementExpression::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_statement()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgStatementExpression");
        }

     return result;
   }

unsigned int
SgAsmOp::cfgIndexForEnd() const
   {
     return 0;
   }

std::vector<VirtualCFG::CFGEdge>
SgAsmOp::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgAsmOp::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     ROSE_ASSERT (idx == 0);
     makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
     return result;
   }

unsigned int SgSubscriptExpression::cfgIndexForEnd() const
   {
     return 3;
   }

std::vector<VirtualCFG::CFGEdge>
SgSubscriptExpression::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_lowerBound()->cfgForBeginning(), result); break;
          case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_upperBound()->cfgForBeginning(), result); break;
          case 2: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_stride()->cfgForBeginning(), result); break;
          case 3: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgSubscriptExpression");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgSubscriptExpression::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_lowerBound()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          case 2: makeEdge(this->get_upperBound()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          case 3: makeEdge(this->get_stride()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgSubscriptExpression");
        }

     return result;
   }

unsigned int SgAsteriskShapeExp::cfgIndexForEnd() const {
  return 0;
}

std::vector<VirtualCFG::CFGEdge> SgAsteriskShapeExp::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  ROSE_ASSERT (idx == 0);
  makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgAsteriskShapeExp::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  ROSE_ASSERT (idx == 0);
  makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
  return result;
}

unsigned int SgImpliedDo::cfgIndexForEnd() const
   {
     return 4;
   }

std::vector<VirtualCFG::CFGEdge>
SgImpliedDo::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_do_var()->cfgForBeginning(), result); break;
          case 1: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_first_val()->cfgForBeginning(), result); break;
          case 2: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_last_val()->cfgForBeginning(), result); break;
          case 3: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_increment()->cfgForBeginning(), result); break;
          case 4: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgImpliedDo");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgImpliedDo::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_do_var()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          case 2: makeEdge(this->get_first_val()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          case 3: makeEdge(this->get_last_val()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          case 4: makeEdge(this->get_increment()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgImpliedDo");
        }

     return result;
   }

unsigned int
SgActualArgumentExpression::cfgIndexForEnd() const 
   {
     return 1;
   }

std::vector<VirtualCFG::CFGEdge>
SgActualArgumentExpression::cfgOutEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_expression()->cfgForBeginning(), result); break;
          case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
          default: ROSE_ASSERT (!"Bad index for SgActualArgumentExpression");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge>
SgActualArgumentExpression::cfgInEdges(unsigned int idx)
   {
     std::vector<VirtualCFG::CFGEdge> result;
     switch (idx)
        {
          case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
          case 1: makeEdge(this->get_expression()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
          default: ROSE_ASSERT (!"Bad index for SgActualArgumentExpression");
        }

     return result;
   }

std::vector<VirtualCFG::CFGEdge> SgDesignatedInitializer::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::CFGNode(this, idx), this->get_memberInit()->cfgForBeginning(), result); break;
    case 1: makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDesignatedInitializer");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge> SgDesignatedInitializer::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result); break;
    case 1: makeEdge(this->get_memberInit()->cfgForEnd(), VirtualCFG::CFGNode(this, idx), result); break;
    default: ROSE_ASSERT (!"Bad index for SgDesignatedInitializer");
  }
  return result;
}

unsigned int
SgDesignatedInitializer::cfgFindChildIndex(SgNode* n)
   {
     if (n == this->get_memberInit()) return 0;
     ROSE_ASSERT (!"Bad child in cfgFindChildIndex on SgDesignatedInitializer");
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

std::vector<VirtualCFG::CFGEdge>
SgInitializedName::cfgOutEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0:
    if (this->get_initializer()) {
      makeEdge(VirtualCFG::CFGNode(this, idx), this->get_initializer()->cfgForBeginning(),
	       result);
    } else {
      makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
    }
    break;

    case 1:
    ROSE_ASSERT (this->get_initializer());
    makeEdge(VirtualCFG::CFGNode(this, idx), VirtualCFG::getNodeJustAfterInContainer(this), result);
    break;

    default: ROSE_ASSERT (!"Bad index in SgInitializedName");
  }
  return result;
}

std::vector<VirtualCFG::CFGEdge>
SgInitializedName::cfgInEdges(unsigned int idx) {
  std::vector<VirtualCFG::CFGEdge> result;
  switch (idx) {
    case 0:
    makeEdge(VirtualCFG::getNodeJustBeforeInContainer(this), VirtualCFG::CFGNode(this, idx), result);
    break;

    case 1:
    ROSE_ASSERT (this->get_initializer());
    makeEdge(this->get_initializer()->cfgForEnd(),
	     VirtualCFG::CFGNode(this, idx), result);
    break;

    default: ROSE_ASSERT (!"Bad index in SgInitializedName");
  }
  return result;
}

