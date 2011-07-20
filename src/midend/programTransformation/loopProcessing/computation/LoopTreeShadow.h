
#ifndef SHADOW_LOOP_TREE
#define SHADOW_LOOP_TREE

#include <PtrMap.h>
#include <LoopTree.h>

class LoopTreeShadowCreate;
class LoopTreeShadowNode : public LoopTreeNode
{
  LoopTreeNode *repr;
  PtrMapWrap <LoopTreeNode, LoopTreeShadowNode>* map;
 protected:
  void ReplaceRepr( LoopTreeNode *n)
     { if (repr != 0) 
          map->RemoveMapping(repr);
       repr = n;
       if (repr != 0) {
          assert(map->Map(n) == 0);
          map->InsertMapping(repr, this);
       }
     }
  virtual ~LoopTreeShadowNode()
     { if (repr != 0)
           map->RemoveMapping(repr); 
     }
  LoopTreeShadowNode* QueryShadowNode(const LoopTreeNode* n) const
    { return map->Map(const_cast<LoopTreeNode*>(n)); }
  LoopTreeShadowNode( LoopTreeNode *n, LoopTreeShadowCreate *c) ;
  LoopTreeShadowNode( LoopTreeNode *n, const LoopTreeShadowNode &that)
     : repr(n), map(that.map) {}
 public:
  AstNodePtr GetOrigStmt() const { return repr->GetOrigStmt(); }
  LoopInfo * GetLoopInfo() { return repr->GetLoopInfo(); }
  LoopTreeNode* GetRepr() const { return repr; }
  int IncreaseLoopLevel() const { return repr->IncreaseLoopLevel(); }

  virtual LoopTreeNode* set_preAnnot(const std::string& content) { return 0; }
  virtual LoopTreeNode* set_postAnnot(const std::string& content) { return 0; }
  
  std::string GetClassName() const { return "LoopTreeShadowNode"; }
  LoopTreeNode *Clone() const { return 0; }
  virtual LoopTreeShadowNode* CloneNode(LoopTreeNode *n) const
    { return new LoopTreeShadowNode(n, *this); }
  
  void write( std::ostream& out) const { if (repr != NULL) repr->write(out); }
  AstNodePtr CodeGen( const AstNodePtr& c) const
    { return repr->CodeGen(c); }
 friend class LoopTreeShadowCreate;
};

class LoopTreeShadowCreate : public LoopTreeCreate
{
  PtrMapWrap <LoopTreeNode, LoopTreeShadowNode> nodeMap;
 protected:
  virtual LoopTreeShadowNode* CreateShadowNode( LoopTreeNode *n)
          { return new LoopTreeShadowNode( n, this ); }
  LoopTreeShadowCreate( int _level) : LoopTreeCreate(_level) {}
 public:
  virtual ~LoopTreeShadowCreate();
  LoopTreeShadowNode* QueryShadowNode( const LoopTreeNode *n) const
    { return nodeMap.Map(const_cast<LoopTreeNode*>(n)); }

 friend class LoopTreeShadowNode;
};

class LoopTreeShadowSelect : public LoopTreeShadowCreate
{
  LoopTreeNode* Build(LoopTreeNode *orig, LoopTreeNode* r = 0);
  virtual bool select_stmt(LoopTreeNode* n) const = 0;
 public:
  ~LoopTreeShadowSelect() {}
  LoopTreeShadowSelect( LoopTreeNode *orig);
};

#endif

