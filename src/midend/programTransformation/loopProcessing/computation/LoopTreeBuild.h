
#ifndef BUILD_LOOP_TREE
#define BUILD_LOOP_TREE

#include <LoopTree.h>
#include <ProcessAstTree.h>


class LoopTreeBuild : public ProcessAstTree<AstNodePtr>
{
  LoopTreeCreate *lt;
  LoopTreeNode *cur;
  bool supportNonFortranLoop; 
 protected:
  bool ProcessLoop( AstInterface &fa, const AstNodePtr& loop ,
                      const AstNodePtr& body,
                      AstInterface::TraversalVisitType t)
    {
       LoopTreeNode *root = lt->GetTreeRoot();
       if (t == AstInterface::PreVisit) {
         // Liao, we want to allow a loop with nested non-fortran loop in the loop tree. 
          if (fa.IsFortranLoop(loop)||supportNonFortranLoop) {
              LoopTreeNode* result = lt->CreateLoopNode(loop);
              result->Link(cur, LoopTreeNode::AsLastChild);
              cur = result;
           } 
           else  {
             std::cerr << "Warning: treating non-Fortran loop as a single statement\n";
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
       if (t == AstInterface::PreVisit) {
            LoopTreeIfCond* result = lt->CreateIfCond(cond); 
            result->Link(cur, LoopTreeNode::AsLastChild);
            cur = result;
       }
       else {
          if (cur != lt->GetTreeRoot())
             cur = cur->Parent();
       }
       return ProcessAstTree::ProcessIf(fa, s, cond, body1, body2, t);
    }

  bool ProcessGoto(AstInterface &fa, const AstNodePtr& start, const AstNodePtr &)
    { return ProcessStmt(fa, start); }

  bool ProcessStmt( AstInterface &fa, const AstNodePtr& start)
    {
       LoopTreeNode *stmtNode = lt->CreateStmtNode( start);
       stmtNode->Link(cur, LoopTreeNode::AsLastChild);
       return ProcessAstTree::ProcessStmt(fa, start);
    } 
 public:
  LoopTreeBuild(bool nonfortran=0) : supportNonFortranLoop(nonfortran) {}
   bool operator ()( AstInterface& fa, const AstNodePtr& top, 
                        LoopTreeCreate *ltc, LoopTreeNode* root = 0)
     { 
        lt = ltc;
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

