#ifndef VIRTUAL_BIN_CFG_H
#define VIRTUAL_BIN_CFG_H

// #include "rose.h"
// #include <string>
// #include <vector>
// #include <cassert>

// DQ (10/21/2007): Added support for optional use of binary support in ROSE.
#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT

namespace VirtualBinCFG {

  class CFGEdge;

  enum EdgeConditionKind {
    eckUnconditional, // Normal, unconditional edge
    eckTrue,	      // True case of a two-way branch
    eckFalse,	      // False case of a two-way branch
    eckCaseLabel,     // Case label (constant is given by caseLabel())
    eckDefault	      // Default label
  };

  class CFGNode {

    SgAsmNode* node; // Must be either a statement, expression, or SgInitializedName

    public:
    CFGNode(): node(0) {}

     explicit CFGNode(SgNode* node)
        {
          assert (!node || isSgAsmInstruction(node));
          this->node = isSgAsmNode(node);
        }
    // Pretty string for Dot node labels, etc.
    std::string toString() const;
    // String for debugging graphs
    std::string toStringForDebugging() const;
    // ID to use for Dot, etc.
    std::string id() const;

    SgAsmNode* getNode() const {return node;}
  
    std::vector<CFGEdge> outEdges() const;
    std::vector<CFGEdge> inEdges() const;
    bool operator==(const CFGNode& o) const {return node == o.node ;}
    bool operator!=(const CFGNode& o) const {return !(*this == o);}
    bool operator<(const CFGNode& o) const {return node < o.node;}

  };

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
    //SgExpression* caseLabel() const;
    //SgExpression* conditionBasedOn() const;
    //std::vector<SgInitializedName*> scopesBeingExited() const;
    //std::vector<SgInitializedName*> scopesBeingEntered() const;
    bool operator==(const CFGEdge& o) const {return src == o.src && tgt == o.tgt;}
    bool operator!=(const CFGEdge& o) const {return src != o.src || tgt != o.tgt;}
    bool operator<(const CFGEdge& o) const {return src < o.src || (src == o.src && tgt < o.tgt);}
  };

  void makeEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result); // Used in inEdges() and outEdges() methods
  CFGNode getNodeJustAfterInContainer(SgNode* n);
  CFGNode getNodeJustBeforeInContainer(SgNode* n);

}

// endif matching ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
#endif


 // VIRTUAL_CFG_H
#endif
