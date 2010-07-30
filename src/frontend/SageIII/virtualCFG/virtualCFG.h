#ifndef VIRTUAL_CFG_H
#define VIRTUAL_CFG_H




//! FIXME: The CFG support for Fortran is still buggy -- if Fortran is
//! re-enabled in the test suite, there are edge mismatches indicating that the
//! CFG's in and out edge sets are not consistent.  The CFG is not recommended
//! for use on Fortran code.
class SgNode;
class SgExpression;
class SgInitializedName;
class SgLabelSymbol;
class SgLabelRefExp;
class SgStatement;

#ifndef _MSC_VER
SgStatement* isSgStatement(SgNode* node);
const SgStatement* isSgStatement(const SgNode* node);
SgExpression* isSgExpression(SgNode* node);
const SgExpression* isSgExpression(const SgNode* node);
SgInitializedName* isSgInitializedName(SgNode* node);
const SgInitializedName* isSgInitializedName(const SgNode* node);
#endif

namespace VirtualCFG {

  class CFGEdge;

  //! The conditions attached to edges are marked to determine which conditions
  //! trigger control to flow along that edge (as opposed to other edges out of
  //! the same source node).  For conditional branches (except eckCaseLabel and
  //! eckDefault), the conditions are implicit and depend on knowledge of the
  //! particular control structure.  Fortran support for this is underdeveloped;
  //! single AST nodes representing variable-length loops was not part of the
  //! original design of the CFG code.
  enum EdgeConditionKind {
    eckUnconditional, //! Normal, unconditional edge
    eckTrue,	      //! True case of a two-way branch
    eckFalse,	      //! False case of a two-way branch
    eckCaseLabel,     //! Case label (constant is given by caseLabel())
    eckDefault,	      //! Default label
    eckDoConditionPassed, //! Enter Fortran do loop body
    eckDoConditionFailed, //! Fortran do loop finished
    eckForallIndicesInRange, //! Start testing forall mask
    eckForallIndicesNotInRange, //! End of forall loop
    eckComputedGotoCaseLabel, //! Case in computed goto -- number needs to be computed separately
    eckArithmeticIfLess, //! Edge for the arithmetic if expression being less than zero
    eckArithmeticIfEqual, //! Edge for the arithmetic if expression being equal to zero
    eckArithmeticIfGreater, //! Edge for the arithmetic if expression being greater than zero
    eckInterprocedural //! Edge spanning two procedures
  };

  //! A node in the control flow graph.  Each CFG node corresponds to an AST
  //! node, but there can be several CFG nodes for a given AST node.  
  class CFGNode {
    //! The AST node from this CFG node
    SgNode* node; // Must be either a SgStatement, SgExpression, or SgInitializedName (FIXME: change this to just SgLocatedNode if SgInitializedName becomes a subclass of that)

    //! An index to differentiate control flow points within a single AST node
    //! (for example, before and after the test of an if statement)
    unsigned int index;

    public:
    CFGNode(): node(0), index(0) {}
    CFGNode(SgNode* node, unsigned int index = 0);

    //! Pretty string for Dot node labels, etc.
    std::string toString() const;
    //! String for debugging graphs
    std::string toStringForDebugging() const;
    //! ID to use for Dot, etc.
    std::string id() const;
    //! The underlying AST node
    SgNode* getNode() const {return node;}
    //! An identifying index within the AST node given by getNode()
    unsigned int getIndex() const {return index;}
    //! Outgoing control flow edges from this node
    std::vector<CFGEdge> outEdges() const;
    //! Incoming control flow edges to this node
    std::vector<CFGEdge> inEdges() const;
    //! Test whether this node satisfies a (fairly arbitrary) standard for
    //! "interestingness".  There are many administrative nodes in the raw CFG
    //! (nodes that do not correspond to operations in the program), and this
    //! function filters them out.
    bool isInteresting() const;
    //! Equality operator
    bool operator==(const CFGNode& o) const {return node == o.node && index == o.index;}
    //! Disequality operator
    bool operator!=(const CFGNode& o) const {return !(*this == o);}
    //! Less-than operator
    bool operator<(const CFGNode& o) const {return node < o.node || (node == o.node && index < o.index);}
  }; // end class CFGNode

  //! A control flow edge connecting two CFG nodes, with an edge condition to
  //! indicate edge types
  class CFGEdge {
    CFGNode src, tgt;
    public:
    //! Constructor
    CFGEdge(CFGNode src, CFGNode tgt): src(src), tgt(tgt) {}
    //! Pretty string for Dot node labels, etc.
    std::string toString() const;
    //! String for debugging graphs
    std::string toStringForDebugging() const;
    //! ID to use for Dot, etc.
    std::string id() const;
    //! The source (beginning) CFG node
    CFGNode source() const {return src;}
    //! The target (ending) CFG node
    CFGNode target() const {return tgt;}
    //! The control flow condition that enables this edge
    EdgeConditionKind condition() const;
    //! The label of the case represented by an eckCaseLabel edge
    SgExpression* caseLabel() const;
    //! The expression of the computed goto represented by the eckArithmeticIf* conditions
    unsigned int computedGotoCaseIndex() const;
    //! The test or case key that is tested as a condition of this control flow edge
    SgExpression* conditionBasedOn() const;
    //! Variables going out of scope across this edge (not extensively tested)
    std::vector<SgInitializedName*> scopesBeingExited() const;
    //! Variables coming into scope across this edge (not extensively tested)
    std::vector<SgInitializedName*> scopesBeingEntered() const;
    //! Compare equality of edges
    bool operator==(const CFGEdge& o) const {return src == o.src && tgt == o.tgt;}
    //! Compare disequality of edges
    bool operator!=(const CFGEdge& o) const {return src != o.src || tgt != o.tgt;}
#if 0
    //! We ban operator<() because it relies on system-specific comparisons among AST node pointers
    bool operator<(const CFGEdge& o) const {return src < o.src || (src == o.src && tgt < o.tgt);}
#endif
  }; // end CFGEdge

  //! \internal A CFG path is a set of connected CFG edges; condition and
  //! variable information is combined across the edges in the path.  Paths are
  //! used as parts of edges in filtered views of the CFG.  They implement almost
  //! the same functions as CFG edges, and their functions are just forwarded to
  //! by filtered edge methods.  This is an internal class.
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

  //! \internal Merge two CFG paths
  inline CFGPath mergePaths(const CFGPath& hd, const CFGPath& tl) {
    // Assumes the edges don't do anything too complicated with scopes
    return CFGPath(hd, tl);
  }

  //! \internal mergePaths() with the arguments reversed
  inline CFGPath mergePathsReversed(const CFGPath& tl, const CFGPath& hd) {
    return mergePaths(hd, tl);
  }

  //! The first CFG node for a construct (before the construct starts to
  //! execute)
  inline CFGNode cfgBeginningOfConstruct(SgNode* c) {
    return CFGNode(c, 0);
  }

  //! \internal Non-member wrapper for SgNode::cfgIndexForEnd since that has
  //! not yet been defined
  unsigned int cfgIndexForEndWrapper(SgNode* n);

  //! The last CFG node for a construct (after the entire construct has finished
  //! executing).  This node may not actually be reached if, for example, a goto
  //! causes a loop to be exited in the middle
  inline CFGNode cfgEndOfConstruct(SgNode* c) {
    return CFGNode(c, cfgIndexForEndWrapper(c));
  }

  //! Returns CFG node for just before start
  inline CFGNode makeCfg(SgNode* start) {
    return cfgBeginningOfConstruct(start);
  }

  //! "Interesting" node and edge filters
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

  //! \internal Get the CFG node for a Fortran label from its symbol
  CFGNode getCFGTargetOfFortranLabelSymbol(SgLabelSymbol* sym);
  //! \internal Get the CFG node for a Fortran label from a reference to it
  CFGNode getCFGTargetOfFortranLabelRef(SgLabelRefExp* lRef);
}

#endif // VIRTUAL_CFG_H
