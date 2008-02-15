// #include "cfgView.h"
// #include "rose.h"
// #include "loopHelpers.h"
// #include <sstream>
// #include <iomanip>
// #include <stdint.h>

#include <rose.h>
#include "virtualBinCFG.h"

using namespace std;

#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

// DQ (10/21/2007): Added support for optional use of binary support in ROSE.
#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT

namespace VirtualBinCFG {

  string CFGNode::toString() const {
    if (isSgAsmFunctionDeclaration(node)) {
      return "BinaryFunctionDefinition";
      }
    return "";
  }

  string CFGNode::toStringForDebugging() const {
    ostringstream s;
    if (node == NULL) {
      s << "End of procedure";
    } else {
      string nodeText;
    }
    return s.str();
  }

  string CFGNode::id() const {
    ostringstream s;
    s << "n_" << hex << uintptr_t(node) << "_" << dec ;
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
	  //	  s << caseLabel()->unparseToString();
	  break;
	case eckDefault:
	  s << "default";
	  break;
	default:
	  s << "unknown";
	  break;
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




  inline CFGNode findParentNode(SgNode* n) {
    // Find the CFG node of which n is a child (subtree descended into)
    // This is mostly just doing lookups in the children of n's parent to find
    // out which index n is at

    SgAsmNode* parent = isSgAsmNode(n->get_parent());
    ROSE_ASSERT (parent);
    /*
    if (isSgAsmFunctionDeclaration(n)) return CFGNode(0, 0); // Should not be used
    return CFGNode(parent, parent->cfgBinFindChildIndex(n));
*/
      return CFGNode(parent);
  }

  CFGNode getNodeJustAfterInContainer(SgNode* n) {
    // Only handles next-statement control flow
    SgAsmNode* parent = isSgAsmNode(n->get_parent()); 
    return CFGNode(parent) ; // todo //CFGNode(parent, parent->cfgBinFindNextChildIndex(n));
  }

  CFGNode getNodeJustBeforeInContainer(SgNode* n) {
    // Only handles previous-statement control flow
    return findParentNode(n);
  }

  SgNode* leastCommonAncestor(SgNode* a, SgNode* b) {
    // Find the closest node which is an ancestor of both a and b
    vector<SgNode*> ancestorsOfA;
    for (SgNode* p = a; p; p = isSgNode(p->get_parent())) ancestorsOfA.push_back(p);
    while (b) {
      vector<SgNode*>::const_iterator i = std::find(ancestorsOfA.begin(), ancestorsOfA.end(), b);
      if (i != ancestorsOfA.end()) return *i;
      b = isSgNode(b->get_parent());
    }
    return NULL;
  }

  EdgeConditionKind CFGEdge::condition() const {
    //SgAsmNode* srcNode = src.getNode();
    //unsigned int srcIndex = src.getIndex();
    //SgAsmNode* tgtNode = tgt.getNode();
    //unsigned int tgtIndex = tgt.getIndex();
    //if (isSgAsmMov(srcNode) ) {
      //SgAsmMov* ifs = isSgAsmMov(srcNode);
      /*
      if (ifs->get_true_body() == tgtNode) {
	return eckTrue;
      } else if (ifs->get_false_body() == tgtNode) {
	return eckFalse;
      } else ROSE_ASSERT (!"Bad successor in if statement");
      */
    //}
    /*
    else if (isSgWhileStmt(srcNode) && srcIndex == 1) {
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
    } else if (isSgSwitchStatement(srcNode) && isSgCaseOptionStmt(tgtNode)) {
      return eckCaseLabel;
    } else if (isSgSwitchStatement(srcNode) && isSgDefaultOptionStmt(tgtNode)){
      return eckDefault;
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
    } 
    */
    //else {
      // No key
      return eckUnconditional;
      //}
  }





  void makeEdge(CFGNode from, CFGNode to, vector<CFGEdge>& result) {
    // Makes a CFG edge, adding appropriate labels
    //SgAsmNode* fromNode = from.getNode();
    //unsigned int fromIndex = from.getIndex();
    //SgAsmNode* toNode = to.getNode();
 // unsigned int toIndex = to.getIndex();

    /*
    // Exit early if the edge should not exist because of a control flow discontinuity
    if (fromIndex == 1 && (isSgGotoStatement(fromNode) || isSgBreakStmt(fromNode) || isSgContinueStmt(fromNode))) {
      return;
    }
    if (isSgReturnStmt(fromNode) && toNode == fromNode->get_parent()) {
      SgReturnStmt* rs = isSgReturnStmt(fromNode);
      if (fromIndex == 1 || fromIndex == 0 && !rs->get_expression()) return;
    }
    if (fromIndex == 1 && isSgSwitchStatement(fromNode) &&
	isSgSwitchStatement(fromNode)->get_body() == toNode) return;
    */
    // Create the edge
    result.push_back(CFGEdge(from, to));
  }

  vector<CFGEdge> CFGNode::outEdges() const {
    ROSE_ASSERT (node);
    return node->cfgBinOutEdges();
  }

  vector<CFGEdge> CFGNode::inEdges() const {
    ROSE_ASSERT (node);
    return node->cfgBinInEdges();
  }


}

// endif matching ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
#endif
