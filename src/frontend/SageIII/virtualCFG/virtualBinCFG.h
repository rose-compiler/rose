#ifndef VIRTUAL_BIN_CFG_H
#define VIRTUAL_BIN_CFG_H

// #include "rose.h"
// #include <string>
// #include <vector>
// #include <cassert>

#include <stdint.h>

namespace VirtualBinCFG {

  class CFGEdge;

  enum EdgeConditionKind {
    eckUnconditional, // Normal, unconditional edge
    eckTrue,	      // True case of a two-way branch
    eckFalse,	      // False case of a two-way branch
    eckCaseLabel,     // Case label (constant is given by caseLabel())
    eckDefault	      // Default label
  };

  struct AuxiliaryInformation {
    std::map<uint64_t, SgAsmInstruction*> addressToInstructionMap;
    std::map<SgAsmInstruction*, std::set<uint64_t> > indirectJumpTargets;
    std::map<SgAsmStatement*, std::set<uint64_t> > returnTargets; // Block or function
    std::map<SgAsmInstruction*, std::set<uint64_t> > incomingEdges;

    public:
    AuxiliaryInformation() {}

    AuxiliaryInformation(SgNode* top);

    SgAsmInstruction* getInstructionAtAddress(uint64_t addr) const {
      std::map<uint64_t, SgAsmInstruction*>::const_iterator i = addressToInstructionMap.find(addr);
      if (i == addressToInstructionMap.end()) return NULL;
      return i->second;
    }

    const std::set<uint64_t>& getPossibleSuccessors(SgAsmInstruction* insn) const;

    const std::set<uint64_t>& getPossiblePredecessors(SgAsmInstruction* insn) const {
      static const std::set<uint64_t> emptySet;
      std::map<SgAsmInstruction*, std::set<uint64_t> >::const_iterator predsIter = incomingEdges.find(insn);
      if (predsIter == incomingEdges.end()) {
        return emptySet;
      } else {
        return predsIter->second;
      }
    }

  };

  class CFGNode {

    SgAsmInstruction* node;
    const AuxiliaryInformation* info;

    public:
    explicit CFGNode(SgAsmInstruction* node, const AuxiliaryInformation* info = NULL): node(node), info(info) {ROSE_ASSERT (node);}
    std::string toString() const;
    // String for debugging graphs
    std::string toStringForDebugging() const;
    // ID to use for Dot, etc.
    std::string id() const;

    SgAsmInstruction* getNode() const {return node;}
  
    std::vector<CFGEdge> outEdges() const;
    std::vector<CFGEdge> inEdges() const;
    bool operator==(const CFGNode& o) const {return node == o.node ;}
    bool operator!=(const CFGNode& o) const {return !(*this == o);}
    bool operator<(const CFGNode& o) const {return node < o.node;}

  };

  class CFGEdge {
    CFGNode src, tgt;
    const AuxiliaryInformation* info;
    public:
    CFGEdge(CFGNode src, CFGNode tgt, const AuxiliaryInformation* info = NULL): src(src), tgt(tgt), info(info) {}
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

    void makeEdge(SgAsmInstruction* from, SgAsmInstruction* to, const AuxiliaryInformation* info, std::vector<CFGEdge>& result); // Used in inEdges() and outEdges() methods

}

 // VIRTUAL_CFG_H
#endif
