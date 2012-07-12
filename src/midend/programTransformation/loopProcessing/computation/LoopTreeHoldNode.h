
#ifndef LOOP_TREE_HOLDNODE_OBSERVER
#define LOOP_TREE_HOLDNODE_OBSERVER

#include <stdlib.h>
#include <LoopTreeObserver.h>

class HoldTreeNodeObserver : public LoopTreeObserver
{
  LoopTreeNode *node;
 protected:
  void UpdateDeleteNode( const LoopTreeNode *n ) { node = 0; }
 public:
  HoldTreeNodeObserver( LoopTreeNode *n, LoopTreeObserver *_next = 0)
     : LoopTreeObserver(_next){ node = n; node->AttachObserver(*this); }
  HoldTreeNodeObserver() { node = 0; } 
  ~HoldTreeNodeObserver() 
      {
         if (node != 0)
            node->DetachObserver( *this );
      }

  LoopTreeNode * GetTreeNode() const { return node;}
  void ResetHoldTreeNode( LoopTreeNode *n)
      {
        if (node != 0)
           node->DetachObserver( *this );
        node = n;
        if (node != 0)
           node->AttachObserver( *this );
      }
};

class HoldLoopAlignObserver : protected HoldTreeNodeObserver
{
  int align;
  
  void UpdateMergeLoop( const MergeLoopInfo &info) 
       {
          LoopTreeNode *l = info.GetNewLoop();
          ResetHoldTreeNode( l );
          align += info.GetMergeAlign();
       }
 public:
  HoldLoopAlignObserver( LoopTreeNode *n, int _align, 
                         LoopTreeObserver *_next = 0)
     : HoldTreeNodeObserver( n, _next ) { align = _align; }
  ~HoldLoopAlignObserver() {}

  int GetLoopAlign() const { return align; }
  void SetLoopAlign( int a ) { align = a; }
};

class HoldAncesLoopObserver : protected HoldLoopAlignObserver
{
  LoopTreeNode *desc;

  void UpdateDistNode(const DistNodeInfo &info) 
      {
        const LoopTreeNode *orig = info.GetObserveNode();
        LoopTreeNode *nloop = info.GetNewNode();
        LoopTreeNode *n = desc->Parent();
        for ( ;  n != nloop && n != orig; n = n->Parent()) ;
        if (n == nloop) 
            ResetHoldTreeNode( nloop );
       }
 public:
  HoldAncesLoopObserver( LoopTreeNode *l, int _align, 
                         LoopTreeNode *_desc, LoopTreeObserver *h=0)
   : HoldLoopAlignObserver ( l, _align, h), desc(_desc) {}
  ~HoldAncesLoopObserver() {}

  LoopTreeNode* GetAncesLoop() const { return GetTreeNode(); }
  LoopTreeNode* GetDescNode() const  { return desc; }
  HoldLoopAlignObserver::GetLoopAlign;
  HoldLoopAlignObserver::SetLoopAlign;
};

#endif
