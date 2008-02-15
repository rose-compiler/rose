
#ifndef DEP_COMP_TRANSFORM_H
#define DEP_COMP_TRANSFORM_H

#include <LoopTreeLocality.h>
#include <GraphScope.h>
#include <DoublyLinkedList.h>

class DepCompDistributeLoop
{
 public:
  //selective distribution
  struct Result 
     { LoopTreeNode* node; PtrSetWrap<LoopTreeNode> sel; 
       Result() : node(0) {}
       Result( LoopTreeNode* n, const PtrSetWrap<LoopTreeNode>& _sel) 
            : node(n), sel(_sel) {} 
     };
  Result operator ()( LoopTreeDepComp &tc, LoopTreeNode *l,
                       LoopTreeNodeIterator iter);
  // max distribution
  Result operator ()( LoopTreeDepComp &tc, LoopTreeNode *l);
};

class DepCompCopyArrayCollect
{
 public:
  struct CopyArrayUnit {
    typedef PtrSetWrap<const DepCompAstRefGraphNode> NodeSet;
    typedef SelectPtrSet<const DepCompAstRefGraphNode> SelectNodeSet;
    typedef NotSelectPtrSet<const DepCompAstRefGraphNode> NotSelectNodeSet;
    typedef GraphSelectEndSet2<DepCompAstRefGraphCreate, SelectNodeSet> SelectInsideNode;
    typedef GraphSelectEndSet1<DepCompAstRefGraphCreate, SelectNodeSet> SelectCrossNode;
    typedef GraphSelectEndSet<DepCompAstRefGraphCreate, SelectNodeSet, NotSelectNodeSet>
                               SelectCrossOutNode;
    typedef GraphSelectEndSet<DepCompAstRefGraphCreate, NotSelectNodeSet, SelectNodeSet>
                               SelectCrossInNode;
    typedef GraphSelectCompound<DepCompAstRefGraphCreate, SelectInsideNode, SelectDepLevel> 
              SelectInside;
    typedef GraphSelectCompound<DepCompAstRefGraphCreate, SelectCrossNode, SelectDepLevel> 
              SelectCross;
    typedef GraphSelectCompound<DepCompAstRefGraphCreate, SelectCrossOutNode, SelectDepLevel> 
              SelectCrossOut;
    typedef GraphSelectCompound<DepCompAstRefGraphCreate, SelectCrossInNode, SelectDepLevel> 
              SelectCrossIn;
    class InsideGraph: public GraphSelect<DepCompAstRefGraphCreate,SelectInside> 
    { public:
       InsideGraph(const DepCompAstRefGraphCreate* g, const CopyArrayUnit& unit)
        : GraphSelect<DepCompAstRefGraphCreate,SelectInside> 
            (g, SelectInside(SelectInsideNode(g, unit.refs), 
                             SelectDepLevel(unit.copylevel()-1)))
        {}
    };
    class CrossGraph: public GraphSelect<DepCompAstRefGraphCreate,SelectCross> 
    { public:
       CrossGraph(const DepCompAstRefGraphCreate* g, const CopyArrayUnit& unit)
        : GraphSelect<DepCompAstRefGraphCreate,SelectCross> 
            (g,SelectCross(SelectCrossNode(g, unit.refs), 
                           SelectDepLevel(unit.copylevel())))
       {}
    };
    class CrossGraphOut: public GraphSelect<DepCompAstRefGraphCreate,SelectCrossOut> 
    { public:
       CrossGraphOut(const DepCompAstRefGraphCreate* g, const CopyArrayUnit& unit)
        : GraphSelect<DepCompAstRefGraphCreate,SelectCrossOut>
            (g,SelectCrossOut(SelectCrossOutNode(g, unit.refs,unit.refs), 
                              SelectDepLevel(unit.copylevel())))
       {}
    };
    class CrossGraphIn: public GraphSelect<DepCompAstRefGraphCreate,SelectCrossIn> 
    { public:
       CrossGraphIn(const DepCompAstRefGraphCreate* g, const CopyArrayUnit& unit)
        : GraphSelect<DepCompAstRefGraphCreate,SelectCrossIn>
            (g,SelectCrossIn(SelectCrossInNode(g, unit.refs,unit.refs), 
                             SelectDepLevel(unit.copylevel())))
       {}
    };

    NodeSet refs;
    LoopTreeNode* root;
    bool carrybyroot;
    int copylevel() const 
    { return (!root->IncreaseLoopLevel())? root->LoopLevel() : root->LoopLevel()+1; }
    CopyArrayUnit() : root(0),carrybyroot(false) {}
    CopyArrayUnit(const NodeSet& that, LoopTreeNode* r, bool carry = false)
     : refs(that), root(r), carrybyroot(carry) {}
  };


  typedef DoublyLinkedListWrap<CopyArrayUnit>::iterator iterator;

  CopyArrayUnit& AddCopyArray()
    {
       DoublyLinkedEntryWrap<CopyArrayUnit>* r = arraycopy.AppendLast(CopyArrayUnit());
       return r->GetEntry();
    }
  void RemoveCopyArray( iterator& cur)
   { arraycopy.Delete(cur.CurrentPtr()); }

  iterator begin() { return arraycopy.begin(); }
  iterator end() { return arraycopy.end(); }

  const DepCompAstRefAnal& get_stmtref_info() const { return  stmtrefInfo; }
  DepCompAstRefAnal& get_stmtref_info() { return  stmtrefInfo; }
  LoopTreeNode* get_tree_root() const { return stmtrefInfo.get_tree_root(); }

  LoopTreeNode* ComputeCommonRoot(CopyArrayUnit::NodeSet& refs);
  LoopTreeNode* OutmostCopyRoot( DepCompCopyArrayCollect::CopyArrayUnit& unit, 
                               DepCompAstRefGraphCreate& refDep, LoopTreeNode* treeroot);

  DepCompCopyArrayCollect( LoopTransformInterface& la, LoopTreeNode* root) 
      : stmtrefInfo(la, root) {}
 private:
  DoublyLinkedListWrap<CopyArrayUnit> arraycopy;
  DepCompAstRefAnal stmtrefInfo;
};

class DepCompCopyArrayToBuffer 
{ 
 public: 
  void EnforceCopyRoot( DepCompCopyArrayCollect::CopyArrayUnit& curunit, 
                       const DepCompAstRefGraphCreate& refDep,
                        DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts);
  void EnforceCopyRoot( DepCompCopyArrayCollect::CopyArrayUnit& curunit, 
                       const DepCompAstRefGraphCreate& refDep,
                       const DepCompAstRefGraphNode* outnode,
                       DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts);
  void CollectCopyArray(LoopTransformInterface& la, DepCompCopyArrayCollect& col, 
                         const DepCompAstRefGraphCreate& refDep);
  void ApplyCopyArray(LoopTransformInterface& la, DepCompCopyArrayCollect& col, 
                         const DepCompAstRefGraphCreate& refDep);
};

#endif
