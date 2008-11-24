#ifndef VIRTUAL_CFG_H
#define VIRTUAL_CFG_H

// #include "rose.h"
// #include <string>
// #include <vector>
// #include <cassert>

namespace VirtualCFG {

  class CFGEdge;

  enum EdgeConditionKind {
    eckUnconditional, // Normal, unconditional edge
    eckTrue,	      // True case of a two-way branch
    eckFalse,	      // False case of a two-way branch
    eckCaseLabel,     // Case label (constant is given by caseLabel())
    eckDefault,	      // Default label
    eckDoConditionPassed, // Enter Fortran do loop body
    eckDoConditionFailed, // Fortran do loop finished
    eckForallIndicesInRange, // Start testing forall mask
    eckForallIndicesNotInRange, // End of forall loop
    eckComputedGotoCaseLabel, // Case in computed goto -- number needs to be computed separately
    eckArithmeticIfLess,
    eckArithmeticIfEqual,
    eckArithmeticIfGreater // Three options from a Fortran arithmetic if statement
  };

  // CFG node is based on a subset of SgNode with indices
  class CFGNode {
    SgNode* node; // Must be either a statement, expression, or SgInitializedName
    unsigned int index;

    public:
    CFGNode(): node(0), index(0) {}
    explicit CFGNode(SgNode* node, unsigned int index = 0): node(node), index(index) {
      assert (!node || isSgStatement(node) || isSgExpression(node) || isSgInitializedName(node));
    }
    // Pretty string for Dot node labels, etc.
    std::string toString() const;
    // String for debugging graphs
    std::string toStringForDebugging() const;
    // ID to use for Dot, etc.
    std::string id() const;
    SgNode* getNode() const {return node;}
    unsigned int getIndex() const {return index;}
    std::vector<CFGEdge> outEdges() const;
    std::vector<CFGEdge> inEdges() const;
    bool isInteresting() const;
    bool operator==(const CFGNode& o) const {return node == o.node && index == o.index;}
    bool operator!=(const CFGNode& o) const {return !(*this == o);}
    bool operator<(const CFGNode& o) const {return node < o.node || (node == o.node && index < o.index);}
    static unsigned int childCount(SgNode* n);
  }; // end class CFGNode

  // A CFG edge connecting two CFG nodes, with an edge condition to indicate edge types
  class CFGEdge {
    CFGNode src, tgt;
    public:
    CFGEdge(CFGNode src, CFGNode tgt): src(src), tgt(tgt) {}
    // Pretty string for Dot node labels, etc.
    std::string toString() const;
    // String for debugging graphs
    std::string toStringForDebugging() const;
    // ID to use for Dot, etc.
    std::string id() const;
    CFGNode source() const {return src;}
    CFGNode target() const {return tgt;}
    EdgeConditionKind condition() const;
    SgExpression* caseLabel() const;
    unsigned int computedGotoCaseIndex() const;
    SgExpression* conditionBasedOn() const;
    std::vector<SgInitializedName*> scopesBeingExited() const;
    std::vector<SgInitializedName*> scopesBeingEntered() const;
    bool operator==(const CFGEdge& o) const {return src == o.src && tgt == o.tgt;}
    bool operator!=(const CFGEdge& o) const {return src != o.src || tgt != o.tgt;}
#if 0
    bool operator<(const CFGEdge& o) const {return src < o.src || (src == o.src && tgt < o.tgt);}
#endif
  }; // end CFGEdge

  void makeEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result); // Used in inEdges() and outEdges() methods
  CFGNode getNodeJustAfterInContainer(SgNode* n);
  CFGNode getNodeJustBeforeInContainer(SgNode* n);

  // ! A CFG path is a set of connected CFG edges
  class CFGPath {
    std::vector<CFGEdge> edges;
    public:
 // DQ (8/28/2006): This constructor causes a bug to be brought out in ROSE 
 // (in compiling this file using ROSE) see test2006_124.C for a smaller example.
    CFGPath(CFGEdge e): edges(1, e) {}
    // Merge two CFG paths
    CFGPath(const CFGPath& a, const CFGPath& b): edges(a.edges) {
      assert (!a.edges.empty());
      assert (!b.edges.empty());
      assert (a.edges.back().target() == b.edges.front().source());
      edges.insert(edges.end(),b.edges.begin(),b.edges.end());
    }
    std::string toString() const;
    std::string toStringForDebugging() const;
    std::string id() const;
    // Get the head CFG node of the path
    CFGNode source() const {assert (!edges.empty()); return edges.front().source();}
    // Get the tail CFG node of the path
    CFGNode target() const {assert (!edges.empty()); return edges.back().target();}
    //Return the first non-unconditional edge's condition
    EdgeConditionKind condition() const {
      for (unsigned int i = 0; i < edges.size(); ++i) {
	EdgeConditionKind kind = edges[i].condition();
	if (kind != eckUnconditional) return kind;
      }
      return eckUnconditional;
    }
    // Return the case label of its first edge representing a case
    SgExpression* caseLabel() const {
      for (unsigned int i = 0; i < edges.size(); ++i) {
          SgExpression* label = edges[i].caseLabel();
          if (label != NULL) return label;
        }
      return NULL;
    }
    SgExpression* conditionBasedOn() const {
      for (unsigned int i = 0; i < edges.size(); ++i) {
          SgExpression* base = edges[i].conditionBasedOn();
          if (base != NULL) return base;
      }
      return NULL;
    }
    std::vector<SgInitializedName*> scopesBeingExited() const {
      std::vector<SgInitializedName*> result;
      for (unsigned int i = 0; i < edges.size(); ++i) {
	std::vector<SgInitializedName*> s_i = edges[i].scopesBeingExited();
	result.insert(result.end(), s_i.begin(), s_i.end());
      }
      return result;
    }
    std::vector<SgInitializedName*> scopesBeingEntered() const {
      std::vector<SgInitializedName*> result;
      for (unsigned int i = 0; i < edges.size(); ++i) {
	std::vector<SgInitializedName*> s_i = edges[i].scopesBeingEntered();
	result.insert(result.end(), s_i.begin(), s_i.end());
      }
      return result;
    }
    bool operator==(const CFGPath& o) const {return edges == o.edges;}
    bool operator!=(const CFGPath& o) const {return edges != o.edges;}
#if 0
    bool operator<(const CFGPath& o) const {
      // An arbitrary order
      if (edges.size() != o.edges.size()) {
	return edges.size() < o.edges.size();
      }
      for (unsigned int i = 0; i < edges.size(); ++i) {
	if (edges[i] != o.edges[i]) {
	  return edges[i] < o.edges[i];
	}
      }
      return false;
    }
#endif
  }; // end CFGPath

  inline CFGPath mergePaths(const CFGPath& hd, const CFGPath& tl) {
    // Assumes the edges don't do anything too complicated with scopes
    return CFGPath(hd, tl);
  }

  inline CFGPath mergePathsReversed(const CFGPath& tl, const CFGPath& hd) {
    return mergePaths(hd, tl);
  }

  inline CFGNode cfgBeginningOfConstruct(SgNode* c) {
    return CFGNode(c, 0);
  }

  inline CFGNode cfgEndOfConstruct(SgNode* c) {
    return CFGNode(c, CFGNode::childCount(c));
  }

  inline CFGNode makeCfg(SgNode* start) {
    // Returns CFG node for just before start
    return cfgBeginningOfConstruct(start);
  }

  // "Interesting" node and edge filters
  class InterestingEdge;

  class InterestingNode {
    CFGNode n;

    public:
    InterestingNode(CFGNode n): n(n) {}
    std::string toString() const {return n.toString();}
    std::string toStringForDebugging() const {return n.toStringForDebugging();}
    std::string id() const {return n.id();}
    SgNode* getNode() const {return n.getNode();}
    unsigned int getIndex() const {return n.getIndex();}
    std::vector<InterestingEdge> outEdges() const;
    std::vector<InterestingEdge> inEdges() const;
    bool isInteresting() const {return true;}
    bool operator==(const InterestingNode& o) const {return n == o.n;}
    bool operator!=(const InterestingNode& o) const {return !(*this == o);}
    bool operator<(const InterestingNode& o) const {return n < o.n;}
  };

  class InterestingEdge {
    CFGPath p;

    public:
    InterestingEdge(CFGPath p): p(p) {}
    std::string toString() const {return p.toString();}
    std::string toStringForDebugging() const {return p.toStringForDebugging();}
    std::string id() const {return p.id();}
    InterestingNode source() const {return InterestingNode(p.source());}
    InterestingNode target() const {return InterestingNode(p.target());}
    EdgeConditionKind condition() const {return p.condition();}
    SgExpression* caseLabel() const {return p.caseLabel();}
    SgExpression* conditionBasedOn() const {return p.conditionBasedOn();}
    std::vector<SgInitializedName*> scopesBeingExited() const {return p.scopesBeingExited();}
    std::vector<SgInitializedName*> scopesBeingEntered() const {return p.scopesBeingEntered();}
    bool operator==(const InterestingEdge& o) const {return p == o.p;}
    bool operator!=(const InterestingEdge& o) const {return p != o.p;}
#if 0
    bool operator<(const InterestingEdge& o) const {return p < o.p;}
#endif
  };

  inline InterestingNode makeInterestingCfg(SgNode* start) {
    // Returns CFG node for just before start
    return InterestingNode(cfgBeginningOfConstruct(start));
  }

  SgFunctionDeclaration* getDeclaration(SgExpression* func);
  SgExpression* forallMaskExpression(SgForAllStatement* stmt);

  CFGNode getCFGTargetOfFortranLabelSymbol(SgLabelSymbol* sym);
  CFGNode getCFGTargetOfFortranLabelRef(SgLabelRefExp* lRef);
  void addIncomingFortranGotos(SgStatement* stmt, unsigned int index, std::vector<CFGEdge>& result);
  void addOutEdgeOrBypassForExpressionChild(SgNode* me, unsigned int idx, SgExpression* e, std::vector<CFGEdge>& result);
  void addInEdgeOrBypassForExpressionChild(SgNode* me, unsigned int idx, SgExpression* e, std::vector<CFGEdge>& result);
  bool handleFortranIOCommonOutEdges(SgIOStatement* me, unsigned int idx, unsigned int numChildren, std::vector<CFGEdge>& result);
  bool handleFortranIOCommonInEdges(SgIOStatement* me, unsigned int idx, unsigned int numChildren, std::vector<CFGEdge>& result);
  static const unsigned int numberOfFortranIOCommonEdges = 5;
  // These are too complicated to do within Statement.code
  unsigned int doForallCfgIndexForEnd(const SgForAllStatement* me);
  bool doForallCfgIsIndexInteresting(const SgForAllStatement* me, unsigned int idx);
  unsigned int doForallCfgFindChildIndex(SgForAllStatement* me, SgNode* tgt);
  unsigned int doForallCfgFindNextChildIndex(SgForAllStatement* me, SgNode* tgt);
  std::vector<VirtualCFG::CFGEdge> doForallCfgOutEdges(SgForAllStatement* me, unsigned int idx);
  std::vector<VirtualCFG::CFGEdge> doForallCfgInEdges(SgForAllStatement* me, unsigned int idx);
}

#endif // VIRTUAL_CFG_H
