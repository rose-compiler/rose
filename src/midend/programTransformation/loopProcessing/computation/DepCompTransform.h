
#ifndef DEP_COMP_TRANSFORM_H
#define DEP_COMP_TRANSFORM_H

#include <LoopTreeLocality.h>
#include <LoopTreeTransform.h>
#include <GraphScope.h>
#include <DoublyLinkedList.h>

class DepCompDistributeLoop
{
 public:
  //selective distribution
  struct Result 
     { LoopTreeNode* node; SelectPtrSet<LoopTreeNode> sel; 
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
    class InsideGraph: public GraphSelect<DepCompAstRefGraphCreate> 
    { 
        NodeSet nodesel;
        int level;
      public:
       InsideGraph(const DepCompAstRefGraphCreate* g, const CopyArrayUnit& unit)
        : GraphSelect<DepCompAstRefGraphCreate> (g), nodesel(unit.refs),
           level(unit.copylevel()-1) {}
       bool ContainNode( const DepCompAstRefGraphNode* n) const
          { return nodesel.IsMember(n); }
       bool ContainEdge( const DepInfoEdge* e) const
        {
         return nodesel.IsMember(impl->GetEdgeEndPoint(e,GraphAccess::EdgeOut)) 
               && nodesel.IsMember(impl->GetEdgeEndPoint(e,GraphAccess::EdgeIn))
               && SelectDepLevel(e->GetInfo(), level);
        } 
    };
   class CrossGraph: public GraphSelect<DepCompAstRefGraphCreate> 
   { 
        NodeSet nodesel;
        int level;
    public:
      CrossGraph(const DepCompAstRefGraphCreate* g, const CopyArrayUnit& unit)
        : GraphSelect<DepCompAstRefGraphCreate>(g), 
           nodesel(unit.refs), level(unit.copylevel()) {}
      bool ContainNode( const DepCompAstRefGraphNode* n) const
         { return impl->ContainNode(n); }
      bool ContainEdge( const DepInfoEdge* e) const
       {
        if (!SelectDepLevel(e->GetInfo(), level))
            return false;
        bool b1=nodesel.IsMember(impl->GetEdgeEndPoint(e,GraphAccess::EdgeOut));
        bool b2=nodesel.IsMember(impl->GetEdgeEndPoint(e,GraphAccess::EdgeIn));
        return (b1 && !b2) || (!b1 && b2);
       } 
    };
    class CrossGraphOut: public GraphSelect<DepCompAstRefGraphCreate>
    { 
        NodeSet nodesel;
        int level;
      public:
       CrossGraphOut(const DepCompAstRefGraphCreate* g, 
                     const CopyArrayUnit& unit)
        : GraphSelect<DepCompAstRefGraphCreate>(g),
            nodesel(unit.refs),  level(unit.copylevel()) {}
      bool ContainNode( const DepCompAstRefGraphNode* n) const
         { return impl->ContainNode(n); }
      bool ContainEdge( const DepInfoEdge* e) const
       {
         return nodesel.IsMember(impl->GetEdgeEndPoint(e,GraphAccess::EdgeOut)) 
             && !nodesel.IsMember(impl->GetEdgeEndPoint(e,GraphAccess::EdgeIn))
               && SelectDepLevel(e->GetInfo(), level);
       } 
    };
    class CrossGraphIn: public GraphSelect<DepCompAstRefGraphCreate>
    { 
        NodeSet nodesel;
        int level;
      public:
       CrossGraphIn(const DepCompAstRefGraphCreate* g, 
                     const CopyArrayUnit& unit)
        : GraphSelect<DepCompAstRefGraphCreate>(g),
            nodesel(unit.refs),  level(unit.copylevel()) {}
      bool ContainNode( const DepCompAstRefGraphNode* n) const
         { return impl->ContainNode(n); }
      bool ContainEdge( const DepInfoEdge* e) const
       {
         return nodesel.IsMember(impl->GetEdgeEndPoint(e,GraphAccess::EdgeIn))  
             && !nodesel.IsMember(impl->GetEdgeEndPoint(e,GraphAccess::EdgeOut))
               && SelectDepLevel(e->GetInfo(), level);
       } 
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

  DepCompCopyArrayCollect( LoopTreeNode* root) : stmtrefInfo(root) {}
 private:
  DoublyLinkedListWrap<CopyArrayUnit> arraycopy;
  DepCompAstRefAnal stmtrefInfo;
};

class DepCompCopyArrayToBuffer 
{ 
 protected:
 /*QY: perform all copy + init + save transformations specified in (collect) */
  virtual void ApplyXform( DepCompCopyArrayCollect::CopyArrayUnit& curarray,
                CopyArrayConfig& config, LoopTreeNode* replRoot,
                LoopTreeNode* initStmt, LoopTreeNode* saveStmt);
 public: 
  void EnforceCopyRoot( DepCompCopyArrayCollect::CopyArrayUnit& curunit, 
                       const DepCompAstRefGraphCreate& refDep,
                        DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts);
  void EnforceCopyRoot( DepCompCopyArrayCollect::CopyArrayUnit& curunit, 
                       const DepCompAstRefGraphCreate& refDep,
                       const DepCompAstRefGraphNode* outnode,
                       DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts);
  void CollectCopyArray( DepCompCopyArrayCollect& col, 
                         const DepCompAstRefGraphCreate& refDep);
  void ApplyCopyArray( DepCompCopyArrayCollect& col, 
                         const DepCompAstRefGraphCreate& refDep);
};

#endif
