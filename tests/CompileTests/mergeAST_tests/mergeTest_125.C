#include <vector>
#include <string>

class SgNode;
class SgInitializedName;
class SgExpression;

// namespace VirtualCFG {

  class CFGEdge;

  class CFGNode {
    SgNode* node;
    unsigned int index;
#if 0
    public:
    CFGNode(SgNode* node, unsigned int index = 0): node(node), index(index) {
//    (static_cast<void> ((!node || isSgStatement(node) || isSgExpression(node) || isSgInitializedName(node)) ? 0 : (__assert_fail ("!node || isSgStatement(node) || isSgExpression(node) || isSgInitializedName(node)", "/home/dquinlan/ROSE/NEW_ROSE/src/frontend/SageIII/virtualCFG/virtualCFG.h", 19, __PRETTY_FUNCTION__), 0)));
    }
    std::string toString() const;
    std::string id() const;
    SgNode* getNode() const {return node;}
    unsigned int getIndex() const {return index;}
    std::vector<CFGEdge> outEdges() const;
    std::vector<CFGEdge> inEdges() const;
    bool isInteresting() const;
    bool operator==(const CFGNode& o) const {return node == o.node && index == o.index;}
    bool operator!=(const CFGNode& o) const {return !(*this == o);}
    static CFGNode endOfProcedure() {return CFGNode(__null, 0);}
    static unsigned int childCount(SgNode* n);
#endif
  };

#if 1
  class CFGEdge {
    CFGNode src, tgt;
    public:
    CFGEdge(CFGNode src, CFGNode tgt): src(src), tgt(tgt) {}
    std::string toString() const;
    std::string id() const;
    CFGNode source() const {return src;}
    CFGNode target() const {return tgt;}

    SgExpression* optionKey() const;
    std::vector<SgInitializedName*> scopesBeingExited() const;
    std::vector<SgInitializedName*> scopesBeingEntered() const;
    bool operator==(const CFGEdge& o) const {return id() == o.id();}
    bool operator!=(const CFGEdge& o) const {return id() != o.id();}
    bool operator<(const CFGEdge& o) const {return id() < o.id();}
  };
#endif

#if 0
// This generates the extra errors:
// Node is not in parent's child list, node: 0x9229a88 = SgFunctionDefinition = operator== parent: 0xb7ea05e0 = SgFunctionDeclaration
// Node is not in parent's child list, node: 0x9229b10 = SgFunctionDefinition = operator!= parent: 0xb7ea0748 = SgFunctionDeclaration
// This SgTypedefDeclaration is not in parent's child list, node: 0xb7f0a61c = SgTypedefDeclaration = pointer parent: 0x95753b4 = SgTemplateInstantiationDefn
// SgVariableSymbol is not in parent's child list, node: 0x920b8f0 = SgVariableSymbol = __assert_fail_symbol_ parent: 0x86bab0c = SgSymbolTable
// SgVariableSymbol is not in parent's child list, node: 0x920b908 = SgVariableSymbol = __assert_fail_symbol_ parent: 0x86baba4 = SgSymbolTable
// SgVariableSymbol is not in parent's child list, node: 0x920b920 = SgVariableSymbol = __assert_fail_symbol_ parent: 0x86bac3c = SgSymbolTable
// Node is not in parent's child list, node: 0x93fb418 = SgMemberFunctionSymbol = ~CFGEdge()_symbol_ parent: 0x86b9dfc = SgSymbolTable

  void makeEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result);
  CFGNode getNodeJustAfterInContainer(SgNode* n);
  CFGNode getNodeJustBeforeInContainer(SgNode* n);

  class CFGPath {
    std::vector<CFGEdge> edges;
    public:


    CFGPath(CFGEdge e): edges(1, e) {}

    CFGPath(const CFGPath& a, const CFGPath& b): edges(a.edges) {
      (static_cast<void> ((!a.edges.empty()) ? 0 : (__assert_fail ("!a.edges.empty()", "/home/dquinlan/ROSE/NEW_ROSE/src/frontend/SageIII/virtualCFG/virtualCFG.h", 63, __PRETTY_FUNCTION__), 0)));
      (static_cast<void> ((!b.edges.empty()) ? 0 : (__assert_fail ("!b.edges.empty()", "/home/dquinlan/ROSE/NEW_ROSE/src/frontend/SageIII/virtualCFG/virtualCFG.h", 64, __PRETTY_FUNCTION__), 0)));
      (static_cast<void> ((a.edges.back().target() == b.edges.front().source()) ? 0 : (__assert_fail ("a.edges.back().target() == b.edges.front().source()", "/home/dquinlan/ROSE/NEW_ROSE/src/frontend/SageIII/virtualCFG/virtualCFG.h", 65, __PRETTY_FUNCTION__), 0)));
      edges.insert(edges.end(),b.edges.begin(),b.edges.end());
    }
    std::string toString() const;
    std::string id() const;
    CFGNode source() const {(static_cast<void> ((!edges.empty()) ? 0 : (__assert_fail ("!edges.empty()", "/home/dquinlan/ROSE/NEW_ROSE/src/frontend/SageIII/virtualCFG/virtualCFG.h", 70, __PRETTY_FUNCTION__), 0))); return edges.front().source();}
    CFGNode target() const {(static_cast<void> ((!edges.empty()) ? 0 : (__assert_fail ("!edges.empty()", "/home/dquinlan/ROSE/NEW_ROSE/src/frontend/SageIII/virtualCFG/virtualCFG.h", 71, __PRETTY_FUNCTION__), 0))); return edges.back().target();}

    SgExpression* optionKey() const {
      for (unsigned int i = 0; i < edges.size(); ++i) {
 SgExpression* k_i = edges[i].optionKey();
 if (k_i) return k_i;
      }
      return __null;
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
    bool operator<(const CFGPath& o) const {

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
  };

  inline CFGPath mergePaths(const CFGPath& hd, const CFGPath& tl) {

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

    return cfgBeginningOfConstruct(start);
  }


  class InterestingEdge;

  class InterestingNode {
    CFGNode n;

    public:
    InterestingNode(CFGNode n): n(n) {}
    std::string toString() const {return n.toString();}
    std::string id() const {return n.id();}
    SgNode* getNode() const {return n.getNode();}
    unsigned int getIndex() const {return n.getIndex();}
    std::vector<InterestingEdge> outEdges() const;
    std::vector<InterestingEdge> inEdges() const;
    bool isInteresting() const {return true;}
    bool operator==(const InterestingNode& o) const {return n == o.n;}
    bool operator!=(const InterestingNode& o) const {return !(*this == o);}
  };

  class InterestingEdge {
    CFGPath p;

    public:
    InterestingEdge(CFGPath p): p(p) {}
    std::string toString() const {return p.toString();}
    std::string id() const {return p.id();}
    InterestingNode source() const {return InterestingNode(p.source());}
    InterestingNode target() const {return InterestingNode(p.target());}
    SgExpression* optionKey() const {return p.optionKey();}
    std::vector<SgInitializedName*> scopesBeingExited() const {return p.scopesBeingExited();}
    std::vector<SgInitializedName*> scopesBeingEntered() const {return p.scopesBeingEntered();}
  };

  inline InterestingNode makeInterestingCfg(SgNode* start) {

    return InterestingNode(cfgBeginningOfConstruct(start));
  }

#endif

