// #include "cfgView.h"
// #include "rose.h"
// #include "loopHelpers.h"
// #include <sstream>
// #include <iomanip>
// #include <stdint.h>

#include <rose.h>

using namespace std;

#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

namespace VirtualCFG {

  SgFunctionDeclaration* getDeclaration(SgExpression* func) {
    if (isSgFunctionRefExp(func)) {
      return isSgFunctionRefExp(func)->get_symbol()->get_declaration();
    } else if (isSgDotExp(func) || isSgArrowExp(func)) {
      SgExpression* func2 = isSgBinaryOp(func)->get_rhs_operand();
      ROSE_ASSERT (isSgMemberFunctionRefExp(func2));
      return isSgMemberFunctionRefExp(func2)->get_symbol()->get_declaration();
    } else return 0;
  }

  inline string variableName(SgInitializedName* in) {
    string s = in->get_name().str();
    if (s.empty()) s = "<anon>";
    return s;
  }

  string CFGNode::toString() const {
    if (isSgFunctionDefinition(node)) {
      switch (index) {
	case 0: return "Start";
	case 1: return "After parameters";
	case 2: return "End";
	default: ROSE_ASSERT (!"Bad index");
      }
    } else {
      return toStringForDebugging();
    }
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
	default:
	  s << "unknown";
	  break;
      }
      s << ")";
      anyNonEmpty = true;
    }
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

  inline unsigned int CFGNode::childCount(SgNode* node) {
    ROSE_ASSERT (node);
    return node->cfgIndexForEnd();
  }

// DQ (10/8/2006): This is a link error when optimized using g++ -O2
// inline bool CFGNode::isInteresting() const {
  bool CFGNode::isInteresting() const {
    ROSE_ASSERT (node);
    return node->cfgIsIndexInteresting(index);
  }

  inline CFGNode findParentNode(SgNode* n) {
    // Find the CFG node of which n is a child (subtree descended into)
    // This is mostly just doing lookups in the children of n's parent to find
    // out which index n is at
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

  CFGNode getNodeJustAfterInContainer(SgNode* n) {
    // Only handles next-statement control flow
    SgNode* parent = n->get_parent();
    if (isSgFunctionParameterList(n)) {
      SgFunctionDeclaration* decl = isSgFunctionDeclaration(isSgFunctionParameterList(n)->get_parent());
      ROSE_ASSERT (decl);
      return CFGNode(decl->get_definition(), 1);
    }
    return CFGNode(parent, parent->cfgFindNextChildIndex(n));
  }

  CFGNode getNodeJustBeforeInContainer(SgNode* n) {
    // Only handles previous-statement control flow
    return findParentNode(n);
  }

  SgNode* leastCommonAncestor(SgNode* a, SgNode* b) {
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
      if (forallMaskExpression(isSgForAllStatement(srcNode))) {
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
    } else {
      // No key
      return eckUnconditional;
    }
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
      return forallMaskExpression(isSgForAllStatement(srcNode));
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

  void makeEdge(CFGNode from, CFGNode to, vector<CFGEdge>& result) {
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
    set<CFGPath> rawEdges(orig.begin(), orig.end());
    // cerr << "makeClosure starting with " << orig.size() << endl;
    while (true) {
      top:
      // cerr << "makeClosure loop: " << rawEdges.size() << endl;
      for (set<CFGPath>::iterator i = rawEdges.begin(); i != rawEdges.end(); ++i) {
	if (!((*i).*otherSide)().isInteresting()) {
	  unsigned int oldSize = rawEdges.size();
	  vector<CFGEdge> rawEdges2 = (((*i).*otherSide)().*closure)();
	  for (unsigned int j = 0; j < rawEdges2.size(); ++j) {
	    CFGPath merged = (*merge)(*i, rawEdges2[j]);
	    pair<set<CFGPath>::iterator, bool> x = rawEdges.insert(merged);
	    // if (x.second)
	      // cerr << "Added " << merged.source().toString() << " -> " << merged.target().toString() << ": " << merged.toString() << endl;
	  }
	  if (rawEdges.size() != oldSize) goto top; // To restart iteration
	}
      }
      break; // If the iteration got all of the way through
    }
    // cerr << "makeClosure loop done: " << rawEdges.size() << endl;
    vector<InterestingEdge> edges;
    for (set<CFGPath>::const_iterator i = rawEdges.begin(); i != rawEdges.end(); ++i) {
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
      default: ROSE_ASSERT (!"Invalid Fortran label type");
    }
  }

  CFGNode getCFGTargetOfFortranLabelRef(SgLabelRefExp* lRef) {
    ROSE_ASSERT (lRef);
    SgLabelSymbol* sym = lRef->get_symbol();
    ROSE_ASSERT (sym);
    return getCFGTargetOfFortranLabelSymbol(sym);
  }

  void addIncomingFortranGotos(SgStatement* stmt, unsigned int index, vector<CFGEdge>& result) {
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
    VirtualCFG::CFGNode cfgNode(stmt, index);
    // Find all gotos to this CFG node, functionwide
    SgFunctionDefinition* thisFunction = SageInterface::getEnclosingProcedure(stmt, true);
    Rose_STL_Container<SgNode*> allGotos = NodeQuery::querySubTree(thisFunction, V_SgGotoStatement);
    for (Rose_STL_Container<SgNode*>::const_iterator i = allGotos.begin(); i != allGotos.end(); ++i) {
      SgLabelRefExp* lRef = isSgGotoStatement(*i)->get_label_expression();
      if (!lRef) continue;
      SgLabelSymbol* sym = lRef->get_symbol();
      ROSE_ASSERT(sym);
      if (getCFGTargetOfFortranLabelSymbol(sym) == cfgNode) {
        makeEdge(VirtualCFG::CFGNode(isSgGotoStatement(*i), 0), cfgNode, result);
      }
    }
    Rose_STL_Container<SgNode*> allComputedGotos = NodeQuery::querySubTree(thisFunction, V_SgComputedGotoStatement);
    for (Rose_STL_Container<SgNode*>::const_iterator i = allComputedGotos.begin(); i != allComputedGotos.end(); ++i) {
      const Rose_STL_Container<SgExpression*>& labels = isSgComputedGotoStatement(*i)->get_labelList()->get_expressions();
      for (Rose_STL_Container<SgExpression*>::const_iterator j = labels.begin(); j != labels.end(); ++j) {
        SgLabelRefExp* lRef = isSgLabelRefExp(*j);
        ROSE_ASSERT (lRef);
        SgLabelSymbol* sym = lRef->get_symbol();
        ROSE_ASSERT(sym);
        if (getCFGTargetOfFortranLabelSymbol(sym) == cfgNode) {
          makeEdge(VirtualCFG::CFGNode(isSgComputedGotoStatement(*i), 1), cfgNode, result);
        }
      }
    }
    Rose_STL_Container<SgNode*> allArithmeticIfs = NodeQuery::querySubTree(thisFunction, V_SgArithmeticIfStatement);
    for (Rose_STL_Container<SgNode*>::const_iterator i = allArithmeticIfs.begin(); i != allArithmeticIfs.end(); ++i) {
      SgArithmeticIfStatement* aif = isSgArithmeticIfStatement(*i);
      if (getCFGTargetOfFortranLabelRef(aif->get_less_label()) == cfgNode ||
          getCFGTargetOfFortranLabelRef(aif->get_equal_label()) == cfgNode ||
          getCFGTargetOfFortranLabelRef(aif->get_greater_label()) == cfgNode) {
        makeEdge(VirtualCFG::CFGNode(aif, 1), cfgNode, result);
      }
    }
  }

  void addOutEdgeOrBypassForExpressionChild(SgNode* me, unsigned int idx, SgExpression* e, vector<CFGEdge>& result) {
    if (e) {
      makeEdge(VirtualCFG::CFGNode(me, idx), e->cfgForBeginning(), result);
    } else {
      makeEdge(VirtualCFG::CFGNode(me, idx), VirtualCFG::CFGNode(me, idx + 1), result);
    }
  }

  void addInEdgeOrBypassForExpressionChild(SgNode* me, unsigned int idx, SgExpression* e, vector<CFGEdge>& result) {
    if (e) {
      makeEdge(e->cfgForEnd(), VirtualCFG::CFGNode(me, idx), result);
    } else {
      makeEdge(VirtualCFG::CFGNode(me, idx - 1), VirtualCFG::CFGNode(me, idx), result);
    }
  }

  bool handleFortranIOCommonOutEdges(SgIOStatement* me, unsigned int idx, unsigned int numChildren, vector<CFGEdge>& result) {
    switch (idx - numChildren) {
      case 0: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_io_stmt_list(), result); return true;
      case 1: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_unit(), result); return true;
      case 2: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_iostat(), result); return true;
      case 3: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_err(), result); return true;
      case 4: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_iomsg(), result); return true;
      default: return false;
    }
  }

  bool handleFortranIOCommonInEdges(SgIOStatement* me, unsigned int idx, unsigned int numChildren, vector<CFGEdge>& result) {
    switch (idx - numChildren) {
      case 1: addInEdgeOrBypassForExpressionChild(me, idx, me->get_io_stmt_list(), result); return true;
      case 2: addInEdgeOrBypassForExpressionChild(me, idx, me->get_unit(), result); return true;
      case 3: addInEdgeOrBypassForExpressionChild(me, idx, me->get_iostat(), result); return true;
      case 4: addInEdgeOrBypassForExpressionChild(me, idx, me->get_err(), result); return true;
      case 5: addInEdgeOrBypassForExpressionChild(me, idx, me->get_iomsg(), result); return true;
      default: return false;
    }
  }

  SgExpression* forallMaskExpression(SgForAllStatement* stmt) {
    SgExprListExp* el = stmt->get_forall_header();
    const SgExpressionPtrList& ls = el->get_expressions();
    if (ls.empty()) return 0;
    if (isSgAssignOp(ls.back())) return 0;
    return ls.back();
  }

  // Forall CFG layout:
  // forall:0 -> header -> forall:1 (representing initial assignments)
  // forall:1 -> forall:2 (conditioned on loop tests) and forall:7
  // forall:2 -> mask (if any) -> forall:3
  // forall:3 -> forall:4 (on mask)
  // forall:3 -> forall:6 (on !mask)
  // forall:4 -> body -> forall:5 -> forall:6 (for increment(s)) -> forall:1
  // forall:7 -> successor

  unsigned int doForallCfgIndexForEnd(const SgForAllStatement* me) {
    return 7;
  }

  bool doForallCfgIsIndexInteresting(const SgForAllStatement* me, unsigned int idx) {
    return idx == 1 || idx == 3 || idx == 6;
  }

  unsigned int doForallCfgFindChildIndex(SgForAllStatement* me, SgNode* tgt) {
    if (tgt == me->get_forall_header()) {
      return 0;
    } else if (tgt && tgt == forallMaskExpression(me)) {
      return 2;
    } else if (tgt == me->get_body()) {
      return 4;
    } else ROSE_ASSERT (!"Bad child in doForallCfgFindChildIndex");
  }

  unsigned int doForallCfgFindNextChildIndex(SgForAllStatement* me, SgNode* tgt) {
    return doForallCfgFindChildIndex(me, tgt) + 1;
  }

  std::vector<VirtualCFG::CFGEdge> doForallCfgOutEdges(SgForAllStatement* me, unsigned int idx) {
    vector<VirtualCFG::CFGEdge> result;
    switch (idx) {
      case 0: addOutEdgeOrBypassForExpressionChild(me, idx, me->get_forall_header(), result); break;
      case 1: {
        makeEdge(CFGNode(me, 1), CFGNode(me, 2), result);
        makeEdge(CFGNode(me, 1), CFGNode(me, 7), result);
        break;
      }
      case 2: addOutEdgeOrBypassForExpressionChild(me, idx, forallMaskExpression(me), result); break;
      case 3: {
        makeEdge(CFGNode(me, 3), CFGNode(me, 4), result);
        makeEdge(CFGNode(me, 3), CFGNode(me, 6), result);
        break;
      }
      case 4: makeEdge(CFGNode(me, 4), me->get_body()->cfgForBeginning(), result); break;
      case 5: makeEdge(CFGNode(me, 5), CFGNode(me, 6), result); break;
      case 6: makeEdge(CFGNode(me, 6), CFGNode(me, 1), result); break;
      case 7: makeEdge(CFGNode(me, 7), getNodeJustAfterInContainer(me), result); break;
      default: ROSE_ASSERT (!"Bad index in doForallCfgOutEdges");
    }
    return result;
  }

  std::vector<VirtualCFG::CFGEdge> doForallCfgInEdges(SgForAllStatement* me, unsigned int idx) {
    vector<VirtualCFG::CFGEdge> result;
    addIncomingFortranGotos(me, idx, result);
    switch (idx) {
      case 0: makeEdge(VirtualCFG::getNodeJustBeforeInContainer(me), VirtualCFG::CFGNode(me, idx), result); break;
      case 1: {
        addInEdgeOrBypassForExpressionChild(me, idx, me->get_forall_header(), result);
        makeEdge(CFGNode(me, 6), CFGNode(me, 1), result);
        break;
      }
      case 2: makeEdge(CFGNode(me, 1), CFGNode(me, 2), result); break;
      case 3: addInEdgeOrBypassForExpressionChild(me, idx, forallMaskExpression(me), result); break;
      case 4: makeEdge(CFGNode(me, 3), CFGNode(me, 4), result); break;
      case 5: makeEdge(me->get_body()->cfgForEnd(), CFGNode(me, 5), result); break;
      case 6: {
        makeEdge(CFGNode(me, 3), CFGNode(me, 6), result);
        makeEdge(CFGNode(me, 5), CFGNode(me, 6), result);
        break;
      }
      case 7: makeEdge(CFGNode(me, 1), CFGNode(me, 7), result); break;
      default: ROSE_ASSERT (!"Bad index in doForallCfgInEdges");
    }
    return result;
  }

}
