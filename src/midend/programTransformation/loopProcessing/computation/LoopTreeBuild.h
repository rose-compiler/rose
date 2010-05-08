
#ifndef BUILD_LOOP_TREE
#define BUILD_LOOP_TREE

#include <LoopTree.h>
#include <ProcessAstTree.h>

class LoopTreeBuild : public ProcessAstTree
{
  LoopTreeCreate *lt;
  LoopTransformInterface* la;
  LoopTreeNode *cur;
 protected:
  bool ProcessLoop( AstInterface &fa, const AstNodePtr& loop ,
                      const AstNodePtr& body,
                      AstInterface::TraversalVisitType t)
    {
       LoopTreeNode *root = lt->GetTreeRoot();
       if (t == AstInterface::PreVisit) {
          if (fa.IsFortranLoop(loop)) {
              LoopTreeNode* result = lt->CreateLoopNode( *la, loop);
              result->Link(cur, LoopTreeNode::AsLastChild);
              cur = result;
           } 
           else  {
             std::cerr << "Warning: treating non-Fortran loop at:"
               << getAstLocation(loop)
               <<" as a single statement\n";
             ProcessStmt(fa, loop);
           }
        }
        else {
          if (cur != root)
             cur = cur->Parent();
        }
        return ProcessAstTree::ProcessLoop(fa, loop, body, t);
     }
  bool ProcessIf( AstInterface &fa, const AstNodePtr& s, const AstNodePtr& cond,
                     const AstNodePtr& body1, const AstNodePtr& body2,
                      AstInterface::TraversalVisitType t)
    { 
        if (t == AstInterface::PreVisit) return ProcessStmt(fa, s); 
         return true;
    }

  bool ProcessGoto( AstInterface &fa, const AstNodePtr& start, 
                       const AstNodePtr& dest)
    { return ProcessStmt(fa, start); }

  bool ProcessStmt( AstInterface &fa, const AstNodePtr& start)
    {
       LoopTreeNode *stmtNode = lt->CreateStmtNode( start);
       stmtNode->Link(cur, LoopTreeNode::AsLastChild);
       return ProcessAstTree::ProcessStmt(fa, start);
    } 
 public:
   bool operator ()( AstInterface& fa, const AstNodePtr& top, 
                        LoopTreeCreate *ltc, LoopTransformInterface* _la,
                        LoopTreeNode* root = 0)
     { 
        lt = ltc;
        la = _la;
        cur = (root)? root: lt->GetTreeRoot();
        return ReadAstTraverse(fa, top, *this, AstInterface::PreAndPostOrder);
     }

   LoopTreeNode* LastProcessedNode()
     {
        if (cur->ChildCount() > 0)
           return cur->LastChild();
        else
           return cur;
     }
};

#endif

