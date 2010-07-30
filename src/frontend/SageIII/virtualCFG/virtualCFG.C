// #include "cfgView.h"
// #include "rose.h"
// #include "loopHelpers.h"
// #include <sstream>
// #include <iomanip>
// #include <stdint.h>

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

using namespace std;

namespace VirtualCFG {

  unsigned int cfgIndexForEndWrapper(SgNode* n) {
    return n->cfgIndexForEnd();
  }

  static inline string variableName(SgInitializedName* in) {
    string s = in->get_name().str();
    if (s.empty()) s = "<anon>";
    return s;
  }

  CFGNode::CFGNode(SgNode* node, unsigned int index): node(node), index(index) {
#ifndef _MSC_VER 
    assert (!node || isSgStatement(node) || isSgExpression(node) || isSgInitializedName(node));
#endif
    assert (node && index <= node->cfgIndexForEnd());
  }

  string CFGNode::toString() const {
      ostringstream s;
    if (isSgFunctionDefinition(node)) {
      switch (index) {
          case 0: s << "Start("; break; 
          case 1: s << "After parameters("; break;
          case 2: s << "End("; break;
          default: { ROSE_ASSERT (!"Bad index"); /* Avoid MSVC warning. */ return "error"; }
      }
      s << isSgFunctionDefinition(node)->get_declaration()->get_qualified_name().str() << ")" << std::endl; 
    }
    s << toStringForDebugging();
  //if (isSgFunctionDefinition(node)) {
  //  s << std::endl << "decl'd by: <" << isSgFunctionDefinition(node)->get_declaration()->class_name() << "> @" << 
  //    isSgFunctionDefinition(node)->get_declaration()->get_startOfConstruct()->get_line();
    return s.str();
  }

  string CFGNode::toStringForDebugging() const {
    ostringstream s;
    if (node == NULL) {
      s << "End of procedure";
    } else {
      string nodeText;
      if (isSgInitializedName(node)) {
	nodeText = variableName(isSgInitializedName(node));
      } else {
	// nodeText = node->unparseToString(); -- Fortran CFG nodes can't always be unparsed
        ostringstream nt;
        nt << "@" << node->get_startOfConstruct()->get_line();
        nodeText += nt.str();
      }
      if (nodeText.length() > 20) {nodeText.resize(20); nodeText += "...";}
      s << "<" << node->class_name() << "> " << nodeText << " :" << index;
      // s << node->class_name() << " @" << hex << uintptr_t(node) << " " << dec << index;
    }
    return s.str();
  }

  string CFGNode::id() const {
    ostringstream s;
    s << "n_" << hex << uintptr_t(node) << "_" << dec << index;
    return s.str();
  }

  string CFGEdge::toString() const {
    return toStringForDebugging();
  }

  string CFGEdge::toStringForDebugging() const {
    ostringstream s;
    // s << src.id() << " -> " << tgt.id();
    bool anyNonEmpty = false;
    EdgeConditionKind cond = condition();
    if (cond != eckUnconditional) {
      if (anyNonEmpty) s << " "; // For consistency
      s << "key(";
      switch (cond) {
	case eckTrue:
	  s << "true";
	  break;
	case eckFalse:
	  s << "false";
	  break;
	case eckCaseLabel:
	  s << caseLabel()->unparseToString();
	  break;
	case eckDefault:
	  s << "default";
	  break;
        case eckDoConditionPassed:
          s << "do passed";
          break;
        case eckDoConditionFailed:
          s << "do failed";
          break;
        case eckForallIndicesInRange:
          s << "indices in range";
          break;
        case eckForallIndicesNotInRange:
          s << "indices not in range";
          break;
	case eckComputedGotoCaseLabel:
	  s << computedGotoCaseIndex();
	  break;
        case eckArithmeticIfLess:
          s << "less";
          break;
        case eckArithmeticIfEqual:
          s << "equal";
          break;
        case eckArithmeticIfGreater:
          s << "greater";
          break;
    case eckInterprocedural:
          s << "interprocedural";
          break;
	default:
	  s << "unknown";
	  break;
      }
      s << ")";
      anyNonEmpty = true;
    }
#if 0
    // MD 2010-07-19 This analysis should be disabled by default
    vector<SgInitializedName*> exitingScopes = scopesBeingExited();
    vector<SgInitializedName*> enteringScopes = scopesBeingEntered();
    if (!exitingScopes.empty()) {
      if (anyNonEmpty) s << " ";
      s << "leaving (";
      for (unsigned int i = 0; i < exitingScopes.size(); ++i) {
	if (i != 0) s << ", ";
	s << variableName(exitingScopes[i]);
      }
      s << ")";
      anyNonEmpty = true;
    }
    if (!enteringScopes.empty()) {
      if (anyNonEmpty) s << " ";
      s << "entering (";
      for (unsigned int i = 0; i < enteringScopes.size(); ++i) {
	if (i != 0) s << ", ";
	s << variableName(enteringScopes[i]);
      }
      s << ")";
      anyNonEmpty = true;
    }
#endif
    return s.str();
  }

  string CFGEdge::id() const {
    ostringstream s;
    s << src.id() << "__" << tgt.id();
    return s.str();
  }

  string CFGPath::toString() const {
    ostringstream s;
    bool anyNonEmpty = false;
    for (unsigned int i = 0; i < edges.size(); ++i) {
      string str = edges[i].toString();
      if (anyNonEmpty && !str.empty()) s << "; ";
      if (!str.empty()) anyNonEmpty = true;
      s << str;
    }
    return s.str();
  }

  string CFGPath::toStringForDebugging() const {
    ostringstream s;
    bool anyNonEmpty = false;
    for (unsigned int i = 0; i < edges.size(); ++i) {
      string str = edges[i].toStringForDebugging();
      if (anyNonEmpty && !str.empty()) s << "; ";
      if (!str.empty()) anyNonEmpty = true;
      s << str;
    }
    return s.str();
  }

  string CFGPath::id() const {
    ostringstream s;
    ROSE_ASSERT (!edges.empty());
    for (unsigned int i = 0; i < edges.size(); ++i) {
      s << edges[i].source().id() << "___";
    }
    s << edges.back().target().id();
    return s.str();
  }

  static vector<SgInitializedName*>
  findVariablesDirectlyInScope(SgStatement* sc) {
    // This can't be done with a traversal because it needs to avoid entering
    // inner scope statements
    vector<SgInitializedName*> variablesInScope;
    vector<SgNode*> succs = sc->get_traversalSuccessorContainer();
    for (int i = (int)succs.size() - 1; i > -1; --i) {
      if (isSgInitializedName(succs[i])) {
	variablesInScope.push_back(isSgInitializedName(succs[i]));
      } else if (isSgVariableDeclaration(succs[i])) {
	SgVariableDeclaration* vd = isSgVariableDeclaration(succs[i]);
	const SgInitializedNamePtrList& vars = vd->get_variables();
	variablesInScope.insert(variablesInScope.end(), vars.rbegin(), vars.rend());
      } else if (isSgForInitStatement(succs[i])) {
	vector<SgInitializedName*> initVars = findVariablesDirectlyInScope(isSgForInitStatement(succs[i]));
	variablesInScope.insert(variablesInScope.end(), initVars.begin(), initVars.end());
      } else if (isSgFunctionParameterList(succs[i])) {
	SgFunctionParameterList* pl = isSgFunctionParameterList(succs[i]);
	const SgInitializedNamePtrList& params = pl->get_args();
	variablesInScope.insert(variablesInScope.end(), params.begin(), params.end());
      }
    }
    return variablesInScope;
  }

// DQ (10/8/2006): This is a link error when optimized using g++ -O2
// inline bool CFGNode::isInteresting() const {
  bool CFGNode::isInteresting() const {
    ROSE_ASSERT (node);
    return node->cfgIsIndexInteresting(index);
  }

  static SgNode* leastCommonAncestor(SgNode* a, SgNode* b) {
    // Find the closest node which is an ancestor of both a and b
    vector<SgNode*> ancestorsOfA;
    for (SgNode* p = a; p; p = p->get_parent()) ancestorsOfA.push_back(p);
    while (b) {
      vector<SgNode*>::const_iterator i = std::find(ancestorsOfA.begin(), ancestorsOfA.end(), b);
      if (i != ancestorsOfA.end()) return *i;
      b = b->get_parent();
    }
    return NULL;
  }

  EdgeConditionKind CFGEdge::condition() const {
    SgNode* srcNode = src.getNode();
    unsigned int srcIndex = src.getIndex();
    SgNode* tgtNode = tgt.getNode();
    unsigned int tgtIndex = tgt.getIndex();
    if (isSgIfStmt(srcNode) && srcIndex == 1) {
      SgIfStmt* ifs = isSgIfStmt(srcNode);
      if (ifs->get_true_body() == tgtNode) {
	return eckTrue;
      } else if (tgtNode != NULL && ifs->get_false_body() == tgtNode) {
	return eckFalse;
      } else if (ifs->get_false_body() == NULL && tgtNode == ifs && tgtIndex == 2) {
        return eckFalse;
      } else ROSE_ASSERT (!"Bad successor in if statement");
    } else if (isSgArithmeticIfStatement(srcNode) && srcIndex == 1) {
      SgArithmeticIfStatement* aif = isSgArithmeticIfStatement(srcNode);
      if (getCFGTargetOfFortranLabelRef(aif->get_less_label()) == tgt) {
        return eckArithmeticIfLess;
      } else if (getCFGTargetOfFortranLabelRef(aif->get_equal_label()) == tgt) {
        return eckArithmeticIfEqual;
      } else if (getCFGTargetOfFortranLabelRef(aif->get_greater_label()) == tgt) {
        return eckArithmeticIfGreater;
      } else ROSE_ASSERT (!"Bad successor in arithmetic if statement");
    } else if (isSgWhileStmt(srcNode) && srcIndex == 1) {
      if (srcNode == tgtNode) {
	// False case for while test
	return eckFalse;
      } else {
	return eckTrue;
      }
    } else if (isSgDoWhileStmt(srcNode) && srcIndex == 2) {
      // tgtIndex values are 0 for true branch and 3 for false branch
      if (tgtIndex == 0) {
	return eckTrue;
      } else {
	return eckFalse;
      }
    } else if (isSgForStatement(srcNode) && srcIndex == 2) {
      if (srcNode == tgtNode) {
	// False case for test
	return eckFalse;
      } else {
	return eckTrue;
      }
    } else if (isSgFortranDo(srcNode) && srcIndex == 3) {
      if (tgtIndex == 4) {
        return eckDoConditionPassed;
      } else if (tgtIndex == 6) {
        return eckDoConditionFailed;
      } else ROSE_ASSERT (!"Bad successor in do loop");
    } else if (isSgForAllStatement(srcNode) && srcIndex == 1) {
      if (tgtIndex == 2) {
        return eckForallIndicesInRange;
      } else if (tgtIndex == 7) {
        return eckForallIndicesNotInRange;
      } else ROSE_ASSERT (!"Bad successor in forall loop");
    } else if (isSgForAllStatement(srcNode) && srcIndex == 3) {
      if (SageInterface::forallMaskExpression(isSgForAllStatement(srcNode))) {
        if (tgtIndex == 4) {
          return eckTrue;
        } else if (tgtIndex == 6) {
          return eckFalse;
        } else ROSE_ASSERT (!"Bad successor in forall loop");
      } else {
        return eckUnconditional;
      }
    } else if (isSgSwitchStatement(srcNode) && isSgCaseOptionStmt(tgtNode)) {
      return eckCaseLabel;
    } else if (isSgSwitchStatement(srcNode) && isSgDefaultOptionStmt(tgtNode)){
      return eckDefault;
    } else if (isSgComputedGotoStatement(srcNode) && srcIndex == 1) {
      if (tgtNode == srcNode) {
        return eckDefault;
      } else {
        return eckCaseLabel;
      }
    } else if (isSgConditionalExp(srcNode) && srcIndex == 1) {
      SgConditionalExp* ce = isSgConditionalExp(srcNode);
      if (ce->get_true_exp() == tgtNode) {
	return eckTrue;
      } else if (ce->get_false_exp() == tgtNode) {
	return eckFalse;
      } else ROSE_ASSERT (!"Bad successor in conditional expression");
    } else if (isSgAndOp(srcNode) && srcIndex == 1) {
      if (srcNode == tgtNode) {
	// Short-circuited false case
	return eckFalse;
      } else {
	return eckTrue;
      }
    } else if (isSgOrOp(srcNode) && srcIndex == 1) {
      if (srcNode == tgtNode) {
	// Short-circuited true case
	return eckTrue;
      } else {
	return eckFalse;
      }
    } else if (isSgFunctionCallExp(srcNode) && 
               srcIndex == 2 && 
               !isSgFunctionCallExp(tgtNode)) {
        return eckInterprocedural;
    } else if (isSgFunctionCallExp(tgtNode) && 
               tgtIndex == 3 && 
               !isSgFunctionCallExp(srcNode)) {
        return eckInterprocedural;
    } else {
      // No key
      return eckUnconditional;
    }

 // DQ (11/29/2009): It should be an error to reach this point in the function.
    ROSE_ASSERT(false);
 // DQ (11/29/2009): Avoid MSVC warning.
    return eckFalse;
  }

  SgExpression* CFGEdge::caseLabel() const {
    SgNode* srcNode = src.getNode();
    SgNode* tgtNode = tgt.getNode();
    if (isSgSwitchStatement(srcNode) && isSgCaseOptionStmt(tgtNode)) {
      return isSgCaseOptionStmt(tgtNode)->get_key();
    } else {
      return NULL;
    }
  }

  unsigned int CFGEdge::computedGotoCaseIndex() const {
    SgNode* srcNode = src.getNode();
    SgNode* tgtNode = tgt.getNode();
    if (isSgComputedGotoStatement(srcNode) && srcNode != tgtNode) {
      const Rose_STL_Container<SgExpression*>& labels = isSgComputedGotoStatement(srcNode)->get_labelList()->get_expressions();
      for (Rose_STL_Container<SgExpression*>::const_iterator j = labels.begin(); j != labels.end(); ++j) {
        SgLabelRefExp* lRef = isSgLabelRefExp(*j);
        if (getCFGTargetOfFortranLabelRef(lRef) == tgt) {
          return j - labels.begin() + 1;
        }
      }
      return 0;
    } else {
      return 0;
    }
  }

  static SgExpression* getExpressionForTest(SgStatement* t) {
    if (isSgExprStatement(t))
      return isSgExprStatement(t)->get_expression();
    else if (isSgVariableDeclaration(t)) {
      const SgInitializedNamePtrList& vars =
	isSgVariableDeclaration(t)->get_variables();
      ROSE_ASSERT (!vars.empty());
      SgInitializedName* v1 = vars.front();
      ROSE_ASSERT (isSgAssignInitializer(v1->get_initializer()));
      return isSgAssignInitializer(v1->get_initializer())->get_operand();
    } else {
      ROSE_ASSERT (!"Bad statement type in getExpressionForTest");

   // DQ (11/29/2009): Avoid MSVC warning about missign return stmt.
	  return NULL;
    }
  }

  SgExpression* CFGEdge::conditionBasedOn() const {
    SgNode* srcNode = src.getNode();
    unsigned int srcIndex = src.getIndex();
    if (isSgIfStmt(srcNode) && srcIndex == 1) {
      SgIfStmt* ifs = isSgIfStmt(srcNode);
      return getExpressionForTest(ifs->get_conditional());
    } else if (isSgArithmeticIfStatement(srcNode) && srcIndex == 1) {
      return isSgArithmeticIfStatement(srcNode)->get_conditional();
    } else if (isSgWhileStmt(srcNode) && srcIndex == 1) {
      return getExpressionForTest(isSgWhileStmt(srcNode)->get_condition());
    } else if (isSgDoWhileStmt(srcNode) && srcIndex == 2) {
      return getExpressionForTest(isSgDoWhileStmt(srcNode)->get_condition());
    } else if (isSgForStatement(srcNode) && srcIndex == 2) {
      return getExpressionForTest(isSgForStatement(srcNode)->get_test());
    } else if (isSgForAllStatement(srcNode) && srcIndex == 1) {
      return isSgForAllStatement(srcNode)->get_forall_header();
    } else if (isSgForAllStatement(srcNode) && srcIndex == 3) {
      return SageInterface::forallMaskExpression(isSgForAllStatement(srcNode));
    } else if (isSgFortranDo(srcNode) && srcIndex == 3) {
      return isSgFortranDo(srcNode)->get_bound();
    } else if (isSgSwitchStatement(srcNode)) {
      return getExpressionForTest(
	       isSgSwitchStatement(srcNode)->get_item_selector());
    } else if (isSgComputedGotoStatement(srcNode) && srcIndex == 1) {
      return isSgComputedGotoStatement(srcNode)->get_label_index();
    } else if (isSgConditionalExp(srcNode) && srcIndex == 1) {
      SgConditionalExp* ce = isSgConditionalExp(srcNode);
      return ce->get_conditional_exp();
    } else if (isSgAndOp(srcNode) && srcIndex == 1) {
      return isSgAndOp(srcNode)->get_lhs_operand();
    } else if (isSgOrOp(srcNode) && srcIndex == 1) {
      return isSgOrOp(srcNode)->get_lhs_operand();
    } else {
      return NULL;
    }
  }

  template <typename DoEffectsForwards,
	    typename DoEffectsBackwards,
	    typename DoEffectsForEnteringScope,
	    typename DoEffectsForLeavingScope>
  vector<SgInitializedName*>
  genericWindUnwind(SgNode* src, bool srcEnd, SgNode* tgt, bool tgtEnd,
		    DoEffectsForwards doEffectsForwards,
		    DoEffectsBackwards doEffectsBackwards,
		    DoEffectsForEnteringScope doEffectsForEnteringScope,
		    DoEffectsForLeavingScope doEffectsForLeavingScope) {
    vector<SgInitializedName*> data;
    vector<SgNode*> ancestorsOfTo, ancestorsOfFrom;
    SgNode* lca = leastCommonAncestor(src, tgt);
    for (SgNode* n = tgt; n != lca; n = n->get_parent()) ancestorsOfTo.push_back(n);
    ancestorsOfTo.push_back(lca);
    reverse(ancestorsOfTo.begin(), ancestorsOfTo.end()); // So outermost scopes are at the beginning
    for (SgNode* n = src; n != lca; n = n->get_parent()) ancestorsOfFrom.push_back(n);
    ancestorsOfFrom.push_back(lca);
    reverse(ancestorsOfFrom.begin(), ancestorsOfFrom.end()); // So outermost scopes are at the beginning

    // cerr << "++ Beginning of walk from " << src->unparseToString() << " to " << tgt->unparseToString() << endl;

    // Find positions of ancestors of "from" and "to" in children of "lca"
    unsigned int positionOfChild1, positionOfChild2;
    {
      vector<SgNode*> children = lca->get_traversalSuccessorContainer();
      positionOfChild1 = src == lca ? (srcEnd ? children.size() : 0) :
			 find(children.begin(), children.end(),
			      ancestorsOfFrom[1]) - children.begin();
      positionOfChild2 = tgt == lca ? (tgtEnd ? children.size() : 0) :
			 find(children.begin(), children.end(),
			      ancestorsOfTo[1]) - children.begin();
      ROSE_ASSERT (positionOfChild1 <= children.size());
      ROSE_ASSERT (positionOfChild2 <= children.size());
      ROSE_ASSERT (src == lca || positionOfChild1 != children.size());
      ROSE_ASSERT (tgt == lca || positionOfChild2 != children.size());
      // if (positionOfChild1 == positionOfChild2) return data; // Do nothing in this case
      // cerr << "In lca, pos of from is " << positionOfChild1 << ", of to is " << positionOfChild2 << endl;
    }

    // Walk from "src" to "lca", but not "lca" itself
    for (int i = (int)ancestorsOfFrom.size() - 2; i > 0; --i) {
      SgNode* anc = ancestorsOfFrom[i];
      // cerr << "Walking up ancestor " << anc->unparseToString() << endl;
      // cerr << "  to find " << ancestorsOfFrom[i + 1]->unparseToString() << endl;
      vector<SgNode*> children = anc->get_traversalSuccessorContainer();
      // This doesn't use findParent because here we care about lexical
      // children, not CFG children
      size_t positionOfChild = std::find(children.begin(), children.end(),
				 ancestorsOfFrom[i + 1])
			    - children.begin();
      ROSE_ASSERT (positionOfChild != children.size());
      for (int j = positionOfChild - 1; j >= 0; --j) {
	doEffectsBackwards(children[j], data);
      }
      doEffectsForLeavingScope(anc, data);
    }
    // Walk "lca"'s children
    {
      vector<SgNode*> children = lca->get_traversalSuccessorContainer();
      if (positionOfChild1 < positionOfChild2) {
	for (unsigned int j = positionOfChild1 + 1; j < positionOfChild2; ++j) {
	  // cerr << "Walking forward in lca: " << (children[j] ? children[j]->unparseToString() : "<null>") << endl;
	  doEffectsForwards(children[j], data);
	}
      } else if (positionOfChild1 > positionOfChild2) {
	for (int j = (int)positionOfChild1 - 1; j > (int)positionOfChild2; --j) {
	  // cerr << "Walking backward in lca: " << (children[j] ? children[j]->unparseToString() : "<null>") << endl;
	  doEffectsBackwards(children[j], data);
	}
      }
    }
    // Walk from "lca" to "tgt", but not "lca" itself
    for (unsigned int i = 1; i < ancestorsOfTo.size() - 1; ++i) {
      SgNode* anc = ancestorsOfTo[i];
      // cerr << "Walking down ancestor " << anc->unparseToString() << endl;
      // cerr << "  to find " << ancestorsOfTo[i + 1]->unparseToString() << endl;
      vector<SgNode*> children = anc->get_traversalSuccessorContainer();
      // This doesn't use findParent because here we care about lexical
      // children, not CFG children
      size_t positionOfChild = std::find(children.begin(), children.end(),
				 ancestorsOfTo[i + 1])
			    - children.begin();
      ROSE_ASSERT (positionOfChild != children.size());
      doEffectsForEnteringScope(anc, data);
      for (unsigned int j = 0; j < positionOfChild; ++j) {
	doEffectsForwards(children[j], data);
      }
    }
    // cerr << "-- End of walk" << endl;

    return data;
  }

  void getEntriesNull(SgNode*, vector<SgInitializedName*>&) {}

  template <bool Reverse>
  void getEntriesForScope(SgNode* n, vector<SgInitializedName*>& entries) {
    SgStatement* s = isSgStatement(n);
    if (!s) return;
    vector<SgInitializedName*> names = findVariablesDirectlyInScope(s);
    if (Reverse) {
      entries.insert(entries.end(), names.begin(), names.end());
    } else {
      entries.insert(entries.end(), names.rbegin(), names.rend());
    }
  }

  vector<SgInitializedName*> windToLocation(CFGNode src, CFGNode tgt) {
    SgNode* srcNode = src.getNode();
 // unsigned int srcIndex = src.getIndex();
    SgNode* tgtNode = tgt.getNode();
 // unsigned int tgtIndex = tgt.getIndex();
    if (isSgExpression(srcNode) && isSgExpression(tgtNode)) {
      return vector<SgInitializedName*>(); // Common case, since these cannot be arbitrary jumps
    }
    if (srcNode->get_parent() == tgtNode || tgtNode->get_parent() == srcNode) {
      return vector<SgInitializedName*>(); // We assume that these are consecutive program points
    }
    vector<SgInitializedName*> scopesEntering = genericWindUnwind(srcNode, false, tgtNode, false, getEntriesForScope<false>, getEntriesNull, getEntriesNull, getEntriesNull);

    return scopesEntering;
  }

  vector<SgInitializedName*> unwindToLocation(CFGNode src, CFGNode tgt) {
    SgNode* srcNode = src.getNode();
 // unsigned int srcIndex = src.getIndex();
    SgNode* tgtNode = tgt.getNode();
    unsigned int tgtIndex = tgt.getIndex();
    if (isSgExpression(srcNode) && isSgExpression(tgtNode)) {
      return vector<SgInitializedName*>(); // Common case, since these cannot be arbitrary jumps
    }
    vector<SgInitializedName*> scopesLeaving;
    if (srcNode->get_parent() == tgtNode && isSgScopeStatement(srcNode)) {
      scopesLeaving = findVariablesDirectlyInScope(isSgScopeStatement(srcNode));
    } else if (srcNode->get_parent() == tgtNode || tgtNode->get_parent() == srcNode) {
      scopesLeaving = vector<SgInitializedName*>(); // We assume that these are consecutive program points
    } else {
      scopesLeaving = genericWindUnwind(srcNode, false, tgtNode, false, getEntriesNull, getEntriesForScope<true>, getEntriesNull, getEntriesNull);
    }

    bool isEndOfProcedure = isSgFunctionDefinition(tgtNode) && tgtIndex == 2; // This only applies when this function is used for unwinding in a return statement
    if (isEndOfProcedure) {
      // Special case: add in procedure parameters
      SgFunctionDefinition* fd = isSgFunctionDefinition(tgtNode);
      SgFunctionParameterList* pl = fd->get_declaration()->get_parameterList();
      const SgInitializedNamePtrList& params = pl->get_args();
      scopesLeaving.insert(scopesLeaving.end(),
			   params.rbegin(), params.rend());
    }
    return scopesLeaving;
  }

  vector<SgInitializedName*> CFGEdge::scopesBeingExited() const {
    return unwindToLocation(source(), target());
  }

  vector<SgInitializedName*> CFGEdge::scopesBeingEntered() const {
    vector<SgInitializedName*> scopesEntering = 
      windToLocation(source(), target());
    if (isSgInitializedName(src.getNode()) && src.getIndex() == 0) {
      scopesEntering.push_back(isSgInitializedName(src.getNode()));
    }
    return scopesEntering;
  }

  vector<CFGEdge> CFGNode::outEdges() const {
    ROSE_ASSERT (node);
    return node->cfgOutEdges(index);
  }

  vector<CFGEdge> CFGNode::inEdges() const {
    ROSE_ASSERT (node);
    return node->cfgInEdges(index);
  }


  vector<InterestingEdge> makeClosure(const vector<CFGEdge>& orig,
				      vector<CFGEdge> (CFGNode::*closure)() const,
				      CFGNode (CFGPath::*otherSide)() const,
				      CFGPath (*merge)(const CFGPath&, const CFGPath&)) {
    vector<CFGPath> rawEdges(orig.begin(), orig.end());
    // cerr << "makeClosure starting with " << orig.size() << endl;
    while (true) {
      top:
      // cerr << "makeClosure loop: " << rawEdges.size() << endl;
      
// CH (5/27/2010): 'push_back' may invalidate iterators of a vector.
// Using index instead to fix this subtle bug.
#if 0 
      for (vector<CFGPath>::iterator i = rawEdges.begin(); i != rawEdges.end(); ++i) {
	if (!((*i).*otherSide)().isInteresting()) {
	  unsigned int oldSize = rawEdges.size();
	  vector<CFGEdge> rawEdges2 = (((*i).*otherSide)().*closure)();
	  for (unsigned int j = 0; j < rawEdges2.size(); ++j) {
	    CFGPath merged = (*merge)(*i, rawEdges2[j]);
            if (std::find(rawEdges.begin(), rawEdges.end(), merged) == rawEdges.end()) {
              rawEdges.push_back(merged);
            }
	  }
	  if (rawEdges.size() != oldSize) goto top; // To restart iteration
	}
      }
#else
      for (int i = 0; i < rawEdges.size(); ++i) {
	if (!(rawEdges[i].*otherSide)().isInteresting()) {
	  unsigned int oldSize = rawEdges.size();
	  vector<CFGEdge> rawEdges2 = ((rawEdges[i].*otherSide)().*closure)();
	  for (unsigned int j = 0; j < rawEdges2.size(); ++j) {
	    CFGPath merged = (*merge)(rawEdges[i], rawEdges2[j]);
            if (std::find(rawEdges.begin(), rawEdges.end(), merged) == rawEdges.end()) {
              rawEdges.push_back(merged);
            }
	  }
	  if (rawEdges.size() != oldSize) goto top; // To restart iteration
	}
      }
#endif
      break; // If the iteration got all of the way through
    }
    // cerr << "makeClosure loop done: " << rawEdges.size() << endl;
    vector<InterestingEdge> edges;
    for (vector<CFGPath>::const_iterator i = rawEdges.begin(); i != rawEdges.end(); ++i) {
      if (((*i).*otherSide)().isInteresting())
	edges.push_back(InterestingEdge(*i));
    }
    // cerr << "makeClosure done: " << edges.size() << endl;
    return edges;
  }

  vector<InterestingEdge> InterestingNode::outEdges() const {
    return makeClosure(n.outEdges(), &CFGNode::outEdges, &CFGPath::target, &mergePaths);
  }

  vector<InterestingEdge> InterestingNode::inEdges() const {
    return makeClosure(n.inEdges(), &CFGNode::inEdges, &CFGPath::source, &mergePathsReversed);
  }

  CFGNode getCFGTargetOfFortranLabelSymbol(SgLabelSymbol* sym) {
    SgStatement* st = sym->get_fortran_statement();
    ROSE_ASSERT (st);
    switch (sym->get_label_type()) {
      case SgLabelSymbol::e_non_numeric_label_type: return st->cfgForBeginning();
      case SgLabelSymbol::e_start_label_type: return st->cfgForBeginning();
      case SgLabelSymbol::e_else_label_type: {
        SgIfStmt* ifs = isSgIfStmt(st);
        ROSE_ASSERT (ifs);
        ROSE_ASSERT (ifs->get_false_body() != NULL);
        return ifs->get_false_body()->cfgForBeginning();
      }
      case SgLabelSymbol::e_end_label_type: {
        if (isSgProgramHeaderStatement(st)) {
          return isSgProgramHeaderStatement(st)->get_definition()->cfgForEnd();
        } else if (isSgProcedureHeaderStatement(st)) {
          return isSgProcedureHeaderStatement(st)->get_definition()->cfgForEnd();
        } else {
          return st->cfgForEnd();
        }
      }
	  default: { ROSE_ASSERT (!"Invalid Fortran label type"); /* avoid MSVC warning of no return stmt */ return st->cfgForEnd(); }
    }
  }

  CFGNode getCFGTargetOfFortranLabelRef(SgLabelRefExp* lRef) {
    ROSE_ASSERT (lRef);
    SgLabelSymbol* sym = lRef->get_symbol();
    ROSE_ASSERT (sym);
    return getCFGTargetOfFortranLabelSymbol(sym);
  }

}
